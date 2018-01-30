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

#include "debug.hh"

using namespace std;

string uintToString(uint64_t value, uint8_t numberOfBits)  {
    boost::dynamic_bitset<> valueBitSet(numberOfBits, value);
    string valueStr;
    boost::to_string(valueBitSet, valueStr);

    return valueStr;
}

string intToString(int64_t value, uint8_t numberOfBits)  {
    boost::dynamic_bitset<> valueBitSet(numberOfBits, value);
    string valueStr;
    boost::to_string(valueBitSet, valueStr);

    return valueStr;
}

string doubleToString(double value)  {
    //currently only double's with 64 bits
    assert(sizeof(double) == sizeof(uint64_t));
    uint64_t converted;
    memcpy(&converted, &value, sizeof(double));
    return uintToString(converted, 64);
}
