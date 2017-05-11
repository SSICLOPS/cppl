#pragma once

#include <stack>

#include "options.hh"
#include "relation_set.hh"
#include "node_parameter.hh"

enum PolicyStackOperationType{
	AND,
	OR,
	NEXT_RELATION,
	SPECIFIC_RELATION
};

struct StackOperation{
	PolicyStackOperationType type;
	id_type relationId;
};

template<typename PolicyStackProcessor>
class __PolicyStack:public PolicyStackProcessor{
	public:
		id_type addStackOperation(StackOperation & so){policyStack.push(so);return policyStack.size() - 1;}
		id_type addRelation(Relation * r){return relationSet.addRelation(r);}
		id_type addVariable(Variable * v){return relationSet.addVariable(v);}

		__PolicyStack<PolicyStackProcessor>(const NodeParameters ** nodeParameters):PolicyStackProcessor(policyStack, relationSet, nodeParameters){}

	private:
		std::stack<StackOperation> policyStack;
		RelationSet relationSet;
};
