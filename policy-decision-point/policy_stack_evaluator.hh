#pragma once

#include <stack>
#include <vector>

#include "options.hh"
#include "__policy_stack.hh"

#include "reason_printer.hh"

enum class PolicyStackEvaluatorNodeType{
	AND,
	OR,
	RELATION
};

struct PolicyStackEvaluatorNode{
	PolicyStackEvaluatorNodeType type;
	bool result;
	id_type reason[2];
};

class PolicyStackEvaluator{
	public:
		void handleAnd();
		void handleOr();
		void handleNextRelation(id_type relationId, bool value);
		void handleSpecificRelation(id_type relationId, bool value);

		bool getResult(){return nodeList[nodeList.size() - 1].result;}
		
		bool doEval(std::stack<StackOperation> & policyStack, RelationSet & relationSet);

		void printReason(ReasonPrinter & printer);
	private:
		vector<PolicyStackEvaluatorNode> getReason();
		std::vector<PolicyStackEvaluatorNode> nodeList;
		std::stack<id_type> workStack;
};
