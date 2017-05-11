#pragma once

#include <stack>
#include <set>
#include "options.hh"

#include "__policy_stack.hh"

typedef std::vector<std::set<id_type> > DNFConverterNode;

class DNFConvertor{
	public:
		DNFConvertor(const stack_type & rpn):m_rpn(rpn){}
		void doConvert();
		DNFConverterNode & getDNF(){return m_dnfConvertorStack.top();}
	private:
		std::stack<DNFConverterNode> m_dnfConvertorStack;
		stack_type m_rpn;
};
