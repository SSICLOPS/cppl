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

#include "policy_stack.hh"

void PolicyStack::push_back(StackOperation stackOp)  {
    //push the current operation to the stack
    m_policyStack.push(stackOp);

    //DEBUG output for PolicyStack and iteration order
    #if DEBUG_POLICY_GENERATION
        //stack operation int to bit string
        string bitString = uintToString(operationTypeToInt[stackOp.type], bitsForStackOperation);

        if(stackOp.type == PolicyStackOperationType::OR)  {
            cout << RED << " OR:" << bitString << RESET; //DEBUG
            asString = string(RED) + bitString + RESET + asString;
        }
        else if(stackOp.type == PolicyStackOperationType::AND) {
            cout << BLUE << " AND:" << bitString << RESET; //DEBUG
            asString = string(BLUE) + bitString + RESET + asString;
        }
        else if(stackOp.type == PolicyStackOperationType::NEXT_RELATION) {
            cout << CYAN << " NEXT_REL:" << bitString << RESET; //DEBUG
            asString = string(CYAN) + bitString + RESET + asString;
        }
        else if(stackOp.type == PolicyStackOperationType::SPECIFIC_RELATION) {
            //add the relation id to the output
            if(stackOp.relationId == -2)  //stack end delimiter
                asString = string(WHITE) + "0" + RESET + asString;
            else //a redundant (specific) relation
                asString = string(WHITE) + "1" + uintToString(stackOp.relationId, relationSet.bitsForSpecificRelationId) + RESET + asString;
            //add the relation type (normal or redundant) to the output
            cout << YELLOW << " SPEC_REL:" << bitString << RESET; //DEBUG
            asString = string(YELLOW) + bitString + RESET + asString;
        }
    #endif
}

void PolicyStack::addEndDelimiter()  {
    //end delimiter: a specific relation with a 1 bit value of 0, s.t. all normal specific relation ids must start with a 1
    StackOperation stackOp;
    stackOp.type = PolicyStackOperationType::SPECIFIC_RELATION;
    stackOp.relationId = -2;
    push_back(stackOp);
}

ostream& operator<<(ostream &out, PolicyStack &policyStack) {
    #if DEBUG_POLICY_GENERATION
        out << policyStack.asString;
    #endif
    return out;
}

PolicyStackOperationType PolicyStack::get()  {
    PolicyStackOperationType type = intToOperationType.at(policyBinary.next(2));

    #if DEBUG_POLICY_EVALUATION
        cout << "Operation: " << uintToString(operationTypeToInt[type], bitsForStackOperation) << endl;
    #endif

    return type;
}

void PolicyStack::parseStack()  {
    uint64_t savedPosition = policyBinary.getPosition(); //used to restore the stack position
    numberOfRelations = 0;

    StackOperation stackOp;
    PolicyStackOperationType opType;
    while(true)  { //get the next operation from the stack
        stackOp.relationId = -1; //DEBUG
        opType = get();
        if(opType == PolicyStackOperationType::AND)
            stackOp.type = PolicyStackOperationType::AND;
        else if(opType == PolicyStackOperationType::OR)
            stackOp.type = PolicyStackOperationType::OR;
        else if(opType == PolicyStackOperationType::NEXT_RELATION)  { //count the relations s.t. we can determine the relation set size afterwards
            stackOp.type = PolicyStackOperationType::NEXT_RELATION;
            stackOp.relationId = -1; //also store the relation id for this NEXT_RELATION for an easier execution
            numberOfRelations++;
        }
        else if(opType == PolicyStackOperationType::SPECIFIC_RELATION)  {
            uint8_t firstBit = policyBinary.next(1); 
            if(firstBit == 0) //end delimiter relation id
                break; //just break on end delimiter
            else  { //specific relation
                stackOp.type = PolicyStackOperationType::SPECIFIC_RELATION;
                stackOp.relationId = policyBinary.next(relationSet.bitsForSpecificRelationId); //also store the relation id for this NEXT_RELATION for an easier execution
            }
        }
        else
            throw "Stack operation not implemented!";

        m_policyStack.push(stackOp);
    }

    relationSetOffset = policyBinary.getPosition(); //since we are at the end of the stack
    policyBinary.setPosition(savedPosition); //restore the stack position
}

uint64_t PolicyStack::getRelationSetOffset()  {
    return relationSetOffset;
}

uint64_t PolicyStack::getNumberOfRelations()  {
    return numberOfRelations;
}

bool PolicyStack::processStack(RelationSet &relationSet)  { //XXX add better error handling
    #if DEBUG_POLICY_EVALUATION
        cout << "Processing the evaluation stack..." << endl;
    #endif
    uint64_t nextRelationCounter = 0;
    //std::stack<bool> resultStack;
    StackOperation stackOp;
	stack<StackOperation> policyStack = m_policyStack;

    while(!policyStack.empty())  {
        stackOp = policyStack.top();

        if(stackOp.type == PolicyStackOperationType::AND)  {
            //get the two lastest booleans added to the result stack
            /*bool latestResult = resultStack.top();
            resultStack.pop();
            bool secondLatestResult = resultStack.top();
            bool result = latestResult & secondLatestResult;
            #if DEBUG_POLICY_EVALUATION
                cout << latestResult << " AND " << secondLatestResult << " so storing " << result << " on the stack" << endl;
            #endif
            resultStack.pop();
            resultStack.push(result);*/
			policyStackProcessor.addStackOperation(PolicyStackProcessorNodeType::AND);
        }
        else if(stackOp.type == PolicyStackOperationType::OR)  {
            //get the two lastest booleans added to the result stack
            /*bool latestResult = resultStack.top();
            resultStack.pop();
            bool secondLatestResult = resultStack.top();
            bool result = latestResult | secondLatestResult;
            #if DEBUG_POLICY_EVALUATION
                cout << latestResult << " OR " << secondLatestResult << " so storing " << result << " on the stack" << endl;
            #endif
            resultStack.pop();
            resultStack.push(result);*/
			policyStackProcessor.addStackOperation(PolicyStackProcessorNodeType::OR);
        }
        else if(stackOp.type == PolicyStackOperationType::NEXT_RELATION)  {
            stackOp.relationId = nextRelationCounter;
            nextRelationCounter++; //increase the counter s.t. the next NEXT_RELATION symbol uses the right id
            bool result = relationSet.getRelationResult(stackOp.relationId);
            #if DEBUG_POLICY_EVALUATION
                cout << "storing relation (" << stackOp.relationId << ") -> " << result << " on the stack" << endl;
            #endif
            //resultStack.push(result);
			policyStackProcessor.addStackOperation(PolicyStackProcessorNodeType::RELATION, result, stackOp.relationId);
        }
        else if(stackOp.type == PolicyStackOperationType::SPECIFIC_RELATION)  {
            bool result = relationSet.getRelationResult(stackOp.relationId);
            #if DEBUG_POLICY_EVALUATION
                cout << "storing relation (" << stackOp.relationId << ") -> " << result << " on the stack" << endl;
            #endif
           // resultStack.push(result);
		   policyStackProcessor.addStackOperation(PolicyStackProcessorNodeType::RELATION, result, stackOp.relationId);
        }
        else
            throw "StackOperationType not implemented!";

        policyStack.pop();
    }

    //assert(resultStack.size() == 1);
    //return resultStack.top();
	return policyStackProcessor.getResult();
}

void PolicyStack::store()  {
    stackSize = 0;
    StackOperation stackOp;
	stack<StackOperation> policyStack = m_policyStack;

    while(!policyStack.empty())  {
        stackOp = policyStack.top();

        //add the stack operation type to the binary
        policyBinary.push_back(operationTypeToInt[stackOp.type], bitsForStackOperation);

        //Eval for PolicyStack size output
        #if EVALUATION_OUTPUT 
            stackSize += bitsForStackOperation;
        #endif

        //store the relation id for a specific id
        //a special case of this is the stack end delimiter which uses the specific relation
        if(stackOp.type == PolicyStackOperationType::SPECIFIC_RELATION)  {
            if(stackOp.relationId == -2)  { //stack end delimiter (size is 1 bit, not spec. rel id size)
                policyBinary.push_back(0, 1);
                //eval for PolicyStack size output
                #if EVALUATION_OUTPUT 
                    stackSize += 1;
                #endif
            }
            else  { //specific relation id with bitForSpecificRelationId size
                policyBinary.push_back(1, 1); //indicates that a spec relation id is coming not the end delimiter
                //eval for PolicyStack size output
                #if EVALUATION_OUTPUT 
                    stackSize += 1;
                #endif
               policyBinary.push_back(stackOp.relationId, relationSet.bitsForSpecificRelationId);
                //Eval for PolicyStack size output
                #if EVALUATION_OUTPUT 
                    stackSize += relationSet.bitsForSpecificRelationId;
                #endif
            }
        }

        policyStack.pop();
    }
}

//print the size of the policy stack
void PolicyStack::printSize()  {
    //DEBUG output for PolicyStack size
    cout << "Policy Stack size: " << stackSize << endl;
}
