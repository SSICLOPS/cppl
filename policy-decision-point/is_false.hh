#pragma once

#include "variable.hh"
#include "relation.hh"

class IsFalse:public Relation{
	public:
		IsFalse(id_type lid):Relation(Relation::Types::IS_TRUE, lid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return ! lv->isTrue();}
};
