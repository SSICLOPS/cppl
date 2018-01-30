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

#include "reason_printer.hh"
#include "policy_definition.hh"
#include "__policy_stack.hh"
#include "relation_set.hh"
#include "dnf_convertor.hh"
#include "policy_stack_evaluator.hh"

using namespace std;

class FalseReasonPrinter:public ReasonPrinter{
	public:
		void print(bool result, vector<PolicyStackEvaluatorNode> reason);

		void init(const PolicyDefinition * policyDefinition, const stack_type * policyStack, const RelationSet * relationSet){
			mPolicyDefinition = policyDefinition;
			mRelationSet = relationSet;
			DNFConvertor dnfConvertor(*policyStack);
			dnfConvertor.doConvert();
			mDNF = dnfConvertor.getDNF();
		}

		vector<PolicyStackEvaluatorNode> & getReasonTree(){return reasonTree;}

		string printReasonToString();
		void printDNF();
	private:
		vector<PolicyStackEvaluatorNode> reasonTree;

		const PolicyDefinition * mPolicyDefinition;
		const stack_type * mPolicyStack;
		const RelationSet * mRelationSet;

		DNFConverterNode mDNF;

		string _variableToString(const Variable * v, string str_surrounding="\"");
		string _relationToString(id_type relationId);
		string _relationTypeToString(Relation::Types t);
};
