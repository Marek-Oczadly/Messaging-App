// Author : Marek Oczadly
// License : MIT
// simdDetection.hpp

#pragma once
#include <iostream>
#include <array>

constexpr const char NEWL = '\n';
constexpr const char TAB = '\t';
typedef unsigned char uint8_t;


class SIMDIntegerSupport {
private:
	unsigned short supportedSIMD;

	/// @brief Check if a bit is 1
	inline bool getBit(int value, uint8_t bit) {
		return (value & (1U << bit)) != 0;
	}

	/// @brief Set a bit to 1
	inline void setBit(int& value, uint8_t bit) {
		value |= (1U << bit);
	}

	/// @brief Clear a bit (set to 0)
	inline void clearBit(int& value, uint8_t bit) {
		value &= ~(1U << bit);
	}

public:
	void displaySupport() {
		std::cout << "SIMD Support: " << NEWL;
		std::cout << "SSE2: " << (getBit(supportedSIMD, 0) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE3: " << (getBit(supportedSIMD, 1) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSSE3: " << (getBit(supportedSIMD, 2) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE4.1: " << (getBit(supportedSIMD, 3) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE4.2: " << (getBit(supportedSIMD, 4) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX: " << (getBit(supportedSIMD, 5) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX2: " << (getBit(supportedSIMD, 6) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512F: " << (getBit(supportedSIMD, 7) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512DQ: " << (getBit(supportedSIMD, 8) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512BW: " << (getBit(supportedSIMD, 9) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512VL: " << (getBit(supportedSIMD, 10) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "NEON: " << (getBit(supportedSIMD, 11) ? "Enabled " : "Disabled") << NEWL;
	}
};

//#ifdef _MSC_VER		// MSVC is required for intrin.h - not C++ standard
//#include <intrin.h>		// Required for __cpuid()
//
//SIMDSUPPORT detectSIMDSupport() {
//	int cpuInfo[4];		// EAX, EBX, ECX, EDX
//#if defined(_M_IX86) || defined(_M_X64)
//	__cpuidex(cpuInfo, 7, 0);
//	if (getBit(cpuInfo[1], 31)) return SIMDSUPPORT::AVX512VL;
//	if (getBit(cpuInfo[1], 17)) return SIMDSUPPORT::AVX512DQ;
//	if (getBit(cpuInfo[1], 30)) return SIMDSUPPORT::AVX512BW;
//	if (getBit(cpuInfo))
//#endif
//}
//
//const SIMDSUPPORT simd = detectSIMDSupport();
//#else	// not implementing non-msvc functionality but required for compatibility
//const SIMDSUPPORT simd = SIMDSUPPORT::NONE;
//#endif