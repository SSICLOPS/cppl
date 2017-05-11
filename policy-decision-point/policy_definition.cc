#include <cmath>
#include<dlfcn.h>

#include "policy_definition.hh"

PolicyDefinition::~PolicyDefinition(){
	clear();
}

void PolicyDefinition::clear(){
	for (std::map<std::string, PDEntry *>::iterator it = variable_defs.begin(); it != variable_defs.end(); ++it)
		delete it->second;

	variable_index.clear();

	if (funcHandler != NULL)
		dlclose(funcHandler);
}

void PolicyDefinition::load(const std::string & def_str, const std::string & libname){
	void * h = NULL;
	if (libname != ""){
		h =  dlopen(libname.c_str(), RTLD_LAZY); 
		if (!h){
			std::string str = dlerror();
			throw "Can't load lib" + libname;
		}
	}
	load(def_str, h);
}

void PolicyDefinition::load(const std::string & def_str, void * func_handler){
	clear();

	funcHandler = func_handler;

	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(def_str.c_str(), root))
		throw "Parsing of policy definition failed" + reader.getFormattedErrorMessages();

	version = root["version"].asUInt64();

	if (root.isMember("variables") == false)
		throw "Policy definition contains no variables object";

	id_type varIdCounter = 0;
	for (Json::ValueIterator it = root["variables"].begin(); it != root["variables"].end(); ++it)
		loadVariableList("", varIdCounter, *it);

	bitsForVariableList = ceil(log2(varIdCounter));
}

void PolicyDefinition::loadVariableList(const std::string & prefix, id_type & varIdCounter, Json::Value elem){
	PDEntry * entry = NULL;
	std::string var_name;
	//todo: use small object allocator
	if (elem.isObject() && elem.isMember("name") && elem.isMember("type")){
		if (elem.isMember("values")){
			if (elem["values"].isArray() == false)
				throw "Policy Definition: A values member in the policy definition json file is not an array";

			std::string strType = elem["type"].asString();
			if (strType.compare("string") == 0){
				EnumEntry<std::string> * enumEntry = new EnumEntry<std::string>();
				enumEntry->enum_type = PDEntryTypes::STRING;

				for (Json::ValueIterator it = elem["values"].begin(); it != elem["values"].end(); ++it){
					if ((*it).isString() == false)
						throw "Policy Definition: An enum value has not the same type as the defined enum";

					enumEntry->enum_elem.push_back((*it).asString());
				}

				entry = enumEntry;
			}
			else if (strType.compare("int64") == 0){
				EnumEntry<int64_t> * enumEntry = new EnumEntry<int64_t>();
				enumEntry->enum_type = PDEntryTypes::INT64;

				for (Json::ValueIterator it = elem["values"].begin(); it != elem["values"].end(); ++it){
					if ((*it).isInt64() == false)
						throw "Policy Definition: An enum value has not the same type as the defined enum";

					enumEntry->enum_elem.push_back((*it).asInt64());
				}

				entry = enumEntry;
			}
			else if (strType.compare("uint64") == 0){
				EnumEntry<uint64_t> * enumEntry = new EnumEntry<uint64_t>();
				enumEntry->enum_type = PDEntryTypes::UINT64;

				for (Json::ValueIterator it = elem["values"].begin(); it != elem["values"].end(); ++it){
					if ((*it).isUInt64() == false)
						throw "Policy Definition: An enum value has not the same type as the defined enum";

					enumEntry->enum_elem.push_back((*it).asUInt64());
				}

				entry = enumEntry;
			}
			else
				throw "Policy Definition: Unknown enum type";

			entry->type = PDEntryTypes::ENUM_VALUE;
		}
		else if (elem.isMember("parameters")){
			if (elem["parameters"].isArray() == false)
				throw "Policy Definition: A parameters member of a function is not an array";

			FuncEntry * funcEntry = new FuncEntry();
			funcEntry->type = PDEntryTypes::FUNCTION;
			funcEntry->enum_para_list = NULL;
			std::string strType;

			for (Json::ValueIterator it = elem["parameters"].begin(); it != elem["parameters"].end(); ++it){
				strType = (*it).asString();
				if (strType.compare("string") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::STRING);
				}
				else if (strType.compare("boolean") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::BOOLEAN);
				}
				else if (strType.compare("int64") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::INT64);
				}
				else if (strType.compare("int32") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::INT32);
				}
				else if (strType.compare("int16") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::INT16);
				}
				else if (strType.compare("int8") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::INT8);
				}
				else if (strType.compare("uint64") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::UINT64);
				}
				else if (strType.compare("uint32") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::UINT32);
				}
				else if (strType.compare("uint16") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::UINT16);
				}
				else if (strType.compare("uint8") == 0){
					funcEntry->para_list.push_back(PDEntryTypes::UINT8);
				}
				else{
					const PDEntry * e = query(strType);
					if (e != NULL && e->type == PDEntryTypes::ENUM_VALUE){
						if (funcEntry->enum_para_list == NULL){
							funcEntry->enum_para_list = new std::vector<id_type>();
						}
						funcEntry->enum_para_list->push_back(e->id);
						funcEntry->para_list.push_back(PDEntryTypes::ENUM_VALUE);
					}
					else{
						delete funcEntry;
						throw "Policy Definition: Unkown parameter type";
					}
				}

				if (funcHandler != NULL){
					std::string func_name = prefix + elem["name"].asString();
					std::replace(func_name.begin(), func_name.end(), '.', '_');
					*(void**)(&(funcEntry->handler)) = dlsym(funcHandler, func_name.c_str());
#ifdef __DEBUG__
					std::cout<<"PolicyDefinition function handler: "<<func_name<<": "<<funcEntry->handler<<std::endl;
#endif
				}
			}

			entry = funcEntry;
		}
		else {
			entry = new PDEntry();
			std::string strType = elem["type"].asString();
			if (strType.compare("string") == 0){
				entry->type = PDEntryTypes::STRING;
			}
			else if (strType.compare("boolean") == 0){
				entry->type = PDEntryTypes::BOOLEAN;
			}
			//else if (strType.compare("function") == 0){
				//todo
			//}
			else if (strType.compare("int64") == 0){
				entry->type = PDEntryTypes::INT64;
			}
			else if (strType.compare("int32") == 0){
				entry->type = PDEntryTypes::INT32;
			}
			else if (strType.compare("int16") == 0){
				entry->type = PDEntryTypes::INT16;
			}
			else if (strType.compare("int8") == 0){
				entry->type = PDEntryTypes::INT8;
			}
			else if (strType.compare("uint64") == 0){
				entry->type = PDEntryTypes::UINT64;
			}
			else if (strType.compare("uint32") == 0){
				entry->type = PDEntryTypes::UINT32;
			}
			else if (strType.compare("uint16") == 0){
				entry->type = PDEntryTypes::UINT16;
			}
			else if (strType.compare("uint8") == 0){
				entry->type = PDEntryTypes::UINT8;
			}
			else{
				delete entry;
				throw "Policy Definition: Unknown variable type";
			}
		}

		entry->id = varIdCounter++;
		std::string varName = prefix + elem["name"].asString();
		variable_defs.insert(std::pair<std::string, PDEntry *>(varName, entry));
		variable_index.insert(std::pair<id_type, std::string>(entry->id, varName));
	}
	else if (elem.isObject() && elem.isMember("name") && elem.isMember("variables")){
		std::string newPrefix = prefix + elem["name"].asString() + ".";
		for (Json::ValueIterator it = elem["variables"].begin(); it != elem["variables"].end(); ++it)
			loadVariableList(newPrefix, varIdCounter, (*it));
	}
	
}

const PDEntry * PolicyDefinition::query(const std::string var_name) const{
	try{
		return variable_defs.at(var_name);
	}
	catch(std::out_of_range e){
		return NULL;
	}
}

const PDEntry * PolicyDefinition::queryByID(const id_type i) const {
	try{
		return query(variable_index.at(i));
	}
	catch(std::out_of_range e){
		return NULL;
	}
}

std::string PolicyDefinition::getNameByID(const id_type i) const {
	try{
		return variable_index.at(i);
	}
	catch(std::out_of_range e){
		return "";
	}
}

id_type PolicyDefinition::getEnumElemNum(id_type id) const{
	try{
		const PDEntry * pde = query(variable_index.at(id));
		if (pde->type != PDEntryTypes::ENUM_VALUE)
			return 0;

		return ((CommonEnumEntry *)pde)->getElemNum();
	}
	catch(std::out_of_range e){
		return 0;
	}
}
