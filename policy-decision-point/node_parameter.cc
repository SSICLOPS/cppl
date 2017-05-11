#include "node_parameter.hh"
#include "policy_definition_manager.hh"

void NodeParameters::load(const std::string & param_str, pol_def_version_t v){
	clear();

	const PolicyDefinition * policyDefinition = PolicyDefinitionManager::getInstance()->getPolicyDefinition(v);
	if (policyDefinition == NULL)
		throw "no such policy definition";

	load(param_str, policyDefinition);
}

void NodeParameters::load(const std::string & param_str, const PolicyDefinition * policyDefinition){
	
	version = policyDefinition->getVersionNumber();

	Json::Value real_root;
	Json::Reader reader;
	if (!reader.parse(param_str.c_str(), real_root))
		throw "Parsing of node parameters failed" + reader.getFormattedErrorMessages();

	//version = root["version"].asUInt64();

	for (Json::ValueIterator root = real_root.begin(); root != real_root.end(); ++root){

		if ((*root).isMember("variables") == false)
			throw "node parameter file contains no variables object";

		//id_type varIdCounter = 0;
		//for (Json::ValueIterator it = root["variables"].begin(); it != root["variables"].end(); ++it)
		//loadVariableList("", varIdCounter, *it);

		//bitsForVariableList = ceil(log2(varIdCounter));

		//runtime parameter
		if ((*root)["variables"].isArray()){
			for(Json::ValueIterator it = (*root)["variables"].begin(); it != (*root)["variables"].end(); ++it){
				std::string type = (*it)["type"].asString();
				Variable * v = NULL;
				if (type.compare("int64") == 0){
					v = new Int64((*it)["value"].asInt64());
				}
				else if (type.compare("int32") == 0){
					v = new Int32((*it)["value"].asInt());
				}
				else if (type.compare("int16") == 0){
					v = new Int16((*it)["value"].asInt());
				}
				else if (type.compare("int8") == 0){
					v = new Int8((*it)["value"].asInt());
				}
				else if (type.compare("uint64") == 0){
					v = new Uint64((*it)["value"].asUInt64());
				}
				else if (type.compare("uint32") == 0){
					v = new Uint32((*it)["value"].asUInt());
				}
				else if (type.compare("uint16") == 0){
					v = new Uint16((*it)["value"].asUInt());
				}
				else if (type.compare("uint8") == 0){
					v = new Uint8((*it)["value"].asUInt());
				}
				else if (type.compare("string") == 0){
					v = new String((*it)["value"].asString());
				}
				else if (type.compare("boolean")){
					v = new Boolean((*it)["value"].asBool());
				}

				if (runtime_param_list.insert(std::pair<std::string, Variable *>((*it)["name"].asString(), v)).second == false)
					delete v;
			}
		}
		//static parameter
		else{
			for (Json::ValueIterator it = (*root)["variables"].begin(); it != (*root)["variables"].end(); ++it){
				const PDEntry * entry = policyDefinition->query(it.key().asString());
				Variable * v = NULL;
				if (entry->type == PDEntryTypes::INT64){
					v = new Int64(it->asUInt64());
				}
				else if (entry->type == PDEntryTypes::INT32){
					v = new Int32(it->asInt());
				}
				else if (entry->type == PDEntryTypes::INT16){
					v = new Int16(it->asInt());
				}
				else if (entry->type == PDEntryTypes::INT8){
					v = new Int8(it->asInt());
				}
				else if (entry->type == PDEntryTypes::UINT64){
					v = new Uint64(it->asUInt64());
				}
				else if (entry->type == PDEntryTypes::UINT32){
					v = new Uint32(it->asUInt());
				}
				else if (entry->type == PDEntryTypes::UINT16){
					v = new Uint16(it->asUInt());
				}
				else if (entry->type == PDEntryTypes::UINT8){
					v = new Uint8(it->asUInt());
				}
				else if (entry->type == PDEntryTypes::BOOLEAN){
					v = new Boolean(it->asBool());
				}
				else if (entry->type == PDEntryTypes::STRING){
					v = new String(it->asString());
				}
				else if (entry->type == PDEntryTypes::ENUM_VALUE){
					const CommonEnumEntry * c = static_cast<const CommonEnumEntry *>(entry);
					if (c->enum_type == PDEntryTypes::STRING){
						const EnumEntry<std::string> * e = static_cast<const EnumEntry<std::string> *>(c);
						v = new Enum(e->id, e->getOffset(it->asString()));
					}
					else if (c->enum_type == PDEntryTypes::INT64){
						const EnumEntry<int64_t> * e = static_cast<const EnumEntry<int64_t> *>(c);
						v = new Enum(e->id, e->getOffset(it->asInt64()));
					}
					else{
						const EnumEntry<uint64_t> * e = static_cast<const EnumEntry<uint64_t> *>(c);
						v = new Enum(e->id, e->getOffset(it->asUInt64()));
					}
				}

				if(param_list.insert(std::pair<id_type, Variable *>(entry->id, v)).second == false)
					delete v;
			}
		}
	}
};

const Variable * NodeParameters::getParameter(const id_type i) const{
	try{
		return param_list.at(i);
	}
	catch(std::out_of_range e){
		return NULL;
	}
}

const Variable * NodeParameters::getRuntimeParameter(const std::string & param_name) const{
	try{
		return runtime_param_list.at(param_name);
	}
	catch(std::out_of_range e){
		return NULL;
	}
}

void NodeParameters::clear(){
	for (auto it = param_list.begin(); it != param_list.end(); ++it)
		delete it->second;

	for (auto it = runtime_param_list.begin(); it != runtime_param_list.end(); ++it)
		delete it->second;

	param_list.clear();
	runtime_param_list.clear();
}

std::string NodeParameters::getParameterName(const id_type i) const {
	PolicyDefinitionManager * pdm = PolicyDefinitionManager::getInstance();

	return pdm->getPolicyDefinition(version)->getNameByID(i);
}
