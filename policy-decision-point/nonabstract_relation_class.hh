#include "relation.hh"

//used exclusivly by compressing cppl
class NonabstractRelationClass: public Relation{
	public:
		NonabstractRelationClass(Types t, id_type l = std::numeric_limits<id_type>::max(), id_type r = std::numeric_limits<id_type>::max())
			:Relation(t, l, r)
		{
		}

	private:
		bool doEval(const Variable * lv, const Variable * rv){return false;}
		bool _doEval(const Variable * lv, const Variable * rv) override {return false;}
};
