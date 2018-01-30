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
#include "policy_stack_processor.hh"
#include "policy_definition.hh"
#include "policy_stack.hh"
#include "relation_set.hh"
#include "variable_set.hh"
#include "dnf_convertor.hh"
#include "debug.hh"

using namespace std;

class FalseReasonPrinter:public ReasonPrinter{
	public:
		void print(bool result, vector<PolicyStackProcessorNode> reason);

		void init(PolicyDefinition * policyDefinition, PolicyStack * policyStack, RelationSet * relationSet, VariableSet * variableSet){
			mPolicyDefinition = policyDefinition;
			mRelationSet = relationSet;
			mVariableSet = variableSet;
			DNFConvertor dnfConvertor(policyStack->getStack());
			dnfConvertor.doConvert();
			mDNF = dnfConvertor.getDNF();
		}

		vector<PolicyStackProcessorNode> & getReasonTree(){return reasonTree;}

		string printReasonToString();
		void printDNF();
	private:
		vector<PolicyStackProcessorNode> reasonTree;

		PolicyDefinition * mPolicyDefinition;
		RelationSet * mRelationSet;
		VariableSet * mVariableSet;

		DNFConverterNode mDNF;

		string _variableToString(Variable v, string str_surrounding="\"");
		string _relationToString(uint64_t relationId);
		string _relationTypeToString(RelationSetType t);
};
