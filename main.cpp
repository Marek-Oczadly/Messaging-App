// Author: Marek Oczadly
// License: MIT
// main.cpp


/*
Compilation information: 
> Windows - MSVC
> Linux - G++ (Arch WSL)
> MacOS - Clang
*/


#include "simd-detection.hpp"

int main() {
	SIMDIntegerSupport simdSupport;
	simdSupport.displaySupport();

	return 0;
}