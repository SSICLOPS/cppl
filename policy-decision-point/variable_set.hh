#include <vector>
#include <limits>
#include "variable.hh"

class VariableSet{
	public:
		~VariableSet(){for (auto it = variable_list.begin(); it != variable_list.end(); ++it) delete *it;}
		id_type addVariable(Variable * v){variable_list.push_back(v);return variable_list.size() - 1;}
		const Variable * getVariable(id_type id) const{
			if (id >= variable_list.size())
				return 0;

			return variable_list[id];
		}
	private:
		std::vector<Variable *> variable_list;
};
