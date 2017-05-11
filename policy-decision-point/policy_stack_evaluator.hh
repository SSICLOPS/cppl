#pragma once

#include <stack>

#include "options.hh"
#include "__policy_stack.hh"

class PolicyStackEvaluator{
	public:
		void handleAnd();
		void handleOr();
		void handleNextRelation(id_type relationId, bool value);
		void handleSpecificRelation(id_type relationId, bool value);

		bool getResult(){return workStack.top();}
		
		bool doEval(std::stack<StackOperation> & policyStack, RelationSet & relationSet);

	private:
		std::stack<bool> workStack;
};
