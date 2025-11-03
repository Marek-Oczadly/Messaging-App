// Author : Marek Oczadly
// License : MIT
// UnitTests.cpp

#include <iomanip>
#include <cstdint>
#include "pch.h"
#include "CppUnitTest.h"
#include "largeInt.hpp"
#include "utils.hpp"
#include "bitwise-functions.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;


#define DEFINE_UINT_ARRAY_TOSTRING(N)											\
namespace Microsoft {															\
	namespace VisualStudio {													\
		namespace CppUnitTestFramework {										\
			template <>															\
			std::wstring ToString<uint_array<N>>(const uint_array<N>& q) {		\
				std::wstringstream ss;											\
				ss << L'{';														\
				for (char i = N - 1; i >= 0; --i) {								\
					ss << L"0x" << std::hex << std::uppercase << q[i] << L',';	\
				}																\
				ss << L'}';														\
				return ss.str();												\
			}																	\
		}																		\
	}																			\
}

#define DEFINE_ARR_TO_STRING(N)													\
namespace Microsoft {															\
	namespace VisualStudio {													\
		namespace CppUnitTestFramework {										\
			template <>															\
			std::wstring ToString<Arr64<N>>(const Arr64<N>& q) {				\
				return byteArrayToBinaryString<N>(q);							\
			}																	\
		}																		\
	}																			\
}


#ifdef DEFINE_UINT_ARRAY_TOSTRING
DEFINE_UINT_ARRAY_TOSTRING(4);
DEFINE_UINT_ARRAY_TOSTRING(8);
DEFINE_UINT_ARRAY_TOSTRING(16);
#endif

#ifdef DEFINE_ARR_TO_STRING
DEFINE_ARR_TO_STRING(1);
DEFINE_ARR_TO_STRING(2);
DEFINE_ARR_TO_STRING(4);
#endif


namespace LARGE_INT {
	TEST_CLASS(BASIC_ARITMETIC) {
	
	public:

		TEST_METHOD(EXPECTED_BITFLIP) {
			int64_t temp = INT64_MIN;
			uint64_t a = *reinterpret_cast<uint64_t*>(&temp);
			uint64_t expected = ~a + 1;
			uint64_t test = -(*reinterpret_cast<int64_t*>(&a));
			Assert::AreEqual(test, expected);
		}

		TEST_METHOD(BASIC_256_ADDITION) {
			uint256_t a = { 1, 1, 1, 1 };
			uint256_t b = { 2, 2, 2, 2 };

			Assert::AreEqual(a + b, uint256_t{ 3, 3, 3, 3 });
		}

		TEST_METHOD(BASIC_256_SUBTRACTION) {
			uint256_t a = { 5, 5, 5, 5 };
			uint256_t b = { 2, 2, 2, 2 };

			Assert::AreEqual(a - b, uint256_t{ 3, 3, 3, 3 });
		}

		TEST_METHOD(ADDITION_256_OVERFLOW) {
			uint256_t a = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
			uint256_t b = { 0, 0, 0, 1 };
			// This should overflow and wrap around
			// Keep getting {0, FF..., FF..., FF... } 
			Assert::AreEqual(uint256_t{ 0, 0, 0, 0 }, a + b);
		}

		TEST_METHOD(SUBTRACTION_256_UNDERFLOW) {
			uint256_t a = { 0, 0, 0, 0 };
			uint256_t b = { 0, 0, 0, 1 };
			// This should underflow and wrap around
			Assert::AreEqual(uint256_t{ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF }, a - b);
		}

		TEST_METHOD(ADDITION_512_256) {
			uint512_t a = { 1, 1, 1, 1, 1, 1, 1, 1 };
			uint256_t b = { 2, 2, 2, 2 };

			Assert::AreEqual(uint512_t{ 1, 1, 1, 1, 3, 3, 3, 3 }, a + b);
		}
		TEST_METHOD(ADDITION_512_256_OVERFLOW) {
			uint512_t a = { 0, 0, 0, 0, 0, 0, 0, 1 };
			uint256_t b = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
			uint512_t expected = { 0, 0, 0, 1, 0, 0, 0, 0 };

			Assert::AreEqual(expected, a + b);
		}

		TEST_METHOD(ADDITION_256_512_OVERFLOW) {
			uint512_t a = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
			uint256_t b = { 0x0, 0x0, 0x0, 0x1 };
			uint512_t expected = { 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 };

			Assert::AreEqual(expected, a + b);
		}

		TEST_METHOD(SUBTRACTION_512_256) {
			uint512_t a = { 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 };
			uint256_t b = { 0x0, 0x0, 0x0, 0x1 };
			uint512_t expected = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };

			Assert::AreEqual(expected, a - b);
		}

		TEST_METHOD(SUBTRACTION_256_512) {
			uint256_t a = { 0x0, 0x0, 0x35, 0x0 };
			uint512_t b = { 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0 };

			uint512_t expected = 
				{ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0x0, 0x0, 0x35, 0x0 };
			Assert::AreEqual(expected, a - b);
		}

		TEST_METHOD(EQUAL_SIZE) {
			Assert::AreEqual(sizeof(uint256_t), sizeof(uint64_t) * 4);
		}
	};
}

namespace BITWISE_FUNCTIONS {
	TEST_CLASS(LEFT_SHIFT_RUNTIME) {

	public:
		TEST_METHOD(LEFT_SHIFT_NO_BYTES) {
			Arr64<1> test = { { 0b0101011011001010110101111101001011000100001011011110110100101101ULL} };
			const uint8_t places = 13U;

			Arr64<1> expected = { 0b0101011011001010110101111101001011000100001011011110110100101101ULL << places };
			Assert::AreEqual(expected, leftShift(test, places));
		}
		TEST_METHOD(LEFT_SHIFT_INPLACE_NO_BYTES) {
			Arr64<1> test = { { 0b0101011011001010110101111101001011000100001011011110110100101101ULL} };
			const uint8_t places = 13U;

			Arr64<1> expected = { 0b0101011011001010110101111101001011000100001011011110110100101101ULL << places };
			leftShiftInPlace(test, places);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_NO_PLACES) {
			Arr64<2> test = { 0x1234567890ABCDEF, 0x0FEDCBA098765432 };

			const uint8_t places = 0u;
			Arr64<2> expected = test;
			Assert::AreEqual(expected, leftShift(test, places));
		}
		TEST_METHOD(LEFT_SHIFT_NO_PLACES_INPLACE) {
			Arr64<2> test = { 0x1234567890ABCDEF, 0x0FEDCBA098765432 };
			
			const uint8_t places = 0u;
			Arr64<2> expected = test;
			leftShiftInPlace(test, places);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_SIMPLE) {
			Arr64<2> test = {0x0000000000000001, 0x0000000000000000};
			const uint8_t places = 4;

			Arr64<2> expected = { 0x0000000000000010, 0x0000000000000000 };
			Assert::AreEqual(expected, leftShift(test, places));
		}

		TEST_METHOD(LEFT_SHIFT_SIMPLE_INPLACE) {
			Arr64<2> test = { 0x0000000000000001, 0x0000000000000000 };
			const uint8_t places = 4;

			Arr64<2> expected = { 0x0000000000000010, 0x0000000000000000 };
			leftShiftInPlace(test, places);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_ACROSS_BOUNDARY) {
			Arr64<2> test = { 0x0000000000000001, 0x0000000000000002 };
			const uint8_t places = 68;
			Arr64<2> expected = { 0x0000000000000020, 0x0000000000000000 };
			Assert::AreEqual(expected, leftShift(test, places));
		}
		
		TEST_METHOD(LEFT_SHIFT_ACROSS_BOUNDARY_INPLACE) {
			Arr64<2> test = { 0x0000000000000001, 0x0000000000000002 };
			const uint8_t places = 68;
			Arr64<2> expected = { 0x0000000000000020, 0x0000000000000000 };

			leftShiftInPlace(test, places);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_FULL_WORD) {
			Arr64<2> test = { 0xAAAAAAAAAAAAAAAA, 0x5555555555555555 };
			const uint8_t places = 64;
			Arr64<2> expected = { 0x5555555555555555, 0x0000000000000000 };
			Assert::AreEqual(expected, leftShift(test, places));
		}

		TEST_METHOD(LEFT_SHIFT_FULL_WORD_INPLACE) {
			Arr64<2> test = { 0xAAAAAAAAAAAAAAAA, 0x5555555555555555 };
			const uint8_t places = 64;
			Arr64<2> expected = { 0x5555555555555555, 0x0000000000000000 };
			
			leftShiftInPlace(test, places);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_BEYOND_WIDTH) {
			Arr64<4> test = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
			const uint16_t places = 267;	// 4*64=256 < 260 < 5*64=320
			Arr64<4> expected = { 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 };
			Assert::AreEqual(expected, leftShift(test, places));
		}

		TEST_METHOD(LEFT_SHIFT_BEYOND_WIDTH_INPLACE) {
			Arr64<4> test = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
			const uint16_t places = 267;	// 4*64=256 < 260 < 5*64=320
			Arr64<4> expected = { 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 };

			leftShiftInPlace(test, places);
			Assert::AreEqual(expected, test);

		}
	};

	TEST_CLASS(LEFT_SHIFT_COMPILETIME) {

	public:
		TEST_METHOD(LEFT_SHIFT_1_ELEM) {
			Arr64<1> test = { { 0b0101011011001010110101111101001011000100001011011110110100101101ULL} };
			constexpr uint8_t places = 13U;

			Arr64<1> expected = { 0b0101011011001010110101111101001011000100001011011110110100101101ULL << places };
			Assert::AreEqual(expected, leftShift<1, places>(test));
		}
		TEST_METHOD(LEFT_SHIFT_1_ELEM_INPLACE) {
			Arr64<1> test = { { 0b0101011011001010110101111101001011000100001011011110110100101101ULL} };
			constexpr uint8_t places = 13U;

			Arr64<1> expected = { 0b0101011011001010110101111101001011000100001011011110110100101101ULL << places };
			leftShiftInPlace<1, places>(test);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_NO_PLACES) {
			Arr64<2> test = { 0x1234567890ABCDEF, 0x0FEDCBA098765432 };

			constexpr uint8_t places = 0u;

			Arr64<2> expected = test;
			Assert::AreEqual(expected, leftShift<2, places>(test));
		}

		TEST_METHOD(LEFT_SHIFT_NO_PLACES_INPLACE) {
			Arr64<2> test = { 0x1234567890ABCDEF, 0x0FEDCBA098765432 };
			constexpr uint8_t places = 0u;
			Arr64<2> expected = test;
			leftShiftInPlace<2, places>(test);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_SIMPLE) {
			Arr64<2> test = { 0x0000000000000001, 0x0000000000000000 };
			constexpr uint8_t places = 4;

			Arr64<2> expected = { 0x0000000000000010, 0x0000000000000000 };

			Assert::AreEqual(expected, leftShift<2, places>(test));
		}

		TEST_METHOD(LEFT_SHIFT_SIMPLE_INPLACE) {
			Arr64<2> test = { 0x0000000000000001, 0x0000000000000000 };
			constexpr uint8_t places = 4;
			Arr64<2> expected = { 0x0000000000000010, 0x0000000000000000 };
			leftShiftInPlace<2, places>(test);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_ACROSS_BOUNDARY) {
			Arr64<2> test = { 0x0000000000000001, 0x0000000000000002 };
			constexpr uint8_t places = 68;
			Arr64<2> expected = { 0x0000000000000020, 0x0000000000000000 };
			Assert::AreEqual(expected, leftShift<2, places>(test));
		}

		TEST_METHOD(LEFT_SHIFT_ACROSS_BOUNDARY_INPLACE) {
			Arr64<2> test = { 0x0000000000000001, 0x0000000000000002 };
			constexpr uint8_t places = 68;
			Arr64<2> expected = { 0x0000000000000020, 0x0000000000000000 };
			leftShiftInPlace<2, places>(test);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_FULL_WORD) {
			Arr64<2> test = { 0xAAAAAAAAAAAAAAAA, 0x5555555555555555 };
			constexpr uint8_t places = 64;
			Arr64<2> expected = { 0x5555555555555555, 0x0000000000000000 };
			Assert::AreEqual(expected, leftShift<2, places>(test));
		}

		TEST_METHOD(LEFT_SHIFT_FULL_WORD_INPLACE) {
			Arr64<2> test = { 0xAAAAAAAAAAAAAAAA, 0x5555555555555555 };
			constexpr uint8_t places = 64;
			Arr64<2> expected = { 0x5555555555555555, 0x0000000000000000 };
			leftShiftInPlace<2, places>(test);
			Assert::AreEqual(expected, test);
		}

		TEST_METHOD(LEFT_SHIFT_BEYOND_WIDTH) {
			Arr64<4> test = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
			constexpr uint16_t places = 267;	// 4*64=256 < 260 < 5*64=320
			Arr64<4> expected = { 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 };
			Assert::AreEqual(expected, leftShift<4, places>(test));
		}

		TEST_METHOD(LEFT_SHIFT_BEYOND_WIDTH_INPLACE) {
			Arr64<4> test = { 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF };
			constexpr uint16_t places = 267;	// 4*64=256 < 260 < 5*64=320
			Arr64<4> expected = { 0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000 };
			leftShiftInPlace<4, places>(test);
			Assert::AreEqual(expected, test);
		}
	};
}