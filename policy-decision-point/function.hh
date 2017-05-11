#pragma once

#include <vector>

#include "variable.hh"
#include "node_parameter.hh"

#ifdef __DEBUG__
#include<iostream>
#endif

#define __DO_FUNC_EVAL__(op) \
			Variable * pv = (*handler)(param_list, *current_node_parameters);\
			bool b = (*pv op v);\
			delete pv;\
			return b

typedef Variable * (* func_handler_type)(const std::vector<Variable *> &, const NodeParameters *);

class Function:public Variable{
	public:
		Function(func_handler_type h, const NodeParameters ** current):Variable(Variable::Types::FUNCTION), current_node_parameters(current),handler(h){}
		~Function(){
			for (auto it = param_list.begin(); it != param_list.end(); ++it)
				delete *it;
		}

		void addParam(Variable * v){param_list.push_back(v);}
	private:
		bool isEqu(const Variable & v) const{
			__DO_FUNC_EVAL__(==);
		}

		bool isNeq(const Variable & v) const{
			__DO_FUNC_EVAL__(!=);
		}

		bool isLess(const Variable & v) const {
			__DO_FUNC_EVAL__(<);
		}

		bool isLeq(const Variable & v) const {
			__DO_FUNC_EVAL__(<=);
		}

		bool isGre(const Variable & v) const {
			__DO_FUNC_EVAL__(>);
		}

		bool isGeq(const Variable & v) const {
			__DO_FUNC_EVAL__(>=);
		}

		bool _isTrue() const {
#ifdef __DEBUG__
			std::cout<<"funtion handler: "<<handler<<std::endl;
			std::cout<<"node parameters: "<<current_node_parameters<<std::endl;
#endif
			Variable * pv = (*handler)(param_list, *current_node_parameters);
			bool b = pv->isTrue();
			delete pv;
			return b;
		}

		const NodeParameters ** current_node_parameters;
		std::vector<Variable *> param_list;
		Variable * (*handler)(const std::vector<Variable *> & param, const NodeParameters * current);
};
