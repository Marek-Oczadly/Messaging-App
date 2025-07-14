// Author: Marek Oczadly
// License: MIT
// utils.hpp

#pragma once
#include <cstdint>
#include <bit>

constexpr const char NEWL = '\n';
constexpr const char TAB = '\t';

template <size_t N, size_t M>
constexpr size_t maxValue = (N > M) ? N : M;




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
inline bool getBit(unsigned int value, uint8_t bit) noexcept {
	return (value & (1U << bit)) != 0;
}

/// @brief Check if a bit is 1
inline bool getBit(int value, uint8_t bit) noexcept {
	return (value & (1U << bit)) != 0;
}

/// @brief Check if a bit is 1
inline bool getBit(unsigned short value, uint8_t bit) noexcept {
	return (value & (1U << bit)) != 0;
}

/// @brief Set a bit to 1
inline void setBit(int& value, uint8_t bit) noexcept {
	value |= (1U << bit);
}

/// @brief Set a bit to 1
inline void setBit(unsigned int& value, uint8_t bit) noexcept {
	value |= (1U << bit);
}

/// @brief Set a bit to 1
inline void setBit(unsigned short& value, uint8_t bit) noexcept {
	value |= (1U << bit);
}

/// @brief Clear a bit (set to 0)
inline void clearBit(int& value, uint8_t bit) noexcept {
	value &= ~(1U << bit);
}

/// @brief Clear a bit (set to 0)
inline void clearBit(unsigned short& value, uint8_t bit) noexcept {
	value &= ~(1U << bit);
}


/// @brief Clear a bit (set to 0)
inline void clearBit(unsigned int& value, uint8_t bit) noexcept {
	value &= ~(1U << bit);
}

/// @brief Set a bit to 0 or 1 based on the state
inline void setBit(int& value, uint8_t bit, bool state) noexcept {
	if (state) setBit(value, bit);
	else clearBit(value, bit);
}

/// @brief Set a bit to 0 or 1 based on the state
inline void setBit(unsigned short& value, uint8_t bit, bool state) noexcept {
	if (state) setBit(value, bit);
	else clearBit(value, bit);
}

/// @brief Set a bit to 0 or 1 based on the state
inline void setBit(unsigned int& value, uint8_t bit, bool state) noexcept {
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