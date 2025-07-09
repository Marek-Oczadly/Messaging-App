// Messaging App.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


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