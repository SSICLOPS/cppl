#pragma once

#include<map>

#include "options.hh"
#include "variable.hh"
#include "basic_type.hh"
#include "enumeration.hh"
#include "policy_definition.hh"

class PolicyDefinition;

class NodeParameters{
	public:
		~NodeParameters(){clear();}

		void load(const std::string & param_str, pol_def_version_t version);		
		void load(const std::string & param_str, const PolicyDefinition * policyDefinition);
		const Variable * getParameter(const id_type i) const;
		std::string getParameterName(const id_type i) const;
		const Variable * getRuntimeParameter(const std::string & param_name) const;

		void clear();
	private:
		pol_def_version_t version;
		std::map<id_type, Variable *> param_list;
		std::map<std::string, Variable *>runtime_param_list;
};
