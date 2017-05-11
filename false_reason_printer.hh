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
