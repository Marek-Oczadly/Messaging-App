// Author: Marek Oczadly
// License: MIT
// largeInt.hpp

#pragma once
#include <array>
#include "simdDetection.hpp"


class uint256_t {
private:
	std::array<uint64_t, 4> data;
public:
	uint256_t() : data() {};
	
};


unsigned int CeilLog2(const uint256_t& num) {

}