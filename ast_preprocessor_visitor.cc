#include "ast_print_visitor.hh"
#include "ast_preprocessor_visitor.hh"

void AstPreprocessorVisitor::visit(AstOperation &op) {
    //vector for the types of the childrens
    vector<AstValueType> childTypes; //IDs are replaced with their real types

    //remove NOT operations from the AST by moving them into the relations
    if(op.type == AstOperationType::NOT)  {
        op.type = AstOperationType::ELIMINATED_NOT; //use seperate status to avoid the need of AST structure changes
        //if we already elminiation a NOT, flip the status to really eliminate the current one
        if(flipMeaning)
            flipMeaning = false;
        else
            flipMeaning = true;
    }
    else  {
        //increase the nodeCounter in this branch. Don't count NOTs, since we eliminate them.
        nodeCounter++;

        //flip all other operation types, if necessary
        //we are flipping only operation types, the values are not touched
        //NOT before a boolean is handled seperate via the parent operation type
        if(flipMeaning)  {
            if(op.type == AstOperationType::AND)
                op.type = AstOperationType::OR;
            else if(op.type == AstOperationType::OR)
                op.type = AstOperationType::AND;
            else if(op.type == AstOperationType::EQUAL)
                op.type = AstOperationType::NEQ;
            else if(op.type == AstOperationType::NEQ)
                op.type = AstOperationType::EQUAL;
            else if(op.type == AstOperationType::LESS)
                op.type = AstOperationType::GEQ;
            else if(op.type == AstOperationType::LEQ)
                op.type = AstOperationType::GREATER;
            else if(op.type == AstOperationType::GREATER)
                op.type = AstOperationType::LEQ;
            else if(op.type == AstOperationType::GEQ)
                op.type = AstOperationType::LESS;
            else
                throw "Flip operation is not implemented for this operation type";
        }
    }

    //save the operation type of the parent, s.t. the childs know which type her parent has
    parentType = op.type;

    //call the preprocessor visitor also for the children of the node
    vector<uint64_t> branchSize(op.getNumberOfChildren()); //used to store the number of nodes in the child branches
    for(uint8_t i = 0; i < op.getNumberOfChildren(); i++)  {
        bool beforeChildFlipMeaning = flipMeaning;
        uint64_t nodeCounterBefore = nodeCounter;

        op.getChild(i)->accept(*this);

        branchSize[i] = nodeCounter - nodeCounterBefore; //store the number of nodes in the childrens AST branch
        childTypes.push_back(resultType); //save the childs type for the type checking

        flipMeaning = beforeChildFlipMeaning; //restore flip meaning to its status before the branch
    }

    //replace single boolean IDs without relation by ID IS_TRUE relations (just a shortcut)
    if(op.type == AstOperationType::AND || op.type == AstOperationType::OR)  {
        //only check the number of sides that are available
        for(uint8_t side = 0; side < op.getNumberOfChildren(); side++)  {
            bool compareWithTrue = true; //also handle NOTs before shortcut boolean ids
            AstVisitableNode *child = op.getChild(side);
            while(child->nodeType == AstNodeType::Operation && ((AstOperation *) child)->type == AstOperationType::ELIMINATED_NOT)  {
                child = ((AstOperation *) child)->getChild(0); //NOT has only a left child
                compareWithTrue = !compareWithTrue; //change relation type to eliminate the NOT
            }
            if(child->nodeType == AstNodeType::Id || child->nodeType == AstNodeType::Function)  {
                if(child->nodeType == AstNodeType::Id)  {
                    AstId *astId = (AstId *) child;
                    if(policyDefinition.getIdType(astId->name) != AstValueType::Boolean)
                        throw "Only boolean variables can be used without a relation";
                }

                //use the correct relation type based on previous NOTs before the ID
                AstOperationType relType;
                if(compareWithTrue)
                    relType = AstOperationType::IS_TRUE;
                else
                    relType = AstOperationType::IS_FALSE;

                op.setChild(side, new AstOperation(relType, child)); //XXX pos. mem leak, free NOT
            }
        }
    }

    if(op.getNumberOfChildren() == 2)  { //the following makes only sence for binary operations
        //skip eliminated_nots to get to the real children of the operation
        AstVisitableNode *child[2]; 
        for(uint8_t side = 0; side < 2; side++)  {
            child[side] = op.getChild(side); //init this side
            //go deeper, until we receive the real child
            while(child[side]->nodeType == AstNodeType::Operation && ((AstOperation *) child[side])->type == AstOperationType::ELIMINATED_NOT)
                child[side] = ((AstOperation *) child[side])->getChild(0); //NOT has only a left child
        }
        //compress the value when used with an enum id
        if((child[0]->nodeType == AstNodeType::Id && ((AstId *) child[0])->isEnum == true && child[1]->nodeType != AstNodeType::Id) ||
           (child[1]->nodeType == AstNodeType::Id && ((AstId *) child[1])->isEnum == true && child[0]->nodeType != AstNodeType::Id))
        {
            //get the constant and the id
            AstConstant *constant;
            AstId *enumId;
            if(child[0]->nodeType == AstNodeType::Id)  {
                enumId   = (AstId *)       child[0];
                constant = (AstConstant *) child[1];
            }
            else  {
                constant = (AstConstant *) child[0];
                enumId   = (AstId *)       child[1];

                //also swap the children s.t. the id always comes first
                iter_swap(op.children.begin(), op.children.begin() + 1);
            }

            //get the position of the enum value inside the enum vector for this id
            Variable var;
            var.isEnum = false;
            int32_t enumValuePosition;

            if(constant->type == AstValueType::String)  {
                var.type = VariableSetType::STRING;
                var.value.asString = &(boost::get<string>(constant->value));
            }
            else if(constant->type == AstValueType::Integer)  {
                var.type = VariableSetType::INT32;
                var.value.asInt32 = boost::get<int64_t>(constant->value);
            }
            else
                throw "Unknown AstValueType in enum compression branch";

            if((enumValuePosition = policyDefinition.getEnumValuePosition(enumId->position, var)) == -1)
                throw runtime_error("\"" + *(var.value.asString) + "\" is not a valid enum value");

            constant->type = AstValueType::EnumValue;
            constant->value = (int64_t) enumValuePosition;
            constant->bitsForEnumValuePosition = policyDefinition.getBitsForEnumValuePosition(enumId->position);
        }
        //compress the equal or not equal relation with IS_TRUE or IS_FALSE, if the childs are a boolean ID and a boolean constant or a function with a boolean constant
        else if((op.type == AstOperationType::EQUAL || op.type == AstOperationType::NEQ) &&
                //boolean id with boolean constnat
                (((child[0]->nodeType == AstNodeType::Id && 
                   child[1]->nodeType == AstNodeType::Constant &&
                   ((AstConstant *) child[1])->type == AstValueType::Boolean) ||
                  (child[1]->nodeType == AstNodeType::Id && 
                   child[0]->nodeType == AstNodeType::Constant &&
                   ((AstConstant *) child[0])->type == AstValueType::Boolean))) ||
                //boolean function with boolean constant
                ((child[0]->nodeType == AstNodeType::Function &&
                  child[1]->nodeType == AstNodeType::Constant &&
                  ((AstConstant *) child[1])->type == AstValueType::Boolean) ||
                 (child[1]->nodeType == AstNodeType::Function &&
                  child[0]->nodeType == AstNodeType::Constant &&
                  ((AstConstant *) child[0])->type == AstValueType::Boolean)))
        {
            //get the value of the boolean constant
            bool constantValue;
            if(child[0]->nodeType == AstNodeType::Constant)  {
                constantValue = boost::get<bool>(((AstConstant *) child[0])->value);
                iter_swap(op.children.begin(), op.children.begin() + 1); //swap the values s.t. the id is in the LHS
            }
            else
                constantValue = boost::get<bool>(((AstConstant *) child[1])->value);

            //delete the second children with the boolean constant
            op.children.erase(op.children.begin() + 1);

            //use the constant value for equal and swap it for not equal relations
            if(constantValue == true)  {
                if(op.type == AstOperationType::EQUAL)
                    op.type = AstOperationType::IS_TRUE;
                else
                    op.type = AstOperationType::IS_FALSE;
            }
            else  {
                if(op.type == AstOperationType::EQUAL)
                    op.type = AstOperationType::IS_FALSE;
                else
                    op.type = AstOperationType::IS_TRUE;
            }
        }
        //all previous branches handled relations with id and constant children, s.t. their branch sizes can't be unequal
        //we only get a correct PN, if the childs are ordered after branch sizes
        else  {
            //if LHS is smaller than RHS swap the children, but only if it was not swapped by a previous operation
            //e.g. enum ids are swapped to the LHS
            if(branchSize[0] < branchSize[1])
                iter_swap(op.children.begin(), op.children.begin() + 1);
        }
    }

    //XXX introduce type lookup table for type compatiblities
    //    and table with meta types s.t. uint8_t, int8_t -> meta type

    AstValueType neededType = *(childTypes.begin());
    for(vector<AstValueType>::iterator typeIt = childTypes.begin() ; typeIt != childTypes.end(); ++typeIt)  {
        if(*typeIt != neededType)  {
            AstPrintVisitor printVisitor = AstPrintVisitor();
            cerr << "-----------------------------------------\n"
                 << "Incompatible types in the branch:\n"
                 << "-----------------------------------------" << endl;
            printVisitor.visit(op);
            throw "Incompatible type!";
        }
    }

    //operations always return a boolean value
    resultType = AstValueType::Boolean;
}

void AstPreprocessorVisitor::visit(AstConstant &constant) {
    //increase the nodeCounter in this branch
    nodeCounter++;

    //set the AstValueType of the constant as type result for this node
    resultType = constant.type;

    //flip the value of a boolean, if his parent is a NOT (at this stage ELIMINATED_NOT)
    if(parentType == AstOperationType::ELIMINATED_NOT)  {
        if(constant.type == AstValueType::Boolean)
            constant.value = !boost::get<bool>(constant.value);
        else
            throw "A NOT before this constant is not implemented.";
    }
}

void AstPreprocessorVisitor::visit(AstId &id)  {
    //increase the nodeCounter in this branch
    nodeCounter++;

    //initial of all ID is unknown, since we need to look them up from the policy definition
    id.type = policyDefinition.getIdType(id.name);
    resultType = id.type;

    //also get the placeholder for the ID
    id.position = policyDefinition.getIdPosition(id.name);

    //is the id an enum
    id.isEnum = policyDefinition.isEnumId(id.position);

    //a not before an ID is only supported if it has boolean type (relation is flipped in previous step instead of value)
    if(parentType == AstOperationType::ELIMINATED_NOT && id.type != AstValueType::Boolean)  {
        throw "A NOT before a non boolean ID is not implemented.";
    }
}

void AstPreprocessorVisitor::visit(AstFunction &func) {
    //increase the nodeCounter in this branch
    nodeCounter++;

    //currently we are just supporting boolean return values for functions
    resultType = func.type;

    //also get the placeholder for the function ID
    //use the same methods as for a normal ID
    func.position = policyDefinition.getIdPosition(func.name);

    //check that there is the right amount of parameters for this function
    if(func.parameters.size() != policyDefinition.getNumberOfFunctionParameters(func.position))
        throw runtime_error("\"" + func.name + "\" needs " + boost::lexical_cast<std::string>(policyDefinition.getNumberOfFunctionParameters(func.position)) + " parameters");
    
    //and that they have the right type
    //also convert the unknown typed constant with the value as string here to the right value type
    vector<AstConstant *>::iterator it;
    uint8_t paramPosition = 0;
    for(it = func.parameters.begin(), paramPosition = 0; it != func.parameters.end(); ++it, paramPosition++)  {
        assert((*it)->type == AstValueType::Unknown);

        //get the type for the parameter from the policy definition
        VariableSetType parameterType = policyDefinition.getFunctionParameterType(func.position, paramPosition);
        bool isEnumParameter = policyDefinition.isFunctionEnumParameter(func.position, paramPosition);
        if(isEnumParameter == true)  {
            //get the position of the enum value inside the enum vector for this id
            Variable var;
            var.isEnum = false;
            int32_t enumValuePosition;

            //convert the quoted string to an unquoted one
            string valueStr = boost::get<string>((*it)->value);
            //check that there is a quote at the start and end
            if(valueStr.at(0) != '"' || valueStr.back() != '"')
                throw "Function parameter is not a valid quoted string";
            valueStr = valueStr.substr(1, valueStr.size() - 2); //remove the quotes

            //store the string inside the var s.t. we can search for it
            var.type = VariableSetType::STRING;
            var.value.asString = &valueStr;

            if((enumValuePosition = policyDefinition.getFunctionEnumValuePosition(func.position, paramPosition, var)) == -1)
                throw runtime_error("\"" + *(var.value.asString) + "\" is not a valid enum value");

            (*it)->type = AstValueType::EnumValue;
            (*it)->value = (int64_t) enumValuePosition;
            (*it)->bitsForEnumValuePosition = policyDefinition.getBitsForFunctionEnumValuePosition(func.position, paramPosition);
        }
        else if(parameterType == VariableSetType::STRING)  {
            (*it)->type = AstValueType::String;
            string valueStr = boost::get<string>((*it)->value);
            //check that there is a quote at the start and end
            if(valueStr.at(0) != '"' || valueStr.back() != '"')
                throw "Function parameter is not a valid quoted string";
            valueStr = valueStr.substr(1, valueStr.size() - 2); //remove the quotes
            (*it)->value = valueStr;
        }
        else if(parameterType == VariableSetType::INT64 || //unsigned integer with 64 bits are not supported
                parameterType == VariableSetType::INT32 || parameterType == VariableSetType::UINT32 ||
                parameterType == VariableSetType::INT16 || parameterType == VariableSetType::UINT16 ||
                parameterType == VariableSetType::INT8 || parameterType == VariableSetType::UINT8)
        {
            (*it)->type = AstValueType::Integer;
            string valueStr = boost::get<string>((*it)->value);
            try  {
                (*it)->value = boost::lexical_cast<int64_t>(valueStr);
            }
            catch(boost::bad_lexical_cast const&)  {
                throw "Function parameter is not a valid int32";
            }
        }
        else if(parameterType == VariableSetType::BOOLEAN)  {
            (*it)->type = AstValueType::Boolean;
            string valueStr = boost::get<string>((*it)->value);
            bool value;
            if(valueStr.compare("0") == 0 || valueStr.compare("false") == 0)
                value = false;
            else if(valueStr.compare("1") == 0 || valueStr.compare("true") == 0)
                value = true;
            else
                throw "Function parameter is not a valid boolean";
            (*it)->value = value;
        }
        else
            throw "Undefined function parameter type in preprocessor";
    }

    //flip the value of a boolean, if his parent is a NOT (at this stage ELIMINATED_NOT)
    if(parentType == AstOperationType::ELIMINATED_NOT)  {
        throw "A NOT before a FUNCTION is not implemented.";
    }
}

void AstPreprocessorVisitor::visit(Ast &ast) {
    //init the nodeCounter
    nodeCounter = 0;

    //init the flip operations flag
    flipMeaning = false;

    //check if we have the special case that the AST consits only of NOTs and a boolean ID
    bool compareWithTrue = true;
    AstVisitableNode *firstNoneNotChild = ast.root;
    while(firstNoneNotChild->nodeType == AstNodeType::Operation && ((AstOperation *) firstNoneNotChild)->type == AstOperationType::NOT)  {
        firstNoneNotChild = ((AstOperation *) firstNoneNotChild)->getChild(0); //NOT has only a left child
        compareWithTrue = !compareWithTrue;
    }
    //if the AST consits only of a boolean ID, remove the shortcut by an ID IS_TRUE operation
    if(firstNoneNotChild->nodeType == AstNodeType::Id || firstNoneNotChild->nodeType == AstNodeType::Function)  {
        if(firstNoneNotChild->nodeType == AstNodeType::Id)  {
            AstId *astId = (AstId *) firstNoneNotChild;
            if(policyDefinition.getIdType(astId->name) != AstValueType::Boolean)
                throw "Only boolean variables can be used without a relation";
        }

        //use the correct relation type based on previous NOTs before the ID
        AstOperationType relType;
        if(compareWithTrue)
            relType = AstOperationType::IS_TRUE;
        else
            relType = AstOperationType::IS_FALSE;

        ast.root = new AstOperation(relType, firstNoneNotChild);
    }

    ast.root->accept(*this); //start from the AST root

    #if DEBUG_POLICY_GENERATION
        cout << "Nodes in AST: " << nodeCounter << endl;
        cout << "Type checking the generated AST: ";
    #endif
    //only valid if the root type is boolean
    if(resultType != AstValueType::Boolean)  {
        #if DEBUG_POLICY_GENERATION
        cout << "false";
        #endif
        throw "The root type of the policy is not boolean!";
    }
    #if DEBUG_POLICY_GENERATION
    else  {
        cout << "okay";
    }
    cout << endl;
    #endif
}
