#pragma once

#include "variable.hh"
#include "relation.hh"

class Less:public Relation{
	public:
		Less(id_type lid, id_type rid):Relation(Relation::Types::LESS, lid, rid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return *lv < *rv;}
};

