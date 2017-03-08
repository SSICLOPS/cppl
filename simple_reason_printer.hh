#pragma once

#include<vector>
#include<cstdint>
#include<string>
#include<sstream>
#include<fstream>

#include "reason_printer.hh"
#include "policy_stack_processor.hh"
#include "policy_definition.hh"
#include "relation_set.hh"
#include "variable_set.hh"
#include "debug.hh"

using namespace std;

//A very simple reason printer.
//Print only when result is true.
//when or is met, print the first branch.
class SimpleReasonPrinter:public ReasonPrinter{
	public:
		void print(bool result, vector<PolicyStackProcessorNode> reason);
		vector<uint64_t> getReason(){return reasonList;}
		string printReasonToString();
		void init(PolicyDefinition * policyDefinition, RelationSet * relationSet, VariableSet * variableSet){
			mPolicyDefinition = policyDefinition;
			mRelationSet = relationSet;
			mVariableSet = variableSet;
		}

#ifdef PRINT_REASON_TO_CONSOLE
		string printReasonToJSON();
#endif//PRINT_REASON_TO_CONSOLE

		void printReasonToJSON(string filename);

	private:
		vector<uint64_t> reasonList;
		PolicyDefinition * mPolicyDefinition;
		RelationSet * mRelationSet;
		VariableSet * mVariableSet;

		string _variableToString(Variable v, string str_surrounding="\"");
		string _relationTypeToString(RelationSetType t);
		string _printReasonToJSON_AUX(uint64_t reasonId);
};
