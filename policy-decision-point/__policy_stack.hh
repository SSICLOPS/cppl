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
