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

#include "variable_set.hh"

void VariableSet::clear(){
	_deleteVariableList(variables);
}

void VariableSet::_deleteVariableList(vector<Variable> & v){
	for (vector<Variable>::iterator it = v.begin(); it != v.end(); ++it){
		if (it->type == VariableSetType::STRING && it->value.asString != NULL)
			delete it->value.asString;
		else if (it->isFunction()){
			_deleteVariableList(*(it->funcParams));
			delete it->funcParams;
		}
	}
}

ostream& operator<<(ostream &out, VariableSet &variableSet) {
    #if DEBUG_POLICY_GENERATION
        out << variableSet.asString;
    #endif
    return out;
}

void VariableSet::addVariableForDebug(Variable &variable, bool isFunctionParam)  {
	//DEBUG output
	#if DEBUG_POLICY_GENERATION
		if(isFunctionParam == false) //function params have no leading type (known by policy definition)
			asString = asString + "\n" + string(MAGENTA) + uintToString(variableTypeToInt[variable.type], bitsForVariableSetType) + RESET;

		if(variable.type == VariableSetType::ID)
			asString = asString + string(YELLOW) + uintToString(variable.value.asInt32, policyDefinition.bitsForVariablePosition) + RESET;
		else if(variable.type == VariableSetType::BOOLEAN)
			asString = asString + string(BLUE) + uintToString(variable.value.asBoolean, 1) + RESET;
        else if(variable.type == VariableSetType::STRING)  {
            //add the string as bit representation to the debug output
            string bitString;
            for(ssize_t i = 0; i < variable.value.asString->length(); i++)  {
                bitString = bitString + uintToString(variable.value.asString->at(i), 8);
            }
            bitString = bitString + uintToString(0, 8); //add also the null termination
            asString = asString + string(RED) + bitString + RESET;
        }
        else if(variable.type == VariableSetType::ENUM_VALUE)  {
            asString = asString + string(CYAN) + uintToString(variable.value.asInt32, variable.bitsForEnumValuePosition) + RESET;
        }
        else if(variable.type == VariableSetType::FUNCTION)  {
            asString = asString + string(YELLOW) + uintToString(variable.value.asInt32, policyDefinition.bitsForVariablePosition) + RESET;
            for(vector<Variable>::iterator it = variable.funcParams->begin(); it != variable.funcParams->end(); ++it)
                addVariableForDebug(*it, true); //add func params without leading type (known by policy def)
        }
        //signed integers
        else if(variable.type == VariableSetType::INT64)
            asString = asString + string(GREEN) + uintToString(variable.value.asInt64, 64) + RESET;
        else if(variable.type == VariableSetType::INT32)
            asString = asString + string(GREEN) + uintToString(variable.value.asInt32, 32) + RESET;
        else if(variable.type == VariableSetType::INT16)
            asString = asString + string(GREEN) + uintToString(variable.value.asInt16, 16) + RESET;
        else if(variable.type == VariableSetType::INT8)
            asString = asString + string(GREEN) + uintToString(variable.value.asInt8, 8)   + RESET;
        //unsigned integers
        else if(variable.type == VariableSetType::UINT32)
            asString = asString + string(GREEN) + uintToString(variable.value.asUInt32, 32) + RESET;
        else if(variable.type == VariableSetType::UINT16)
            asString = asString + string(GREEN) + uintToString(variable.value.asUInt16, 16) + RESET;
        else if(variable.type == VariableSetType::UINT8)
            asString = asString + string(GREEN) + uintToString(variable.value.asUInt8, 8)   + RESET;
        //double
        else if(variable.type == VariableSetType::DOUBLE)
            asString = asString + string(BOLDBLUE) + doubleToString(variable.value.asDouble)   + RESET;
    #endif
}

//adds the variable to the variable set
//returns -1 if the variable is the next inside the variable set
//else it returns an old position inside the variable set that has the same value
int64_t VariableSet::addVariable(Variable variable)  {
    //add the variable to the variable list

    //check if the variable is a duplicate
    int64_t variableId = -1; //next variable is default
    if(variable.type != VariableSetType::BOOLEAN && //using the redundant feature with booleans is not worth it
       !(variable.type == VariableSetType::ID && variable.isEnum)) //do not use specific variables with enum ids, since then we need them in the set to identify the enum values
    { 
        vector<Variable>::iterator varId = find(variables.begin(), variables.end(), variable);
        if(varId != variables.end()) {//is a duplicate
            int64_t originalVarId = distance(variables.begin(), varId);
            //skip the specific var id feature when the relation pos needs more bits than assigned for this feature
            if(originalVarId < (((int64_t) 1) << bitsForSpecificVariableId ))  { 
                variableId = originalVarId; //store the original variable id
            }
        }
    }

    if(variableId == -1) //just add new variables
        variables.push_back(variable);
    else
        return variableId; //don't add specific variables to the debug output

    //DEBUG output for VariableSet
    #if DEBUG_POLICY_GENERATION
        addVariableForDebug(variable);
    #endif

    return variableId;
}

inline VariableSetType VariableSet::getType()  {
    return intToVariableSetType.at(policyBinary.next(bitsForVariableSetType));
}

void VariableSet::getVariableValueFromBinary(Variable &var)  {
    //store every variable inside the internal variable structure
    static uint32_t lastUsedVariableId;

    if(var.type == VariableSetType::BOOLEAN)
        var.value.asBoolean = policyBinary.next(1);
    else if(var.type == VariableSetType::STRING)  {
        string *str = new string();
        char nextChar;
        while((nextChar = policyBinary.next(8)) != '\0')  {
            *str += nextChar; //FIXME directly init from mem (faster)
        }
        var.value.asString = str;
    }
    else if(var.type == VariableSetType::ID)  {
        //insert the actual variable value into the variable set
        uint32_t id = policyBinary.next(policyDefinition.bitsForVariablePosition);
        lastUsedVariableId = id; //store the id s.t. we could use it if an enum value follows

        var.type = policyDefinition.getIdType(id);
        var.value = policyDefinition.getIdValue(id);
		var.idInPolicyDef = id;
    }
    else if(var.type == VariableSetType::ENUM_VALUE)  {
        //an enum value comes always after the related enum id, s.t. we can get the type from it
        var.type = variables.back().type;
        uint8_t bitsForEnumValuePosition = policyDefinition.getBitsForEnumValuePosition(lastUsedVariableId);

        //get the enum position
        uint64_t enumValuePosition = policyBinary.next(bitsForEnumValuePosition);

        //store the actual enum value in the variable set
        var.value = policyDefinition.getEnumValue(lastUsedVariableId, enumValuePosition);
    }
    else if(var.type == VariableSetType::FUNCTION)  {
        //insert the actual variable value into the variable set
        uint32_t functionPosition = policyBinary.next(policyDefinition.bitsForVariablePosition);

        var.type = VariableSetType::BOOLEAN; //currently we are just supporting function with a boolean return type

        //get the function parameters from the policy binary
        var.funcParams = new vector<Variable>();
        for(uint64_t paramPos = 0; paramPos < policyDefinition.getNumberOfFunctionParameters(functionPosition); paramPos++)  {
            Variable paramVar;
            paramVar.type = policyDefinition.getFunctionParameterType(functionPosition, paramPos); //get the type of the param from the policy def.
            if(paramVar.type != VariableSetType::ENUM_VALUE) {//normal function parameter
                getVariableValueFromBinary(paramVar); //get the parameter value from the binary
            }
            else  { //enum function parameter
                paramVar.type = VariableSetType::STRING; //XXX currently only string enum function parameters are implemented
                uint8_t bitsForEnumValuePosition = policyDefinition.getBitsForFunctionEnumValuePosition(functionPosition, paramPos);
                //get the enum position
                uint64_t enumValuePosition = policyBinary.next(bitsForEnumValuePosition);
                //store the actual enum value in the variable set
                paramVar.value = policyDefinition.getFunctionEnumValue(functionPosition, paramPos, enumValuePosition);
            }
            var.funcParams->push_back(paramVar); //store it as function parameter
        }

        //get the in the policy definition defined name for the function
        string functionName = policyDefinition.getIdName(functionPosition);
        //use the function handler to get the result of the function (defined by policy provider)
        var.value.asBoolean = functionHandler.processFunction(functionName, *(var.funcParams));
		var.idInPolicyDef = functionPosition;
    }
    //signed integers
    else if(var.type == VariableSetType::INT64)
        var.value.asInt64 = policyBinary.next(64);
    else if(var.type == VariableSetType::INT32)
        var.value.asInt32 = policyBinary.next(32);
    else if(var.type == VariableSetType::INT16)
        var.value.asInt16 = policyBinary.next(16);
    else if(var.type == VariableSetType::INT8)
        var.value.asInt8  = policyBinary.next(8);
    //unsigned integers
    else if(var.type == VariableSetType::UINT32)
        var.value.asUInt32 = policyBinary.next(32);
    else if(var.type == VariableSetType::UINT16)
        var.value.asUInt16 = policyBinary.next(16);
    else if(var.type == VariableSetType::UINT8)
        var.value.asUInt8  = policyBinary.next(8);
    //double
    else if(var.type == VariableSetType::DOUBLE)  {
        //currently only doubles with 64 bits are supported
        assert(sizeof(double) == sizeof(uint64_t));
        uint64_t valueAsInt = policyBinary.next(64);
        memcpy(&(var.value.asDouble), &valueAsInt, sizeof(double));
    }
    else
        throw "VariableType is not implemented in getVariableValueFromBinary()";
}

//parses the variable set in the binary policy (from offset)
void VariableSet::parse(uint64_t variableSetOffset, uint64_t numberOfVariables)  {
    //go to the right position in the binary policy
    policyBinary.setPosition(variableSetOffset);
    variables.clear(); //ensures that the variable set is empty

    for(uint64_t varId = 0; varId < numberOfVariables; varId++)  {
        Variable var;
        var.type = getType(); //get the variable type

        getVariableValueFromBinary(var);

        variables.push_back(var);
    }

    #if DEBUG_POLICY_EVALUATION
        cout << "Variables: " << endl;
        for(int i = 0; i < variables.size(); i++) {
            Variable var = variables.at(i);
            cout << uintToString(variableTypeToInt[var.type], bitsForVariableSetType) << " ";
            if(var.type == VariableSetType::BOOLEAN)  {
                cout << " Boolean " << var.value.asBoolean << endl;
            }
            else if(var.type == VariableSetType::STRING) {
                cout << " String " << *(var.value.asString) << endl;
            }
            else if(var.type == VariableSetType::ID)  {
                throw "Should now be replaced by the actual type and value!";
            }
            else if(var.type == VariableSetType::FUNCTION)  {
                throw "Should now be replaced by the actual type and value!";
            }
            //signed integers
            else if(var.type == VariableSetType::INT64)
                cout << " Int64 " << var.value.asInt64 << endl;
            else if(var.type == VariableSetType::INT32)
                cout << " Int32 " << var.value.asInt32 << endl;
            else if(var.type == VariableSetType::INT16)
                cout << " Int16 " << var.value.asInt16 << endl;
            else if(var.type == VariableSetType::INT8)
                cout << " Int8 " << (int16_t) var.value.asInt8 << endl;
            //unsigned integers
            else if(var.type == VariableSetType::UINT32)
                cout << " UInt32 " << var.value.asUInt32 << endl;
            else if(var.type == VariableSetType::UINT16)
                cout << " UInt16 " << var.value.asUInt16 << endl;
            else if(var.type == VariableSetType::UINT8)
                cout << " UInt8 " << (uint16_t) var.value.asUInt8 << endl;
            //double
            else if(var.type == VariableSetType::DOUBLE)
                cout << " Double " << var.value.asDouble << endl;
        }
    #endif
}


uint64_t VariableSet::storeVariable(Variable var, bool isFunctionParam)  {
    uint64_t variableSize = 0;

    //only add the variable type, if the constant is not a function parameter
    //for function parameters we know the type via the policy definition
    if(isFunctionParam == false)  {
        //add the variable type to the binary
        policyBinary.push_back(variableTypeToInt[var.type], bitsForVariableSetType);
        //DEBUG for VariableSet size output
        #if EVALUATION_OUTPUT 
            variableSize += bitsForVariableSetType;
        #endif
    }

    //add the variable value to the binary
    if(var.type == VariableSetType::BOOLEAN)  {
        policyBinary.push_back(var.value.asBoolean, 1);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 1;
        #endif
    }
    else if(var.type == VariableSetType::STRING)  {
        //add the string to the bit representation
        for(ssize_t i = 0; i < var.value.asString->length(); i++)  {
            policyBinary.push_back(var.value.asString->at(i), 8);
            //DEBUG for RelationSet size output
            #if EVALUATION_OUTPUT 
                variableSize += 8;
            #endif
        }
        policyBinary.push_back(0, 8); //add also the null termination
        #if EVALUATION_OUTPUT 
            variableSize += 8;
        #endif
    }
    else if(var.type == VariableSetType::ID)  {
        policyBinary.push_back(var.value.asInt32, policyDefinition.bitsForVariablePosition);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += policyDefinition.bitsForVariablePosition;
        #endif
    }
    else if(var.type == VariableSetType::ENUM_VALUE)  {
        policyBinary.push_back(var.value.asInt32, var.bitsForEnumValuePosition);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += var.bitsForEnumValuePosition;
        #endif
    }
    else if(var.type == VariableSetType::FUNCTION)  {
        policyBinary.push_back(var.value.asInt32, policyDefinition.bitsForVariablePosition);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += policyDefinition.bitsForVariablePosition;
        #endif

        //add the function parameters to the variable set
        for(vector<Variable>::iterator it = var.funcParams->begin(); it != var.funcParams->end(); ++it)  {
            uint64_t paramSize = storeVariable(*it, true); //as parameter so without leading type
            //DEBUG for RelationSet size output
            #if EVALUATION_OUTPUT 
                variableSize += paramSize;
            #endif
        }
    }
    //signed integers
    else if(var.type == VariableSetType::INT64)  {
        policyBinary.push_back(var.value.asInt64, 64);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 64;
        #endif
    }
    else if(var.type == VariableSetType::INT32)  {
        policyBinary.push_back(var.value.asInt32, 32);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 32;
        #endif
    }
    else if(var.type == VariableSetType::INT16)  {
        policyBinary.push_back(var.value.asInt16, 16);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 16;
        #endif
    }
    else if(var.type == VariableSetType::INT8)  {
        policyBinary.push_back(var.value.asInt8, 8);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 8;
        #endif
    }
    //unsigned integers
    else if(var.type == VariableSetType::UINT32)  {
        policyBinary.push_back(var.value.asUInt32, 32);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 32;
        #endif
    }
    else if(var.type == VariableSetType::UINT16)  {
        policyBinary.push_back(var.value.asUInt16, 16);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 16;
        #endif
    }
    else if(var.type == VariableSetType::UINT8)  {
        policyBinary.push_back(var.value.asUInt8, 8);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 8;
        #endif
    }
    //double
    else if(var.type == VariableSetType::DOUBLE)  {
        //currently only doubles with 64 bits are supported
        //XXX problem with endianess?
        assert(sizeof(double) == sizeof(uint64_t));
        uint64_t value;
        memcpy(&value, &(var.value.asDouble), 8); //size given in byte
        policyBinary.push_back(value, 64);
        //DEBUG for RelationSet size output
        #if EVALUATION_OUTPUT 
            variableSize += 64;
        #endif
    }
    else
        throw "Unknown VariableSetType in store!"; //SPECIFIC VARIABLE

    return variableSize;
}

void VariableSet::store()  {
    variableSetSize = 0;

    for(uint32_t varId = 0; varId < variables.size(); varId++)  {
        const Variable var = variables[varId];

        //add the variable value to the binary
        uint64_t variableSize = storeVariable(var);
        #if EVALUATION_OUTPUT 
            variableSetSize += variableSize;
        #endif
    }
}

//print the size of the variable set
void VariableSet::printSize()  {
    //DEBUG output for variable set size
    cout << "VariableSet size: " << variableSetSize << endl;
}
