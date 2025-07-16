// Author: Marek Oczadly
// License: MIT
// largeInt.hpp

#pragma once
#include <array>
#include <ostream>
#include <iostream>
#include <initializer_list>
#include "utils.hpp"

// TODO: Switch the direction of significant bits so it is more cache efficient. Abstract in inputs and outputs

/// @brief A 256-bit unsigned integer class that can be used for large integer arithmetic
template <char N>
class uint_array {
	static_assert(N > 1 && N < 128, "N must be between or including 2 and 127");
	/*
	*	======================= REPRESENTATION =======================
	* Size N: 1 < N < 128 (due to char limits and would be inefficient)
	* { x_0,    x_1,    x_2,    x_3,    ...,   x_(N-1) }
	* ^ least significant (2^(64*0))         ^ most significant (2^(64*(N-1)))
	*/
private:
	std::array<uint64_t, N> data;
public:
	// Allows the use of private members within templated methods
	template <char M>
	friend class uint_array;

	uint_array() noexcept = default;
	uint_array(uint64_t value) : data() {
		data[0] = value;
	}
	uint_array(const std::array<uint64_t, N>& arr) noexcept : data(arr) {};

	uint_array(const std::initializer_list<uint64_t>& list) : data() {
		if (list.size() > N) {
			throw std::out_of_range("Initializer list size exceeds array size");
		}
		auto it = list.begin();
		for (int i = 0; i < list.size(); ++i) {
			data[i] = *it;
			++it;
		}
	}

	template <char M>
	uint_array(const uint_array<M>& other) noexcept : data() {
		if constexpr (M == N) {
			data = other.data;	// Direct copy if sizes match
		}
		else {
			unroll<minValue<N, M>>([&](char i) {	// for (char i = 0; i < M; ++i) {
				data[i] = other.data[i];	// Copy only the first M elements
			});
		}
	}

	constexpr uint_array(const std::array<uint64_t, N> arr) : data(arr) {}

	inline constexpr char size() const noexcept {
		return N;
	}
	uint64_t operator[](unsigned char index) const {
		if (index >= N) {
			throw std::out_of_range("Index out of range");
		}
		return data[index];
	}

	uint_array<N>& operator++() noexcept {
		unroll<N>([&](char i) {
			if (++data[i] != 0) { 
				return *this;
			}
		});
	}

	uint_array<N> operator++(int) noexcept {
		uint_array<N> temp = *this;
		++(*this);
		return temp;
	}

	uint_array<N>& operator--() noexcept {
		unroll<N>([&](char i) {
			if (--data[i] != UINT64_MAX) {
				return *this;
			}
		});
	}

	uint_array<N> operator--(int) noexcept {
		uint_array temp = *this;
		--(*this);
		return temp;
	}

	template <char M>
	uint_array<maxValue<N, M>> operator+(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result;
			unsigned char carry = 0;
			unroll<N>([&](char i) {	// for (char i = 0; i < N; ++i) {
				addWithOverflow(other.data[i], data[i], result.data[i], carry);	// Uses the addWithOverflow function to handle overflow
			});
			return result;
		}
		else if constexpr(N > M) {
			uint_array<N> result;
			unsigned char carry = 0;
			unroll<M>([&](char i) {
				addWithOverflow(other.data[i], data[i], result.data[i], carry);
			});
			unroll<M + 1, N>([&](char i) {
				result.data[i] = data[i] + carry;
				carry = (data[i] == UINT64_MAX && carry) ? 1 : 0;
			});
			return result;
		}
		else {
			uint_array<M> result;
			unsigned char carry = 0;
			unroll<N>([&](char i) {
				addWithOverflow(other.data[i], data[i], result.data[i], carry);
			});
			unroll<N + 1, M>([&](char i) {
				result.data[i] = other.data[i] + carry;
				carry = (other.data[i] == UINT64_MAX && carry) ? 1 : 0;
			});
			return result;
		}
	}

	template <char M>
	uint_array<maxValue<M, N>> operator-(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result;
			unsigned char borrow = 0;
			unroll<N>([&](char i) {
				subtractWithBorrow(data[i], other.data[i], result.data[i], borrow);
			});
			return result;
		}
		else if constexpr (N > M) {
			uint_array<N> result;
			unsigned char borrow = 0;
			unroll<M>([&](char i) {
				subtractWithBorrow(data[i], other.data[i], result.data[i], borrow);
			});
			unroll<M + 1, N>([&](char i) {
				result.data[i] = data[i] - borrow;
				borrow = (borrow && data[i] == 0) ? 1 : 0; // Check for underflow
			});
			return result;
		}
		else {
			uint_array<M> result;
			unsigned char borrow = 0;
			unroll<N>([&](char i) {
				subtractWithBorrow(data[i], other.data[i], result.data[i], borrow);
			});
			unroll<N + 1, M>([&](char i) {
				result.data[i] = ~other.data[i];
			});
			return result;
		}
		
	}

	template <char M>
	operator uint_array<M>() const noexcept {
		if constexpr (N == M) {	// No change
			return *this;
		}
		else if constexpr (N > M) {
			uint_array<M> result;
			uint64_t* pos = &result.data[0];
			unroll<N - M, N>([&](char i) {
				*pos = data[i];
				++pos;
			});
			return result;
		}
		else {
			uint_array<M> result;
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

	template <char M>
	uint_array<N>& operator+=(const uint_array<M>& other) noexcept {
		if constexpr (M == N) {
			uint64_t carry = 0;
			unrollReverseInclusive<N - 1, 0>([&](char i) {	// for (char i = N - 1; i >= 0; --i) {
				uint64_t sum = data[i] + other.data[i] + carry;
				data[i] = sum;
				carry = (sum < data[i]) ? 1 : 0; // Check for overflow
			});
		}
		else if constexpr (M < N) {
			uint64_t carry = 0;
			unrollReverseInclusive<N - 1, N - M>([&](char i) {

				});
		}

		return *this;
	}

	uint_array& operator-=(const uint_array& other) noexcept {
		uint64_t borrow = 0;
		for (char i = 3; i >= 0; --i) {
			uint64_t diff = data[i] - other.data[i] - borrow;
			data[i] = diff;
			borrow = (diff > data[i]) ? 1 : 0; // Check for underflow
		}
		return *this;
	}

	uint_array& operator=(const uint_array& other) noexcept {
		if (this != &other) {
			data = other.data;
		}
		return *this;
	}

	uint_array& operator=(const uint64_t other) noexcept {
		data.fill(0);
		data[0] = other;
		return *this;
	}

	bool operator==(const uint_array& other) const noexcept {
		return data == other.data;
	}

	friend std::ostream& operator<<(std::ostream& os, const uint_array<N>& num) {
		// Temporary - will print in base 10 soon
		for (int i = N - 1; i >= 0; --i) {
			os << num.data[i] << ' ';
		}
		return os;
	}
};

typedef uint_array<4> uint256_t;
typedef uint_array<8> uint512_t;
typedef uint_array<16> uint1024_t;