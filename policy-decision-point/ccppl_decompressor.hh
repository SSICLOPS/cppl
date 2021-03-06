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

#include <stack>

#include "options.hh"
#include "node_parameter.hh"
#include "relation_set.hh"
#include "__policy_stack.hh"
#include "policy_definition.hh"
#include "relation.hh"
#include "variable.hh"
#include "binary.hh"

class CcpplDecompressor{
	public:
	   CcpplDecompressor(const NodeParameters ** np):nodeParameters(np){}
	   void decompress(const void * data, uint16_t len, std::stack<StackOperation> & policyStack, RelationSet & relationSet);
	   void decompress(Binary & binary,
			   const PolicyDefinition * policyDefinition,
			   std::stack<StackOperation> & policyStack,
			   RelationSet & relationSet);
	   inline pol_def_version_t getVersion(){return _version;}
	private:
	   void doDecompress(Binary & binary,
			   const PolicyDefinition * policyDefinition,
			   std::stack<StackOperation> & policyStack,
			   RelationSet & relationSet);
	   PolicyStackOperationType getStackOperationType(Binary & binary);
	   Relation::Types getRelationType(Binary & binary);
	   id_type getVariableID(Binary & binary, uint64_t & var_num);
	   Variable::Types getVariableType(Binary & binary);
	   Variable * createBasicVariableFromBin(Binary & binary, Variable::Types type);
	   Variable::Types PolDefEntryType2VarType(PDEntryTypes type);
	   const NodeParameters ** nodeParameters;
	   pol_def_version_t _version;

	   enum BitsFor{
		   SPECIFIC_RELATION_ID = 8,
		   SPECIFIC_VARIABLE_ID = 8,
		   VARIABLE_TYPE = 4,
		   RELATION_TYPE = 3,
		   STACK_OPERATION_TYPE = 2
	   };
};
