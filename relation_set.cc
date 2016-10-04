#include "relation_set.hh"

void RelationSet::push_back(Relation relation)  {
    //store the relation in relations
    relations.push_back(relation);

    //DEBUG output for RelationSet
    #if DEBUG_POLICY_GENERATION
        #if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
            if(relation.type == RelationSetType::EQUAL &&
               relation.variableIds[0] == -1 &&
               relation.variableIds[1] == -1)
            {
                asString = asString + "\n" + string(WHITE) + "0" + RESET;
            }
            else  {
                asString = asString + "\n" + string(WHITE) + "1" + string(MAGENTA) + uintToString(relationTypeToInt[relation.type], bitsForRelationType) + RESET;
        #else
                asString = asString + "\n" + string(WHITE) + string(MAGENTA) + uintToString(relationTypeToInt[relation.type], bitsForRelationType) + RESET;
        #endif
                uint8_t numberOfSides = 2; //default: binary operation
                //IS_TRUE and IS_FALSE only have one child
                if(relation.type == RelationSetType::IS_TRUE ||
                   relation.type == RelationSetType::IS_FALSE)
                    numberOfSides = 1;
                for(uint8_t side = 0; side < numberOfSides; side++)  {
                    if(relation.variableIds[side] == -1)
                        asString = asString + string(GREEN) + "0" + RESET;
                    else 
                        asString = asString + string(YELLOW) + "1" + uintToString(relation.variableIds[side], variableSet.bitsForSpecificVariableId) + RESET;
                }
        #if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
            }
        #endif
    #endif
}

ostream& operator<<(ostream &out, RelationSet &relationSet) {
    #if DEBUG_POLICY_GENERATION
        out << relationSet.asString;
    #endif
    return out;
}

//converts the AstConstant to a Variable based on the defined type
//s.t. it fails if the AstConstant type and specified type do not match
Variable RelationSet::toVariableByType(AstConstant &constant, VariableSetType type)  {
    Variable var;
    var.isEnum = false;

    if(constant.type == AstValueType::Integer)  {
        //get the value as the maximal type possible
        int64_t value = boost::get<int64_t>(constant.value);
        if(type == VariableSetType::INT64) {
            var.type = VariableSetType::INT64;
            var.value.asInt64 = value;
        }
        else if(type == VariableSetType::INT32) {
            var.type = VariableSetType::INT32;
            var.value.asInt32 = value;
        }
        else if(type == VariableSetType::INT16) {
            var.type = VariableSetType::INT16;
            var.value.asInt16 = value;
        }
        else if(type == VariableSetType::INT8) {
            var.type = VariableSetType::INT8;
            var.value.asInt8 = value;
        }
        else if(type == VariableSetType::UINT32) {
            var.type = VariableSetType::UINT32;
            var.value.asUInt32 = value;
        }
        else if(type == VariableSetType::UINT16) {
            var.type = VariableSetType::UINT16;
            var.value.asUInt16 = value;
        }
        else if(type == VariableSetType::UINT8) {
            var.type = VariableSetType::UINT8;
            var.value.asUInt8 = value;
        }
        else {
            throw "Defined type is not implemented in toVariableByType()!";
        }
    }
    else if(constant.type == AstValueType::Float && type == VariableSetType::DOUBLE)  {
        var.type = VariableSetType::DOUBLE;
        var.value.asDouble = boost::get<double>(constant.value);
    }
    else if(constant.type == AstValueType::Boolean && type == VariableSetType::BOOLEAN)  {
        var.type = VariableSetType::BOOLEAN;
        var.value.asBoolean = boost::get<bool>(constant.value);
    }
    else if(constant.type == AstValueType::String && type == VariableSetType::STRING)  {
        var.type = VariableSetType::STRING;
        var.value.asString = &(boost::get<string>(constant.value));
    }
    else if(constant.type == AstValueType::EnumValue && type == VariableSetType::ENUM_VALUE)  {
        var.type = VariableSetType::ENUM_VALUE;
        var.value.asInt32 = boost::get<int64_t>(constant.value);
        var.bitsForEnumValuePosition = constant.bitsForEnumValuePosition;
    }
    else
        throw "Unsupported AstValueType and VariableSetType combination in RelationSet::toVariableByType()";

    return var;
}

//converts the AstConstant to a Variable
//also compresses integer variables to their minimal storage type
Variable RelationSet::toVariable(AstConstant &constant)  {
    if(constant.type == AstValueType::Integer)  {
        //get the value as the maximal type possible
        int64_t value = boost::get<int64_t>(constant.value);
        //then compress it to a smaller one based on its value
        if(value < 0)  { //use signed integers
            if(value < -2147483648) //use 64 bits
                return toVariableByType(constant, VariableSetType::INT64);
            else if(value < -32768) //use 32 bits
                return toVariableByType(constant, VariableSetType::INT32);
            else if(value < -128) //use 16 bits
                return toVariableByType(constant, VariableSetType::INT16);
            else //use 8 bits
                return toVariableByType(constant, VariableSetType::INT8);
        }
        else  { //use unsigned integers, but only if it fits into an UINT32 or a smaller type else INT64 is used
            if(value > 9223372036854775807) //use 64 bits
                throw "The value can't be stored, since unsigned integers with 64 bits are not supported! Maximal type is signed integer with 64 bits.";
            else if(value > 4294967295) //use 64 bits SIGNED integer for integers that can't be stored in an UINT32
                return toVariableByType(constant, VariableSetType::INT64);
            else if(value > 65535) //use 32 bits
                return toVariableByType(constant, VariableSetType::UINT32);
            else if(value > 255) //use 16 bits
                return toVariableByType(constant, VariableSetType::UINT16);
            else //use 8 bits
                return toVariableByType(constant, VariableSetType::UINT8);
        }
    }
    else if(constant.type == AstValueType::Float)
        return toVariableByType(constant, VariableSetType::DOUBLE);
    else if(constant.type == AstValueType::Boolean)
        return toVariableByType(constant, VariableSetType::BOOLEAN);
    else if(constant.type == AstValueType::String)
        return toVariableByType(constant, VariableSetType::STRING);
    else if(constant.type == AstValueType::EnumValue)
        return toVariableByType(constant, VariableSetType::ENUM_VALUE);
    else
        throw "Undefined AstValueType in RelationSet::toVariable()";
}

void RelationSet::addRelationElement(AstConstant &constant)  {
    addVariable(toVariable(constant));
}

void RelationSet::addRelationElement(AstId &id)  {
    Variable var;
    var.type = VariableSetType::ID;
    var.value.asInt32 = id.position; //XXX okay?
    var.isEnum = id.isEnum;
    addVariable(var);
}

void RelationSet::addRelationElement(AstFunction &function)  {
    Variable var;
    var.type = VariableSetType::FUNCTION;
    var.value.asInt32 = function.position; //XXX okay?
    var.isEnum = false;
    var.funcParams = new vector<Variable>();
    //also add the function parameters
    uint32_t paramPos = 0;
    for(vector<AstConstant *>::iterator it = function.parameters.begin(); it != function.parameters.end(); ++it)  {
        var.funcParams->push_back(toVariableByType(*(*it), policyDefinition.getFunctionParameterType(function.position, paramPos))); //convert the AST constant node to a variable and store it inside the func as param
        paramPos++; //increase the paramter position counter
    }
    addVariable(var);
}

void RelationSet::addVariable(Variable var)  {
    //add the variable to the variable set
    int32_t position = variableSet.addVariable(var);

    //add the variable position to the relation RHS or LHS, but only if it is not a function parameter
    if(currentRelation.variableIds[0] == -2) //LHS of current rel is free
        currentRelation.variableIds[0] = position;
    else if(currentRelation.variableIds[1] == -2) //RHS of current rel is free
        currentRelation.variableIds[1] = position;
    else
        throw "Should not happen that no relation side is free!";
}

int64_t RelationSet::addType(RelationSetType relType)  {
    //store the relation in relations
    //the variable ids are set before the type
    currentRelation.type = relType;

    //check if the relation is a duplicate of a previous one
    int64_t relationId = -1;
    uint64_t variableCounter = 0; //a -1 variable id says that the next var is used s.t. we now need to get the acutal value
    for(uint64_t compareId = 0; compareId < relations.size(); compareId++)  {
        //get the real variable ids, instead of the -1 -> NEXT_VARIABLE
        const uint64_t lhsVarId = relations[compareId].variableIds[0] == -1 ? variableCounter++ : relations[compareId].variableIds[0];
        if(relations[compareId].type == RelationSetType::IS_TRUE || relations[compareId].type == RelationSetType::IS_FALSE)  {
            if(currentRelation.type == relations[compareId].type &&
               currentRelation.variableIds[0] == lhsVarId)
            {
                if(compareId < (((int64_t) 1) << bitsForSpecificRelationId))  { //just use the spec rel feature when the id can be stored inside the given space
                    relationId = compareId; //current relation is a dup of the current loop relation
                    break; //stop, since we found that the relation is a duplicate
                }
            }
        }
        else  { //binary operations
            const uint64_t rhsVarId = relations[compareId].variableIds[1] == -1 ? variableCounter++ : relations[compareId].variableIds[1];
            if(currentRelation.type == relations[compareId].type &&
               currentRelation.variableIds[0] == lhsVarId &&
               currentRelation.variableIds[1] == rhsVarId)
            {
                if(compareId < (((int64_t) 1) << bitsForSpecificRelationId))  { //just use the spec rel feature when the id can be stored inside the given space
                    relationId = compareId; //current relation is a dup of the current loop relation
                    break; //stop, since we found that the relation is a duplicate
                }
            }
        }
    }

    if(relationId == -1)  { //it's a new relation
        push_back(currentRelation);
    }

    //clear the current relation variables s.t. they are free for the next relation
    currentRelation.variableIds[0] = -2; //indicates that the variable is unset
    currentRelation.variableIds[1] = -2;

    return relationId;
}

RelationSetType RelationSet::getType()  {
    return intToRelationType.at(policyBinary.next(bitsForRelationType));
}

uint64_t RelationSet::getVariableSetOffset()  {
    return variableSetOffset;
}

uint64_t RelationSet::getNumberOfVariables()  {
    return numberOfVariables;
}

//determines the relation set size and directly processes the relations s.t. their structure is stored in a vector 
//returns the starting offset of the variable set
//can't directly evaluate the relations, since we dont't now the starting offset of the variable set before we determine it
//the relations set size
void RelationSet::parse(uint64_t relationSetOffset, uint64_t numberOfRelations)  {
    uint32_t variableCounter = 0;
    relations.clear(); //ensures that we start a new relation set

    //go to the start of the relation set
    policyBinary.setPosition(relationSetOffset);

    //process every relation inside the relation set
    for(uint16_t relId = 0; relId < numberOfRelations; relId++)  {
        uint8_t numberOfElements = 2; //normally we have binary relations
        Relation relation;

        #if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
            //if the compressed bit (first bit of a relation) is a 0, the relation is a compressed =, next, next
            //else with a 1 a normal relation defintion follows
            uint64_t compressedBit = policyBinary.next(1);
            if(compressedBit == 0)  {
                relation.type = RelationSetType::EQUAL;

                #if DEBUG_POLICY_EVALUATION
                    cout << "Compressed Relation: " << uintToString(relationTypeToInt[relation.type], bitsForRelationType) << " ";
                #endif

                for(uint16_t elementId = 0; elementId < numberOfElements; elementId++)  { 
                    #if DEBUG_POLICY_EVALUATION
                        cout << "NEXT(" << variableCounter << ") ";
                    #endif

                    relation.variableIds[elementId] = variableCounter;
                    variableCounter++;
                }
            }
            else  {
        #endif
                relation.type = getType(); //get the relation type
                #if DEBUG_POLICY_EVALUATION
                    cout << "Uncompressed Relation: " << uintToString(relationTypeToInt[relation.type], bitsForRelationType) << " ";
                #endif

                //we only have one child, if it's a IS_TRUE or IS_FALSE relation
                if(relation.type == RelationSetType::IS_TRUE || relation.type == RelationSetType::IS_FALSE)
                    numberOfElements = 1;
                //now get the LHS and RHS for this relation
                for(uint16_t elementId = 0; elementId < numberOfElements; elementId++)  { 
                    uint64_t firstBit = policyBinary.next(1);
                    if(firstBit == 0)  { //NEXT_VARIABLE
                        #if DEBUG_POLICY_EVALUATION
                            cout << "NEXT(" << variableCounter << ") ";
                        #endif

                        //store the variable position as relation element
                        relation.variableIds[elementId] = variableCounter;

                        variableCounter++; //since it is a new variable
                    }
                    else { //SPECIFIC_VARIABLE
                        if(variableCounter == 0) //check that the first relation is not a SPECIFIC one
                            throw "Invalid policy binary: SPECIFIC_VARIABLE (allows only to step back) is used before any variable is defined!";

                        const uint64_t specVarId = policyBinary.next(variableSet.bitsForSpecificVariableId);
                        relation.variableIds[elementId] = specVarId;
                        #if DEBUG_POLICY_EVALUATION
                            cout << "SPEC_VAR(" << specVarId << ") ";
                        #endif
                    }
                }
        #if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
            }
        #endif

        relations.push_back(relation);

        #if DEBUG_POLICY_EVALUATION
            cout << endl;
        #endif
    }

    numberOfVariables = variableCounter;
    variableSetOffset = policyBinary.getPosition(); //end position of the relation set is the start of the variable set
    #if DEBUG_POLICY_GENERATION | DEBUG_POLICY_EVALUATION
        cout << "VariableSetOffset: " << variableSetOffset << endl;
    #endif
}

//return the Variable value as boost variant
//used for the relation processing
NumberVariant RelationSet::variableToNumberVariant(Variable var)  {
    NumberVariant result;
    //signed integers
    if(var.type == VariableSetType::INT64)
        result = var.value.asInt64;
    else if(var.type == VariableSetType::INT32)
        result = var.value.asInt32;
    else if(var.type == VariableSetType::INT16)
        result = var.value.asInt16;
    else if(var.type == VariableSetType::INT8)
        result = var.value.asInt8;
    //unsigned integers
    else if(var.type == VariableSetType::UINT32)
        result = var.value.asUInt32;
    else if(var.type == VariableSetType::UINT16)
        result = var.value.asUInt16;
    else if(var.type == VariableSetType::UINT8)
        result = var.value.asUInt8;
    else
        throw "Unimplemented type in variableToNumberVariant()!";

    return result;
}

//processes the relations by evaluating the internal relation set structure
//called after parseRelationSet(), since then the variabel set is initialized
void RelationSet::process()  {
    #if DEBUG_POLICY_EVALUATION
        int relationCounter = 0;
    #endif
    for(vector<Relation>::iterator it = relations.begin(); it != relations.end(); ++it) {
        #if DEBUG_POLICY_EVALUATION
            cout << "Processing relation " << relationCounter++ << endl;
        #endif
        Variable variables[2];
        //get the value of the LHS
        variables[0] = variableSet.variables.at(it->variableIds[0]);
        NumberVariant lhsNum;
        if(variables[0].isInteger())
            lhsNum = variableToNumberVariant(variables[0]);
        //get the RHS value, if available
        NumberVariant rhsNum;
        if(it->type != RelationSetType::IS_TRUE && it->type != RelationSetType::IS_FALSE)  { //only if there is a rhs
            variables[1] = variableSet.variables.at(it->variableIds[1]); //rhs
            if(variables[1].isInteger())
                rhsNum = variableToNumberVariant(variables[1]);
        }

        bool relationResult;
        if(it->type == RelationSetType::EQUAL)  {
            if(variables[0].isInteger() && variables[1].isInteger())
                relationResult = (toInt64(lhsNum) == toInt64(rhsNum));
            else if(variables[0].type == VariableSetType::DOUBLE && variables[1].type == VariableSetType::DOUBLE)
                relationResult = (variables[0].value.asDouble == variables[1].value.asDouble);
            else if(variables[0].type == VariableSetType::BOOLEAN && variables[1].type == VariableSetType::BOOLEAN)
                relationResult = (variables[0].value.asBoolean == variables[1].value.asBoolean);
            else if(variables[0].type == VariableSetType::STRING && variables[1].type == VariableSetType::STRING)
                relationResult = ((variables[0].value.asString)->compare(*(variables[1].value.asString)) == 0); //0 if strings are equal
            else
                throw "(EQUAL) Relation processing with these types is not implemented!";
        }
        else if(it->type == RelationSetType::NEQ)  {
            if(variables[0].isInteger() && variables[1].isInteger())
                relationResult = (toInt64(lhsNum) != toInt64(rhsNum));
            else if(variables[0].type == VariableSetType::DOUBLE && variables[1].type == VariableSetType::DOUBLE)
                relationResult = (variables[0].value.asDouble != variables[1].value.asDouble);
            else if(variables[0].type == VariableSetType::BOOLEAN && variables[1].type == VariableSetType::BOOLEAN)
                relationResult = (variables[0].value.asBoolean != variables[1].value.asBoolean);
            else if(variables[0].type == VariableSetType::STRING && variables[1].type == VariableSetType::STRING)
                relationResult = ((variables[0].value.asString)->compare(*(variables[1].value.asString)) != 0); //0 if strings are equal
            else
                throw "(NEQ) Relation processing with these types is not implemented!";
        }
        else if(it->type == RelationSetType::IS_TRUE)  {
            if(variables[0].type == VariableSetType::BOOLEAN)
                relationResult = (variables[0].value.asBoolean == true);
            else
                throw "(IS_TRUE) Relation processing with these types is not implemented!";
        }
        else if(it->type == RelationSetType::IS_FALSE)  {
            if(variables[0].type == VariableSetType::BOOLEAN)
                relationResult = (variables[0].value.asBoolean == false);
            else
                throw "(IS_FALSE) Relation processing with these types is not implemented!";
        }
        else if(it->type == RelationSetType::LESS)  {
            if(variables[0].isInteger() && variables[1].isInteger())
                relationResult = (toInt64(lhsNum) < toInt64(rhsNum));
            else if(variables[0].type == VariableSetType::DOUBLE && variables[1].type == VariableSetType::DOUBLE)
                relationResult = (variables[0].value.asDouble < variables[1].value.asDouble);
            else
                throw "(LESS) Relation processing with these types is not implemented!";
        }
        else if(it->type == RelationSetType::LEQ)  {
            if(variables[0].isInteger() && variables[1].isInteger())
                relationResult = (toInt64(lhsNum) <= toInt64(rhsNum));
            else if(variables[0].type == VariableSetType::DOUBLE && variables[1].type == VariableSetType::DOUBLE)
                relationResult = (variables[0].value.asDouble <= variables[1].value.asDouble);
            else
                throw "(LEQ) Relation processing with these types is not implemented!";
        }
        else if(it->type == RelationSetType::GREATER)  {
            if(variables[0].isInteger() && variables[1].isInteger())
                relationResult = (toInt64(lhsNum) > toInt64(rhsNum));
            else if(variables[0].type == VariableSetType::DOUBLE && variables[1].type == VariableSetType::DOUBLE)
                relationResult = (variables[0].value.asDouble > variables[1].value.asDouble);
            else
                throw "(GREATER) Relation processing with these types is not implemented!";
        }
        else if(it->type == RelationSetType::GEQ)  {
            if(variables[0].isInteger() && variables[1].isInteger())
                relationResult = (toInt64(lhsNum) >= toInt64(rhsNum));
            else if(variables[0].type == VariableSetType::DOUBLE && variables[1].type == VariableSetType::DOUBLE)
                relationResult = (variables[0].value.asDouble >= variables[1].value.asDouble);
            else
                throw "(GEQ) Relation processing with these types is not implemented!";
        }
        else
            throw "Relation type not implemented!";

        it->result = relationResult;
    }

    #if DEBUG_POLICY_EVALUATION
        cout << "Relation result: " << endl;
        uint16_t nr = 0;
        for(vector<Relation>::iterator it = relations.begin(); it != relations.end(); ++it) {
            cout << nr << ": " << (uint16_t) it->result << endl;
            nr++;
        }
    #endif
}

//stores the relation set in the binary
void RelationSet::store()  {
    relationSetSize = 0;

    for(uint32_t relId = 0; relId < relations.size(); relId++)  {
        #if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
            //compress the popular combination =, next, next by a starting 0
            //a starting 1 indicates that a normal relation definition follows
            if(relations[relId].type == RelationSetType::EQUAL &&
               relations[relId].variableIds[0] == -1 &&
               relations[relId].variableIds[1] == -1)
            {
                //store just a 0 for the popular =, next, next combination
                policyBinary.push_back(0, 1);
                //eval RelationSet size output
                #if EVALUATION_OUTPUT 
                    relationSetSize += 1;
                #endif
                continue; //don't execute the following code for normal relations
            }

            //else a normal relations follows s.t.
            //a 1 indicates that a normal relation definition follows
            policyBinary.push_back(1, 1);

            //eval RelationSet size output
            #if EVALUATION_OUTPUT 
                relationSetSize += 1;
            #endif
        #endif

        //add the relation type to the binary
        policyBinary.push_back(relationTypeToInt[relations[relId].type], bitsForRelationType);
        //eval RelationSet size output
        #if EVALUATION_OUTPUT 
            relationSetSize += bitsForRelationType;
        #endif

        //add the relation sides to the binary
        uint8_t numberOfSides = 2; //normal is a binary operation
        //IS_TRUE and IS_FALSE only have one child
        if(relations[relId].type == RelationSetType::IS_TRUE ||
           relations[relId].type == RelationSetType::IS_FALSE)
            numberOfSides = 1;

        for(uint8_t side = 0; side < numberOfSides; side++)  {
            if(relations[relId].variableIds[side] == -1)  { //next variable in the variable set
                policyBinary.push_back(0, 1); //first bit 0, indicates a next variable
                //Eval RelationSet size output
                #if EVALUATION_OUTPUT 
                    relationSetSize += 1;
                #endif
            }
            else  { //specific (redundant) variable, so insert to the id to the original
                //first bit 1 indicates that a specific variable is following
                policyBinary.push_back(1, 1); 
                //eval RelationSet size output
                #if EVALUATION_OUTPUT 
                    relationSetSize += 1;
                #endif
                //followed by the variable id to point to
                policyBinary.push_back(relations[relId].variableIds[side], variableSet.bitsForSpecificVariableId);
                //eval RelationSet size output
                #if EVALUATION_OUTPUT 
                    relationSetSize += variableSet.bitsForSpecificVariableId;
                #endif
            }
        }
    }
}

//print the size of the relation set
void RelationSet::printSize()  {
    //DEBUG output for relation set size
    cout << "RelationSet size: " << relationSetSize << endl;
}
