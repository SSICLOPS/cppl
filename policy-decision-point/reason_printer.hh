#pragma once

#include<vector>
#include "options.hh"
//#include "policy_stack_evaluator.hh"

using namespace std;

struct PolicyStackEvaluatorNode;

class ReasonPrinter{
	public:
//#ifdef CONVERT_TO_DNF
		//virtual void print(bool result, vector<struct PolicyStackProcessorNode> reason, DNFConverterNode & dnf) = 0;
//#else
		virtual void print(bool result, vector<struct PolicyStackEvaluatorNode> reason) = 0;
//#endif
};
