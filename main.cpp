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
#include "largeInt.hpp"

int main()
{
    uint512_t a = {UINT64_MAX, 1, 2};
    uint256_t b = { 1, UINT64_MAX, 3, 4 };

    std::cout << a << std::endl;
    std::cout << b << std::endl;

    auto res = a - b;

    std::cout << res << std::endl;
    return 0;
}