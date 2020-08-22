#include <iostream>

#define CONVERT 0

#if CONVERT
#include "il2cpp_converter.hpp"
#endif

#include "cheat_manager.hpp"

int main( ) {
#if CONVERT
	il2cpp_converter::convert( );
#endif

	cheat_manager::start( );
}