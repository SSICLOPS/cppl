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

#include "json/json.h"
#include <fstream>
#include <vector>
#include <cmath>

#include "ast.hh"
#include "variable_set.hh"

#include <string>
#include <map>
#include <stdint.h>

using namespace std;

enum class VariableSetType;
union VariableSetValue;
struct Variable;

typedef pair<uint64_t, uint64_t> FunctionEnumPair;

class PolicyDefinition  {
    public:
        //populates the varaibles with default values
        PolicyDefinition(string definitionString);

		~PolicyDefinition();
      
        //used for generation
        //get the ID type as defined in the policy definition
        AstValueType getIdType(string name);

        //get the ID placeholder 
        // - used to compresses the space needed for the ID in the binary representation
        uint16_t getIdPosition(string name);

        //get the name for an ID
        string getIdName(uint64_t id);

        //loads the values for the variables from an json file
        void loadVariableValues(string variablesJsonString);

        //loads the values for the runtime variables from an json file (used in function handler)
        void loadRuntimeVariableValues(string runtimeVariablesJsonString);

        //used for functions
        uint64_t getNumberOfFunctionParameters(uint64_t id);
        VariableSetType getFunctionParameterType(uint64_t id, uint64_t paramPosition);
        bool isFunctionEnumParameter(uint64_t id, uint64_t paramPosition);

        //used for evaluation
        VariableSetType  getIdType(uint64_t id);
        VariableSetValue getIdValue(uint64_t id);
        VariableSetValue getIdValue(string name);
        VariableSetValue getRuntimeValue(string name);
        bool isEnumId(uint64_t id);
        VariableSetValue getEnumValue(uint64_t id, uint64_t enumValuePosition);
        VariableSetValue getFunctionEnumValue(uint64_t funcId, uint64_t paramId, uint64_t enumValuePosition);
        int32_t getEnumValuePosition(uint64_t id, Variable var);
        int32_t getFunctionEnumValuePosition(uint64_t funcId, uint64_t paramId, Variable var);
        uint8_t getBitsForEnumValuePosition(uint64_t id);
        uint8_t getBitsForFunctionEnumValuePosition(uint64_t funcId, uint64_t paramId);

        uint16_t getVersion() { return version; }

        //parses the variables object of the policy definition json
        //by recursion s.t. nested variable containers are supported
        void parseJsonVariableList(Json::Value element);

        //how many bits are needed to represent all variables of the policy definition
        uint8_t bitsForVariablePosition;

    private:
        vector<Variable> variables;
        uint16_t version;
        string currentContainerPrefix;

        //stores the runtime variables by their name, initalized by loadRuntimeVariableValues
        map<string, Variable> runtimeVariables;

        //map used to get the position of a variable inside the variableList by its name
        map<string, uint32_t> variablePositions;
        map<uint32_t, string> variableNames;
        uint32_t variableCounter;

        map<uint64_t, vector<Variable>> enumValues;
        map<FunctionEnumPair, vector<Variable>> functionEnumValues;
        map<uint64_t, vector<Variable>> functionParameters;
};
