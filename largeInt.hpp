// Author: Marek Oczadly
// License: MIT
// largeInt.hpp

#pragma once
#include <array>
#include <iostream>
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
		data(arr) {
	};
	uint256_t_naive(const uint256_t_naive& other) :
		data(other.data) {
	};

	uint256_t_naive& operator++() noexcept {
		for (int i = 3; i >= 0; --i) {
			if (++data[i] != 0)
				break;
		}
		return *this;
	}
	uint256_t_naive operator+(const uint256_t_naive& other) const noexcept {
		uint256_t_naive result;
		uint64_t carry = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t sum = data[i] + other.data[i] + carry;
			result.data[i] = sum;
			carry = (sum < data[i]) ? 1 : 0; // Check for overflow
		}
		return result;
	}
	uint256_t_naive operator-(const uint256_t_naive& other) const noexcept {
		uint256_t_naive result;
		uint64_t borrow = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t diff = data[i] - other.data[i] - borrow;
			result.data[i] = diff;
			borrow = (diff > data[i]) ? 1 : 0; // Check for underflow
		}
		return result;
	}

	uint256_t_naive& operator+=(const uint256_t_naive& other) noexcept {
		uint64_t carry = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t sum = data[i] + other.data[i] + carry;
			data[i] = sum;
			carry = (sum < data[i]) ? 1 : 0; // Check for overflow
		}
		return *this;
	}

	uint256_t_naive& operator-=(const uint256_t_naive& other) noexcept {
		uint64_t borrow = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t diff = data[i] - other.data[i] - borrow;
			data[i] = diff;
			borrow = (diff > data[i]) ? 1 : 0; // Check for underflow
		}
		return *this;
	}

	uint256_t_naive& operator=(const uint256_t_naive& other) noexcept {
		if (this != &other) {
			data = other.data;
		}
		return *this;
	}

	bool operator==(const uint256_t_naive& other) const noexcept {
		return data == other.data;
	}
};


unsigned int CeilLog2(const uint256_t& num) {

}