#pragma once

#include "policy_stack_evaluator.hh"
#include "ccppl_decompressor.hh"
#include "__policy_stack.hh"

template<typename Evaluator, typename Decompressor>
class PDPPolicyStackProcessor:private Evaluator, private Decompressor{
	public:
		PDPPolicyStackProcessor<Evaluator, Decompressor>(std::stack<StackOperation> & policy_stack, RelationSet & relation_set, const NodeParameters ** nodeParameters)
			:Decompressor(nodeParameters), _policy_stack(policy_stack), _relation_set(relation_set)
		{
		}

		void load(const void * ccppl_data, unsigned int len){CcpplDecompressor::decompress(ccppl_data, len, _policy_stack, _relation_set);}
		bool doEval(){return Evaluator::doEval(_policy_stack, _relation_set);}

		inline pol_def_version_t getVersion(){return CcpplDecompressor::getVersion();}
		inline bool getResult(){return PolicyStackEvaluator::getResult();}
	private:
		std::stack<StackOperation> & _policy_stack;
		RelationSet & _relation_set;
};
