#pragma once

#include "variable.hh"

#ifdef __USE_VIRTUAL_FUNC__
#define __DO_EVAL__(op) return ((*this).get_type() == v.get_type())\
							&& (reinterpret_cast<const T &>(*this) op reinterpret_cast<const T &>(v))
#endif //__USE_VIRTUAL_FUNC__

template<typename T, Variable::Types U>
class __StrongTypeVariable:public Variable{
	protected:
		__StrongTypeVariable<T,U>():Variable(U){}

		bool isEqu(const Variable & v) const{
			__DO_EVAL__(==);
		}
		bool isNeq(const Variable & v) const{
			__DO_EVAL__(!=);
		}
		bool isLess(const Variable & v) const{
			__DO_EVAL__(<);
		}
		bool isLeq(const Variable & v) const{
			__DO_EVAL__(<=);
		}
		bool isGre(const Variable & v) const{
			__DO_EVAL__(>);
		}
		bool isGeq(const Variable & v) const{
			__DO_EVAL__(>=);
		}

		bool _isTrue() const{return (static_cast<const T *>(this)->isTrue());}

	private:	
		bool operator==(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator!=(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator<(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator<=(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator>(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator>=(const __StrongTypeVariable<T, U> &) const{return false;}
};
