#pragma once

#include <stack>
#include <memory>

#include "options.hh"
#include "relation_set.hh"
#include "node_parameter.hh"

enum PolicyStackOperationType{
	AND = 3,
	OR = 2,
	NEXT_RELATION = 0,
	SPECIFIC_RELATION = 1
};

struct StackOperation{
	PolicyStackOperationType type;
	id_type relationId;
};

typedef std::stack<StackOperation> stack_type;

template<typename PolicyStackProcessor>
class __PolicyStack:public PolicyStackProcessor{
	public:
		typedef std::shared_ptr<__PolicyStack<PolicyStackProcessor>> pointer;

		id_type addStackOperation(StackOperation & so){policyStack.push(so);return policyStack.size() - 1;}
		id_type addRelation(Relation * r){return relationSet.addRelation(r);}
		id_type addVariable(Variable * v){return relationSet.addVariable(v);}

		__PolicyStack<PolicyStackProcessor>(const NodeParameters ** nodeParameters = NULL)
			:PolicyStackProcessor(policyStack, relationSet, (nodeParameters == NULL)? &_nodeParams:nodeParameters)
			{
				_nodeParams = NULL;
			}

		inline void setNodeParameters(const NodeParameters * np){_nodeParams = np;}

		inline const stack_type & getStack(){return policyStack;}
		inline const RelationSet & getRelationSet(){return relationSet;}

	private:
		stack_type policyStack;
		RelationSet relationSet;

		const NodeParameters * _nodeParams;
};
