// Author : Marek Oczadly
// License : MIT
// bitwise-functions.hpp

#pragma once
#include <cstdint>
#include <array>
#include "utils.hpp"


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
	else if constexpr (N == 1) {
		return AlignedUInt8Array<N>({ .data64 = {arr.data64[0] << PLACES} });
	}
	else if constexpr (PLACES % 64 == 0) {
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
	else if constexpr (N == 1) {
		return AlignedUInt8Array<N>({ .data64 = {arr.data64[0] >> PLACES} });
	}
	else if constexpr (PLACES % 64 == 0) {
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