// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <boost/dynamic_bitset.hpp>
#include <string>
#include <cstdint>
#include <cstring>

#define DEBUG_AST_PRINT 0
#define DEBUG_POLICY_GENERATION 0
#define DEBUG_POLICY_EVALUATION 0

#define EVALUATION_OUTPUT 0

#define PRINT_REASON_TO_CONSOLE

std::string uintToString(uint64_t value, uint8_t numberOfBits);
std::string intToString(int64_t value, uint8_t numberOfBits);
std::string doubleToString(double value);
