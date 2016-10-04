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
