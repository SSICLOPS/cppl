#pragma once 

#include "reason_printer.hh"
#include "policy_definition.hh"
#include "__policy_stack.hh"
#include "relation_set.hh"
#include "dnf_convertor.hh"
#include "policy_stack_evaluator.hh"

using namespace std;

class FalseReasonPrinter:public ReasonPrinter{
	public:
		void print(bool result, vector<PolicyStackEvaluatorNode> reason);

		void init(const PolicyDefinition * policyDefinition, const stack_type * policyStack, const RelationSet * relationSet){
			mPolicyDefinition = policyDefinition;
			mRelationSet = relationSet;
			DNFConvertor dnfConvertor(*policyStack);
			dnfConvertor.doConvert();
			mDNF = dnfConvertor.getDNF();
		}

		vector<PolicyStackEvaluatorNode> & getReasonTree(){return reasonTree;}

		string printReasonToString();
		void printDNF();
	private:
		vector<PolicyStackEvaluatorNode> reasonTree;

		const PolicyDefinition * mPolicyDefinition;
		const stack_type * mPolicyStack;
		const RelationSet * mRelationSet;

		DNFConverterNode mDNF;

		string _variableToString(const Variable * v, string str_surrounding="\"");
		string _relationToString(id_type relationId);
		string _relationTypeToString(Relation::Types t);
};
