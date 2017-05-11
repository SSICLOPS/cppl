#pragma once

#include<map>
#include<string>

#include "options.hh"

class SimpleInstaller{
	public:
		struct Node{
			std::string jsonFilePath;
			std::string funcHandlerPath;
		};

		SimpleInstaller(){
			Node n;
			n.jsonFilePath = "../examples/example_paper/policy_definition.json";
			n.funcHandlerPath = "./func_handler_23.so";

			installed_list.insert(std::pair<pol_def_version_t, Node>(23, n));
		}

		const Node * getFilePath(pol_def_version_t v){try{return &(installed_list.at(v));}catch (std::out_of_range e){return NULL;}}
	private:
		std::map<pol_def_version_t, Node> installed_list;
};
