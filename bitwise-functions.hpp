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
	return 4U * CEIL(static_cast<double>(binary_bitwidth) * 0.30102999566398119521373889472449); // log10(2) ~ 0.30103
}

constexpr uint64_t BINARY_BITWIDTH_FROM_BCD(const uint64_t bcd_bitwidth) noexcept {
	return CEIL(static_cast<double>(bcd_bitwidth) / 0.30102999566398119521373889472449 / 4.0); // log10(2) ~ 0.30103
}

constexpr uint64_t UINT8_BCD_ARRAY_SIZE(const uint64_t uint64_count) noexcept {
	return CEIL(static_cast<double>(BCD_BITWIDTH(uint64_count * 64)) / 8.0);
}

constexpr uint64_t UINT64_BCD_ARRAY_SIZE(const uint64_t uint64_count) noexcept {
	return CEIL(static_cast<double>(BCD_BITWIDTH(uint64_count * 64)) / 64.0);
}

template <uint8_t N>
using Arr64 = std::array<uint64_t, N>;

template <uint8_t N>
inline Arr64<N> leftShift(const Arr64<N>& arr, const uint16_t places) noexcept {
	if constexpr (N == 1) {	// Evaluated at compile time so no performance impact
		return Arr64<N>{ arr[0] << places };
	}
	else {
		Arr64<N>returnVal{ 0 };
		if (places >= 64U * N) {
			return returnVal;
		}	// All bits shifted out
		if (places == 0) {
			return Arr64<N>{ arr };
		}	// No shift needed
		const uint8_t interWordShifts = places / 64U;
		const uint8_t intraWordShiftsL = places % 64U;
		const uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
		uint64_t low = arr[interWordShifts] << intraWordShiftsL;
		uint64_t high;

		for (uint8_t i = interWordShifts + 1; i < N; ++i) {
			high = arr[i] >> intraWordShiftsR;
			returnVal[i - interWordShifts - 1] = high | low;
			low = arr[i] << intraWordShiftsL;
		}
		returnVal[N - interWordShifts - 1] = low;
		return returnVal;
	}
}

template <uint8_t N>
inline void leftShiftInPlace(Arr64<N>& arr, const uint16_t places) noexcept {
	if constexpr (N == 1) {
		arr[0] <<= places;
		return;
	}
	else {
		if (places >= 64U * N) {
			arr.fill(0);
			return;
		}
		if (places == 0) {
			return;
		}
		const uint8_t interWordShifts = places / 64U;
		const uint8_t intraWordShiftsL = places % 64U;
		const uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
		uint64_t low = arr[interWordShifts] << intraWordShiftsL;
		uint64_t high;

		for (uint8_t i = interWordShifts + 1; i < N; ++i) {
			high = arr[i] >> intraWordShiftsR;
			arr[i - interWordShifts - 1] = high | low;
			low = arr[i] << intraWordShiftsL;
		}

		arr[N - interWordShifts - 1] = low;

		for (uint8_t i = N - interWordShifts; i < N; ++i) {
			arr[i] = 0;
		}
	}
}


template <uint8_t N, uint16_t PLACES>
inline Arr64<N> leftShift(const Arr64<N>& arr) noexcept {
	if constexpr (PLACES == 0) {
		return arr;
	}
	else if constexpr (PLACES >= 64U * N) {
		return Arr64<N>{ 0 };
	}
	else if constexpr (N == 1) {
		return Arr64<N>{arr[0] << PLACES};
	}
	else if constexpr (PLACES % 64 == 0) {
		constexpr uint8_t wordShifts = PLACES / 64U;
		Arr64<N> returnVal;

		loopUnroll(N - wordShifts)
			returnVal[i] = arr[i + wordShifts];
		endLoop

		loopUnrollFrom(N - wordShifts, N)
			returnVal[i] = 0;
		endLoop

		return returnVal;
	}
	else {
		constexpr uint8_t interWordShifts = PLACES / 64U;
		constexpr uint8_t intraWordShiftsL = PLACES % 64U;
		constexpr uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
		if constexpr (interWordShifts >= N) {
			return Arr64<N>{0};
		}
		Arr64<N> returnVal;
		uint64_t low = arr[interWordShifts] << intraWordShiftsL;
		uint64_t high;

		loopUnrollFrom(interWordShifts + 1, N)
			high = arr[i] >> intraWordShiftsR;
			returnVal[i - interWordShifts - 1] = high | low;
			low = arr[i] << intraWordShiftsL;
		endLoop
		
		returnVal[N - interWordShifts - 1] = low;

		loopUnrollFrom(N - interWordShifts, N)
			returnVal[i] = 0;
		endLoop

		return returnVal;
	}
}

template <uint8_t N, uint16_t PLACES>
inline void leftShiftInPlace(Arr64<N>& arr) noexcept {
	if constexpr (PLACES == 0) {
		return;
	}
	else if constexpr (PLACES >= 64U * N) {
		arr.fill(0);
		return;
	}
	else if constexpr (N == 1) {
		arr[0] <<= PLACES;
		return;
	}
	else if constexpr (PLACES % 64 == 0) {
		constexpr uint8_t wordShifts = PLACES / 64U;
		loopUnroll(N - wordShifts)
			arr[i] = arr[i + wordShifts];
		endLoop
		
		loopUnrollFrom(N - wordShifts, N)
			arr[i] = 0;
		endLoop
			
		return;
	}
	else {
		constexpr uint8_t interWordShifts = PLACES / 64U;
		constexpr uint8_t intraWordShiftsL = PLACES % 64U;
		constexpr uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
		if constexpr (interWordShifts >= N) {
			arr.fill(0);
			return;
		}
		uint64_t low = arr[interWordShifts] << intraWordShiftsL;
		uint64_t high;
		
		loopUnrollFrom(interWordShifts + 1, N)
			high = arr[i] >> intraWordShiftsR;
			arr[i - interWordShifts - 1] = high | low;
			low = arr[i] << intraWordShiftsL;
		endLoop

		arr[N - interWordShifts - 1] = low;

		loopUnrollFrom(N - interWordShifts, N)
			arr[i] = 0;
		endLoop
	}
}

template <uint8_t N, uint16_t PLACES>
inline void leftShiftInPlace(uint64_t (&arr) [N]) {
	leftShiftInPlace<N, PLACES>(reinterpret_cast<Arr64<N>&>(arr));
}

template <uint8_t N>
std::wstring byteArrayToBinaryString(const Arr64<N>& arr) noexcept {
	std::wstringstream ss;
	ss << L'\n' << L'{';
	for (uint8_t i = 0; i < N; ++i) {
		std::bitset<64> bits(arr[i]);
		ss << bits << L' ';
	}
	ss << L'}';
	return ss.str();
}

template <uint8_t N>
inline uint8_t getBit(const Arr64<N>& arr, const uint16_t idx) noexcept {
	// No bounds checking for performance. Only used internally with valid indices.
#ifdef _DEBUG
	const uint8_t arrPos = idx / 64U;
	const uint8_t bitPos = 63 - idx % 64U;
	const uint64_t word = arr[arrPos];
	const auto shiftWord = word >> bitPos;
	const uint8_t returned = shiftWord & 0x01U;
	return returned;
#else
	return (arr[idx / 64U] >> (63 - idx % 64U)) & 0x01U;
#endif
}

inline void add3Module(uint8_t& bcd_byte) noexcept {
#ifdef _DEBUG
	uint8_t rightBits = bcd_byte & 0b00001111;
	if (rightBits >= 5) {
		bcd_byte += 0x03;
	}
	uint8_t leftBits = (bcd_byte >> 4);

	if (leftBits >= 5) {
		bcd_byte += 0x30;
	}
#else
	bcd_byte += ((bcd_byte & 0b00001111) >= 5) ? 0x03 : 0x00;
	bcd_byte += ((bcd_byte >> 4) >= 5) ? 0x30 : 0x00;
#endif
}

inline void add3Module(uint64_t& bcd_word) noexcept {
	std::array<uint8_t, 8>& byte_arr = reinterpret_cast<std::array<uint8_t, 8>&>(bcd_word);
	add3Module(byte_arr[0]);
	add3Module(byte_arr[1]);
	add3Module(byte_arr[2]);
	add3Module(byte_arr[3]);
	add3Module(byte_arr[4]);
	add3Module(byte_arr[5]);
	add3Module(byte_arr[6]);
	add3Module(byte_arr[7]);
}

template <uint8_t N>
inline Arr64<UINT64_BCD_ARRAY_SIZE(N)> binaryToBCD(const Arr64<N>& arr) noexcept {
	constexpr auto BCD_SIZE_BITS = BCD_BITWIDTH(64ULL * N);
	constexpr auto BCD_ARR_WIDTH = UINT64_BCD_ARRAY_SIZE(N);

	std::array<uint64_t, BCD_ARR_WIDTH> bcdArray = { 0 };
	
	for (uint16_t j = 0; j < N * 64U; ++j) {
		loopUnroll(BCD_ARR_WIDTH)
			add3Module(bcdArray[i]);
		endLoop
		leftShiftInPlace<BCD_ARR_WIDTH, 1>(bcdArray);
		bcdArray[BCD_ARR_WIDTH - 1] |= getBit(arr, j);
	}

	return bcdArray;
}