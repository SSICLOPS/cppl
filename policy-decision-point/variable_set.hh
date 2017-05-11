#include <vector>
#include <limits>
#include "variable.hh"

class VariableSet{
	public:
		~VariableSet(){for (auto it = variable_list.begin(); it != variable_list.end(); ++it) delete *it;}
		id_type addVariable(Variable * v){variable_list.push_back(v);return variable_list.size() - 1;}
		id_type getVariableID(const Variable & v) const{
			id_type id = 0;
			for(; id != variable_list.size() && !(v.sameAs(*(variable_list[id]))); ++id);

			if (id == variable_list.size())
				return std::numeric_limits<id_type>::max();
			else
				return id;
		}
		const Variable * getVariable(id_type id) const{
			if (id >= variable_list.size())
				return 0;

			return variable_list[id];
		}

		inline id_type getVariableNum() const {return variable_list.size();}
	private:
		std::vector<Variable *> variable_list;
};
