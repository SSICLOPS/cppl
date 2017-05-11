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
