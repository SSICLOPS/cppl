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
