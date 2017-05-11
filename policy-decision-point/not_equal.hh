#pragma once

#include "variable.hh"
#include "relation.hh"

class NotEqual:public Relation{
	public:
		NotEqual(id_type lid, id_type rid):Relation(Relation::Types::NEQ, lid, rid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return *lv != *rv;}
};
