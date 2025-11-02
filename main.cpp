// Author: Marek Oczadly
// License: MIT
// main.cpp


/*
Compilation information: 
> Windows - MSVC
> Linux - G++ (Arch WSL)
> MacOS - Clang
*/


#include "largeInt.hpp"
#include <iostream>

int main() {
	uint512_t a = 12345678901234567890ULL;

	std::cout << "a: " << a << NEWL;

	return 0;
}