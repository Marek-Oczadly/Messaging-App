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
	x86 = 1 << 0,
	x86_64 = 1 << 1,
	ARM = 1 << 2,
	ARM64 = 1 << 3,
	Unknown = 1 << 4
};

enum class SIMDLevels : unsigned short {
	NONE = 0,			// 0
	SSE2 = 1 << 0,		// 1
	SSE3 = 1 << 1,		// 2
	SSSE3 = 1 << 2,		// 4
	SSE4_1 = 1 << 3,	// 8
	SSE4_2 = 1 << 4,	// 16
	AVX = 1 << 5,		// 32
	AVX2 = 1 << 6,		// 64
	AVX512F = 1 << 7,	// 128
	AVX512DQ = 1 << 8,  // 256
	AVX512BW = 1 << 9,  // 512
	AVX512VL = 1 << 10, // 1024
	NEON = 1 << 11,     // 2048
};

std::string toString(CPUArchitectures arch) {
	switch (arch) {
	case CPUArchitectures::x86: return "x86";
	case CPUArchitectures::x86_64: return "x86-64";
	case CPUArchitectures::ARM: return "ARM";
	case CPUArchitectures::ARM64: return "ARM64";

	default: return "Unknown";
	}
}

std::string toString(SIMDLevels level) {
	switch (level) {
	case SIMDLevels::NONE: return "None";
	case SIMDLevels::SSE2: return "SSE2";
	case SIMDLevels::SSE3: return "SSE3";
	case SIMDLevels::SSSE3: return "SSSE3";
	case SIMDLevels::SSE4_1: return "SSE4.1";
	case SIMDLevels::SSE4_2: return "SSE4.2";
	case SIMDLevels::AVX: return "AVX";
	case SIMDLevels::AVX2: return "AVX2";
	case SIMDLevels::AVX512F: return "AVX-512 Foundation";
	case SIMDLevels::AVX512DQ: return "AVX-512 Doubleword and Quadword";
	case SIMDLevels::AVX512BW: return "AVX-512 Byte and Word";
	case SIMDLevels::AVX512VL: return "AVX-512 Vector Length Extensions";
	case SIMDLevels::NEON: return "NEON";

	default: return "Unknown";
	}
}

class SIMDIntegerSupport {
private:
	const unsigned short supportedSIMD;
	const CPUArchitectures arch;

#if defined (__linux__)		// G++ does not define _XCR_XFEATURE_ENABLED_MASK
	static inline unsigned long long getXCR0() {
		unsigned int eax, edx;
		__asm__ volatile (
			"xgetbx" : "=a"(eax), "=d"(edx) : "c"(0)
		);
		return ((uint64_t)edx << 32) | eax;
	}

#endif


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

	 static CPUArchitectures getCPUArchitecture() noexcept {
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

	static unsigned short getSIMDSupport() {
		unsigned short supportedSIMD = 0; // Initialize to 0, no SIMD support
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
#		elif ((defined(__linux__) && defined(__GNUG__)) || (defined(__apple__) && defined(__clang__)))	// Linux or MacOS with G++ or Clang compiler respectively
		&& (defined(__x86_64__) || defined(__i386__))	// x86 or x86_64 architecture
			// Effectively the same as MSVC however using a different header and inline asm
#			include <cpuid.h>	// G++ exclusive header
			unsigned int eax, ebx, ecx, edx, maxLeaf;
			unsigned long long xcrFeatureMask = getXCR0();
			if (!__get_cpuid(0, &maxLeaft, &ebx, &ecx, &edx)) { // leaf 0
				return 0; // No CPUID support so no SIMD can be assumed
			}
			if (maxLeaf >= 1) {
				__get_cpuid(1, &eax, &ebx, &ecx, &edx);		// Leaf 1

				// SSE support
				setBit(supportedSIMD, 0, getBit(edx, 26)); // SSE2 support
				setBit(supportedSIMD, 1, getBit(ecx, 0)); // SSE3 support
				setBit(supportedSIMD, 2, getBit(ecx, 9)); // SSSE3 support
				setBit(supportedSIMD, 3, getBit(ecx, 19)); // SSE4.1 support
				setBit(supportedSIMD, 4, getBit(ecx, 20)); // SSE4.2 support

				// AVX
				setBit(supportedSIMD, 5, getBit(ecx, 27) && getBit(ecx, 28)
					&& ((xcrFeatureMask & 0x6) == 0x6)); // AVX support (requires OS support)

				if (maxLeaf >= 7 && getBit(supportedSIMD, 5)) { // Checking further AVX support (requires AVX support)
					__get_cpuid(7, 0, &eax, &ebx, &ecx, &edx); // Leaf 7, subleaf 0

					setBit(supportedSIMD, 6, getBit(ebx, 5)); // AVX2 Support

					if ((xcrFeatureMask & 0xE6) == 0xE6) {	// Check if AVX512 is supported
						setBit(supportedSIMD, 7, getBit(ebx, 16)); // AVX512F Support
						setBit(supportedSIMD, 8, getBit(ebx, 17)); // AVX512DQ Support
						setBit(supportedSIMD, 9, getBit(ebx, 30)); // AVX512BW Support
						setBit(supportedSIMD, 10, getBit(ebx, 31)); // AVX512VL Support
					}
			}
			
#		elif defined(__linux__) && defined(__aarch64__)	// Linux on ARM64
#			include <sys/auxv.h>
# 			include <asm/hwcap.h>
			setBit(supportedSIMD, 11, getauxval(AT_HWCAP) & HWCAP_ASIMD); // NEON support not guaranteed due to different distros
#		elif defined(__apple__) && defined(__aarch64__)	// ARM64 or Apple silicon
			setBit(supportedSIMD, 11, true); // NEON support on Apple platforms
#		endif
			return supportedSIMD;
	}

public:
	SIMDIntegerSupport() : supportedSIMD(getSIMDSupport()),
						   arch(getCPUArchitecture()) {}

	void displaySupport() {
		std::cout << "CPU Architecture: " << toString(getCPUArchitecture()) << NEWL;
		std::cout << "SIMD Support: " << NEWL;
		std::cout << "SSE2:" << TAB << TAB << (getBit(supportedSIMD, 0) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE3:" << TAB << TAB << (getBit(supportedSIMD, 1) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSSE3:" << TAB << TAB << (getBit(supportedSIMD, 2) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE4.1: " << TAB << (getBit(supportedSIMD, 3) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "SSE4.2: " << TAB << (getBit(supportedSIMD, 4) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX:" << TAB << TAB << (getBit(supportedSIMD, 5) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX2:" << TAB << TAB << (getBit(supportedSIMD, 6) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512F: " << TAB << (getBit(supportedSIMD, 7) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512DQ: " << TAB << (getBit(supportedSIMD, 8) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512BW: " << TAB << (getBit(supportedSIMD, 9) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "AVX512VL: " << TAB << (getBit(supportedSIMD, 10) ? "Enabled " : "Disabled") << NEWL;
		std::cout << "NEON: " << TAB << TAB << (getBit(supportedSIMD, 11) ? "Enabled " : "Disabled") << NEWL;
	}

	void getMaximumSIMDLevel();
};