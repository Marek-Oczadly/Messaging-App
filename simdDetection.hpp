// Author : Marek Oczadly
// License : MIT
// simdDetection.hpp

#pragma once

enum class SIMDSUPPORT : char {
	NONE,
	NEON,
	SVE,
	RISCV,
	SSE,
	SSE3,
	AVX2,
	AVX512
};

#ifdef _MSC_VER
#include <intrin.h>

SIMDSUPPORT detectSIMDSupport() {
#ifdef _M_ARM64		// All ARM64 devices support NEON
	return SIMDSUPPORT::NEON;
#else

#endif
}


#else 
SIMDSUPPORT simd = SIMDSUPPORT::NONE;
#endif