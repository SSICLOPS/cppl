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

enum PDEntryTypes{
	INT8,
	INT16,
	INT32,
	INT64,
	UINT8,
	UINT16,
	UINT32,
	UINT64,
	DOUBLE,
	BOOLEAN,
	STRING,
	ENUM_VALUE,
	FUNCTION
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

	 	void load(std::string def_str, void * func_handler = NULL);
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
