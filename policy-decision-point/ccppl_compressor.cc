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

#include<cstring>
#include "ccppl_compressor.hh"
#include "ID.hh"
#include "function.hh"

void CcpplCompressor::compress(Binary & binary,
		const PolicyDefinition * policyDefinition,
		const std::stack<StackOperation> & policyStack,
		const RelationSet & relationSet)
{

	binary.reset();
	bitsForVariablePosition = ceil(log2(policyDefinition->getVarNum()));

	binary.push_back(policyDefinition->getVersionNumber(), BitsFor::POLICY_DEFINITION_VER);

	StackOperation stackOp;
	std::stack<StackOperation> polStack = policyStack;

	while(!polStack.empty()){
		stackOp = polStack.top();

		binary.push_back(stackOp.type, BitsFor::STACK_OPERATION_TYPE);
		if (stackOp.type == PolicyStackOperationType::SPECIFIC_RELATION){
			if (stackOp.relationId != std::numeric_limits<id_type>::max()){
				binary.push_back(1, 1);
				binary.push_back(stackOp.relationId, BitsFor::SPECIFIC_RELATION_ID);
			}
			else{
				binary.push_back(0, 1);
			}
		}

		polStack.pop();
	}

	//relation set
	id_type varCounter = 0;
	for (id_type relId = 0; relId < relationSet.getRelationNum(); ++relId){
		const Relation * relation = relationSet.getRelation(relId);
#if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
		if (relation->getType() == Relation::Types::EQUAL
				&& relation->getLeftID() == varCounter
				&& relation->getRightID() == varCounter + 1)
		{
			binary.push_back(0, 1);
			varCounter += 2;
			continue;
		}

		binary.push_back(1, 1);
#endif

		binary.push_back(relation->getType(), BitsFor::RELATION_TYPE);
		if (relation->getLeftID() == varCounter){
			binary.push_back(0, 1);
			++varCounter;
		}
		else{
			binary.push_back(1, 1);
			binary.push_back(relation->getLeftID(), BitsFor::SPECIFIC_VARIABLE_ID);
		}

		if (relation->getRightID() != std::numeric_limits<id_type>::max()){
			if (relation->getRightID() == varCounter){
				binary.push_back(0, 1);
				++varCounter;
			}
			else{
				binary.push_back(1, 1);
				binary.push_back(relation->getRightID(), BitsFor::SPECIFIC_VARIABLE_ID);
			}
		}
	}

	//variable set
	for (id_type varId = 0; varId < relationSet.getVariableNum(); ++varId){
		const Variable * variable = relationSet.getVariable(varId);

		storeVariable(binary, *variable, false, policyDefinition);
	}
}

void CcpplCompressor::storeVariable(Binary & binary, const Variable & var, bool isFunctionParam, const PolicyDefinition * policyDefinition){
	if (isFunctionParam == false){
		binary.push_back(var.get_type(), BitsFor::VARIABLE_TYPE);
	}

	if (var.get_type() == Variable::Types::BOOLEAN){
		binary.push_back((((Boolean &)var).isTrue())?1:0, 1);
	}
	else if (var.get_type() == Variable::Types::STRING){
		std::string str = ((String &)var).get_value();
		const char *cstr = str.c_str();
		size_t cstr_len = str.size();
		for (size_t i = 0; i < cstr_len; ++i){
			binary.push_back(cstr[i], 8);
		}
		// Push Null-terminator
		binary.push_back(0, 8);
	}
	else if (var.get_type() == Variable::Types::ID){
		binary.push_back(((ID &)var).getID(), bitsForVariablePosition);
	}
	else if (var.get_type() == Variable::Types::ENUM_VALUE){
		binary.push_back(((Enum &)var).get_offset(), ceil(log2(policyDefinition->getEnumElemNum(((Enum &)var).get_id()))));
	}
	else if (var.get_type() == Variable::Types::FUNCTION){
		Function & func = (Function &)var;
		binary.push_back(func.get_id(), bitsForVariablePosition);
		for (id_type i = 0; i < func.getParameterNum(); ++i){
			storeVariable(binary, *func.getParameter(i), true, policyDefinition);
		}
	}
	else if (var.get_type() == Variable::Types::INT64){
		binary.push_back(((Int64 &)var).get_value(), 64);
	}
	else if (var.get_type() == Variable::Types::INT32){
		binary.push_back(((Int32 &)var).get_value(), 32);
	}
	else if (var.get_type() == Variable::Types::INT16){
		binary.push_back(((Int16 &)var).get_value(), 16);
	}
	else if (var.get_type() == Variable::Types::INT8){
		binary.push_back(((Int8 &)var).get_value(), 8);
	}
	else if (var.get_type() == Variable::Types::UINT32){
		binary.push_back(((Uint32 &)var).get_value(), 32);
	}
	else if (var.get_type() == Variable::Types::UINT16){
		binary.push_back(((Uint16 &)var).get_value(), 16);
	}
	else if (var.get_type() == Variable::Types::UINT8){
		binary.push_back(((Uint8 &)var).get_value(), 8);
	}
	else if (var.get_type() == Variable::Types::DOUBLE){
		assert(sizeof(double) == sizeof(uint64_t));
		uint64_t value;
		double d = ((Double &)var).get_value();
		memcpy(&value, &d, 8);
		binary.push_back(value, 64);
	}
}
