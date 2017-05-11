#pragma once

#include "variable.hh"
#include "relation.hh"

class Equal:public Relation{
	public:
		Equal(id_type lid, id_type rid):Relation(Relation::Types::EQUAL, lid, rid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return *lv == *rv;}
};
