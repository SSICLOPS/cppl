#pragma once

#include<vector>
#include<cstdint>
#include<string>
#include<sstream>
#include<fstream>

#include "reason_printer.hh"
#include "policy_stack_evaluator.hh"
#include "policy_definition.hh"
#include "relation_set.hh"
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
		void print(bool result, vector<PolicyStackEvaluatorNode> reason);
//#endif
		vector<uint64_t> getReason(){return reasonList;}
		string printReasonToString();
		void init(const PolicyDefinition * policyDefinition, const RelationSet * relationSet){
			mPolicyDefinition = policyDefinition;
			mRelationSet = relationSet;
		}

#ifdef PRINT_REASON_TO_CONSOLE
		string printReasonToJSON();
#endif//PRINT_REASON_TO_CONSOLE

		void printReasonToJSON(string filename);

//#ifdef CONVERT_TO_DNF
		//void printDNF();
//#endif

	private:
		vector<id_type> reasonList;
		const PolicyDefinition * mPolicyDefinition;
		const RelationSet * mRelationSet;

#ifdef CHOOSE_FIRST_OR_BRANCH_BY_TRUE
		inline PolicyStackEvaluatorNode & choose(vector<PolicyStackEvaluatorNode> & reason, PolicyStackEvaluatorNode & pspn){return reason[pspn.reason[0]];}
#endif// CHOOSE_FIRST_OR_BRANCH_BY_TRUE

#ifdef CHOOSE_SECOND_OR_BRANCH_BY_TRUE
		inline PolicyStackEvaluatorNode & choose(vector<PolicyStackEvaluatorNode> & reason, PolicyStackEvaluatorNode & pspn){return reason[pspn.reason[1]];}
#endif// CHOOSE_FIRST_OR_BRANCH_BY_TRUE

//#ifdef CONVERT_TO_DNF
		//DNFConverterNode mDNF;
//#endif

		string _variableToString(const Variable * v, string str_surrounding="\"");
		string _relationToString(id_type relationId, string str_surrounding="\"");
		string _relationTypeToString(Relation::Types t);
		string _printReasonToJSON_AUX(id_type reasonId);
};
