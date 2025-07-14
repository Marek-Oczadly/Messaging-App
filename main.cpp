// Author: Marek Oczadly
// License: MIT
// main.cpp


/*
Compilation information: 
> Windows - MSVC
> Linux - G++ (Arch WSL)
> MacOS - Clang
*/

//#include "utils.hpp"


#include "largeInt.hpp"
#include <iostream>	

int main()
{
	unroll<10>([](int i) {
		std::cout << "Unrolled iteration: " << i << std::endl;
		});

    return 0;
}