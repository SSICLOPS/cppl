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
#include "options.hh"

using namespace std;

//A very simple reason printer.
//Print only when result is true.
//when or is met, print the first branch.
class SimpleReasonPrinter:public ReasonPrinter{
	public:
//#ifdef CONVERT_TO_DNF
		//void print(bool result, vector<PolicyStackProcessorNode> reason, DNFConverterNode & dnf);
//#else
		void print(bool result, vector<PolicyStackProcessorNode> reason);
//#endif
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

//#ifdef CONVERT_TO_DNF
		//void printDNF();
//#endif

	private:
		vector<uint64_t> reasonList;
		PolicyDefinition * mPolicyDefinition;
		RelationSet * mRelationSet;
		VariableSet * mVariableSet;

#ifdef CHOOSE_FIRST_OR_BRANCH_BY_TRUE
		inline PolicyStackProcessorNode & choose(vector<PolicyStackProcessorNode> & reason, PolicyStackProcessorNode & pspn){return reason[pspn.reason[0]];}
#endif// CHOOSE_FIRST_OR_BRANCH_BY_TRUE

#ifdef CHOOSE_SECOND_OR_BRANCH_BY_TRUE
		inline PolicyStackProcessorNode & choose(vector<PolicyStackProcessorNode> & reason, PolicyStackProcessorNode & pspn){return reason[pspn.reason[1]];}
#endif// CHOOSE_FIRST_OR_BRANCH_BY_TRUE

//#ifdef CONVERT_TO_DNF
		//DNFConverterNode mDNF;
//#endif

		string _variableToString(Variable v, string str_surrounding="\"");
		string _relationToString(uint64_t relationId, string str_surrounding="\"");
		string _relationTypeToString(RelationSetType t);
		string _printReasonToJSON_AUX(uint64_t reasonId);
};
