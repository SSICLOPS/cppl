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

#include <cmath>
#include <map>
#include <stack>
#include <cstdint>

#include "binary.hh"
#include "ast.hh"
#include "colors.hh"
#include "relation_set.hh"
#include "policy_stack_processor.hh"
#include "reason_printer.hh"

#include "debug.hh"

using namespace std;

enum class PolicyStackOperationType {
    AND,
    OR,
    NEXT_RELATION,
    SPECIFIC_RELATION,
};

struct StackOperation  {
    PolicyStackOperationType type;
    int64_t relationId; //unused for OR and AND
};

class PolicyStack  {
    public:
        PolicyStack(RelationSet &relationSet, Binary &binary, uint16_t stackStartOffset) : relationSet(relationSet), policyBinary(binary) {
            //generate the operation translation maps
            operationTypeToInt[PolicyStackOperationType::NEXT_RELATION] = 0;
            intToOperationType[0] = PolicyStackOperationType::NEXT_RELATION;

            operationTypeToInt[PolicyStackOperationType::SPECIFIC_RELATION] = 1;
            intToOperationType[1] = PolicyStackOperationType::SPECIFIC_RELATION;

            operationTypeToInt[PolicyStackOperationType::OR] = 2;
            intToOperationType[2] = PolicyStackOperationType::OR;

            operationTypeToInt[PolicyStackOperationType::AND] = 3;
            intToOperationType[3] = PolicyStackOperationType::AND;

            //init the binary stack offset
            policyBinary.setPosition(stackStartOffset);

            //set the stack end position
            if(binary.isNull())  {
                relationSetOffset = 0;
            }
            else  {//if the policy is not empty
                parseStack();
            }
            
            //DEBUG output
            #if DEBUG_POLICY_GENERATION
                legend = string(RED) + "10:OR " + string(BLUE) + "11:AND " + string(CYAN) + "00:NEXT_REL " + string(YELLOW) + "01:SPEC_REL " + string(WHITE) + "Position" + RESET;
            #endif
        }

        //add the operation to the stack
        void push_back(StackOperation stackOp);

        //adds the policy stack end delimiter to the stack
        void addEndDelimiter(); 

        //return the start of the relation set form binary start
        uint64_t getRelationSetOffset();

        //returns the number of relations in the relations set
        uint64_t getNumberOfRelations();

        //returns the next policy stack operation
        PolicyStackOperationType get();

        friend ostream& operator<<(ostream &out, PolicyStack &policyStack);

        //DEBUG output
        #if DEBUG_POLICY_GENERATION
            //color coded string of the policy
            string asString;
            string legend;
        #endif

        //evaluates the policy stack with the given relation set
        bool processStack(RelationSet &relationSet);

        //adds the current stack to the binary
        void store();

        //print the size of the policy stack
        void printSize();

		//print the reason of processing result
		void printReason(ReasonPrinter & reasonPrinter){policyStackProcessor.printReason(reasonPrinter);}

		std::stack<StackOperation> & getStack(){return m_policyStack;}

    private:
        Binary &policyBinary;
        RelationSet &relationSet;
        uint64_t relationSetOffset;
        uint64_t numberOfRelations;
		//handle the processing work
		PolicyStackProcessor policyStackProcessor;

        static const uint8_t bitsForStackOperation = 2;

        //used for the policy evaluation
        std::stack<StackOperation> m_policyStack;
        
        //used for the policy stack size calculation
        uint64_t stackSize = 0;

        //calculates the stack size via the binary by searching for the first end of stack limiter (SPECIFIC_RELATION with value 1)
        void parseStack();

        //executes the generated poliyStack with the given processed relation set

        //map that stores for each operation type it bit sequence as string
        //e.g. PolicyStackOperationType::AND -> "11" (MSB left)
        map<PolicyStackOperationType, uint8_t> operationTypeToInt;
        map<uint8_t, PolicyStackOperationType> intToOperationType;

        //adds the bit string to the stack (MSB in string is left)
        void appendBitString(string bitString);
};
