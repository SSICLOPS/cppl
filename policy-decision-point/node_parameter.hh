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
