#include "dnf_convertor.hh"

void DNFConvertor::doConvert(){
	uint64_t nextRelationCounter = 0;
	
	StackOperation stackOp;


	while(! m_rpn.empty()){
		stackOp = m_rpn.top();
		m_rpn.pop();

		if (stackOp.type == PolicyStackOperationType::AND){
	DNFConverterNode latestSet;
	DNFConverterNode secondLatestSet; 
	DNFConverterNode newSet;
	set<uint64_t> Set;
	uint64_t i,j;
			latestSet = m_dnfConvertorStack.top();
			m_dnfConvertorStack.pop();
			secondLatestSet = m_dnfConvertorStack.top();
			m_dnfConvertorStack.pop();
			for (i = 0; i < latestSet.size(); ++i)
				for (j = 0; j < secondLatestSet.size(); ++j){
					Set = latestSet[i];
					Set.insert(std::begin(secondLatestSet[j]), std::end(secondLatestSet[j]));
					newSet.push_back(Set);
				}
			m_dnfConvertorStack.push(newSet);
		}
		else if (stackOp.type == PolicyStackOperationType::OR){
	DNFConverterNode latestSet;
	DNFConverterNode secondLatestSet; 
			latestSet = m_dnfConvertorStack.top();
			m_dnfConvertorStack.pop();
			secondLatestSet = m_dnfConvertorStack.top();
			m_dnfConvertorStack.pop();
			latestSet.insert(std::end(latestSet), std::begin(secondLatestSet), std::end(secondLatestSet));
			m_dnfConvertorStack.push(latestSet);
		}
		else if (stackOp.type == PolicyStackOperationType::NEXT_RELATION){
	DNFConverterNode converterNode;
	set<uint64_t> relationSet;
			relationSet.insert(nextRelationCounter++);
			converterNode.push_back(relationSet);
			m_dnfConvertorStack.push(converterNode);
		}
		else if (stackOp.type == PolicyStackOperationType::SPECIFIC_RELATION){
	DNFConverterNode converterNode;
	set<uint64_t> relationSet;
			relationSet.insert(stackOp.relationId);
			converterNode.push_back(relationSet);
			m_dnfConvertorStack.push(converterNode);
		}
	}
}
