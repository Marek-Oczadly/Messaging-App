#pragma once
#include <cstdint>

#if defined(_MSC_VER)
#include <intrin.h>
#endif

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