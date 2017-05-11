#pragma once

#include "variable.hh"
#include "relation.hh"

class LessEqual:public Relation{
	public:
		LessEqual(id_type lid, id_type rid):Relation(Relation::Types::LEQ, lid, rid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return *lv <= *rv;}
};

