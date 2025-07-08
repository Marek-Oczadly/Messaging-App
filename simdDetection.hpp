// Author : Marek Oczadly
// License : MIT
// simdDetection.hpp

#pragma once
#include <iostream>
#include <string>
#include <array>

constexpr const char NEWL = '\n';
constexpr const char TAB = '\t';
typedef unsigned char uint8_t;

enum class CPUArchitectures : unsigned char {
	x86,
	x86_64,
	ARM,
	ARM64,
	Unknown
};

class SIMDIntegerSupport {
private:
	unsigned short supportedSIMD;

	/// @brief Check if a bit is 1
	inline static bool getBit(int value, uint8_t bit) noexcept {
		return (value & (1U << bit)) != 0;
	}

	/// @brief Check if a bit is 1
	inline static bool getBit(unsigned short value, uint8_t bit) noexcept {
		return (value & (1U << bit)) != 0;
	}

	/// @brief Set a bit to 1
	inline static void setBit(int& value, uint8_t bit) noexcept {
		value |= (1U << bit);
	}

	/// @brief Set a bit to 1
	inline static void setBit(unsigned short& value, uint8_t bit) noexcept {
		value |= (1U << bit);
	}

	/// @brief Clear a bit (set to 0)
	inline static void clearBit(int& value, uint8_t bit) noexcept {
		value &= ~(1U << bit);
	}

	/// @brief Clear a bit (set to 0)
	inline static void clearBit(unsigned short& value, uint8_t bit) noexcept {
		value &= ~(1U << bit);
	}

	/// @brief Set a bit to 0 or 1 based on the state
	inline static void setBit(int& value, uint8_t bit, bool state) noexcept {
		if (state) setBit(value, bit);
		else clearBit(value, bit);
	}

	/// @brief Set a bit to 0 or 1 based on the state
	inline static void setBit(unsigned short& value, uint8_t bit, bool state) noexcept {
		if (state) setBit(value, bit);
		else clearBit(value, bit);
	}

	 CPUArchitectures getCPUArchitecture() {
#		 if defined(__x86_64__) || defined(_M_X64)	// x86_64
			 return CPUArchitectures::x86_64;
#		 elif defined(__i386__) || defined(_M_IX86)	// x86
			 return CPUArchitectures::x86;
#		 elif defined(__aarch64__) || defined(_M_ARM64)	// ARM64
			 return CPUArchitectures::ARM64;
#		 elif defined(__arm__) && defined(_M_ARM)	// ARM
			 return CPUArchitectures::ARM;
#		 else
			return CPUArchitectures::Unknown; // Unknown architecture
#		 endif
	 }

	void getSIMDSupport() {
		supportedSIMD = 0; // Initialize to 0, no SIMD support
#		if defined(_WIN32) && defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))	// Windows for X86 or X86_64 in MSVC (check readme)
#			include <intrin.h>	// MSVC exclusive header for SIMD detection
			int cpuInfo[4]; // EAX, EBX, ECX, EDX
			__cpuid(cpuInfo, 1);
			unsigned long long xcrFeatureMask = _xgetbv(_XCR_XFEATURE_ENABLED_MASK);
			if (cpuInfo[1] >= 1) {
				// Checking SSE support
				setBit(supportedSIMD, 0, getBit(cpuInfo[3], 26)); // SSE2 support
				setBit(supportedSIMD, 1, getBit(cpuInfo[2], 0)); // SSE3 support
				setBit(supportedSIMD, 2, getBit(cpuInfo[2], 9)); // SSSE3 support
				setBit(supportedSIMD, 3, getBit(cpuInfo[2], 19)); // SSE4.1 support
				setBit(supportedSIMD, 4, getBit(cpuInfo[2], 20)); // SSE4.2 support

				setBit(supportedSIMD, 5, getBit(cpuInfo[2], 27) && getBit(cpuInfo[2], 28)
					&& ((xcrFeatureMask & 0x6) == 0x6)); // AVX support (requires OS support)
			}
			if (cpuInfo[1] >= 7 && getBit(supportedSIMD, 5)) { // Checking further AVX support (requires AVX support)
				__cpuidex(cpuInfo, 7, 0);	// { EAX, EBX, ECX, EDX }	Leaf 7, Subleaf 0

				setBit(supportedSIMD, 6, getBit(cpuInfo[1], 5)); // AVX2 Support

				if ((xcrFeatureMask & 0xE6) == 0xE6) {
					setBit(supportedSIMD, 7, getBit(cpuInfo[1], 16)); // AVX512F Support
					setBit(supportedSIMD, 8, getBit(cpuInfo[1], 17)); // AVX512DQ Support
					setBit(supportedSIMD, 9, getBit(cpuInfo[1], 30)); // AVX512BW Support
					setBit(supportedSIMD, 10, getBit(cpuInfo[1], 31)); // AVX512VL Support
				}
			}
#		elif defined(_WIN32) && defined(__aarch64__)	// Windows on ARM64 - NEON guaranteed
			setBit(supportedSIMD, 11, true); // NEON support on Windows ARM64
#		elif defined(__linux__) && (defined(__x86_64__) || defined(__i386__))	// Linux on x86 or x86_64 (check readme for compiler info)
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
		std::cout << "SSE2: " << TAB << (getBit(supportedSIMD, 0) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE3: " << TAB << (getBit(supportedSIMD, 1) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSSE3: " << TAB << (getBit(supportedSIMD, 2) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE4.1: " << TAB << (getBit(supportedSIMD, 3) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE4.2: " << TAB << (getBit(supportedSIMD, 4) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX: " << TAB << (getBit(supportedSIMD, 5) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX2:	" << TAB << (getBit(supportedSIMD, 6) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512 Foundation: " << (getBit(supportedSIMD, 7) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512 Double word and Quad word: " << (getBit(supportedSIMD, 8) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512 Byte and word: " << (getBit(supportedSIMD, 9) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512 Vector length extensions: " << (getBit(supportedSIMD, 10) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "NEON: " << TAB << (getBit(supportedSIMD, 11) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX10" << TAB << (getBit(supportedSIMD, 12) ? "Enabled " : "Disabled") << NEWL;
	}
};