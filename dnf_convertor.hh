#pragma once

#include <stack>

#include "policy_stack.hh"

typedef std::vector<std::set<uint64_t>> DNFConverterNode;

class DNFConvertor{
	public:
		DNFConvertor(std::stack<StackOperation> & rpn):m_rpn(rpn){}
		void doConvert();
		DNFConverterNode & getDNF(){return m_dnfConvertorStack.top();}
	private:
		std::stack<DNFConverterNode> m_dnfConvertorStack;
		std::stack<StackOperation> m_rpn;
};
