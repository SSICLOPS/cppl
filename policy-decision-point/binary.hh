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

#include <string>
#include <fstream>
#include <iostream>
#include <assert.h>
#include <cstdint>
#include <cmath>

using namespace std;

//allows to store and receive bits from the memory adress
//PolicyPart allows to use for each part of the policy a seperate offsets
class Binary {
    public:
        //inits the binary with an existing memory
        Binary(uint8_t *mem = NULL) : mem(mem) { 
            bitOffset = 0;
            nextFreeBit = 0;
        }
		
		~Binary();

        //adds the bits to the binary
        void push_back(uint64_t bits, uint8_t numberOfBits = 64);

        //gets the next numberOfBits bits from the binary
        //since of the return type the maximum numberOfBits is 64
        uint64_t next(uint8_t numberOfBits);

        //gets (without popping them!) as many bits as defined in numberOfBits from the binary
        //since of the return type the maximum numberOfBits is 64
        uint64_t at(uint64_t offset, uint8_t numberOfBits) const;

        uint64_t getPosition();
        void setPosition(uint64_t newPosition);

        bool isNull() { return mem == NULL; }

        uint32_t size();

        void print();

        void write(string filename) const;
		void write(FILE * file) const;
        void read_from_file(string filename);
		void read_from_mem(const void * data, uint64_t len);

        uint8_t *getPointer();

		//void attach(RawBinary rawBinary){
			//if (mem)
				//free(mem);

			//mem = rawBinary.mem;
			//nextFreeBit = rawBinary.nextFreeBit;
			//allocatedBytes = rawBinary.allocatedBytes;
			//bitOffset = 0;
		//}

		//RawBinary detach(){
			//RawBinary rawBinary;

			//rawBinary.mem = mem;
			//rawBinary.nextFreeBit = nextFreeBit;
			//rawBinary.allocatedBytes = allocatedBytes;

			//mem = NULL;
			//return rawBinary;
		//}

		inline void reset(){bitOffset = nextFreeBit = 0;}

    private:
        uint8_t *mem;
        uint32_t bitOffset; //offset for the current position in bit from the start
        uint32_t nextFreeBit; //last used bit
        uint32_t allocatedBytes;
        const uint16_t initMallocSize = 8;
};
