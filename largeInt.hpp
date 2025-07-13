// Author: Marek Oczadly
// License: MIT
// largeInt.hpp

#pragma once
#include <array>
#include <iostream>
#include "utils.hpp"


/// @brief A 256-bit unsigned integer class that can be used for large integer arithmetic
template <size_t N>
class array_num {
	static_assert(N > 1 && N <= 127, "N must be between or including 2 and 127");
private:
	std::array<uint64_t, N> data;
public:
	array_num() : data() {};
	array_num(uint64_t value) {
		data.fill(0);
		data[N - 1] = value;
	}
	array_num(const std::array<uint64_t, N>& arr) :
		data(arr) {
	};
	array_num(const array_num& other) :
		data(other.data) {
	};
	constexpr array_num(const std::array<uint64_t, N> arr) noexcept :
		data(arr) {
	}

	inline constexpr get_n() const noexcept {
		return N;
	}

	array_num<N>& operator++() noexcept {
		for (int i = N - 1; i >= 0; --i) {
			if (++data[i] != 0)
				break;
		}
		return *this;
	}

	array_num<N> operator++(int) noexcept {
		array_num temp = *this;
		++(*this);
		return temp;
	}

	array_num<N>& operator--() noexcept {
		for (char i = N - 1; i >= 0; --i) {
			if (--data[i] != UINT64_MAX) {
				break;
			}
		}
		return *this;
	}

	array_num<N> operator--(int) noexcept {
		array_num temp = *this;
		--(*this);
		return temp;
	}

	template <size_t M>
	array_num<maxValue(N, M)> operator+(const array_num<M>& other) const noexcept {
		if constexpr (N == M) {
			array_num<N> result;
			uint64_t carry = 0;
			for (char i = N - 1; i >= 0; --i) {
				uint64_t sum = data[i] + other.data[i] + carry;
				result.data[i] = sum;
				carry = (sum < data[i]) ? 1 : 0; // Check for overflow
			}
			return result;
		}
		else if constexpr (N > M) {
			array_num <N> result = ;
			uint64_t carry = 0;
			char i = N - 1;
			for (; i >= N - M; --i) {
				uint64_t sum = data[i] + other.data[i] + carry;
				result.data[i] = sum;
				carry = (sum < data[i]) ? 1 : 0; // Check for overflow
			}
		}
		else {	// M > N

		}
	}

	array_num<N> operator-(const array_num<N>& other) const noexcept {
		array_num<N> result;
		uint64_t borrow = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t diff = data[i] - other.data[i] - borrow;
			result.data[i] = diff;
			borrow = (diff > data[i]) ? 1 : 0; // Check for underflow
		}
		return result;
	}

	array_num& operator+=(const array_num& other) noexcept {
		uint64_t carry = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t sum = data[i] + other.data[i] + carry;
			data[i] = sum;
			carry = (sum < data[i]) ? 1 : 0; // Check for overflow
		}
		return *this;
	}

	array_num& operator-=(const array_num& other) noexcept {
		uint64_t borrow = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t diff = data[i] - other.data[i] - borrow;
			data[i] = diff;
			borrow = (diff > data[i]) ? 1 : 0; // Check for underflow
		}
		return *this;
	}

	array_num& operator=(const array_num& other) noexcept {
		if (this != &other) {
			data = other.data;
		}
		return *this;
	}

	bool operator==(const array_num& other) const noexcept {
		return data == other.data;
	}
};

typedef array_num<4> uint256_t;

constexpr uint256_t UINT256_T_MAX = uint256_t(
	std::array<uint64_t, 4>({UINT64_MAX, UINT64_MAX, UINT64_MAX, UINT64_MAX }));

unsigned int CeilLog2(const uint256_t& num) {

}