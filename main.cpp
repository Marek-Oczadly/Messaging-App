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
#include "utils.hpp"

int main()
{
	unroll<3, 10>([](char i) {
		std::cout << static_cast<int>(i) << NEWL;
		});
    return 0;
}