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
#include "unity_sdk.hpp"

#include <emmintrin.h>
#include <xmmintrin.h>

unity_sdk::process_memory_handler unity_sdk::read_memory = nullptr;
unity_sdk::process_memory_handler unity_sdk::write_memory = nullptr;

std::string unity_sdk::read_string( uint64_t address ) {
	char buf[ 64 ];
	if ( !read_memory( address, buf, 64 ) )
		return std::string( );
	return std::string( buf );
}

std::wstring unity_sdk::read_mono_string( System_String_o& str, uint64_t ptr ) {
	std::wstring native_str;
	native_str.resize( str.fields.m_stringLength - 1 );

	read_memory( ptr + 0x14, (void*)native_str.data( ), str.fields.m_stringLength * 2 );

	return native_str;
}

//TODO reduce rpm calls
vector unity_sdk::get_position( uint64_t transform_internal ) {
	auto some_ptr = read_memory_wrapper<uint64_t>( transform_internal + 0x38 );
	auto index = read_memory_wrapper<int32_t>( transform_internal + 0x38 + sizeof( uint64_t ) );
	if ( !some_ptr )
		return vector( );

	auto relation_array = read_memory_wrapper<uint64_t>( some_ptr + 0x18 );
	if ( !relation_array )
		return vector( );

	auto dependency_index_array = read_memory_wrapper<uint64_t>( some_ptr + 0x20 );
	if ( !dependency_index_array )
		return vector( );

	__m128i temp_0;
	__m128 xmmword_1410D1340 = { -2.f, 2.f, -2.f, 0.f };
	__m128 xmmword_1410D1350 = { 2.f, -2.f, -2.f, 0.f };
	__m128 xmmword_1410D1360 = { -2.f, -2.f, 2.f, 0.f };
	__m128 temp_1;
	__m128 temp_2;
	auto temp_main = read_memory_wrapper<__m128>( relation_array + index * 48 );
	auto dependency_index = read_memory_wrapper<int32_t>( dependency_index_array + 4 * index );

	while ( dependency_index >= 0 ) {
		auto relation_index = 6 * dependency_index;

		temp_0 = read_memory_wrapper<__m128i>( relation_array + 8 * relation_index + 16 );
		temp_1 = read_memory_wrapper<__m128>( relation_array + 8 * relation_index + 32 );
		temp_2 = read_memory_wrapper<__m128>( relation_array + 8 * relation_index );

		__m128 v10 = _mm_mul_ps( temp_1, temp_main );
		__m128 v11 = _mm_castsi128_ps( _mm_shuffle_epi32( temp_0, 0 ) );
		__m128 v12 = _mm_castsi128_ps( _mm_shuffle_epi32( temp_0, 85 ) );
		__m128 v13 = _mm_castsi128_ps( _mm_shuffle_epi32( temp_0, -114 ) );
		__m128 v14 = _mm_castsi128_ps( _mm_shuffle_epi32( temp_0, -37 ) );
		__m128 v15 = _mm_castsi128_ps( _mm_shuffle_epi32( temp_0, -86 ) );
		__m128 v16 = _mm_castsi128_ps( _mm_shuffle_epi32( temp_0, 113 ) );
		__m128 v17 = _mm_add_ps(
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps( _mm_mul_ps( v11, xmmword_1410D1350 ), v13 ),
							_mm_mul_ps( _mm_mul_ps( v12, xmmword_1410D1360 ), v14 ) ),
						_mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( v10 ), -86 ) ) ),
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps( _mm_mul_ps( v15, xmmword_1410D1360 ), v14 ),
							_mm_mul_ps( _mm_mul_ps( v11, xmmword_1410D1340 ), v16 ) ),
						_mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( v10 ), 85 ) ) ) ),
				_mm_add_ps(
					_mm_mul_ps(
						_mm_sub_ps(
							_mm_mul_ps( _mm_mul_ps( v12, xmmword_1410D1340 ), v16 ),
							_mm_mul_ps( _mm_mul_ps( v15, xmmword_1410D1350 ), v13 ) ),
						_mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( v10 ), 0 ) ) ),
					v10 ) ),
			temp_2 );

		temp_main = v17;
		dependency_index = read_memory_wrapper<int32_t>( dependency_index_array + 4 * dependency_index );
	}

	return *reinterpret_cast<vector*>( &temp_main );
}

//TODO reduce rpm calls
vector unity_sdk::get_forward( uint64_t transform_internal ) {
	auto some_ptr = read_memory_wrapper<uint64_t>( transform_internal + 0x38 );
	auto index = read_memory_wrapper<int32_t>( transform_internal + 0x38 + sizeof( uint64_t ) );
	if ( !some_ptr )
		return vector( );

	auto relation_array = read_memory_wrapper<uint64_t>( some_ptr + 0x18 );
	if ( !relation_array )
		return vector( );

	auto dependency_index_array = read_memory_wrapper<uint64_t>( some_ptr + 0x20 );
	if ( !dependency_index_array )
		return vector( );

	__m128i temp_0;
	__m128i temp_1;
	__m128 temp_2;
	__m128 temp_3;

	__m128 temp_4;
	__m128i temp_5;
	__m128i temp_6;
	__m128 temp_7;

	__m128 xmmword_142E890 = { 0.f, 0.f, 0.f, 0.f };
	__m128 xmmword_142E830 = { -1.f, -1.f, -1.f, 0.f };
	__m128 xmmword_142E8A0 = { 0.f, 0.f, 0.f, FLT_MIN };
	__m128 xmmword_142CC80 = { 1.f, 1.f, 1.f, 1.f };

	auto temp_main = read_memory_wrapper<__m128i>( relation_array + index * 48 + 16 );
	auto dependency_index = read_memory_wrapper<int32_t>( dependency_index_array + 4 * index );

	if ( dependency_index >= 0 ) {
		temp_0 = _mm_load_si128( ( const __m128i* ) & xmmword_142E890 );
		temp_1 = _mm_and_si128( _mm_castps_si128( xmmword_142E830 ), temp_0 );
		temp_2 = _mm_castsi128_ps( _mm_load_si128( ( const __m128i* ) & xmmword_142E8A0 ) );
		temp_3 = _mm_and_ps( _mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( xmmword_142E890 ), 0 ) ), temp_2 );

		do {
			auto relation_index = 6 * dependency_index;
			temp_4 = read_memory_wrapper<__m128>( relation_array + 8 * relation_index + 16 );
			auto temp_external = read_memory_wrapper<__m128i>( relation_array + 8 * relation_index + 32 );
			temp_5 = _mm_loadu_si128( &temp_external );
			dependency_index = read_memory_wrapper <int32_t>( dependency_index_array + 4 * dependency_index );
			temp_6 = _mm_xor_si128( _mm_and_si128( temp_5, temp_0 ), _mm_castps_si128( xmmword_142CC80 ) );
			temp_7 = _mm_castsi128_ps( _mm_xor_si128(
				_mm_and_si128(
					temp_0,
					_mm_castps_si128( _mm_or_ps(
						_mm_andnot_ps(
							temp_2,
							_mm_mul_ps(
								_mm_castsi128_ps( _mm_shuffle_epi32( temp_6, 65 ) ),
								_mm_castsi128_ps( _mm_shuffle_epi32( temp_6, -102 ) ) ) ),
						temp_3 ) ) ),
				temp_main ) );
			temp_main = _mm_xor_si128(
				temp_1,
				_mm_shuffle_epi32(
					_mm_castps_si128( _mm_sub_ps(
						_mm_sub_ps(
							_mm_sub_ps(
								_mm_mul_ps( temp_4, _mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( temp_7 ), -46 ) ) ),
								_mm_castsi128_ps( _mm_shuffle_epi32(
									_mm_castps_si128( _mm_mul_ps(
										temp_4,
										_mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( temp_7 ), -120 ) ) ) ),
									30 ) ) ),
							_mm_castsi128_ps( _mm_shuffle_epi32(
								_mm_castps_si128( _mm_mul_ps(
									_mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( temp_4 ), -81 ) ),
									temp_7 ) ),
								-115 ) ) ),
						_mm_castsi128_ps( _mm_shuffle_epi32(
							_mm_castps_si128( _mm_mul_ps(
								_mm_movelh_ps( temp_4, temp_4 ),
								_mm_castsi128_ps( _mm_shuffle_epi32( _mm_castps_si128( temp_7 ), -11 ) ) ) ),
							99 ) ) ) ),
					-46 ) );
		} while ( dependency_index >= 0 );
	}
	auto quad = *reinterpret_cast<quaternion*>( &temp_main );
	return vector( 0.f, 1.f, 0.f ) * quad;
}