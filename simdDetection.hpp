// Author : Marek Oczadly
// License : MIT
// simdDetection.hpp

#pragma once
#include <iostream>
#include <string>
#include <array>
#include "utils.hpp"

#if ((defined(__linux__) && defined(__GNUG__)) || (defined(__apple__) && defined(__clang__))) && (defined(__x86_64__) || defined(__i386__))	// G++ or Clang for X86 or X86_64
#	include <cpuid.h>	// G++/clang exclusive header
#endif

# if defined(__linux__) && defined(__aarch64__)	// Linux on ARM64
#	include <sys/auxv.h>
# 	include <asm/hwcap.h>
# endif

#if defined(_WIN32) && defined(_MSC_VER) && (defined(_M_X64) || defined(_M_IX86))	// Windows for X86 or X86_64 in MSVC
#	include <intrin.h>	// MSVC exclusive header for SIMD detection
#endif

enum class CPUArchitectures : unsigned char {
	Unknown = 0,		// 0
	x86 = 1 << 0,		// 1
	x86_64 = 1 << 1,	// 2
	ARM = 1 << 2,		// 4
	ARM64 = 1 << 3,		// 8
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

#if defined (__linux__) || defined(__apple__)		// G++ and Clang do not define _XCR_XFEATURE_ENABLED_MASK. Same for linux and Apple platforms
	static inline unsigned long long getXCR0() {
		unsigned int eax, edx;
		__asm__ volatile (
			"xgetbv" : "=a"(eax), "=d"(edx) : "c"(0)
		);
		return ((unsigned long long)edx << 32) | eax;
	}
#endif

	 static inline CPUArchitectures findCPUArchitecture() noexcept {
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

			// Had to make the below line long because g++ was being a bitch and not letting me split the statement
#		elif ((defined(__linux__) && defined(__GNUG__)) || (defined(__apple__) && defined(__clang__))) && (defined(__x86_64__) || defined(__i386__))	// x86 or x86_64 architecture
			// Effectively the same as MSVC however using a different header and inline asm
			unsigned int eax, ebx, ecx, edx, maxLeaf;
			unsigned long long xcrFeatureMask = getXCR0();
			if (not __get_cpuid(0, &maxLeaf, &ebx, &ecx, &edx)) { // leaf 0
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
					__get_cpuid_count(7, 0, &eax, &ebx, &ecx, &edx); // Leaf 7, subleaf 0

					setBit(supportedSIMD, 6, getBit(ebx, 5)); // AVX2 Support

					if ((xcrFeatureMask & 0xE6) == 0xE6) {	// Check if AVX512 is supported
						setBit(supportedSIMD, 7, getBit(ebx, 16)); // AVX512F Support
						setBit(supportedSIMD, 8, getBit(ebx, 17)); // AVX512DQ Support
						setBit(supportedSIMD, 9, getBit(ebx, 30)); // AVX512BW Support
						setBit(supportedSIMD, 10, getBit(ebx, 31)); // AVX512VL Support
					}
				}
			}
#		elif defined(__linux__) && defined(__aarch64__)	// Linux on ARM64
			setBit(supportedSIMD, 11, getauxval(AT_HWCAP) & HWCAP_ASIMD); // NEON support not guaranteed due to different distros
#		elif defined(__apple__) && defined(__aarch64__)	// ARM64 or Apple silicon
			setBit(supportedSIMD, 11, true); // NEON support on Apple platforms
#		endif
			return supportedSIMD;
	}

public:
	SIMDIntegerSupport() : supportedSIMD(getSIMDSupport()),
						   arch(findCPUArchitecture()) {}

	void displaySupport() const noexcept {
		std::cout << "CPU Architecture: " << toString(arch) << NEWL;
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

	SIMDLevels getMaximumSIMDLevel() const noexcept {
		unsigned char power = floorLog2(supportedSIMD);
		return static_cast<SIMDLevels>(
			(power <= 11) ? (1 << power) : 0);
	}

	CPUArchitectures CPUArchitecture() const noexcept {
		return arch;
	}

	bool SSE2() const noexcept {
		return getBit(supportedSIMD, 0);
	}
	bool SSE3() const noexcept {
		return getBit(supportedSIMD, 1);
	}
	bool SSSE3() const noexcept {
		return getBit(supportedSIMD, 2);
	}
	bool SSE4_1() const noexcept {
		return getBit(supportedSIMD, 3);
	}
	bool SSE4_2() const noexcept {
		return getBit(supportedSIMD, 4);
	}
	bool AVX() const noexcept {
		return getBit(supportedSIMD, 5);
	}
	bool AVX2() const noexcept {
		return getBit(supportedSIMD, 6);
	}
	bool AVX512F() const noexcept {
		return getBit(supportedSIMD, 7);
	}
	bool AVX512DQ() const noexcept {
		return getBit(supportedSIMD, 8);
	}
	bool AVX512BW() const noexcept {
		return getBit(supportedSIMD, 9);
	}
	bool AVX512VL() const noexcept {
		return getBit(supportedSIMD, 10);
	}
	bool NEON() const noexcept {
		return getBit(supportedSIMD, 11);
	}
};

#ifndef SIMD_INTEGER_SUPPORT
#define SIMD_INTEGER_SUPPORT
extern SIMDIntegerSupport simdSupport;	// Global instance of SIMDIntegerSupport
#endif