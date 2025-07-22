// Author: Marek Oczadly
// License: MIT
// main.cpp


/*
Compilation information: 
> Windows - MSVC
> Linux - G++ (Arch WSL)
> MacOS - Clang
*/


#include <iostream>
#include <chrono>
#include "largeInt.hpp"

int main()
{
	uint256_t a = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
	uint256_t b = { 0, 0, 0, 1 };

	std::cout << a + b << std::endl;
}