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
#pragma once
#include <cinttypes>
#include <string>
#include <memory>
#include "vector.h"

#undef s_addr
#undef stdout
#undef stderr
#undef stdin

#define save_class(c,p) unity_sdk::write_memory(p,&c,sizeof(c));

namespace unity_sdk {
	typedef bool( *process_memory_handler )( uint64_t address, void* buffer, size_t size );

	extern process_memory_handler read_memory;
	extern process_memory_handler write_memory;

	template <typename t>
	t read_memory_wrapper( uint64_t address ) {
		t ret;
		if ( !read_memory( address, &ret, sizeof( t ) ) )
			memset( (void*)&ret, 0x00, sizeof( t ) );
		return ret;
	}

	std::string read_string( uint64_t address );
	vector get_position( uint64_t transform_internal );
	vector get_forward( uint64_t transform_internal );
}

#include "il2cpp_external.h"

namespace unity_sdk {
	std::wstring read_mono_string( System_String_o& str, uint64_t ptr );
}