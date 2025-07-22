#include <iomanip>
#include "pch.h"
#include "CppUnitTest.h"
#include "largeInt.hpp"

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

#ifdef DEFINE_UINT_ARRAY_TOSTRING
DEFINE_UINT_ARRAY_TOSTRING(4);
DEFINE_UINT_ARRAY_TOSTRING(8);
#endif

namespace LARGE_INT {
	TEST_CLASS(BASIC_ARITMETIC) {
	
public:
		
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
	};
}
