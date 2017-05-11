#pragma once

#include "variable.hh"
#include "relation.hh"

class GreaterEqual:public Relation{
	public:
		GreaterEqual(id_type lid, id_type rid):Relation(Relation::Types::GEQ, lid, rid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return *lv >= *rv;}
};
