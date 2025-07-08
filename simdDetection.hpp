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

	/// @brief Check if a bit is 1
	inline bool getBit(unsigned short value, uint8_t bit) {
		return (value & (1U << bit)) != 0;
	}

	/// @brief Set a bit to 1
	inline void setBit(int& value, uint8_t bit) {
		value |= (1U << bit);
	}

	/// @brief Set a bit to 1
	inline void setBit(unsigned short& value, uint8_t bit) {
		value |= (1U << bit);
	}

	/// @brief Clear a bit (set to 0)
	inline void clearBit(int& value, uint8_t bit) {
		value &= ~(1U << bit);
	}

	/// @brief Clear a bit (set to 0)
	inline void clearBit(unsigned short& value, uint8_t bit) {
		value &= ~(1U << bit);
	}

	/// @brief Set a bit to 0 or 1 based on the state
	inline void setBit(int& value, uint8_t bit, bool state) {
		if (state) setBit(value, bit);
		else clearBit(value, bit);
	}

	/// @brief Set a bit to 0 or 1 based on the state
	inline void setBit(unsigned short& value, uint8_t bit, bool state) {
		if (state) setBit(value, bit);
		else clearBit(value, bit);
	}

	void getSIMDSupport() {
		supportedSIMD = 0; // Initialize to 0, no SIMD support
#		if defined(_WIN32) && defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))	// Windows for X86 or X86_64 in MSVC
#			include <intrin.h>	// MSVC exclusive header for SIMD detection
			int cpuInfo[4]; // EAX, EBX, ECX, EDX
			__cpuid(cpuInfo, 1);
			setBit(supportedSIMD, 0, getBit(cpuInfo[3], 26)); // SSE2 support
			setBit(supportedSIMD, 1, getBit(cpuInfo[2], 0 )); // SSE3 support
			setBit(supportedSIMD, 2, getBit(cpuInfo[2], 9 )); // SSSE3 support
			setBit(supportedSIMD, 3, getBit(cpuInfo[2], 19)); // SSE4.1 support
#		elif defined(_WIN32) && defined(__aarch64__)	// Windows on ARM64 - NEON guaranteed
			setBit(supportedSIMD, 11, true); // NEON support on Windows ARM64
#		elif defined(__linux__) && (defined(__x86_64__) || defined(__i386__))	// Linux on x86 or x86_64
			{

			}
#		elif defined(__linux__) && defined(__aarch64__)	// Linux on ARM64
#			include <sys/auxv.h>
# 			include <asm/hwcap.h>
			setBit(supportedSIMD, 11, getauxval(AT_HWCAP) & HWCAP_ASIMD); // NEON support not guaranteed due to different distros
#		elif defined(__apple__) && defined(__aarch64__)	// ARM64 or Apple silicon
			setBit(supportedSIMD, 11, true); // NEON support on Apple platforms
#		endif
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