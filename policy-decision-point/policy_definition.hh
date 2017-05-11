#pragma once

#include<string>
#include<vector>
#include<map>
#include<limits>
#include<algorithm>
#include <dlfcn.h>

#include "../json/json.h"
#include "options.hh"
#include "variable.hh"
#include "node_parameter.hh"

class NodeParameters;

enum PDEntryTypes : uint8_t{
	INT8 = 0,
	INT16 = 1,
	INT32 = 2,
	INT64 = 3,
	UINT8 = 4,
	UINT16 = 5,
	UINT32 = 6,
	UINT64 = 7,
	DOUBLE = 8,
	BOOLEAN = 9,
	STRING = 10,
	ENUM_VALUE = 11,
	FUNCTION = 12
};

struct PDEntry{
	PDEntryTypes type;
	id_type id;

	virtual ~PDEntry(){}
};

struct CommonEnumEntry:public PDEntry{
	PDEntryTypes enum_type;
	virtual ~CommonEnumEntry(){}
	virtual id_type getElemNum() const {return 0;}
	virtual void print(std::ostream & out, id_type offset) const {out<<offset;}
};

#define no_entry (std::numeric_limits<id_type>::max())

template<typename T>
struct EnumEntry:public CommonEnumEntry{
	std::vector<T> enum_elem;
	id_type getOffset(const T & elem) const {
			auto it = std::find(enum_elem.begin(), enum_elem.end(), elem);
			if (it == enum_elem.end())
				return no_entry;

			return std::distance(enum_elem.begin(), it);
	}

	id_type getElemNum() const {return enum_elem.size();}
	void print(std::ostream & out, id_type offset) const {
		if (offset < enum_elem.size()){
			out<<enum_elem[offset];
		}
	}
};

struct FuncEntry:public PDEntry{
	std::vector<PDEntryTypes> para_list;
	Variable * (* handler)(const std::vector<Variable *> &, const NodeParameters *);
	std::vector<id_type> * enum_para_list;

	~FuncEntry(){if (enum_para_list) delete enum_para_list;}
};

class PolicyDefinition{
	public:
		PolicyDefinition():version(0), bitsForVariableList(0), funcHandler(NULL){}
		~PolicyDefinition();

		void load(const std::string & def_str, const std::string & function_handler_path);
	 	void load(const std::string & def_str, void * func_handler = NULL);
		const PDEntry * query(const std::string var_name) const;
		const PDEntry * queryByID(const id_type i) const;
		std::string getNameByID(const id_type i) const;

		inline pol_def_version_t getVersionNumber() const {return version;}
		inline id_type getVarNum() const {return variable_defs.size();}
		id_type getEnumElemNum(id_type id) const;
	private:
		pol_def_version_t version;
		std::map<std::string, PDEntry *> variable_defs;
		std::map<id_type, std::string> variable_index;
		uint8_t bitsForVariableList;
		void * funcHandler;

		void loadVariableList(const std::string & prefix, id_type & varIdCounter, Json::Value elem);
		void clear();
};
