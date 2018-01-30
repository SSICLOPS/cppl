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

#include "binary.hh"
#include "colors.hh"

#include "debug.hh"

using namespace std;

class PolicyHeader  {
    public:
        //init the class with the following header parameters
        PolicyHeader(Binary &policyBinary, uint16_t version) : policyBinary(policyBinary), version(version)
        {
            #if DEBUG_POLICY_GENERATION
                asString = string(RED) + uintToString(version, bitsForVersion) + RESET + asString;
                legend = string(RED) + "version " + RESET;
            #endif
        }

        //gets the header parameters from the binary representation
        PolicyHeader(Binary &policyBinary) : policyBinary(policyBinary)
        {
            //first get the version
            version = policyBinary.next(16);

            #if DEBUG_POLICY_GENERATION
                asString = string(RED) + uintToString(version, bitsForVersion) + RESET + asString;
                legend = string(RED) + "version " + RESET;
            #endif
        }

        friend ostream& operator<<(ostream &out, PolicyHeader &policyHeader);

        //DEBUG output
        #if DEBUG_POLICY_GENERATION
            //for the debug output
            string asString;
            string legend;
        #endif

        //adds the policy header to the binary
        void store();

        uint16_t version;

        //print the fixed header size
        void printSize();

    private:
        Binary &policyBinary;

        //gets a value as uint16_t and writes number of bits of it to the bit set
        void push_back(uint16_t value, string color, uint8_t numberOfBits = 16);
        void appendBitString(string bitString);

        const uint8_t bitsForVersion = 16;
};
