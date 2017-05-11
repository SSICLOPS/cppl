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
