#pragma once

#include<limits>
#include "options.hh"
#include "variable.hh"


class Relation{
	public:
		enum Types{
			EQUAL = 0,
			NEQ = 1,
			LESS = 2,
			LEQ = 3,
			GREATER = 4,
			GEQ = 5,
			IS_TRUE = 6,
			IS_FALSE = 7
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

		bool operator==(const Relation & r) const{
			if (type != r.getType() || left != r.getLeftID())
				return false;

			if (type != Types::IS_TRUE && type != Types::IS_FALSE)
				return right == r.getRightID();

			return true;
		}
		
		bool operator!=(const Relation & r) const{
			return !(*this == r);
		}

		bool doEval(const Variable * lv, const Variable * rv){return _doEval(lv, rv);}
	protected:
		virtual bool _doEval(const Variable * lv, const Variable * rv) = 0;
	private:
		Types type;

		id_type left;
		id_type right;
};
