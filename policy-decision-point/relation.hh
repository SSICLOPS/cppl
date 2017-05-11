#pragma once

#include<limits>
#include "options.hh"
#include "variable.hh"


class Relation{
	public:
		enum Types{
			EQUAL,
			NEQ,
			LESS,
			LEQ,
			GREATER,
			GEQ,
			IS_TRUE,
			IS_FALSE
//			END
		};

		Relation(Types t, id_type l = std::numeric_limits<id_type>::max(), id_type r = std::numeric_limits<id_type>::max()):type(t),left(l), right(r){}
		virtual ~Relation(){}

		inline void setLeftID(id_type l) {left = l;}
		inline void setRightID(id_type r) {right = r;}
		inline void setType(Types t) {type = t;}
		inline id_type getLeftID() const {return left;}
		inline id_type getRightID() const {return right;}
		inline Types getType() const {return type;}

		bool doEval(const Variable * lv, const Variable * rv){return _doEval(lv, rv);}
	protected:
		virtual bool _doEval(const Variable * lv, const Variable * rv) = 0;
	private:
		Types type;

		id_type left;
		id_type right;
};
