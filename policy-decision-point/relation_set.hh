#pragma once

#include "relation.hh"
#include "variable_set.hh"

class RelationSet{
	public:
		~RelationSet(){
			for (auto it = relation_list.begin(); it != relation_list.end(); ++it)
				delete (*it).relation;
		}

		id_type addRelation(Relation * r){relation_list.push_back({false, r});return relation_list.size() - 1;}
		id_type addVariable(Variable * v){return variable_set.addVariable(v);}

		bool getEvalResult(id_type relation_id){return relation_list[relation_id].evalResult;}

		void doEval();
	private:
		struct RelationSetNode{
			bool evalResult;
			Relation * relation;
		};

		std::vector<RelationSetNode> relation_list;
		VariableSet variable_set;
};
