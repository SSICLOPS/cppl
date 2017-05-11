#pragma once

#include<cstdint>
#include<stack>
#include<vector>
#include<set>

#include "options.hh"

//#ifdef CONVERT_TO_DNF
//typedef std::vector<std::set<uint64_t>> DNFConverterNode;
//#endif

#include "reason_printer.hh"

using namespace std;

class ReasonPrinter;

enum class PolicyStackProcessorNodeType {
	AND,
	OR,
	RELATION
};

struct PolicyStackProcessorNode {
	PolicyStackProcessorNodeType type;
	bool result;
	int64_t reason[2];
};


class PolicyStackProcessor {
	public:
		PolicyStackProcessor(){PolicyStackProcessorNode pspn;nodeList.push_back(pspn);}
		~PolicyStackProcessor(){}
		void addStackOperation(PolicyStackProcessorNodeType type, bool value = false, int64_t relatedId = 0);
		bool getResult(){return nodeList[nodeList.size() - 1].result;}
		vector<PolicyStackProcessorNode> getReason();
		void printReason(ReasonPrinter & printer);

//#ifdef CONVERT_TO_DNF
		//DNFConverterNode & getDNF(){return dnfConvertorStack.top();}
//#endif
	private:
		vector<PolicyStackProcessorNode> nodeList;	
		stack<uint64_t> workStack;
//#ifdef CONVERT_TO_DNF
		//stack<DNFConverterNode> dnfConvertorStack;
//#endif
};

