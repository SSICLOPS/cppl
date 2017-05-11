#pragma once

#include<limits>

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

		id_type getRelationID(const Relation & r) const;
		inline id_type getVariableID(const Variable & v) const {return variable_set.getVariableID(v);}

		inline id_type getRelationNum() const {return relation_list.size();}
		inline id_type getVariableNum() const {return variable_set.getVariableNum();}

		inline const Relation * getRelation(id_type relation_id) const {return relation_list[relation_id].relation;}
		inline const Variable * getVariable(id_type variable_id) const {return variable_set.getVariable(variable_id);}

		void doEval();
	private:
		struct RelationSetNode{
			bool evalResult;
			Relation * relation;
		};

		std::vector<RelationSetNode> relation_list;
		VariableSet variable_set;
};
