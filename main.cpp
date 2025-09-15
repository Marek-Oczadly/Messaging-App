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
    std::cout << "Alignment of std::array<uint8_t, 8>: "
        << alignof(std::array<uint8_t, 8>) << "\n";

    std::cout << "Alignment of std::array<uint64_t, 1>: "
        << alignof(std::array<uint64_t, 1>) << "\n";

}