// Author: Marek Oczadly
// License: MIT
// utils.hpp

#pragma once
#include <cstdint>
#include <bit>

#if defined(_MSC_VER)
	#include <intrin.h>
#endif

constexpr const char NEWL = '\n';
constexpr const char TAB = '\t';

template <size_t N, size_t M>
constexpr size_t maxValue = (N > M) ? N : M;

/// @brief 
/// @tparam T 
/// @tparam N 
/// @param func Lambda function to be run 
template<short N, typename T>
inline void unroll(T&& func) {
	static_assert(N >= 0 && N < 1000, "N must be non-negative and less than 1000");
	if constexpr (N > 0) {
		unroll<N - 1>(std::forward<T>(func));
		func(N - 1);
	}
}

template<short N, typename T>
inline void unrollReverse(T&& func) {
	static_assert(N >= 0 && N < 1000, "N must be non-negative and less than 1000");
	if constexpr (N > 0) {
		func(N);
		unrollReverse<N - 1>(std::forward<T>(func));
	}
}

template <short N, short M, typename T>
inline void unroll(T&& func) {
	static_assert(N >= M && N < 1000, "N must be greater than or equal to M and less than 1000");
	if constexpr (N > M) {
		unroll<N - 1, M>(std::forward<T>(func));
		func(N - 1);
	}
}

template <short N, short M, typename T>
inline void unrollReverse(T&& func) {
	if constexpr (N > M) {
		func(N);
		unrollReverse<N - 1, M>(std::forward<T>(func));
	}
}

template <short N, short M = 0, typename T> 
inline void unrollReverseInclusive(T&& func) {
	if constexpr (N >= M) {
		func(N);
		unrollReverseInclusive<N - 1, M>(std::forward<T>(func));
	}
}

/// @brief Check if a bit is 1
inline bool getBit(unsigned int value, const uint8_t bit) noexcept {
	return (value & (1U << bit)) != 0;
}

/// @brief Check if a bit is 1
inline bool getBit(int value, const uint8_t bit) noexcept {
	return (value & (1U << bit)) != 0;
}

/// @brief Check if a bit is 1
inline bool getBit(unsigned short value, const uint8_t bit) noexcept {
	return (value & (1U << bit)) != 0;
}

/// @brief Set a bit to 1
inline void setBit(int& value, const uint8_t bit) noexcept {
	value |= (1U << bit);
}

/// @brief Set a bit to 1
inline void setBit(unsigned int& value, const uint8_t bit) noexcept {
	value |= (1U << bit);
}

/// @brief Set a bit to 1
inline void setBit(unsigned short& value, const uint8_t bit) noexcept {
	value |= (1U << bit);
}

/// @brief Clear a bit (set to 0)
inline void clearBit(int& value, const uint8_t bit) noexcept {
	value &= ~(1U << bit);
}

/// @brief Clear a bit (set to 0)
inline void clearBit(unsigned short& value, const uint8_t bit) noexcept {
	value &= ~(1U << bit);
}


/// @brief Clear a bit (set to 0)
inline void clearBit(unsigned int& value, const uint8_t bit) noexcept {
	value &= ~(1U << bit);
}

/// @brief Set a bit to 0 or 1 based on the state
inline void setBit(int& value, const uint8_t bit, const bool state) noexcept {
	if (state) setBit(value, bit);
	else clearBit(value, bit);
}

/// @brief Set a bit to 0 or 1 based on the state
inline void setBit(unsigned short& value, const uint8_t bit, const bool state) noexcept {
	if (state) setBit(value, bit);
	else clearBit(value, bit);
}

/// @brief Set a bit to 0 or 1 based on the state
inline void setBit(unsigned int& value, const uint8_t bit, const bool state) noexcept {
	if (state) setBit(value, bit);
	else clearBit(value, bit);
}


unsigned char floorLog2(const uint8_t num) noexcept {
	if (num == 0) return -1; // Log2(0) is undefined, return 255 for safety
	return std::bit_width(num) - 1;
}

unsigned char floorLog2(const uint16_t num) noexcept {
	if (num == 0) return -1; // Log2(0) is undefined, return 255 for safety
	return std::bit_width(num) - 1;
}
unsigned char floorLog2(const uint32_t num) noexcept {
	if (num == 0) return -1; // Log2(0) is undefined, return 255 for safety
	return std::bit_width(num) - 1;
}

unsigned char floorLog2(const uint64_t num) noexcept {
	if (num == 0) return -1; // Log2(0) is undefined, return 255 for safety
	return std::bit_width(num) - 1;
}

inline void addWithOverflow(uint64_t& a, const uint64_t b, uint64_t& carry) noexcept {
#if defined(_MSC_VER)
	carry = _addcarry_u64(carry, a, b, &a);	// Fastest - uses MSVC intrinsic
#elif defined(__GNUC__) || defined(__clang__)
	uint64_t temp;
	bool c1 = __builtin_add_overflow(a, b, &temp);	// Uses GCC/Clang intrinsic
	bool c2 = __builtin_add_overflow(temp, carry, &a);
	carry = static_cast<uint64_t>(c1 || c2);
#else	// Manual arithmetic if compiler can't be determined
	uint64_t temp = a + b;
	carry = (temp < a) ? 1 : 0;
	a = temp + carry;
	carry |= (a < temp) ? 1 : 0;
#endif
}

/// @brief adds two 64-bit integers and a carry. Optimised for MSVC and G++/Clang
/// @param a First number being added
/// @param b Second number being added
/// @param sum A reference to where the sum is being stored
/// @param carry 
inline void addWithOverflow(const uint64_t a, const uint64_t b, uint64_t& sum, uint64_t& carry) noexcept {
#if defined(_MSC_VER)	// MSVC - fastest
	carry = _addcarry_u64(carry, a, b, &sum);
#elif defined(__GNUC__) || defined(__clang__)
	bool c1 = __builtin_add_overflow(a, b, &sum);	// Uses GCC/Clang intrinsic
	bool c2 = __builtin_add_overflow(sum, carry, &sum);
	carry = static_cast<uint8_t>(c1 || c2);
#else	// Manual arithmetic if compiler can't be determined
	uint64_t temp = a + b;
	bool c1 = (temp < a);	// Check for overflow
	sum = temp + carry;
	carry = static_cast<uint64_t>(c1 || (sum < temp));	// Check if carry occurred
#endif
}