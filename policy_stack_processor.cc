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

#include "policy_stack_processor.hh"

void PolicyStackProcessor::addStackOperation( PolicyStackProcessorNodeType type, bool value, int64_t relatedId){
	PolicyStackProcessorNode pspn;
	pspn.reason[0] = 0;
	pspn.reason[1] = 0;
	int64_t latestResult;
	int64_t secondLatestResult;

//#ifdef CONVERT_TO_DNF	
	//DNFConverterNode latestSet;
	//DNFConverterNode secondLatestSet; 
	//DNFConverterNode converterNode;
	//DNFConverterNode newSet;
	//set<uint64_t> relationSet;
	//set<uint64_t> Set;
	//uint64_t i,j;
//#endif

	switch(type){
		case PolicyStackProcessorNodeType::AND:
			latestResult = workStack.top();
			workStack.pop();
			secondLatestResult = workStack.top();
			workStack.pop();

			pspn.type = PolicyStackProcessorNodeType::AND;
			pspn.result = nodeList[latestResult].result && nodeList[secondLatestResult].result;

			if (pspn.result){
				pspn.reason[0] = secondLatestResult;
				pspn.reason[1] = latestResult;
			}
			else{
				if (nodeList[secondLatestResult].result == false)
					pspn.reason[0] = secondLatestResult;
				if (nodeList[latestResult].result == false){
					if (pspn.reason[0] == 0)
						pspn.reason[0] = latestResult;
					else
						pspn.reason[1] = latestResult;
				}
			}
//#ifdef CONVERT_TO_DNF
			//latestSet = dnfConvertorStack.top();
			//dnfConvertorStack.pop();
			//secondLatestSet = dnfConvertorStack.top();
			//dnfConvertorStack.pop();
			//for (i = 0; i < latestSet.size(); ++i)
				//for (j = 0; j < secondLatestSet.size(); ++j){
					//Set = latestSet[i];
					//Set.insert(std::begin(secondLatestSet[j]), std::end(secondLatestSet[j]));
					//newSet.push_back(Set);
				//}
			//dnfConvertorStack.push(newSet);
//#endif

			break;
		case PolicyStackProcessorNodeType::OR:
			latestResult = workStack.top();
			workStack.pop();
			secondLatestResult = workStack.top();
			workStack.pop();

			pspn.type = PolicyStackProcessorNodeType::OR;
			pspn.result = nodeList[latestResult].result || nodeList[secondLatestResult].result;

			if (pspn.result){
				if (nodeList[secondLatestResult].result == true)
					pspn.reason[0] = secondLatestResult;
				if (nodeList[latestResult].result == true){
					if (pspn.reason[0] == 0)
						pspn.reason[0] = latestResult;
					else
						pspn.reason[1] = latestResult;
				}
			}
			else{
				pspn.reason[1] = secondLatestResult;
				pspn.reason[0] = latestResult;
			}
//#ifdef CONVERT_TO_DNF
			//latestSet = dnfConvertorStack.top();
			//dnfConvertorStack.pop();
			//secondLatestSet = dnfConvertorStack.top();
			//dnfConvertorStack.pop();
			//latestSet.insert(std::end(latestSet), std::begin(secondLatestSet), std::end(secondLatestSet));
			//dnfConvertorStack.push(latestSet);
//#endif
			break;
		case PolicyStackProcessorNodeType::RELATION:
			pspn.type = PolicyStackProcessorNodeType::RELATION;
			pspn.result = value;
			pspn.reason[0] = relatedId;
//#ifdef CONVERT_TO_DNF
			//relationSet.insert(relatedId);
			//converterNode.push_back(relationSet);
			//dnfConvertorStack.push(converterNode);
//#endif
			break;
	};
	nodeList.push_back(pspn);
	workStack.push(nodeList.size() - 1);
}

vector<PolicyStackProcessorNode> PolicyStackProcessor::getReason(){
	vector<PolicyStackProcessorNode> reasonTree;

	//push_back twice to make the index of the first element in reasonTree 1;
	reasonTree.push_back(nodeList[nodeList.size() - 1]);
	reasonTree.push_back(nodeList[nodeList.size() - 1]);
	/*for(vector<PolicyStackProcessorNode>::iterator it = reasonTree.begin()+1; it != reasonTree.end(); ++it){
		if (it->type == PolicyStackProcessorNodeType::RELATION){
			continue;
		}

		reasonTree.push_back(nodeList[it->reason[0]]);
		it->reason[0] = reasonTree.size() - 1;
		if (it->reason[1] != 0){
			reasonTree.push_back(nodeList[it->reason[1]]);
			it->reason[1] = reasonTree.size() - 1;
		}
	}*/
	for (unsigned long i = 1; i != reasonTree.size();++i){
		if (reasonTree[i].type == PolicyStackProcessorNodeType::RELATION){
			continue;
		}

		reasonTree.push_back(nodeList[reasonTree[i].reason[0]]);
		reasonTree[i].reason[0] = reasonTree.size() - 1;
		if (reasonTree[i].reason[1] != 0){
			reasonTree.push_back(nodeList[reasonTree[i].reason[1]]);
			reasonTree[i].reason[1] = reasonTree.size() - 1;
		}
	}

	return reasonTree;
}

void PolicyStackProcessor::printReason(ReasonPrinter & printer){
	printer.print(getResult(), getReason());
}
