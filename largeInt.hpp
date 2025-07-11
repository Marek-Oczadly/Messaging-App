// Author: Marek Oczadly
// License: MIT
// largeInt.hpp

#pragma once
#include <array>
#include "simdDetection.hpp"

#if defined (_MSC_VER)	// MSVC exclusive header
	#include <immintrin.h>
#endif


/// @brief A 256-bit unsigned integer class that can be used for large integer arithmetic. Requires AVX2
class uint256_t;


/// @brief A 256-bit unsigned integer class that can be used for large integer arithmetic. Does not leverage SIMD
class uint256_t_naive {
private:
	std::array<uint64_t, 4> data;
public:
	uint256_t_naive() : data() {};
	uint256_t_naive(uint64_t value) {
		data.fill(0);
		data[3] = value;
	}
	uint256_t_naive(const std::array<uint64_t, 4>& arr) : 
		data(arr) {};
	uint256_t_naive(const uint256_t_naive& other) :
		data(other.data) {};

	uint256_t_naive& operator++() noexcept {
		for (int i = 3; i >= 0; --i) {
			if (++data[i] != 0)
				break;
		}
		return *this;
	}
	uint256_t_naive operator+(const uint256_t_naive& other) noexcept {
		
	}

};


unsigned int CeilLog2(const uint256_t& num) {

}