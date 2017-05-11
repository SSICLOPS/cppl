#pragma once

#include "options.hh"

class Variable{
	public:
		enum Types{
			INT8,
			INT16,
			INT32,
			INT64,
			UINT8,
			UINT16,
			UINT32,
			UINT64,
			DOUBLE,
			BOOLEAN,
			STRING,
			ID,
			ENUM_VALUE,
			FUNCTION
		};

		Variable(Types t):type(t){}

		inline Types get_type() const {return type;}
		inline void set_type(const Types t){type = t;}

		inline bool typeEquTo(const Variable & v) const {return type == v.get_type();}
		//inline bool comparableTo(const Variable & v) const {
			//return (type == Types::ID || type == v.get_type()\
					|| (type <= Types::INT64 && v.get_type() <= INT64)\
					//|| (type <= Types::UINT64 && type >= Types::UINT8 && v.get_type() <= Types::UINT64 && v.get_type() >= Types::UINT8));*/
					//|| (type <= Types::UINT64 && v.get_type() <= Types::UINT64));//should not be this way. the ccppl generator may not corret
		//}

		virtual ~Variable(){}

#ifdef __USE_VIRTUAL_FUNC__
		bool operator==(const Variable & v) const {/*if (!comparableTo(v)) return false; else*/ return isEqu(v);}
		bool operator!=(const Variable & v) const {/*if (!comparableTo(v)) return true; else*/ return isNeq(v);}
		bool operator<(const Variable & v) const {/*if (!comparableTo(v)) return false; else*/ return isLess(v);}
		bool operator<=(const Variable & v) const {/*if (!comparableTo(v)) return false; else*/ return isLeq(v);}
		bool operator>(const Variable & v) const {/*if (!comparableTo(v)) return false; else*/ return isGre(v);}
		bool operator>=(const Variable & v) const {/*if (!comparableTo(v)) return false; else*/ return isGeq(v);}

		bool isTrue() const {return _isTrue();}
#endif //__USE_VIRTAUL_FUNC__

	protected:
		Types type;

#ifdef __USE_VIRTUAL_FUNC__
	protected:
		//Precondition: v is a variable of the same type
		virtual bool isEqu(const Variable & v) const = 0;
		virtual bool isNeq(const Variable & v) const = 0;
		virtual bool isLess(const Variable & v) const = 0;
		virtual bool isLeq(const Variable & v) const = 0;
		virtual bool isGre(const Variable & v) const = 0;
		virtual bool isGeq(const Variable & v) const = 0;

		virtual bool _isTrue() const {return false;}
#endif //__USE_VIRTUAL_FUNC__
};
