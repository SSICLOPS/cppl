#include "policy_definition.hh"

//string replace from stack overflow
void stringReplace(std::string& str, const std::string& oldStr, const std::string& newStr)  {
    size_t pos = 0;
    while((pos = str.find(oldStr, pos)) != std::string::npos){
        str.replace(pos, oldStr.length(), newStr);
        pos += newStr.length();
    }
}

PolicyDefinition::PolicyDefinition(string definitionString)  {
    //parse the policy defintion JSON string
    Json::Value root;   
    Json::Reader reader;
    if(!reader.parse(definitionString.c_str(), root))
        throw runtime_error("Parsing of policy definition failed: " + reader.getFormattedErrorMessages());

    //get the policy version
    version = root["version"].asInt64();

    //check if there is a variable list
    if(root.isMember("variables") == false)
        throw "Policy definition contains no variables object";

    //fills the variable list
    variableCounter = 0;
    currentContainerPrefix = ""; //ensure that the starting container prefix is empty 
    for(Json::ValueIterator it = root["variables"].begin() ; it != root["variables"].end() ; it++ )
        parseJsonVariableList(*it);

    //calculate how much bits are needed to store the variable positions
    bitsForVariablePosition = ceil(log2(variables.size()));
}

//parses the variables object of the policy definition json
//by recursion s.t. nested variable containers are supported
void PolicyDefinition::parseJsonVariableList(Json::Value element)  {
    if(element.isObject() && element.isMember("name") && element.isMember("type"))  { //is variable object
        Variable var;

        if(element["type"].asString().compare("string") == 0)  {
            var.type = VariableSetType::STRING;
            var.value.asString = new string("undefined");
        }
        else if(element["type"].asString().compare("boolean") == 0)  {
            var.type = VariableSetType::BOOLEAN;
            var.value.asBoolean = false;
        }
        else if(element["type"].asString().compare("function") == 0)  {
            var.type = VariableSetType::FUNCTION;
            var.value.asInt32 = 0;
        }
        //signed integers
        else if(element["type"].asString().compare("int64") == 0)  {
            var.type = VariableSetType::INT64;
            var.value.asInt64 = 0;
        }
        else if(element["type"].asString().compare("int32") == 0)  {
            var.type = VariableSetType::INT32;
            var.value.asInt32 = 0;
        }
        else if(element["type"].asString().compare("int16") == 0)  {
            var.type = VariableSetType::INT16;
            var.value.asInt16 = 0;
        }
        else if(element["type"].asString().compare("int8") == 0)  {
            var.type = VariableSetType::INT8;
            var.value.asInt8 = 0;
        }
        //unsigned integers
        else if(element["type"].asString().compare("uint32") == 0)  {
            var.type = VariableSetType::UINT32;
            var.value.asUInt32 = 0;
        }
        else if(element["type"].asString().compare("uint16") == 0)  {
            var.type = VariableSetType::UINT16;
            var.value.asUInt16 = 0;
        }
        else if(element["type"].asString().compare("uint8") == 0)  {
            var.type = VariableSetType::UINT8;
            var.value.asUInt8 = 0;
        }
        else
            throw "Policy Definition: Unknown variable type";

        //get the parameters for functions
        if(var.type == VariableSetType::FUNCTION)  {
            uint64_t functionParameterCounter = 0; //used as ids for parameters
            if(element.isMember("parameters"))  {
                if(element["parameters"].isArray() == false)
                    throw "Policy Definition: A parameters member of a function is not an array";

                //get and store the types of the parameters for the function
                for(Json::ValueIterator it = element["parameters"].begin() ; it != element["parameters"].end() ; it++)  {
                    Variable parameter;

                    //is the parameter an enum?
                    //XXX currently only function parameter enums with type string are supported
                    if((*it).isArray() == true)  {
                        parameter.isEnum = true;
                        parameter.type = VariableSetType::ENUM_VALUE;
                        //get enum values
                        for(Json::ValueIterator enumIt = (*it).begin() ; enumIt != (*it).end() ; enumIt++)  {
                            if((*enumIt).isString() == false)
                                throw "Policy Definition: A function parameter enum is only implemented for string values";
                            Variable enumVar;            
                            enumVar.isEnum = false;
                            enumVar.type = VariableSetType::STRING;
                            enumVar.value.asString = new string((*enumIt).asString());
                            functionEnumValues[FunctionEnumPair(variableCounter, functionParameterCounter)].push_back(enumVar);
                        }
                    }
                    //non enum function parameters
                    else if((*it).isString() == true)  {
                        parameter.isEnum = false;
                        if((*it).asString().compare("string") == 0)
                            parameter.type = VariableSetType::STRING;
                        else if((*it).asString().compare("boolean") == 0)
                            parameter.type = VariableSetType::BOOLEAN;
                        //signed integers
                        else if((*it).asString().compare("int64") == 0)
                            parameter.type = VariableSetType::INT64;
                        else if((*it).asString().compare("int32") == 0)
                            parameter.type = VariableSetType::INT32;
                        else if((*it).asString().compare("int16") == 0)
                            parameter.type = VariableSetType::INT16;
                        else if((*it).asString().compare("int8") == 0)
                            parameter.type = VariableSetType::INT8;
                        //unsigned integers
                        else if((*it).asString().compare("uint32") == 0)
                            parameter.type = VariableSetType::UINT32;
                        else if((*it).asString().compare("uint16") == 0)
                            parameter.type = VariableSetType::UINT16;
                        else if((*it).asString().compare("uint8") == 0)
                            parameter.type = VariableSetType::UINT8;
                        //double
                        else if((*it).asString().compare("double") == 0)
                            parameter.type = VariableSetType::DOUBLE;
                        else
                            throw "Policy Definition: Unkown function parameter type";
                    }
                    else
                        throw "Policy Definition: Function parameter type must be a string or an enum array";

                    functionParameters[variableCounter].push_back(parameter);
                    functionParameterCounter++;
                }
            }
            else
                throw "Policy Definition: A function has no parameters.";
        }

        //the variable is an enum, if it has an array of values and is not a function
        if(var.type == VariableSetType::FUNCTION && element.isMember("values"))
            throw "Policy Definition: A function can not have enum values.";
        else if(element.isMember("values"))  {
            //check that values is an array
            if(element["values"].isArray() == false)
                throw "Policy Definition: A values member in the policy definition json file is not an array";

            var.isEnum = true;
            //store the values for the enum in the enumValues map by its variable position
            //the position of the values in the array will also be used for the internal enum keys
            for(Json::ValueIterator it = element["values"].begin() ; it != element["values"].end() ; it++)  {
                Variable enumVar;            
                enumVar.isEnum = false;

                if(var.type == VariableSetType::STRING)  {
                    if((*it).isString() == false)
                        throw "Policy Definition: An enum value has not the same type as the defined enum";
                    enumVar.type = VariableSetType::STRING;
                    enumVar.value.asString = new string((*it).asString());
                }
                else if(var.type == VariableSetType::INT64)  {
                    if((*it).isInt() == false)
                        throw "Policy Definition: An enum value has not the same type as the defined enum";
                    enumVar.type = VariableSetType::INT64;
                    enumVar.value.asInt64 = (*it).asInt64();
                }
                else //else the type does not make sence in compination with an enum e.g. boolean
                    throw "Policy Definition: A policy definition enum with this type is not supported";

                enumValues[variableCounter].push_back(enumVar);
            }
        }
        else
            var.isEnum = false;

        variables.push_back(var); //variables will have the same order as in the policy definition

        //store the name for the variable in a map for a fast lookup during the eval
        string prefixedVariableName = currentContainerPrefix + element["name"].asString();
        variableNames[variableCounter] = prefixedVariableName;
        //store also the position of the variable inside the variables map in a seperate map
        //adds the currentContainerPrefix to the variable name e.g. storage.location
        variablePositions[prefixedVariableName] = variableCounter++;
    }
    else if(element.isObject() && element.isMember("name") && element.isMember("variables"))  { //must be a container
        //add the container name as prefix to the following variables
        string containerName = element["name"].asString();
        currentContainerPrefix = currentContainerPrefix + containerName + ".";
        for(Json::ValueIterator it = element["variables"].begin() ; it != element["variables"].end() ; it++)
            parseJsonVariableList(*it); //recursion to allow nested containers
        //clear the current container name from the prefix (clould have more than one container e.g. storage.fde.algo)
        stringReplace(currentContainerPrefix, containerName + ".", "");
    }
    else {
        cerr << "Current element: " << element << endl;
        throw "Policy Definition: Unsupported policy definition element ";
    }
}

//used for generation
AstValueType PolicyDefinition::getIdType(string name)  {
    //check that the name is a valid Id
    if(variablePositions.count(name) != 1)
        throw std::runtime_error(name + " is not defined in the policy definition");
    
    VariableSetType type = getIdType(variablePositions.at(name));

    if(type == VariableSetType::STRING)  {
        return AstValueType::String;
    }
    else if(type == VariableSetType::BOOLEAN)  {
        return AstValueType::Boolean;
    }
    else if(type == VariableSetType::INT64 || //unsigned integers with 64 bits are not supported
            type == VariableSetType::INT32 || type == VariableSetType::UINT32 ||
            type == VariableSetType::INT16 || type == VariableSetType::UINT16 ||
            type == VariableSetType::INT8  || type == VariableSetType::UINT8)
    {
        return AstValueType::Integer;
    }

    throw "Unknown ID type in PolicyDefinition getIdType(string)!";
}

//used for evaluation
VariableSetType PolicyDefinition::getIdType(uint64_t id)  {
    return variables.at(id).type;
}

//uses the id enum values?
bool PolicyDefinition::isEnumId(uint64_t id)  {
    return variables.at(id).isEnum;
}

//used for generation
uint16_t PolicyDefinition::getIdPosition(string name)  {
    return variablePositions.at(name);
}

string PolicyDefinition::getIdName(uint64_t id)  {
    return variableNames.at(id);
}

//used for evaluation
VariableSetValue PolicyDefinition::getIdValue(uint64_t id)  {
    Variable var = variables.at(id);
    return var.value;
}

VariableSetValue PolicyDefinition::getIdValue(string name)  {
    Variable var = variables.at(getIdPosition(name));
    return var.value;
}

VariableSetValue PolicyDefinition::getRuntimeValue(string name)  {
    return runtimeVariables.at(name).value;
}

void PolicyDefinition::loadVariableValues(string variablesJsonString)  {
    //parse the variables JSON string
    Json::Value root;
    Json::Reader reader;
    if(!reader.parse(variablesJsonString.c_str(), root))
        throw runtime_error("Parsing of variables file failed: " + reader.getFormattedErrorMessages());

    for(Json::ValueIterator it = root["variables"].begin() ; it != root["variables"].end() ; it++)  {
        //check that there is a variable in the policy definition with this name
        string variableName = it.key().asString();
        if(variablePositions.count(variableName) != 1)
            throw std::runtime_error("Set variable by variables file: " + variableName + " is not defined in the policy definition");
        const uint32_t variablePosition = variablePositions.at(variableName);

        VariableSetType type = variables[variablePosition].type;
        VariableSetValue value;

        if(type == VariableSetType::STRING)  {
            delete variables[variablePosition].value.asString; //free the old value (always present)
            value.asString = new string((*it).asString());
        }
        else if(type == VariableSetType::BOOLEAN)  {
            value.asBoolean = (*it).asBool();
        }
        //signed integers
        //XXX check if there is enough place
        else if(type == VariableSetType::INT64)  {
            value.asInt64 = (*it).asInt64();
        }
        else if(type == VariableSetType::INT32)  {
            value.asInt32 = (*it).asInt64();
        }
        else if(type == VariableSetType::INT16)  {
            value.asInt16 = (*it).asInt64();
        }
        else if(type == VariableSetType::INT8)  {
            value.asInt8 = (*it).asInt64();
        }
        //unsigned integers
        //XXX check if there is enough place
        else if(type == VariableSetType::UINT32)  {
            value.asUInt32 = (*it).asUInt64();
        }
        else if(type == VariableSetType::UINT16)  {
            value.asUInt16 = (*it).asUInt64();
        }
        else if(type == VariableSetType::UINT8)  {
            value.asUInt8 = (*it).asUInt64();
        }
        else
            throw "Unsupported variable type in variables JSON";

        variables[variablePosition].value = value;
    }
}

void PolicyDefinition::loadRuntimeVariableValues(string runtimeVariablesJsonString)  {
    //parse the variables JSON string
    Json::Value root;
    Json::Reader reader;
    if(!reader.parse(runtimeVariablesJsonString.c_str(), root))
        throw runtime_error("Parsing of runtime variables file failed: " + reader.getFormattedErrorMessages());

    for(Json::ValueIterator it = root["variables"].begin() ; it != root["variables"].end() ; it++)  {
        //check that the runtime variable element is valid
        if((*it).isObject() == false || (*it).isMember("name") == false
           || (*it).isMember("type") == false || (*it).isMember("value") == false)
            throw "Runtime variable is not valid!";

        //get the runtime variable name
        string runtimeVariableName = (*it)["name"].asString();

        //get the type and value
        Variable var;

        if((*it)["type"].asString().compare("string") == 0)  {
            var.type = VariableSetType::STRING;
            var.value.asString = new string((*it)["value"].asString());
        }
        else if((*it)["type"].asString().compare("boolean") == 0)  {
            var.type = VariableSetType::BOOLEAN;
            var.value.asBoolean = (*it)["value"].asBool();
        }
        //signed integers
        else if((*it)["type"].asString().compare("int64") == 0)  {
            var.type = VariableSetType::INT64;
            var.value.asInt64 = (*it)["value"].asInt64();
        }
        else if((*it)["type"].asString().compare("int32") == 0)  {
            var.type = VariableSetType::INT32;
            var.value.asInt32 = (*it)["value"].asInt64();
        }
        else if((*it)["type"].asString().compare("int16") == 0)  {
            var.type = VariableSetType::INT16;
            var.value.asInt16 = (*it)["value"].asInt64();
        }
        else if((*it)["type"].asString().compare("int8") == 0)  {
            var.type = VariableSetType::INT8;
            var.value.asInt8 = (*it)["value"].asInt64();
        }
        //unsigned integers
        else if((*it)["type"].asString().compare("uint32") == 0)  {
            var.type = VariableSetType::UINT32;
            var.value.asUInt32 = (*it)["value"].asInt64();
        }
        else if((*it)["type"].asString().compare("uint16") == 0)  {
            var.type = VariableSetType::UINT16;
            var.value.asUInt16 = (*it)["value"].asInt64();
        }
        else if((*it)["type"].asString().compare("uint8") == 0)  {
            var.type = VariableSetType::UINT8;
            var.value.asUInt8 = (*it)["value"].asInt64();
        }
        else
            throw "Policy Definition: Unknown runtime variable type in runtime variables JSON file";

        //add the runtime variable to the runtime variables list
        runtimeVariables[runtimeVariableName] = var;
    }
}

int32_t PolicyDefinition::getEnumValuePosition(uint64_t id, Variable var)  {
    vector<Variable> &values = enumValues.at(id);
    vector<Variable>::iterator enumIt = find(values.begin(), values.end(), var);
    if(enumIt != values.end()) //is a duplicate
        return distance(values.begin(), enumIt);
    else
        return -1;
}

uint8_t PolicyDefinition::getBitsForEnumValuePosition(uint64_t id)  {
    return ceil(log2(enumValues.at(id).size()));
}

uint8_t PolicyDefinition::getBitsForFunctionEnumValuePosition(uint64_t funcId, uint64_t paramId)  {
    return ceil(log2(functionEnumValues.at(FunctionEnumPair(funcId, paramId)).size()));
}

VariableSetValue PolicyDefinition::getEnumValue(uint64_t id, uint64_t enumValuePosition)  {
    return enumValues.at(id).at(enumValuePosition).value;
}

VariableSetValue PolicyDefinition::getFunctionEnumValue(uint64_t funcId, uint64_t paramId, uint64_t enumValuePosition)  {
    return functionEnumValues.at(FunctionEnumPair(funcId, paramId)).at(enumValuePosition).value;
}

uint64_t PolicyDefinition::getNumberOfFunctionParameters(uint64_t id)  {
    return functionParameters.at(id).size();
}

VariableSetType PolicyDefinition::getFunctionParameterType(uint64_t id, uint64_t paramPosition)  {
    return functionParameters.at(id).at(paramPosition).type;
}

bool PolicyDefinition::isFunctionEnumParameter(uint64_t id, uint64_t paramPosition)  {
    return functionParameters.at(id).at(paramPosition).isEnum;
}

int32_t PolicyDefinition::getFunctionEnumValuePosition(uint64_t funcId, uint64_t paramId, Variable var)  {
    vector<Variable> &values = functionEnumValues.at(FunctionEnumPair(funcId, paramId));
    vector<Variable>::iterator enumIt = find(values.begin(), values.end(), var);
    if(enumIt != values.end()) //is a duplicate
        return distance(values.begin(), enumIt);
    else
        return -1;
}
