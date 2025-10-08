// Author: Marek Oczadly
// License: MIT
// utils.hpp

#pragma once
#include <cstdint>
#include <bit>
#include <bitset>
#include <array>

#include "masks.hpp"

#if defined(_MSC_VER)
	#include <intrin.h>
#endif

constexpr const char NEWL = '\n';
constexpr const char TAB = '\t';

constexpr size_t maxValue(size_t N, size_t M) {
	return (N > M) ? N : M;
}

constexpr size_t minValue(size_t N, size_t M) {
	return (N < M) ? N : M;
}

/// @brief Unrolls a loop at compile time
/// @tparam T The type of argument to the function
/// @tparam N How many iterations - starts at 0 ends at N - 1
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
	static_assert(N < 1000 && M < 1000, "N and M must be less than 1000");
	if constexpr (M > N) {
		unroll<N, M - 1>(std::forward<T>(func));
		func(M - 1);
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

#if defined(_DEBUG)
#define loopUnroll(N) for (uint32_t i = 0; i < N; ++i) {
#define loopUnrollFrom(M, N) for (uint32_t i = M; i < N; ++i) {

#define endLoop }

#elif defined(NDEBUG)
#define loopUnroll(N) unroll<N>([&](uint8_t i) {
#define loopUnrollFrom(M, N) unroll<M, N>([&](uint8_t i) {
#define endLoop });
#endif

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

inline uint64_t negate_uint64(const uint64_t& value) noexcept {
#if defined(_MSC_VER)
	return -(*reinterpret_cast<const int64_t*>(&value));
#else
	return ~value + 1U;
#endif
}

template <typename T, uint8_t N>
inline void reverseArrayInPlace(std::array<T, N>& arr) noexcept {
	loopUnroll(N / 2)
		std::swap(arr[i], arr[N - 1 - i]);
	endLoop
}

template <typename T, uint8_t N>
inline std::array<T, N> reverseArray(const std::array<T, N>& arr) noexcept {
	std::array<T, N> result;
	loopUnroll(N)
		result[N - 1 - i] = arr[i];
	endLoop
	return result;
}