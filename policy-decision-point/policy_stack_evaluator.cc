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

#include "policy_stack_evaluator.hh"

void PolicyStackEvaluator::handleAnd(){
	//bool first = workStack.top();
	//workStack.pop();
	//bool second = workStack.top();
	//workStack.pop();
	//workStack.push(first && second);
	
	PolicyStackEvaluatorNode psen;
	psen.reason[0] = std::numeric_limits<id_type>::max();
	psen.reason[1] = std::numeric_limits<id_type>::max();
	id_type latestResult;
	id_type secondLastestResult;

	latestResult = workStack.top();
	workStack.pop();
	secondLastestResult = workStack.top();
	workStack.pop();

	psen.type = PolicyStackEvaluatorNodeType::AND;
	psen.result = nodeList[latestResult].result && nodeList[secondLastestResult].result;

	if (psen.result){
		psen.reason[0] = secondLastestResult;
		psen.reason[1] = latestResult;
	}
	else{
		if (nodeList[secondLastestResult].result == false)
			psen.reason[0] = secondLastestResult;
		if (nodeList[latestResult].result == false){
			if (psen.reason[0] == std::numeric_limits<id_type>::max())
				psen.reason[0] = latestResult;
			else
				psen.reason[1] = latestResult;
		}
	}

	nodeList.push_back(std::move(psen));
	workStack.push(nodeList.size() - 1);
}

void PolicyStackEvaluator::handleOr(){
	//bool first = workStack.top();
	//workStack.pop();
	//bool second = workStack.top();
	//workStack.pop();
	//workStack.push(first || second);
	
	PolicyStackEvaluatorNode psen;
	psen.reason[0] = std::numeric_limits<id_type>::max();
	psen.reason[1] = std::numeric_limits<id_type>::max();
	id_type latestResult;
	id_type secondLastestResult;

	latestResult = workStack.top();
	workStack.pop();
	secondLastestResult = workStack.top();
	workStack.pop();

	psen.type = PolicyStackEvaluatorNodeType::OR;
	psen.result = nodeList[latestResult].result || nodeList[secondLastestResult].result;

	if (psen.result){
		if (nodeList[secondLastestResult].result == true)
			psen.reason[0] = secondLastestResult;
		if (nodeList[latestResult].result == true){
			if (psen.reason[1] == std::numeric_limits<id_type>::max())
				psen.reason[0] = latestResult;
			else
				psen.reason[1] = latestResult;
		}
	}
	else{
		psen.reason[1] = secondLastestResult;
		psen.reason[0] = latestResult;
	}

	nodeList.push_back(std::move(psen));
	workStack.push(nodeList.size() - 1);
}

void PolicyStackEvaluator::handleNextRelation(id_type relationId, bool value){
	//workStack.push(value);
	PolicyStackEvaluatorNode psen;
	psen.reason[0] = std::numeric_limits<id_type>::max();
	psen.reason[1] = std::numeric_limits<id_type>::max();

	psen.type = PolicyStackEvaluatorNodeType::RELATION;
	psen.result = value;
	psen.reason[0] = relationId;

	nodeList.push_back(std::move(psen));
	workStack.push(nodeList.size() - 1);
}

void PolicyStackEvaluator::handleSpecificRelation(id_type relationId, bool value){
	workStack.push(value);
	PolicyStackEvaluatorNode psen;
	psen.reason[0] = std::numeric_limits<id_type>::max();
	psen.reason[1] = std::numeric_limits<id_type>::max();

	psen.type = PolicyStackEvaluatorNodeType::RELATION;
	psen.result = value;
	psen.reason[0] = relationId;

	nodeList.push_back(std::move(psen));
	workStack.push(nodeList.size() - 1);
}

bool PolicyStackEvaluator::doEval(std::stack<StackOperation> & policyStack, RelationSet & relationSet){
	//uint64_t nextRelationCounter = 0;
	relationSet.doEval();

	StackOperation stackOp;
	std::stack<StackOperation> workStack(policyStack);

	while (! workStack.empty()){
		stackOp = workStack.top();

		if (stackOp.type == PolicyStackOperationType::AND){
			handleAnd();
		}
		else if (stackOp.type == PolicyStackOperationType::OR){
			handleOr();
		}
		else if (stackOp.type == PolicyStackOperationType::NEXT_RELATION){
			handleNextRelation(stackOp.relationId, relationSet.getEvalResult(stackOp.relationId));
		}
		else if (stackOp.type == PolicyStackOperationType::SPECIFIC_RELATION){
			handleSpecificRelation(stackOp.relationId, relationSet.getEvalResult(stackOp.relationId));
		}
		else
			throw "StackOperationType not implemented";

		workStack.pop();
	}

	return getResult();
}

std::vector<PolicyStackEvaluatorNode> PolicyStackEvaluator::getReason(){
	std::vector<PolicyStackEvaluatorNode> reasonTree;

	reasonTree.push_back(nodeList[nodeList.size() - 1]);

	for (id_type i = 0; i != reasonTree.size(); ++i){
		if (reasonTree[i].type == PolicyStackEvaluatorNodeType::RELATION)
			continue;

		reasonTree.push_back(nodeList[reasonTree[i].reason[0]]);
		reasonTree[i].reason[0] = reasonTree.size() - 1;
		if (reasonTree[i].reason[1] != std::numeric_limits<id_type>::max()){
			reasonTree.push_back(nodeList[reasonTree[i].reason[1]]);
			reasonTree[i].reason[1] = reasonTree.size() - 1;
		}
	}

	return reasonTree;
}

void PolicyStackEvaluator::printReason(ReasonPrinter & printer){
	printer.print(getResult(), getReason());
}
