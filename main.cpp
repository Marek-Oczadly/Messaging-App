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
#include <bitset>
#include <sstream>
#include "largeInt.hpp"

int main() {
	AlignedUInt8Array<2> test = { .data64 = {0x0000000000000001, 0x0000000000000002} };
	const uint8_t places = 68;
	AlignedUInt8Array<2> expected = { .data64 = {0x0000000000000020, 0x0000000000000000} };
	auto result = leftShift(test, places);

	if (expected == result) {
		std::cout << "Correct\n";
	}
	else {
		std::cout << "Incorrect\n";
	}

	std::wcout << "TEST:  \t" << byteArrayToBinaryString(test) << std::endl;
	std::wcout << "Result:\t" << byteArrayToBinaryString(result) << std::endl;
	std::wcout << "Expect:\t" << byteArrayToBinaryString(expected) << std::endl;

	return 0;
}