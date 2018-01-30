// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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

