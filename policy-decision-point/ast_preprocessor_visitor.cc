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
                    if(policyDefinition.query(astId->name)->type != PDEntryTypes::BOOLEAN)
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
            if(constant->type == AstValueType::String)  {
				constant->enumValuePosition = ((EnumEntry<std::string> *)policyDefinition.query(enumId->name))->getOffset(boost::get<string>(constant->value));
				constant->value = (int64_t)enumId->position;
				constant->type = AstValueType::EnumValue;
            }
            else if(constant->type == AstValueType::Integer)  {
				CommonEnumEntry * cee = (CommonEnumEntry *)policyDefinition.query(enumId->name);
				if (cee->enum_type == PDEntryTypes::INT8)
					constant->enumValuePosition = ((EnumEntry<int8_t> *)policyDefinition.query(enumId->name))->getOffset(boost::get<int64_t>(constant->value));
				else if (cee->enum_type == PDEntryTypes::INT16)
					constant->enumValuePosition = ((EnumEntry<int16_t> *)policyDefinition.query(enumId->name))->getOffset(boost::get<int64_t>(constant->value));
				else if (cee->enum_type == PDEntryTypes::INT32)
					constant->enumValuePosition = ((EnumEntry<int32_t> *)policyDefinition.query(enumId->name))->getOffset(boost::get<int64_t>(constant->value));
				else if (cee->enum_type == PDEntryTypes::INT64)
					constant->enumValuePosition = ((EnumEntry<int64_t> *)policyDefinition.query(enumId->name))->getOffset(boost::get<int64_t>(constant->value));
				else if (cee->enum_type == PDEntryTypes::UINT8)
					constant->enumValuePosition = ((EnumEntry<uint8_t> *)policyDefinition.query(enumId->name))->getOffset(boost::get<int64_t>(constant->value));
				else if (cee->enum_type == PDEntryTypes::UINT16)
					constant->enumValuePosition = ((EnumEntry<uint16_t> *)policyDefinition.query(enumId->name))->getOffset(boost::get<int64_t>(constant->value));
				else if (cee->enum_type == PDEntryTypes::UINT32)
					constant->enumValuePosition = ((EnumEntry<uint32_t> *)policyDefinition.query(enumId->name))->getOffset(boost::get<int64_t>(constant->value));
				
				constant->value = (int64_t)enumId->position;
				constant->type = AstValueType::EnumValue;
            }
            else
                throw "Unknown AstValueType in enum compression branch";

            if(constant->enumValuePosition == std::numeric_limits<id_type>::max())
                throw runtime_error("Invalid enum value");

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
    //operations always return a boolean value
    resultType = AstValueType::Boolean;
}

void AstPreprocessorVisitor::visit(AstConstant &constant) {
    //increase the nodeCounter in this branch
    nodeCounter++;

    //set the AstValueType of the constant as type result for this node
	if (constant.type == AstValueType::Integer){                                                                                                                                                                                      
		int64_t value = boost::get<int64_t>(constant.value);
              if (value < 0){ 
                  if (value < std::numeric_limits<int32_t>::min())
                      constant.type = AstValueType::INT64;
                  else if (value < std::numeric_limits<int16_t>::min())
                      constant.type = AstValueType::INT32;
                  else if (value < std::numeric_limits<int8_t>::min())
                      constant.type = AstValueType::INT16;
                  else
                      constant.type = AstValueType::INT8;
              }   
              else{
                  if (value <= std::numeric_limits<uint8_t>::max())
                      constant.type = AstValueType::UINT8;
                  else if (value <= std::numeric_limits<uint16_t>::max())
                      constant.type = AstValueType::UINT16;
                  else if (value <= std::numeric_limits<uint32_t>::max())
                      constant.type = AstValueType::UINT32;
                  else
                      constant.type = AstValueType::INT64;
          }   
      }   

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

	const PDEntry * pde = policyDefinition.query(id.name);
	if (pde == nullptr)
		throw "Undefined ID";

    //initial of all ID is unknown, since we need to look them up from the policy definition
	(uint8_t &)(id.type) = (uint8_t)(pde->type); 
    resultType = id.type;

    //also get the placeholder for the ID
    id.position = pde->id;

    //is the id an enum
    id.isEnum = (pde->type == PDEntryTypes::ENUM_VALUE)?true:false;

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
	const PDEntry * pde = policyDefinition.query(func.name);
	if (pde == nullptr || pde->type != PDEntryTypes::FUNCTION)
		throw "undefined function" + func.name;

	const FuncEntry * pFunc = (const FuncEntry *)pde;
    func.position = pFunc->id;

    //check that there is the right amount of parameters for this function
    if(func.parameters.size() != pFunc->para_list.size())
        throw runtime_error("\"" + func.name + "\" needs " + boost::lexical_cast<std::string>(pFunc->para_list.size()) + " parameters");
    
    //and that they have the right type
    //also convert the unknown typed constant with the value as string here to the right value type
    vector<AstConstant *>::iterator it;
    uint8_t paramPosition = 0;
	uint8_t enumParamCounter = 0;
    for(it = func.parameters.begin(), paramPosition = 0; it != func.parameters.end(); ++it, paramPosition++)  {
        assert((*it)->type == AstValueType::Unknown);

        //get the type for the parameter from the policy definition
        PDEntryTypes parameterType = pFunc->para_list[paramPosition];
        if(parameterType == PDEntryTypes::ENUM_VALUE)  {
            //get the position of the enum value inside the enum vector for this id
            id_type enumValuePosition = 0;
			const CommonEnumEntry * cee = (const CommonEnumEntry *)policyDefinition.queryByID(pFunc->enum_para_list->at(enumParamCounter++));

			if (cee->enum_type == PDEntryTypes::STRING && (*it)->type == AstValueType::String){
				//convert the quoted string to an unquoted one
				string valueStr = boost::get<string>((*it)->value);
				//check that there is a quote at the start and end
				if(valueStr.at(0) != '"' || valueStr.back() != '"')
					throw "Function parameter is not a valid quoted string";
				valueStr = valueStr.substr(1, valueStr.size() - 2); //remove the quotes

				//store the string inside the var s.t. we can search for it
				if((enumValuePosition = ((EnumEntry<string> *) cee)->getOffset(valueStr)) == std::numeric_limits<id_type>::max())
					throw runtime_error("Invalid enum value");
			}
			//TODO:NON-String

            (*it)->type = AstValueType::EnumValue;
            (*it)->value = (int64_t)cee->id;
            (*it)->enumValuePosition = enumValuePosition;
        }
        else if(parameterType == PDEntryTypes::STRING)  {
            (*it)->type = AstValueType::String;
            string valueStr = boost::get<string>((*it)->value);
            //check that there is a quote at the start and end
            if(valueStr.at(0) != '"' || valueStr.back() != '"')
                throw "Function parameter is not a valid quoted string";
            valueStr = valueStr.substr(1, valueStr.size() - 2); //remove the quotes
            (*it)->value = valueStr;
        }
        else if(parameterType == PDEntryTypes::INT64 || //unsigned integer with 64 bits are not supported
                parameterType == PDEntryTypes::INT32 || parameterType == PDEntryTypes::UINT32 ||
                parameterType == PDEntryTypes::INT16 || parameterType == PDEntryTypes::UINT16 ||
                parameterType == PDEntryTypes::INT8 || parameterType == PDEntryTypes::UINT8)
        {
            (uint8_t &)((*it)->type) = parameterType;
            string valueStr = boost::get<string>((*it)->value);
            try  {
                (*it)->value = boost::lexical_cast<int64_t>(valueStr);
            }
            catch(boost::bad_lexical_cast const&)  {
                throw "Function parameter is not a valid int32";
            }
        }
        else if(parameterType == PDEntryTypes::BOOLEAN)  {
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
            if(policyDefinition.query(astId->name)->type != PDEntryTypes::BOOLEAN)
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
