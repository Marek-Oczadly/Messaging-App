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
#include <chrono>
#include <iostream>	

int main()
{
	uint256_t num1{ UINT64_MAX, 64 };
	uint512_t num2{ 64, 64, 64, 64 };

	auto result = num1 + num2;
	
	std::cout << "num1: " << num1 << std::endl;
	std::cout << "num2: " << num2 << std::endl;
	std::cout << "result: " << result << std::endl;
    return 0;
}