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
		Function(id_type id, func_handler_type h, const NodeParameters ** current)
			:Variable(Variable::Types::FUNCTION), current_node_parameters(current), id(id), handler(h)
		{
		}

		~Function(){
			for (auto it = param_list.begin(); it != param_list.end(); ++it)
				delete *it;
		}

		inline id_type get_id() const{return id;}

		void addParam(Variable * v){param_list.push_back(v);}
		inline id_type getParameterNum() const {return param_list.size();}
		inline const Variable * getParameter(id_type id) const {return param_list[id];}
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

		bool _sameAs(const Variable & v) const{
			if (this->get_type() != v.get_type())
				return false;

			if (id != reinterpret_cast<const Function &>(v).get_id())
				return false;

			size_t i = 0;
			for (; i != param_list.size() && param_list[i]->sameAs(*(reinterpret_cast<const Function &>(v).param_list[i])); ++i);
			if (i == param_list.size())
				return true;
			else
				return false;
		}

		const NodeParameters ** current_node_parameters;
		id_type id;
		std::vector<Variable *> param_list;
		Variable * (*handler)(const std::vector<Variable *> & param, const NodeParameters * current);
};
