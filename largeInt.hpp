// Author: Marek Oczadly
// License: MIT
// largeInt.hpp

#pragma once
#include <array>
#include <iostream>
#include <initializer_list>
#include <string>
#include <sstream>
#include <stdexcept>
#include "utils.hpp"
#include "math-intrinsics.hpp"
#include "bitwise-functions.hpp"


/// @brief A 256-bit unsigned integer class that can be used for large integer arithmetic
template <uint8_t N>
class uint_array {
	static_assert(N > 1 && N < 128, "N must be between or including 2 and 127");
	/**
	 *	======================= REPRESENTATION =======================
	 * Size N: 1 < N < 128 (due to char limits and would be inefficient)
	 * { x_0,    x_1,    x_2,    x_3,    ...,   x_(N-1) }
	 *   ^ least significant (2^(64*0))         ^ most significant (2^(64*(N-1)))
	**/
private:
	std::array<uint64_t, N> data;

	//static uint8_t inline 

	template <uint8_t M>
	inline uint_array<maxValue(N, M)> naiveMultiply(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result = 0;
			uint64_t carry;
			for (char i = 0; i < N; ++i) {
				carry = 0;
				for (char j = 0; j < N - i - 1; ++j) {
					multiply64x64<true>(data[i], other.data[j], carry, result[i + j], result[i + j + 1]);
				}
				uint64_t temp;	// Dummy variable to fill an argument
				multiply64x64<false>(data[i], other.data[N - i - 1], carry, result[N - 1], temp);
			}
			return result;
		}
		else if constexpr (N > M) {
			uint_array<N> result = 0;
			uint64_t carry;
			for (char i = 0; i < M; ++i) {
				carry = 0;
				for (char j = 0; j < N - i - 1; ++j) {
					multiply64x64<true>(other.data[j], data[i], carry, result[i + j], result[i + j + 1]);
				}
				uint64_t temp;	// Dummy variable to fill an argument
				multiply64x64<false>(other.data[i], data[N - i - 1], carry, result[N - 1], temp);
			}
			return result;
		}
		else {	// M > N
			uint_array<M> result = 0;
			uint64_t carry;
			for (char i = 0; i < N; ++i) {
				carry = 0;
				for (char j = 0; j < M - i - 1; ++j) {
					multiply64x64<true>(data[i], other.data[j], carry, result[i + j], result[i + j + 1]);
				}
				uint64_t temp;	// Dummy variable to fill an argument
				multiply64x64<false>(data[i], other.data[M - i - 1], carry, result[M - 1], temp);
			}
			return result;
		}
	}

	// Base = 2^64
	template <uint8_t M>
	inline uint_array<maxValue(N, M)> karatsubaMultiply(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result;
			
		}
		else if constexpr (N > M) {
			// TODO
		}
		else {
			// TODO
		}
	}

	inline Arr64<UINT64_BCD_ARRAY_SIZE(N)> BCD() const noexcept {
		return binaryToBCD(reverseArray(data));
	}


public:
	// Allows the use of private members within templated methods
	template <uint8_t M>
	friend class uint_array;

	template <uint8_t M>
	uint_array(const char(&s)[M]) {
		// Each character is 4 bits / 0.5 bytes
		constexpr auto BCD_ARR_LENGTH = CEIL((4.0 * M) / 64.0); // CBA to simplify its 7am

		if (!isNumeric(s)) {
			throw std::invalid_argument("Input string must only contain digits.");
		}

		Arr64<BCD_ARR_LENGTH> bcdArray = { 0 };
		for (uint8_t i = M - 2; i >= 0; --i) {	// -2 to skip null terminator

		}
	}

	uint_array() noexcept = default;

	uint_array(const uint64_t value) : data() {
		data[0] = value;
	}

	uint_array(const std::array<uint64_t, N>& arr) noexcept : data(arr) {};

	uint_array(const std::initializer_list<uint64_t>& list) : data() {
		if (list.size() > N) {
			throw std::out_of_range("Initializer list size exceeds array size");
		}
		uint64_t* idx = &data[static_cast<char>(list.size()) - 1];
		for (const uint64_t val : list) {
			*idx = val;
			--idx;
		}
	}

	template <uint8_t M>
	uint_array(const uint_array<M>& other) noexcept : data() {
		if constexpr (M == N) {
			data = other.data;	// Direct copy if sizes match
		}
		else {
			loopUnroll(minValue(N, M))	// for (char i = 0; i < M; ++i) {
				data[i] = other.data[i];	// Copy only the first M elements
			endLoop
		}
	}

	constexpr uint_array(const std::array<uint64_t, N> arr) : data(arr) {}

	inline constexpr char size() const noexcept {
		return N;
	}
	const uint64_t& operator[](const char index) const {
		if (index >= N || index < 0) {
			throw std::out_of_range("Index out of range");
		}
		return data[index];
	}

	uint64_t& operator[](const char index) {
		if (index >= N || index < 0) {
			throw std::out_of_range("Index out of range");
		}
		return data[index];
	}

	uint_array<N>& operator++() noexcept {
		loopUnroll(N)
			if (++data[i] != 0) { 
				return *this;
			}
		endLoop
	}

	uint_array<N> operator++(int) noexcept {
		uint_array<N> temp = *this;
		++(*this);
		return temp;
	}

	uint_array<N>& operator--() noexcept {
		loopUnroll(N)
			if (--data[i] != UINT64_MAX) {
				return *this;
			}
		endLoop
	}

	uint_array<N> operator--(int) noexcept {
		uint_array temp = *this;
		--(*this);
		return temp;
	}

	template <uint8_t M>
	uint_array<maxValue(N, M)> operator+(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result;
			unsigned char carry = 0;
			loopUnroll(N)
				addWithOverflow(other.data[i], data[i], result.data[i], carry);	// Uses the addWithOverflow function to handle overflow
			endLoop
			return result;
		}
		else if constexpr(N > M) {
			uint_array<N> result;
			unsigned char carry = 0;
			loopUnroll(M)
				addWithOverflow(other.data[i], data[i], result.data[i], carry);
			endLoop
			loopUnrollFrom(M, N)
				result.data[i] = data[i] + carry;
				carry = (data[i] == UINT64_MAX && carry) ? 1 : 0;
			endLoop
			return result;
		}
		else {
			uint_array<M> result;
			unsigned char carry = 0;
			loopUnroll(N)
				addWithOverflow(other.data[i], data[i], result.data[i], carry);
			endLoop
			loopUnrollFrom(N, M)
				result.data[i] = other.data[i] + carry;
				carry = (other.data[i] == UINT64_MAX && carry) ? 1 : 0;
			endLoop
			return result;
		}
	}

	uint_array<N>& operator+=(const uint64_t other) noexcept {
		data[0] += other;
		bool carry = data[0] < other;	// Check if carry occurred
		char i = 0;
		while (carry && ++i < N) {
			carry = (++data[i] == 0);
		}
		return *this;
	}

	uint_array<N> operator+(const uint64_t other) const noexcept {
		uint_array<N> result = *this;
		return result += other;
	}

	uint_array<N>& operator-=(const uint64_t other) noexcept {
		data[0] -= other;
		bool borrow = data[0] > (~other);
		char i = 0;
		while (borrow && ++i < N) {
			borrow = (--data[i] == UINT64_MAX);
		}
		return *this;
	}

	uint_array<N> operator-(const uint64_t other) const noexcept {
		uint_array<N> result = *this;
		return result -= other;
	}

	template <uint8_t M>
	uint_array<maxValue(M, N)> operator-(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result;
			unsigned char borrow = 0;
			loopUnroll(N)
				subtractWithBorrow(data[i], other.data[i], result.data[i], borrow);
			endLoop
			return result;
		}
		else if constexpr (N > M) {
			uint_array<N> result;
			unsigned char borrow = 0;
			loopUnroll(M)
				subtractWithBorrow(data[i], other.data[i], result.data[i], borrow);
			endLoop
			loopUnrollFrom(M, N)
				result.data[i] = data[i] - borrow;
				borrow = (borrow && data[i] == 0) ? 1 : 0; // Check for underflow
			endLoop
			return result;
		}
		else {
			uint_array<M> result;
			unsigned char borrow = 0;
			loopUnroll(N)
				subtractWithBorrow(data[i], other.data[i], result.data[i], borrow);
			endLoop
			loopUnrollFrom(N, M)
				result.data[i] = (borrow) ? ~other.data[i] : negate_uint64(other.data[i]);
				borrow = (borrow || result.data[i] > 0) ? 1 : 0;
			endLoop
			return result;
		}
		
	}

	template <uint8_t M>
	uint_array<N>& operator+=(const uint_array<M>& other) noexcept {
		if constexpr (M == N) {
			unsigned char carry = 0;
			loopUnroll(N)
				addWithOverflow(data[i], other.data[i], carry);
			endLoop
			return *this;
		}
		else if constexpr (N > M) {
			unsigned char carry = 0;
			uint64_t temp;
			loopUnroll(M)
				addWithOverflow(data[i], other.data[i], carry);
			endLoop
			loopUnrollFrom(M, N)
				temp = data[i];
				data[i] += carry;
				carry = (temp == UINT64_MAX && carry) ? 1 : 0; // Check for overflow
			endLoop
			return *this;
		}
		else {
			unsigned char carry = 0;
			loopUnroll(N)
				addWithOverflow(data[i], other.data[i], carry);
			endLoop
			return *this;
		}
	}

	template <uint8_t M>
	uint_array<N>& operator-=(const uint_array<M>& other) noexcept {
		if constexpr (N == M) {
			unsigned char borrow = 0;
			loopUnroll(N)
				subtractWithBorrow(data[i], other.data[i], borrow);
			endLoop
			return *this;
		}
		else if constexpr (N > M) {
			unsigned char borrow = 0;
			uint64_t temp;
			loopUnroll(M)
				subtractWithBorrow(data[i], other.data[i], borrow);
			endLoop
			loopUnrollFrom(M, N)
				temp = data[i];
				data[i] -= borrow;
				borrow = (borrow && temp == 0) ? 1 : 0; // Check for underflow
			endLoop
		}
		else {
			unsigned char borrow = 0;
			loopUnroll(N)
				subtractWithBorrow(data[i], other.data[i], borrow);
			endLoop
			return *this;
		}
	}

	template <uint8_t M>
	operator uint_array<M>() const noexcept {
		if constexpr (N == M) {	// No change
			return *this;
		}
		else if constexpr (N > M) {
			uint_array<M> r;
			loopUnroll(M)
				r.data[i] = data[i];	// Copy only the first M elements
			endLoop
			return r;
		}
		else {
			uint_array<M> r;
			loopUnroll(N)
				r.data[i] = data[i];	// Copy only the first N elements
			endLoop
			loopUnrollFrom(N, M)
				r.data[i] = 0;	// Fill the rest with zeros
			endLoop
			return r;
		}
	}

	uint_array<N>& operator=(const uint64_t other) noexcept {
		data.fill(0);
		data[0] = other;
		return *this;
	}

	template <uint8_t M>
	uint_array<N>& operator=(const uint_array<M>& other) noexcept {
		if constexpr (N == M) {
			data = other.data;
			return *this;
		}
		else if constexpr (N > M) {
			loopUnroll(M)
				data[i] = other.data[i];
			endLoop
			loopUnrollFrom(M, N)
				data[i] = 0;	// Fill the rest with zeros
			endLoop
			return *this;
		}
		else {
			loopUnroll(N)
				data[i] = other.data[i];
			endLoop
			return *this;
		}
	}

	template <uint8_t M>
	bool operator==(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			return data == other.data;
		}
		else if constexpr (N > M) {
			loopUnroll(M)
				if (data[i] != other.data[i]) return false;
			endLoop
			loopUnrollFrom(M, N)
				if (data[i] != 0) return false;
			endLoop
			return true;
		}
		else {
			loopUnroll(N)
				if (data[i] != other.data[i]) return false;
			endLoop
			loopUnrollFrom(N, M)
				if (other.data[i] != 0) return false;
			endLoop
			return true;
		}
	}

	template <char M>
	bool operator !=(const uint_array<M>& other) const noexcept {
		return not(this->operator==(other));
	}

	uint_array<N> operator<<(const uint16_t places) {
		return leftShift(data, places);
	}

	uint_array<N>& operator<<=(const uint16_t places) {
		leftShiftInPlace(data, places);
		return *this;
	}

	std::wstring toWString() const noexcept {
		constexpr auto numBytes = UINT64_BCD_ARRAY_SIZE(N) * 8;
		auto bcdArray = BCD();
		std::wstringstream ss;
		constexpr auto arrSize = sizeof(bcdArray) / sizeof(bcdArray[0]);
		for (int i = 0; i < arrSize; ++i) {
			ss << std::hex << bcdArray[i];
		}
		return ss.str();
	}

	std::string toString() const noexcept {
		constexpr auto numBytes = UINT64_BCD_ARRAY_SIZE(N) * 8;
		auto bcdArray = BCD();
		std::stringstream ss;
		constexpr auto arrSize = sizeof(bcdArray) / sizeof(bcdArray[0]);
		for (int i = 0; i < arrSize; ++i) {
			ss << std::hex << bcdArray[i];
		}
		return ss.str();
	}

	friend std::ostream& operator<<(std::ostream& os, const uint_array<N>& num) {
		constexpr auto numBytes = UINT64_BCD_ARRAY_SIZE(N) * 8;
		auto bcdArray = num.BCD();
		constexpr auto arrSize = sizeof(bcdArray) / sizeof(bcdArray[0]);
		for (int i = 0; i < arrSize; ++i) {
			os << std::hex << bcdArray[i];
		}
		return os;
	}
};


typedef uint_array<2> uint128_t;
typedef uint_array<4> uint256_t;
typedef uint_array<8> uint512_t;
typedef uint_array<16> uint1024_t;