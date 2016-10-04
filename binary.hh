#pragma once

#include <iostream>
#include <assert.h>
#include <cstdint>
#include <cmath>

//allows to store and receive bits from the memory adress
//PolicyPart allows to use for each part of the policy a seperate offsets
class Binary {
    public:
        //inits the binary with an existing memory
        Binary(uint8_t *mem) : mem(mem) { 
            bitOffset = 0;
            nextFreeBit = 0;
        }

        //adds the bits to the binary
        void push_back(uint64_t bits, uint8_t numberOfBits = 64);

        //gets the next numberOfBits bits from the binary
        //since of the return type the maximum numberOfBits is 64
        uint64_t next(uint8_t numberOfBits);

        //gets (without popping them!) as many bits as defined in numberOfBits from the binary
        //since of the return type the maximum numberOfBits is 64
        uint64_t at(uint64_t offset, uint8_t numberOfBits);

        uint64_t getPosition();
        void setPosition(uint64_t newPosition);

        bool isNull() { return mem == NULL; }

        uint32_t size();

        void print();

        uint8_t *getPointer();

    private:
        uint8_t *mem;
        uint32_t bitOffset; //offset for the current position in bit from the start
        uint32_t nextFreeBit; //last used bit
        uint16_t allocatedBytes;
        const uint16_t initMallocSize = 8;
};
