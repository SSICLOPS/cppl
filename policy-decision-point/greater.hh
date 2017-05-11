#pragma once

#include "variable.hh"
#include "relation.hh"

class Greater:public Relation{
	public:
		Greater(id_type lid, id_type rid):Relation(Relation::Types::GREATER, lid, rid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return *lv > *rv;}
};
