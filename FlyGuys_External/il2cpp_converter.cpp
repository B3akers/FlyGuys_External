/* This file is part of FlyGuys_External by b3akers, licensed under the MIT license:
*
* MIT License
*
* Copyright (c) b3akers 2020
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "il2cpp_converter.hpp"

#include <string>
#include <fstream>
#include <streambuf>
#include <regex>

// trim from start (in place)
static inline void ltrim( std::string& s ) {
	s.erase( s.begin( ), std::find_if( s.begin( ), s.end( ), [ ] ( int ch ) {
		return !std::isspace( ch );
		} ) );
}

// trim from end (in place)
static inline void rtrim( std::string& s ) {
	s.erase( std::find_if( s.rbegin( ), s.rend( ), [ ] ( int ch ) {
		return !std::isspace( ch );
		} ).base( ), s.end( ) );
}

struct replace_string_info {
	int64_t offset;
	int64_t len;
	std::string str;
};

//arrays
void convert2( std::string& il2cpp_external_cpp ) {

	printf( "STAGE 2\n" );

	std::ifstream il2cpp = std::ifstream( "il2cpp_external.h" );

	if ( !il2cpp.good( ) ) {
		il2cpp.close( );
		return;
	}

	std::string str( ( std::istreambuf_iterator<char>( il2cpp ) ),
		std::istreambuf_iterator<char>( ) );

	std::vector<replace_string_info> replaces;

	il2cpp.close( );

	const std::regex array_value( "[_a-zA-Z].+\\[65535.+;" );

	std::regex_iterator<std::string::iterator> it( str.begin( ), str.end( ), array_value );
	std::regex_iterator<std::string::iterator> end;

	auto old_class_name = std::string( );
	while ( it != end ) {
		auto variable = it->str( );
		auto offset = it->position( );

		auto class_name = it->prefix( ).str( );
		auto class_begin = class_name.find_last_of( '{' );
		if ( class_begin != std::string::npos ) {
			class_name = class_name.substr( 0, class_begin );
			rtrim( class_name );
			class_name = class_name.substr( class_name.find_last_of( ' ' ) + 1 );

			if ( class_name.empty( ) )
				class_name = old_class_name;
			else
				old_class_name = class_name;
		} else
			class_name = old_class_name;

		auto variable_type = variable.substr( 0, variable.find_first_of( ' ' ) );
		auto is_pointer_array = variable_type.find( "*" ) != std::string::npos;
		if ( is_pointer_array )
			variable_type = variable_type.substr( 0, variable_type.length( ) - 1 );

		auto str_replace = std::string( );

		if ( is_pointer_array ) {
			str_replace.append( "static " + variable_type + " m_Items(uint64_t* items_ptr, size_t i);\n" );
			str_replace.append( "	static std::unique_ptr<uint64_t[]> m_ItemsArray(uint64_t ptr, size_t i);" );

			il2cpp_external_cpp.append( "std::unique_ptr<uint64_t[]> " + class_name + "::m_ItemsArray (uint64_t ptr, size_t i) { " );
			il2cpp_external_cpp.append( "auto items_array = std::make_unique<uint64_t[]>(i);" );
			il2cpp_external_cpp.append( "unity_sdk::read_memory( ptr + 0x20, items_array.get(), i * 0x8);" );
			il2cpp_external_cpp.append( "return std::move(items_array);" );
			il2cpp_external_cpp.append( "}\n" );

			il2cpp_external_cpp.append( variable_type + " " + class_name + "::m_Items (uint64_t* items_ptr, size_t i) { " );
			il2cpp_external_cpp.append( "return unity_sdk::read_memory_wrapper<" + variable_type + ">(items_ptr[i]);" );
			il2cpp_external_cpp.append( "}\n" );
		} else {
			str_replace.append( "static std::unique_ptr<" + variable_type + "[]> m_Items(uint64_t ptr, size_t i);" );

			il2cpp_external_cpp.append( "std::unique_ptr<" + variable_type + "[]> " + class_name + "::m_Items (uint64_t ptr, size_t i) { " );
			il2cpp_external_cpp.append( "auto items_array = std::make_unique<" + variable_type + "[]>(i);" );
			il2cpp_external_cpp.append( "unity_sdk::read_memory( ptr + 0x20, items_array.get(), i * sizeof(" + variable_type + "));" );
			il2cpp_external_cpp.append( "return std::move(items_array);" );
			il2cpp_external_cpp.append( "}\n" );
		}

		replaces.push_back( { static_cast<int64_t>( offset ), static_cast<int64_t>( variable.length( ) ) ,str_replace } );

		++it;
	}

	printf( "STAGE 3\n" );

	std::ofstream il2cpp_external_file = std::ofstream( "il2cpp_external.h" );

	auto last_subs_offset = int64_t( 0 );
	for ( auto& it : replaces ) {

		il2cpp_external_file.write( str.substr( last_subs_offset, it.offset - last_subs_offset ).data( ), it.offset - last_subs_offset );
		il2cpp_external_file.write( it.str.data( ), it.str.length( ) );

		last_subs_offset = it.offset + it.len;
	}

	auto left_data = str.substr( last_subs_offset );
	il2cpp_external_file.write( left_data.data( ), left_data.length( ) );

	il2cpp_external_file.close( );
}

//very very slow better option is to modify il2cppdumper
void il2cpp_converter::convert( ) {
	std::ifstream il2cpp = std::ifstream( "il2cpp.h" );

	if ( !il2cpp.good( ) ) {
		il2cpp.close( );
		return;
	}

	std::string str( ( std::istreambuf_iterator<char>( il2cpp ) ),
		std::istreambuf_iterator<char>( ) );

	il2cpp.close( );

	str = str.substr( 0, str.find( "struct MethodInfo_0 {" ) );

	const std::regex pointer_value( "[_a-zA-Z].+\\*.+;" );

	std::vector<replace_string_info> replaces;
	auto il2cpp_external_cpp = std::string( );
	il2cpp_external_cpp.append( "#include \"unity_sdk.hpp\"\n" );

	std::printf( "STAGE 0\n" );

	std::regex_iterator<std::string::iterator> it( str.begin( ), str.end( ), pointer_value );
	std::regex_iterator<std::string::iterator> end;

	auto old_class_name = std::string( );
	while ( it != end ) {
		auto variable = it->str( );
		if ( variable.find( "typedef" ) != 0 ) {
			auto last_pointer = variable.find_last_of( '*' ) + 1;
			auto variable_name = variable.substr( last_pointer, variable.length( ) - last_pointer - 1 );
			ltrim( variable_name );

			auto first_pointer = variable.find_first_of( '*' );
			auto variable_type = variable.substr( 0, first_pointer );
			rtrim( variable_type );

			auto is_string = false;
			auto variable_prefix = std::string( );
			if ( variable_type.find( ' ' ) != std::string::npos ) {
				variable_prefix = variable_type.substr( 0, variable_type.find_first_of( ' ' ) );
				variable_type = variable_type.substr( variable_type.find_first_of( ' ' ) + 1 );
			}

			if ( variable_type == "void" )
				variable_type = "uint64_t";

			if ( variable_type == "char" ) {
				variable_type = "std::string";
				is_string = true;
			}

			auto is_struct = variable_prefix == "struct";

			auto class_name = it->prefix( ).str( );
			auto class_begin = class_name.find_last_of( '{' );
			if ( class_begin != std::string::npos ) {
				class_name = class_name.substr( 0, class_begin );
				rtrim( class_name );
				class_name = class_name.substr( class_name.find_last_of( ' ' ) + 1 );

				if ( class_name.empty( ) )
					class_name = old_class_name;
				else
					old_class_name = class_name;
			} else
				class_name = old_class_name;

			auto skip_function_init = class_name == "Il2CppRGCTXData" || class_name == "union";
			auto is_array = variable_name.find( "[" ) != std::string::npos;

			auto offset = it->position( );
			auto str_replace = "uint64_t " + variable_name + "_ptr;";

			if ( !is_array ) {
				if ( !skip_function_init ) {
					il2cpp_external_cpp.append( variable_type + " " + class_name + "::" + variable_name + " () { " );

					if ( is_string )
						il2cpp_external_cpp.append( "return unity_sdk::read_string(" + variable_name + "_ptr);" );
					else
						il2cpp_external_cpp.append( "return unity_sdk::read_memory_wrapper<" + variable_type + ">(" + variable_name + "_ptr);" );

					il2cpp_external_cpp.append( "}\n" );

					if ( !str_replace.empty( ) )
						str_replace.append( "	" );
					if ( is_struct )
						str_replace.append( "struct " );
					str_replace.append( variable_type + " " + variable_name + "();" );
				}
				replaces.push_back( { static_cast<int64_t>( offset ), static_cast<int64_t>( variable.length( ) ) ,str_replace } );
			}

		}
		++it;
	}

	std::printf( "STAGE 1\n" );

	std::ofstream il2cpp_external_file = std::ofstream( "il2cpp_external.h" );

	auto last_subs_offset = int64_t( 0 );
	for ( auto& it : replaces ) {

		il2cpp_external_file.write( str.substr( last_subs_offset, it.offset - last_subs_offset ).data( ), it.offset - last_subs_offset );
		il2cpp_external_file.write( it.str.data( ), it.str.length( ) );

		last_subs_offset = it.offset + it.len;
	}

	auto left_data = str.substr( last_subs_offset );
	il2cpp_external_file.write( left_data.data( ), left_data.length( ) );

	il2cpp_external_file.close( );

	convert2( il2cpp_external_cpp );

	std::ofstream il2cpp_external_cpp_file = std::ofstream( "il2cpp_external.cpp" );

	il2cpp_external_cpp_file.write( il2cpp_external_cpp.data( ), il2cpp_external_cpp.size( ) );

	il2cpp_external_cpp_file.close( );

	std::printf( "DONE\n" );
}