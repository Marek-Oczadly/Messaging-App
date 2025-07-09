
/*
Compilation information: 
> Windows - MSVC
> Linux - G++ (Arch WSL)
> MacOS - Clang
*/

#include <iostream>
#include "simdDetection.hpp"

int main()
{
	SIMDIntegerSupport simdSupport;
	simdSupport.displaySupport();
}