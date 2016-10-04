#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

#include "binary.hh"
#include "ast.hh"
#include "policy_definition.hh"
#include "colors.hh"

#include "debug.hh"
#include "options.hh"

#include "function_handler.hh"

class PolicyDefinition;
class FunctionHandler;

using namespace std;

enum class VariableSetType {
    BOOLEAN,
    STRING,
    INT64,
    INT32,
    INT16,
    INT8,
    UINT32,
    UINT16,
    UINT8,
    DOUBLE,
    ID,
    ENUM_VALUE,
    FUNCTION
};

union VariableSetValue {
    bool asBoolean;
    string *asString; //XXX mem leak?
    int64_t asInt64;
    int32_t asInt32;
    int16_t asInt16;
    int8_t  asInt8;
    uint64_t asUInt64;
    uint32_t asUInt32;
    uint16_t asUInt16;
    uint8_t  asUInt8;
    double asDouble;
};

struct Variable  {
    VariableSetType type;
    VariableSetValue value;
    bool isEnum;
    vector<Variable> *funcParams; //only used when the variable set element is a function
    uint8_t bitsForEnumValuePosition; //only used if a enum value

    //returns true if the variable type represents an integer; false otherwise
    bool isInteger()  {
        if(type == VariableSetType::INT64 ||
           type == VariableSetType::INT32 ||
           type == VariableSetType::INT16 ||
           type == VariableSetType::INT8 ||
           type == VariableSetType::UINT32 ||
           type == VariableSetType::UINT16 ||
           type == VariableSetType::UINT8)
        {
            return true;
        }

        return false;
    }

    //used to compare if two Variable structures are equal in all properties
    friend bool operator==(const Variable &lhs, const Variable &rhs)
    {
        if(lhs.type != rhs.type || lhs.isEnum != rhs.isEnum) //check the types and isEnum first
            return false;
        //since the types matches, we now need to check the values
        //boolean
        if(lhs.type == VariableSetType::BOOLEAN)
            return lhs.value.asBoolean == rhs.value.asBoolean;
        //signed integers
        else if(lhs.type == VariableSetType::INT64)
            return lhs.value.asInt64 == rhs.value.asInt64;
        else if(lhs.type == VariableSetType::INT32)
            return lhs.value.asInt32 == rhs.value.asInt32;
        else if(lhs.type == VariableSetType::INT16)
            return lhs.value.asInt16 == rhs.value.asInt16;
        else if(lhs.type == VariableSetType::INT8)
            return lhs.value.asInt8 == rhs.value.asInt8;
        //unsigned integers
        else if(lhs.type == VariableSetType::UINT32)
            return lhs.value.asUInt32 == rhs.value.asUInt32;
        else if(lhs.type == VariableSetType::UINT16)
            return lhs.value.asUInt16 == rhs.value.asUInt16;
        else if(lhs.type == VariableSetType::UINT8)
            return lhs.value.asUInt8 == rhs.value.asUInt8;
        //DOUBLE 
        else if(lhs.type == VariableSetType::DOUBLE)
            return lhs.value.asDouble == rhs.value.asDouble;
        //ID
        else if(lhs.type == VariableSetType::ID)
            return lhs.value.asInt32 == rhs.value.asInt32;
        //string
        else if(lhs.type == VariableSetType::STRING)
            return (lhs.value.asString)->compare(*(rhs.value.asString)) == 0;
        //function
        else if(lhs.type == VariableSetType::FUNCTION)  {
            //check thats the same function
            if(lhs.value.asInt32 != rhs.value.asInt32)
                return false;
            //both must have the same amount of parameters
            if(lhs.funcParams->size() != rhs.funcParams->size())
                return false;
            //check now that every pair of function parameters match
            for(uint64_t paramPos = 0; paramPos < lhs.funcParams->size(); paramPos++)  {
                if((lhs.funcParams->at(paramPos) == rhs.funcParams->at(paramPos)) == false)
                    return false;
            }
            return true;
        }
        //enum value
        else if(lhs.type == VariableSetType::ENUM_VALUE)  {
            //cout << "ENUM_VALUE in compare" << endl;
            return false;
        }
        else
            throw "Not implemented Variable compare type!";
    }
};

//stores a vector of all binary relations for the policy
class VariableSet {
    public:
        //const for the relation set that indicates how much bits are needed to encode the variable set
        const static uint8_t bitsForVariableSetType = VARIABLE_TYPE_LEN_IN_BITS;

        //inits the operation type to bit string map
        VariableSet(PolicyDefinition &policyDefinition, FunctionHandler &funcHandler, Binary &policyBinary)
            : policyDefinition(policyDefinition), functionHandler(funcHandler), policyBinary(policyBinary)
        {
            variableTypeToInt[VariableSetType::BOOLEAN] = 0;
            intToVariableSetType[0] = VariableSetType::BOOLEAN;

            variableTypeToInt[VariableSetType::ID] = 1;
            intToVariableSetType[1] = VariableSetType::ID;

            variableTypeToInt[VariableSetType::STRING] = 2;
            intToVariableSetType[2] = VariableSetType::STRING;

            variableTypeToInt[VariableSetType::ENUM_VALUE] = 3;
            intToVariableSetType[3] = VariableSetType::ENUM_VALUE;

            variableTypeToInt[VariableSetType::FUNCTION] = 4;
            intToVariableSetType[4] = VariableSetType::FUNCTION;

            //signed integers
            variableTypeToInt[VariableSetType::INT64] = 5;
            intToVariableSetType[5] = VariableSetType::INT64;

            variableTypeToInt[VariableSetType::INT32] = 6;
            intToVariableSetType[6] = VariableSetType::INT32;

            variableTypeToInt[VariableSetType::INT16] = 7;
            intToVariableSetType[7] = VariableSetType::INT16;

            variableTypeToInt[VariableSetType::INT8] = 8;
            intToVariableSetType[8] = VariableSetType::INT8;

            //unsigned integers
            variableTypeToInt[VariableSetType::UINT32] = 9;
            intToVariableSetType[9] = VariableSetType::UINT32;

            variableTypeToInt[VariableSetType::UINT16] = 10;
            intToVariableSetType[10] = VariableSetType::UINT16;

            variableTypeToInt[VariableSetType::UINT8] = 11;
            intToVariableSetType[11] = VariableSetType::UINT8;

            //double
            variableTypeToInt[VariableSetType::DOUBLE] = 12;
            intToVariableSetType[12] = VariableSetType::DOUBLE;

            //DEBUG output
            #if DEBUG_POLICY_GENERATION
                //legend for the DEBUG output shows for what the colors stand
                legend = string(MAGENTA) + "VarType 0000:boolean 0001:ID 0010:string 0011:enumValue 0100:function " +
                                                   "0101:int64 0110:int32 0111:int16 1000:int8 " +
                                                   "1001:uint32 1010:uint16 1011:uint8 1100:double " +
                         string(BLUE) + "boolean " + string(YELLOW) + "IdPosition " + string(RED) + "string " + string(CYAN) + "enumValuePosition " + string(GREEN) + "int " + 
                         string(BOLDBLUE) + "double" + RESET;
            #endif
        }

        //add the variable to the variable set
        //returns the offset from the start of the variable set to this new variable 
        int64_t addVariable(Variable variable);
        void addVariableForDebug(Variable &variable, bool isFunctionParam = false);

        friend ostream& operator<<(ostream &out, VariableSet &variableSet);

        //DEBUG output
        #if DEBUG_POLICY_GENERATION
            //color coded string of the policy
            string asString;
            string legend;
        #endif

        //parses the variable set in the binary policy (from offset)
        void parse(uint64_t variableSetOffset, uint64_t numberOfVariables);

        //get the variable value based on its type from the binary
        void getVariableValueFromBinary(Variable &var);

        //stores the variable set in the binary
        void store();

        //get the next $bitsForVariableSetType bits from the binary policy and interprets them as relation set type
        VariableSetType getType();

        //contains the variables of the set
        vector<Variable> variables;

        //print the calculated relation set size
        void printSize();

        //consts for the specific variable id feature
        static const uint8_t bitsForSpecificVariableId = SPECIFIC_VARIABLE_ID_LEN_IN_BITS; //additionally +1 bit is used, to indicate that a specific variable

    private:
        PolicyDefinition &policyDefinition;
        FunctionHandler &functionHandler;
        Binary &policyBinary;

        uint64_t storeVariable(Variable var, bool isFunctionParam = false);

        //map that stores for each operation type it bit sequence as string
        //e.g. VariableType::BOOLEAN -> "00" (MSB left)
        map<VariableSetType, uint64_t> variableTypeToInt;
        map<uint64_t, VariableSetType> intToVariableSetType;

        //used for the variable set calculation
        uint64_t variableSetSize = 0;
};
