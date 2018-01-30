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

#include "ast.hh"
#include "__policy_stack.hh"
#include "relation_set.hh"

//#define __DEBUG__

#ifdef __DEBUG__
#include <iostream>
#endif

class Ast2PolicyStack: public AstVisitor{
	public:
		Ast2PolicyStack(stack_type & policyStack, RelationSet & relationSet)
			:policyStack(policyStack), relationSet(relationSet)
		{
			_left = _right = std::numeric_limits<id_type>::max();
#ifdef __DEBUG__
			std::cout<<"Ast2PolicyStack init: _left: "<<_left<<" _right: "<<_right<<std::endl;
#endif
		}

		void visit(AstOperation &);
		void visit(AstConstant &);
		void visit(AstId &);
		void visit(AstFunction &);
		void visit(Ast &);

	private:
		stack_type & policyStack;
		RelationSet & relationSet;

		id_type _left;
		id_type _right;
};
