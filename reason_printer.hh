#pragma once

#include<vector>

#include "policy_stack_processor.hh"

using namespace std;

class ReasonPrinter{
	public:
		virtual void print(bool result, vector<struct PolicyStackProcessorNode> reason) = 0;
};
