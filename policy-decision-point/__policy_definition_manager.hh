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
#include<fstream>
#include<dlfcn.h>

#include "options.hh"
#include "policy_definition.hh"

template<typename Installer>
class __PolicyDefinitionManager:public Installer{
	public:
		static __PolicyDefinitionManager<Installer> * getInstance(){
			static Guard g;

			if (instance == NULL)
				instance = new __PolicyDefinitionManager<Installer>();

			return instance;
		}

		const PolicyDefinition * getPolicyDefinition(pol_def_version_t v);

	private:
		__PolicyDefinitionManager<Installer>(){}
		__PolicyDefinitionManager<Installer>(__PolicyDefinitionManager<__PolicyDefinitionManager<Installer>> &){}

		~__PolicyDefinitionManager<Installer>(){
			for (auto it = pol_def_list.begin(); it != pol_def_list.end(); ++it)
				delete it->second;
		}

		const PolicyDefinition * load(pol_def_version_t v);
		std::string getJsonFile(const std::string & filename){
			std::ifstream file(filename);
			if (!file.is_open())
				throw "Can't open file" + filename;

			std::stringstream strStream;
			strStream << file.rdbuf();
			file.close();
			return strStream.str();
		}

		void * getFuncHandler(const std::string & libname){
			void * h =  dlopen(libname.c_str(), RTLD_LAZY); 
			if (!h){
				std::string str = dlerror();
				throw "Can't load lib" + libname;
			}
			return h;
		}
		
		static __PolicyDefinitionManager<Installer> * instance;
		std::map<pol_def_version_t, PolicyDefinition *> pol_def_list;


		class Guard{
			public:
				~Guard(){
					if (instance != NULL)
						delete instance;
				}
		};
};

template<typename Installer>
__PolicyDefinitionManager<Installer> * __PolicyDefinitionManager<Installer>::instance = NULL;

template<typename Installer>
const PolicyDefinition * __PolicyDefinitionManager<Installer>::load(pol_def_version_t v){
	try{
		const typename Installer::Node * path = Installer::getFilePath(v);
		if (path == NULL)
			return NULL;

		void * handler = getFuncHandler(path->funcHandlerPath);
		std::string jsonStr = getJsonFile(path->jsonFilePath);

		PolicyDefinition * pd = new PolicyDefinition;
		pd->load(jsonStr, handler);
		pol_def_list.insert(std::pair<pol_def_version_t, PolicyDefinition *>(pd->getVersionNumber(), pd));
		return pd;
	}
	catch (std::string e){
		return NULL;
	}

}

template<typename Installer>
const PolicyDefinition * __PolicyDefinitionManager<Installer>::getPolicyDefinition(pol_def_version_t v){
	try{
		return pol_def_list.at(v);
	}
	catch(std::out_of_range e){
		return load(v);
	}
}
