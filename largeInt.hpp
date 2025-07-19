// Author: Marek Oczadly
// License: MIT
// largeInt.hpp

#pragma once
#include <array>
#include <ostream>
#include <initializer_list>
#include "utils.hpp"

/// @brief A 256-bit unsigned integer class that can be used for large integer arithmetic
template <char N>
class uint_array {
	static_assert(N > 1 && N < 128, "N must be between or including 2 and 127");
	/*
	*	======================= REPRESENTATION =======================
	* Size N: 1 < N < 128 (due to char limits and would be inefficient)
	* { x_0,    x_1,    x_2,    x_3,    ...,   x_(N-1) }
	*   ^ least significant (2^(64*0))         ^ most significant (2^(64*(N-1)))
	*/
private:
	std::array<uint64_t, N> data;

	template <char M>
	inline uint_array<maxValue<N, M>> naiveMultiply(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result;
			uint64_t carry;
			for (char i = 0; i < N; ++i) {
				carry = 0;
				for (char j = 0; j < N - i - 1; ++j) {
					multiply64x64(data[i], other.data[j], carry, result[i + j], result[i + j + 1]);
				}
				uint64_t temp;	// Dummy variable to fill an argument
				multiply64x64<false>(data[i], other.data[N - i - 1], carry, result[N - 1], temp);
			}
			return result;
		}
		else if constexpr (N > M) {
			uint_array<N> result;
			uint64_t carry;
			for (char i = 0; i < M; ++i) {
				carry = 0;
				for (char j = 0; j < N - i - 1; ++j) {
					multiply64x64(other.data[j], data[i], carry, result[i + j], result[i + j + 1]);
				}
				uint64_t temp;	// Dummy variable to fill an argument
				multiply64x64<false>(other.data[i], data[N - i - 1], carry, result[N - 1], temp);
			}
			return result;
		}
		else {	// M > N
			uint_array<M> result;
			uint64_t carry;
			for (char i = 0; i < N; ++i) {
				carry = 0;
				for (char j = 0; j < M - i - 1; ++i) {
					multiply64x64(data[i], other.data[j], carry, result[i + j], result[i + j + 1]);
				}
				uint64_t temp;	// Dummy variable to fill an argument
				multiply64x64<false>(data[i], other.data[M - i - 1], carry, result[M - 1], temp);
			}
			return result;
		}
	}

	template <char M>
	inline uint_array<maxValue<N, M>> karatsubaMultiply(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			uint_array<N> result;
			
		}
		else if constexpr (N > M) {

		}
		else {

		}
	}

public:
	// Allows the use of private members within templated methods
	template <char M>
	friend class uint_array;

	uint_array() noexcept {};
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
			unroll<M, N>([&](char i) {
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
			unroll<N, M>([&](char i) {
				result.data[i] = other.data[i] + carry;
				carry = (other.data[i] == UINT64_MAX && carry) ? 1 : 0;
			});
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
			unroll<M, N>([&](char i) {
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
			unroll<N, M>([&](char i) {
				result.data[i] = ~other.data[i];
			});
			return result;
		}
		
	}

	template <char M>
	uint_array<N>& operator+=(const uint_array<M>& other) noexcept {
		if constexpr (M == N) {
			unsigned char carry = 0;
			unroll<N>([&](char i) {	// for (char i = N - 1; i >= 0; --i) {
				addWithOverflow(data[i], other.data[i], carry);
			});
			return *this;
		}
		else if constexpr (N > M) {
			unsigned char carry = 0;
			uint64_t temp;
			unroll<M>([&](char i) {
				addWithOverflow(data[i], other.data[i], carry);
			});
			unroll<M, N>([&](char i) {
				temp = data[i];
				data[i] += carry;
				carry = (temp == UINT64_MAX && carry) ? 1 : 0; // Check for overflow
			});
			return *this;
		}
		else {
			unsigned char carry = 0;
			unroll<N>([&](char i) {
				addWithOverflow(data[i], other.data[i], carry);
			});
			return *this;
		}
	}

	template <char M>
	uint_array<N>& operator-=(const uint_array<M>& other) noexcept {
		if constexpr (N == M) {
			unsigned char borrow = 0;
			unroll<N>([&](char i) {
				subtractWithBorrow(data[i], other.data[i], borrow);
			});
			return *this;
		}
		else if constexpr (N > M) {
			unsigned char borrow = 0;
			uint64_t temp;
			unroll<M>([&](char i) {
				subtractWithBorrow(data[i], other.data[i], borrow);
			});
			unroll<M, N>([&](char i) {
				temp = data[i];
				data[i] -= borrow;
				borrow = (borrow && temp == 0) ? 1 : 0; // Check for underflow
			});
		}
		else {
			unsigned char borrow = 0;
			unroll<N>([&](char i) {
				subtractWithBorrow(data[i], other.data[i], borrow);
			});
			return *this;
		}
	}

	template <char M>
	operator uint_array<M>() const noexcept {
		if constexpr (N == M) {	// No change
			return *this;
		}
		else if constexpr (N > M) {
			uint_array<M> r;
			unroll<M>([&](char i) {	// for (char i = 0; i < M; ++i) {
				r.data[i] = data[i];	// Copy only the first M elements
				});
			return r;
		}
		else {
			uint_array<M> r;
			unroll<N>([&](char i) {	// for (char i = 0; i < N; ++i) {
				r.data[i] = data[i];	// Copy only the first N elements
				});
			unroll<N, M>([&](char i) {	// for (char i = N; i < M; ++i) {
				r.data[i] = 0;	// Fill the rest with zeros
				});
			return r;
		}
	}

	uint_array<N>& operator=(const uint64_t other) noexcept {
		data.fill(0);
		data[0] = other;
		return *this;
	}

	template <char M>
	uint_array<N>& operator=(const uint_array<M>& other) noexcept {
		if constexpr (N == M) {
			data = other.data;
			return *this;
		}
		else if constexpr (N > M) {
			unroll<M>([&](char i) {
				data[i] = other.data[i];
			});
			unroll<M, N>([&](char i) {
				data[i] = 0;	// Fill the rest with zeros
			});
			return *this;
		}
		else {
			unroll<N>([&](char i) {
				data[i] = other.data[i];
			});
			return *this;
		}
	}

	template <char M>
	bool operator==(const uint_array<M>& other) const noexcept {
		if constexpr (N == M) {
			return data == other.data;
		}
		else if constexpr (N > M) {
			unroll<M>([&](char i) {
				if (data[i] != other.data[i]) return false;
			});
			unroll<M, N>([&](char i) {
				if (data[i] != 0) return false;
			});
			return true;
		}
		else {
			unroll<N>([&](char i) {
				if (data[i] != other.data[i]) return false;
			});
			unroll<N, M>([&](char i) {
				if (other.data[i] != 0) return false;
			});
			return true;
		}
	}

	template <char M>
	bool operator !=(const uint_array<M>& other) const noexcept {
		return not(this->operator==(other));
	}

	friend std::ostream& operator<<(std::ostream& os, const uint_array<N>& num) {
		// Temporary - will print in base 10 soon
		for (int i = N - 1; i >= 0; --i) {
			os << num.data[i] << ' ';
		}
		return os;
	}
};


typedef uint_array<2> uint128_t;
typedef uint_array<4> uint256_t;
typedef uint_array<8> uint512_t;
typedef uint_array<16> uint1024_t;