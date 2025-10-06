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

inline void addWithOverflow(uint64_t& a, const uint64_t b, unsigned char& carry) noexcept {
#if defined(_MSC_VER)
	carry = _addcarry_u64(carry, a, b, &a);	// Fastest - uses MSVC intrinsic
#elif defined(__GNUC__) || defined(__clang__)
	uint64_t temp;
	bool c1 = __builtin_add_overflow(a, b, &temp);	// Uses GCC/Clang intrinsic
	bool c2 = __builtin_add_overflow(temp, carry, &a);
	carry = static_cast<unsigned char>(c1 || c2);
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
inline void addWithOverflow(const uint64_t a, const uint64_t b, uint64_t& sum, unsigned char& carry) noexcept {
#if defined(_MSC_VER)	// MSVC - fastest
	carry = _addcarry_u64(carry, a, b, &sum);
#elif defined(__GNUC__) || defined(__clang__)
	bool c1 = __builtin_add_overflow(a, b, &sum);	// Uses GCC/Clang intrinsic
	bool c2 = __builtin_add_overflow(sum, carry, &sum);
	carry = static_cast<unsigned char>(c1 || c2);
#else	// Manual arithmetic if compiler can't be determined
	uint64_t temp = a + b;
	bool c1 = (temp < a);	// Check for overflow
	sum = temp + carry;
	carry = static_cast<unsigned char>(c1 || (sum < temp));	// Check if carry occurred
#endif
}

inline void subtractWithBorrow(uint64_t& a, const uint64_t b, unsigned char& borrow) noexcept {
#if defined(_MSC_VER)
	borrow = _subborrow_u64(borrow, a, b, &a);	// Fastest - uses MSVC intrinsic
#elif defined(__GNUC__) || defined(__clang__)
	uint64_t temp;
	bool c1 = __builtin_sub_overflow(a, b, &temp);	// Uses GCC/Clang intrinsic
	bool c2 = __builtin_sub_overflow(temp, borrow, &a);
	borrow = static_cast<unsigned char>(c1 || c2);
#else 
	uint64_t temp = a - b;
	borrow = (a < b) ? 1 : 0;

	a = temp - borrow;
	borrow |= (temp < borrow) ? 1 : 0;	// Check if borrow occurred
#endif
}

inline void subtractWithBorrow(const uint64_t a, const uint64_t b, uint64_t& result, unsigned char& borrow) noexcept {
#if defined(_MSC_VER)	// MSVC - fastest
	borrow = _subborrow_u64(borrow, a, b, &result);
#elif defined(__GNUC__) || defined(__clang__)
	bool c1 = __builtin_sub_overflow(a, b, &result);	// Uses GCC/Clang intrinsic
	bool c2 = __builtin_sub_overflow(result, borrow, &result);
	borrow = static_cast<unsigned char>(c1 || c2);
#else	// Manual arithmetic if compiler can't be determined
	uint16_t temp = a - b;
	borrow = (a < b) ? 1 : 0;	// Check for underflow
	result = temp - borrow;
	borrow |= (temp < borrow) ? 1 : 0;	// Check if borrow occurred
#endif
}


#if defined(_MSC_VER) && defined(_M_ARM64)	// MSVC arm-64
uint64_t _umul128(uint64_t a, uint64_t b, uint64_t* high_product) {
	*high = __umulh(a, b);	// Uses MSVC intrinsic for 128-bit multiplication
	return a * b;	// Returns the low part of the product
}
#endif


template <bool enable_high = true>
inline void multiply64x64(const uint64_t a, const uint64_t b, uint8_t& carry, uint64_t& result_low, uint64_t& result_high) noexcept {
#if defined(_MSC_VER)	// Only available on x64 MSVC
	uint64_t high, low = _umul128(a, b, &high);	// Uses MSVC intrinsic for 128-bit multiplication
	carry = _addcarry_u64(carry, result_low, low, &result_low);
	if constexpr (enable_high) {	// May be off for the last multiplication
		carry = _addcarry_u64(carry, result_high, high, &result_high);
	}
#elif defined(__SIZEOF_INT128__)	// Available on G++ and Clang
	__uint128_t product = static_cast<__uint128_t>(a) * b;
	addWithOverflow(result_low, static_cast<uint64_t>(product), carry);
	if constexpr (enable_high) {	// May be off for the last multiplicaton
		addWithOverflow(result_high, static_cast<uint64_t>(product >> 64), carry);
	}
#else
	// TODO: Implement manual implemtation 
#endif
}

inline uint64_t negate_uint64(const uint64_t& value) noexcept {
#if defined(_MSC_VER)
	return -(*reinterpret_cast<const int64_t*>(&value));
#else
	return ~value + 1U;
#endif
}

constexpr uint64_t CEIL(const double value) noexcept {
	return (static_cast<uint64_t>(value) == value) ? static_cast<uint64_t>(value) : static_cast<uint64_t>(value) + 1;
}

constexpr uint64_t BCD_BITWIDTH(const uint64_t binary_bitwidth) noexcept {
	return 4U * CEIL(binary_bitwidth * 0.30102999566398119521373889472449); // log10(2) ~ 0.30103
}

constexpr uint64_t UINT8_BCD_ARRAY_SIZE(const uint64_t uint64_count) noexcept {
	return CEIL(static_cast<double>(BCD_BITWIDTH(uint64_count * 64)) / 8.0);
}

template <uint8_t N>
union alignas(8) AlignedUInt8Array {
	std::array<uint64_t, N> data64;
	std::array<uint8_t, N * 8> data8;

	bool operator==(const AlignedUInt8Array<N>& other) const noexcept {
		return data64 == other.data64;
	}
};


template <uint8_t N>
inline AlignedUInt8Array<N> leftShift(const AlignedUInt8Array<N>& arr, const uint16_t places) {
	if constexpr (N == 1) {	// Evaluated at compile time so no performance impact
		return AlignedUInt8Array<N>({ .data64 = {arr.data64[0] << places} });
	}
	else {
		AlignedUInt8Array<N> returnVal{ .data64 = {0} };
		if (places >= 64U * N) return returnVal;	// All bits shifted out
		if (places == 0) return arr;				// No shift needed
		const uint8_t interWordShifts = places / 64U;
		const uint8_t intraWordShiftsL = places % 64U;
		const uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
		uint64_t low = arr.data64[interWordShifts] << intraWordShiftsL;
		uint64_t high;

		for (uint8_t i = interWordShifts + 1; i < N; ++i) {
			high = arr.data64[i] >> intraWordShiftsR;
			returnVal.data64[i - interWordShifts - 1] = high | low;
			low = arr.data64[i] << intraWordShiftsL;
		}
		returnVal.data64[N - interWordShifts - 1] = low;
		return returnVal;
	}
}

template <uint8_t N, uint16_t PLACES>
inline AlignedUInt8Array<N> leftShift(const AlignedUInt8Array<N>& arr) {
	if constexpr (PLACES == 0) {
		return arr;
	}
	else if constexpr (PLACES >= 64U * N) {
		return AlignedUInt8Array<N>({ .data64 = {0} });
	}
	else if constexpr(N == 1) {
		return AlignedUInt8Array<N>({ .data64 = {arr.data64[0] << PLACES} });
	}
	else if constexpr(PLACES % 64 == 0) {
		constexpr uint8_t wordShifts = PLACES / 64U;
		AlignedUInt8Array<N> returnVal;
		loopUnroll(N - wordShifts)
			returnVal.data64[i] = arr.data64[i + wordShifts];
		endLoop
		loopUnrollFrom(N - wordShifts, N)
			returnVal.data64[i] = 0;
		endLoop
		return returnVal;
	}
	else {
		constexpr uint8_t interWordShifts = PLACES / 64U;
		constexpr uint8_t intraWordShiftsL = PLACES % 64U;
		constexpr uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
		if constexpr (interWordShifts >= N) {
			return AlignedUInt8Array<N>({ .data64 = {0} });
		}
		AlignedUInt8Array<N> returnVal;
		uint64_t low = arr.data64[interWordShifts] << intraWordShiftsL;
		uint64_t high;
		loopUnroll(N - interWordShifts - 1)
			returnVal.data64[i] = 0;
		endLoop
		loopUnrollFrom(interWordShifts + 1, N)
			high = arr.data64[i] >> intraWordShiftsR;
			returnVal.data64[i - interWordShifts - 1] = high | low;
			low = arr.data64[i] << intraWordShiftsL;
		endLoop
		returnVal.data64[N - interWordShifts - 1] = low;
		return returnVal;
	}
}

template <uint8_t N, uint16_t PLACES>
inline AlignedUInt8Array<N> rightShift(const AlignedUInt8Array<N>& arr) {
	if constexpr (PLACES == 0) {
		return arr;
	}
	else if constexpr (PLACES >= 64U * N) {
		return AlignedUInt8Array<N>({ .data64 = {0} });
	}
	else if constexpr(N == 1) {
		return AlignedUInt8Array<N>({ .data64 = {arr.data64[0] >> PLACES} });
	}
	else if constexpr(PLACES % 64 == 0) {
		constexpr uint8_t wordShifts = PLACES / 64U;
		AlignedUInt8Array<N> returnVal;
		loopUnrollFrom(wordShifts, N)
			returnVal.data64[i] = arr.data64[i - wordShifts];
		endLoop
		loopUnroll(wordShifts)
			returnVal.data64[i] = 0;
		endLoop
		return returnVal;
	}
	else {
		constexpr uint8_t interWordShifts = PLACES / 64U;
		constexpr uint8_t intraWordShiftsR = PLACES % 64U;
		constexpr uint8_t intraWordShiftsL = 64U - intraWordShiftsR;
		if constexpr (interWordShifts >= N) {
			return AlignedUInt8Array<N>({ .data64 = {0} });
		}
		AlignedUInt8Array<N> returnVal;
		uint64_t high = arr.data64[N - 1 - interWordShifts] >> intraWordShiftsR;
		uint64_t low;
		loopUnroll(interWordShifts)
			returnVal.data64[N - 1 - i] = 0;
		endLoop
		loopUnrollFrom(interWordShifts + 1, N)
			low = arr.data64[N - 1 - i] << intraWordShiftsL;
			returnVal.data64[N - i + interWordShifts] = high | low;
			high = arr.data64[N - 1 - i] >> intraWordShiftsR;
		endLoop
		returnVal.data64[interWordShifts] = high;
		return returnVal;
	}
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

template <uint8_t N>
std::wstring byteArrayToBinaryString(const AlignedUInt8Array<N>& arr) noexcept {
	std::wstringstream ss;
	ss << L'{';
	for (uint8_t i = 0; i < 8 * N; ++i) {
		std::bitset<8> bits(arr.data8[i]);
		ss << bits << L' ';
	}
	ss << L'}';
	return ss.str();
}