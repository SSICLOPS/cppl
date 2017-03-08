#pragma once

#include<cstdint>
#include<stack>
#include<vector>

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
	private:
		vector<PolicyStackProcessorNode> nodeList;	
		stack<uint64_t> workStack;
};

