#include "policy_stack_evaluator.hh"

void PolicyStackEvaluator::handleAnd(){
	bool first = workStack.top();
	workStack.pop();
	bool second = workStack.top();
	workStack.pop();
	workStack.push(first && second);
}

void PolicyStackEvaluator::handleOr(){
	bool first = workStack.top();
	workStack.pop();
	bool second = workStack.top();
	workStack.pop();
	workStack.push(first || second);
}

void PolicyStackEvaluator::handleNextRelation(id_type relationId, bool value){
	workStack.push(value);
}

void PolicyStackEvaluator::handleSpecificRelation(id_type relationId, bool value){
	workStack.push(value);
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
