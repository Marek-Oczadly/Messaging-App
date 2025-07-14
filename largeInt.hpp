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
	template <size_t M>
	friend class array_num;

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

	inline constexpr size_t get_n() const noexcept {
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
	array_num<maxValue<N, M>> operator+(const array_num<M>& other) const noexcept {
		if constexpr (N == M) {
			array_num<N> result;
			uint64_t carry = 0;
			unrollReverseInclusive<N - 1, 0>([](char i) {	// for (char i = N - 1; i >= 0; --i) {
				uint64_t sum = data[i] + other.data[i] + carry;
				result.data[i] = sum;
				carry = (sum < data[i]) ? 1 : 0; // Check for overflow
				});
			return result;
		}
		else if constexpr (N > M) {
			array_num <N> result;
			uint64_t carry = 0;
			unrollReverseInclusive<N - 1, N - M>([&, other](char i) {	// for (char i = N - 1; i >= N - M; --i) {
				uint64_t sum = data[i] + other.data[i] + carry;
				result.data[i] = sum;
				carry = (sum < data[i]) ? 1 : 0; // Check for overflow
			});
			result.data[N - M - 1] = other.data[N - M - 1] + carry; // Handle the remaining bits of the larger number
			unrollReverseInclusive<N - M - 2, 0>([&](char i) {
				carry = (result.data[i] == UINT64_MAX && carry) ? 1 : 0;
				result.data[i] = data[i] + carry;
				}
			);
		}
		else {
			array_num<M> result;
			uint64_t carry = 0;
			unrollReverseInclusive<M - 1, M - N>([&](char i) {	// for (char i = M - 1; i >= M - N; --i) {
				uint64_t sum = data[i] + other.data[i] + carry;
				result.data[i] = sum;
				carry = (sum < data[i]) ? 1 : 0; // Check for overflow
			});
			result.data[M - N - 1] = data[M - N - 1] + carry; // Handle the remaining bits of the larger number
			unrollReverseInclusive<M - N - 2, 0>([&](char i) {
				carry = (result.data[i] == UINT64_MAX && carry) ? 1 : 0;
				result.data[i] = other.data[i] + carry;
			});

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

	template <size_t M>
	operator array_num<M>() const noexcept {
		if constexpr (N == M) {	// No change
			return *this;
		}
		else if constexpr (N > M) {
			array_num<M> result;
			uint64_t* pos = &result.data[0];
			unroll<N - M, N>([&](char i) {
				*pos = data[i];
				++pos;
			});
			return result;
		}
		else {
			array_num<M> result;
			uint64_t* pos = &data[0];
			unroll<M - N>([&](char i) {
				result[i] = 0;
			});
			unroll<M - N, M>([&](char i) {
				result[i] = *pos;
				++pos;
			});
			return result;
		}
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

	friend std::ostream& operator<<(std::ostream& os, const array_num<N>& num) {
		os << "0x";
		for (int i = N - 1; i >= 0; --i) {
			os << std::hex << num.data[i];
		}
		return os;
	}
};

typedef array_num<4> uint256_t;
typedef array_num<8> uint512_t;
typedef array_num<16> uint1024_t;