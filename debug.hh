#pragma once

#include <boost/dynamic_bitset.hpp>
#include <string>
#include <cstdint>
#include <cstring>

#define DEBUG_AST_PRINT 0
#define DEBUG_POLICY_GENERATION 0
#define DEBUG_POLICY_EVALUATION 0

#define EVALUATION_OUTPUT 0

std::string uintToString(uint64_t value, uint8_t numberOfBits);
std::string intToString(int64_t value, uint8_t numberOfBits);
std::string doubleToString(double value);
