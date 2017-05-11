#pragma once

#include"variable.hh"
#include "node_parameter.hh"

class ID:public Variable{
	public:
		ID(const NodeParameters ** current, id_type i = 0):Variable(Variable::Types::ID), id(i), current_node_parameters(current){}

		const Variable * getValue() const {return (*current_node_parameters)->getParameter(id);}
		std::string getName() const {return (*current_node_parameters)->getParameterName(id);}
		
		inline id_type getID() const {return id;}
		inline void setID(id_type i){id = i;}

	private:
		bool isEqu(const Variable & v) const{
			return (*getValue()) == v;
		}

		bool isNeq(const Variable & v) const{
			return (*getValue()) != v;
		}

		bool isLess(const Variable & v) const{
			return (*getValue()) < v;
		}

		bool isLeq(const Variable & v) const{
			return (*getValue()) <= v;
		}

		bool isGre(const Variable & v) const{
			return (*getValue()) > v;
		}

		bool isGeq(const Variable & v) const{
			return (*getValue()) >= v;
		}

		bool _isTrue() const {return getValue()->isTrue();}

		bool _sameAs(const Variable & v) const{
			return get_type() == v.get_type() && id == reinterpret_cast<const ID &>(v).getID();
		}

		id_type id;
		const NodeParameters ** current_node_parameters;
};
