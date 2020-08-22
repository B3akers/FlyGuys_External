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
#include "memory.hpp"


#include <cstdio>
#include <psapi.h>

HANDLE memory::handle = INVALID_HANDLE_VALUE;

void memory::attach( uint32_t pid ) {

	if ( handle != INVALID_HANDLE_VALUE )
		return;

	handle = OpenProcess( PROCESS_ALL_ACCESS, false, pid );
}

void memory::detach( ) {
	CloseHandle( handle );
	handle = INVALID_HANDLE_VALUE;
}

bool memory::is_active( ) {
	return handle != INVALID_HANDLE_VALUE && GetProcessId( handle ) != 0;
}

bool memory::read_memory( uint64_t address, void* buffer, size_t size ) {
	return ReadProcessMemory( handle, reinterpret_cast<LPCVOID>( address ), buffer, size, nullptr );
}

bool memory::write_memory( uint64_t address, void* buffer, size_t size ) {
	return WriteProcessMemory( handle, reinterpret_cast<LPVOID>( address ), buffer, size, nullptr );
}

uint64_t memory::get_module( const char* name ) {
	HMODULE mods[ 1024 ];
	DWORD cb_needed;

	if ( EnumProcessModules( handle, mods, sizeof( mods ), &cb_needed ) ) {
		for ( auto i = 0; i < ( cb_needed / sizeof( HMODULE ) ); i++ ) {
			TCHAR szModName[ MAX_PATH ];
			if ( GetModuleBaseNameA( handle, mods[ i ], szModName,
				sizeof( szModName ) / sizeof( TCHAR ) ) ) {

				if ( strcmp( szModName, name ) == 0 )
					return reinterpret_cast<uint64_t>( mods[ i ] );
			}
		}
	}

	return 0;
}