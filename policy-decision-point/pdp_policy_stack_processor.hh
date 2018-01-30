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

//#include "policy_stack_evaluator.hh"
//#include "ccppl_decompressor.hh"
#include "binary.hh"
#include "policy_definition.hh"
#include "__policy_stack.hh"
#include "reason_printer.hh"

template<typename Evaluator, typename Decompressor>
class PDPPolicyStackProcessor:private Evaluator, private Decompressor{
	public:
		PDPPolicyStackProcessor<Evaluator, Decompressor>(std::stack<StackOperation> & policy_stack, RelationSet & relation_set, const NodeParameters ** nodeParameters)
			:Decompressor(nodeParameters), _policy_stack(policy_stack), _relation_set(relation_set)
		{
		}

		void load(const void * ccppl_data, unsigned int len){Decompressor::decompress(ccppl_data, len, _policy_stack, _relation_set);}
		void load(Binary & binary, const PolicyDefinition * policyDefinition){Decompressor::decompress(binary, policyDefinition, _policy_stack, _relation_set);}
		bool doEval(){return Evaluator::doEval(_policy_stack, _relation_set);}

		inline pol_def_version_t getVersion(){return Decompressor::getVersion();}
		inline bool getResult(){return Evaluator::getResult();}
		inline void printReason(ReasonPrinter & printer){Evaluator::printReason(printer);}
	private:
		std::stack<StackOperation> & _policy_stack;
		RelationSet & _relation_set;
};
