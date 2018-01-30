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

#include "policy_definition.hh"
#include "binary.hh"
#include "relation_set.hh"
#include "__policy_stack.hh"
#include "options.hh"

class CcpplCompressor{
	public:
		CcpplCompressor(){}
		void compress(Binary & binary,
				const PolicyDefinition * policyDefinition,
				const std::stack<StackOperation> & policyStack,
				const RelationSet & relationSet);
	private:
		void storeVariable(Binary & binary, const Variable & var, bool isFunctionParam, const PolicyDefinition * policyDefinition);
		uint8_t bitsForVariablePosition;
	   enum BitsFor{
		   SPECIFIC_RELATION_ID = 8,
		   SPECIFIC_VARIABLE_ID = 8,
		   VARIABLE_TYPE = 4,
		   RELATION_TYPE = 3,
		   STACK_OPERATION_TYPE = 2,
		   POLICY_DEFINITION_VER = 16
	   };
};
