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

constexpr uint64_t UINT8_BCD_ARRAY_SIZE(const uint64_t uint64_count) noexcept {
	return CEIL(static_cast<double>(BCD_BITWIDTH(uint64_count * 64)) / 8.0);
}

constexpr uint64_t UINT64_BCD_ARRAY_SIZE(const uint64_t uint64_count) noexcept {
	return CEIL(static_cast<double>(BCD_BITWIDTH(uint64_count * 64)) / 64.0);
}

template <uint8_t N>
union alignas(8) AlignedUInt8Array {
	std::array<uint64_t, N> data64;
	std::array<uint8_t, N * 8> data8;

	bool operator==(const AlignedUInt8Array<N>& other) const noexcept {
		return data64 == other.data64;
	}

	inline AlignedUInt8Array<N> operator<<(const uint16_t places) const noexcept {
		if constexpr (N == 1) {	// Evaluated at compile time so no performance impact
			return AlignedUInt8Array<N>({ .data64 = {data64[0] << places} });
		}
		else {
			AlignedUInt8Array<N> returnVal{ .data64 = {0} };
			if (places >= 64U * N) { 
				return returnVal; 
			}	// All bits shifted out
			if (places == 0) { 
				return AlignedUInt8Array<N>{.data64 = data64 }; 
			}	// No shift needed
			const uint8_t interWordShifts = places / 64U;
			const uint8_t intraWordShiftsL = places % 64U;
			const uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
			uint64_t low = data64[interWordShifts] << intraWordShiftsL;
			uint64_t high;

			for (uint8_t i = interWordShifts + 1; i < N; ++i) {
				high = data64[i] >> intraWordShiftsR;
				returnVal.data64[i - interWordShifts - 1] = high | low;
				low = data64[i] << intraWordShiftsL;
			}
			returnVal.data64[N - interWordShifts - 1] = low;
			return returnVal;
		}
	}

	inline AlignedUInt8Array<N>& operator<<=(const uint16_t places) noexcept {
		if constexpr (N == 1) {
			data64[0] <<= places;
			return *this;
		}
		else {
			if (places >= 64U * N) {
				data64.fill(0);
				return *this;
			}
			if (places == 0) {
				return *this;
			}
			const uint8_t interWordShifts = places / 64U;
			const uint8_t intraWordShiftsL = places % 64U;
			const uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
			uint64_t low = data64[interWordShifts] << intraWordShiftsL;
			uint64_t high;

			for (uint8_t i = interWordShifts + 1; i < N; ++i) {
				high = data64[i] >> intraWordShiftsR;
				data64[i - interWordShifts - 1] = high | low;
				low = data64[i] << intraWordShiftsL;
			}
			data64[N - interWordShifts - 1] = low;
			return *this;
		}
	}

	template <uint16_t PLACES>
	inline AlignedUInt8Array<N> leftShift() const noexcept {
		if constexpr (PLACES == 0) {
			return AlignedUInt8Array<N>{.data64 = data64 };
		}
		else if constexpr (PLACES >= 64U * N) {
			return AlignedUInt8Array<N>({ .data64 = {0} });
		}
		else if constexpr (N == 1) {
			return AlignedUInt8Array<N>({ .data64 = {data64[0] << PLACES} });
		}
		else if constexpr (PLACES % 64 == 0) {
			constexpr uint8_t wordShifts = PLACES / 64U;
			AlignedUInt8Array<N> returnVal;
			loopUnroll(N - wordShifts)
				returnVal.data64[i] = data64[i + wordShifts];
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
			uint64_t low = data64[interWordShifts] << intraWordShiftsL;
			uint64_t high;
			loopUnroll(N - interWordShifts - 1)
				returnVal.data64[i] = 0;
			endLoop
				loopUnrollFrom(interWordShifts + 1, N)
				high = data64[i] >> intraWordShiftsR;
			returnVal.data64[i - interWordShifts - 1] = high | low;
			low = data64[i] << intraWordShiftsL;
			endLoop
				returnVal.data64[N - interWordShifts - 1] = low;
			return returnVal;
		}
	}

	template <uint16_t PLACES>
	inline AlignedUInt8Array<N>& leftShiftInPlace() noexcept {
		if constexpr (PLACES == 0) {
			return *this;
		}
		else if constexpr (PLACES >= 64U * N) {
			data64.fill(0);
			return *this;
		}
		else if constexpr (N == 1) {
			data64[0] <<= PLACES;
			return *this;
		}
		else if constexpr (PLACES % 64 == 0) {
			constexpr uint8_t wordShifts = PLACES / 64U;
			loopUnroll(N - wordShifts)
				data64[i] = data64[i + wordShifts];
			endLoop
				loopUnrollFrom(N - wordShifts, N)
				data64[i] = 0;
			endLoop
				return *this;
		}
		else {
			constexpr uint8_t interWordShifts = PLACES / 64U;
			constexpr uint8_t intraWordShiftsL = PLACES % 64U;
			constexpr uint8_t intraWordShiftsR = 64U - intraWordShiftsL;
			if constexpr (interWordShifts >= N) {
				data64.fill(0);
				return *this;
			}
			uint64_t low = data64[interWordShifts] << intraWordShiftsL;
			uint64_t high;
			loopUnroll(N - interWordShifts - 1)
				data64[i] = 0;
			endLoop
			loopUnrollFrom(interWordShifts + 1, N)
				high = data64[i] >> intraWordShiftsR;
			data64[i - interWordShifts - 1] = high | low;
			low = data64[i] << intraWordShiftsL;
			endLoop
			data64[N - interWordShifts - 1] = low;
			return *this;
		}
	}
	
	inline uint8_t getBit(const uint16_t idx) const noexcept {
		// No bounds checking for performance. Only used internally with valid indices.
		const uint16_t byteIdx = idx / 8U;
		const uint8_t bitIdx = idx % 8U;
		return (data8[byteIdx] >> bitIdx) & 0x01U;
	}
};

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

inline void add3Module(uint8_t& bcd_byte) noexcept {
	bcd_byte += ((bcd_byte << 4) >= 0x0500) ? 0x03 : 0x00;
	bcd_byte += ((bcd_byte >> 4) >= 0x05) ? 0x0300 : 0x00;
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
inline AlignedUInt8Array<UINT64_BCD_ARRAY_SIZE(N)> binaryToBCD(const AlignedUInt8Array<N>& arr) noexcept {
	constexpr auto BCD_SIZE_BITS = BCD_BITWIDTH(64ULL * N);
	constexpr auto BCD_ARR_WIDTH = UINT64_BCD_ARRAY_SIZE(N);

	AlignedUInt8Array<BCD_ARR_WIDTH> bcdArray{ .data64 = {0} };
	
	for (uint16_t i = 0; i < BCD_ARR_WIDTH; ++i) {
		bcdArray.leftShiftInPlace<1>();
		bcdArray.data64[BCD_ARR_WIDTH - 1] |= arr.getBit(N * 8 - 1 - i);
		loopUnroll(BCD_ARR_WIDTH)
			add3Module(bcdArray.data64[i]);
		endLoop
	}
	return bcdArray;
}