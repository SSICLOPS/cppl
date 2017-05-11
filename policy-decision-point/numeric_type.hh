#pragma once

#include<stdint.h>

//#include "__numeric_type.hh"
#include "variable.hh"

#define __DO_NUM_EVAL__(op)\
if (get_type() == v.get_type())\
	return value op reinterpret_cast<const NumericType<T,U> &>(v).get_value();\
switch (v.get_type()){\
	case Variable::Types::UINT8:\
		return value op reinterpret_cast<const Uint8 &>(v).get_value();\
	case Variable::Types::UINT16:\
		return value op reinterpret_cast<const Uint16 &>(v).get_value();\
	case Variable::Types::UINT32:\
		return value op reinterpret_cast<const Uint32 &>(v).get_value();\
	case Variable::Types::UINT64:\
		return value op reinterpret_cast<const Uint64 &>(v).get_value();\
	case Variable::Types::INT8:\
		return value op reinterpret_cast<const Int8 &>(v).get_value();\
	case Variable::Types::INT16:\
		return value op reinterpret_cast<const Int16 &>(v).get_value();\
	case Variable::Types::INT32:\
		return value op reinterpret_cast<const Int32 &>(v).get_value();\
	case Variable::Types::INT64:\
		return value op reinterpret_cast<const Int64 &>(v).get_value();\
	case Variable::Types::DOUBLE:\
		return value op reinterpret_cast<const Double &>(v).get_value();\
	default:\
		return false;\
}

template<typename T, Variable::Types U>
class NumericType;

typedef NumericType<int8_t, Variable::Types::INT8> Int8;
typedef NumericType<int16_t, Variable::Types::INT16> Int16;
typedef NumericType<int32_t, Variable::Types::INT32> Int32;
typedef NumericType<int64_t, Variable::Types::INT64> Int64;
typedef NumericType<uint8_t, Variable::Types::UINT8> Uint8;
typedef NumericType<uint16_t, Variable::Types::UINT16> Uint16;
typedef NumericType<uint32_t, Variable::Types::UINT32> Uint32;
typedef NumericType<uint64_t, Variable::Types::UINT64> Uint64;
typedef NumericType<double, Variable::Types::DOUBLE> Double;

template<typename T, Variable::Types U>
class NumericType: public Variable{//public __NumericType<NumericType<T, U>, U >{
	public:
		NumericType<T, U>():Variable(U){}
		NumericType<T, U>(const T & v):Variable(U),value(v){}
		NumericType<T, U>(const NumericType<T, U> & v):Variable(U), value(v.get_value()){}

		T get_value() const {return value;}
		void set_value(const T & v){value = v;}

		bool operator==(const NumericType<T, U> & other) const {return value == other.get_value();}
		bool operator!=(const NumericType<T, U> & other) const {return value != other.get_value();}
		bool operator<(const NumericType<T, U> & other) const {return value < other.get_value();}
		bool operator<=(const NumericType<T, U> & other) const {return value <= other.get_value();}
		bool operator>(const NumericType<T, U> & other) const {return value > other.get_value();}
		bool operator>=(const NumericType<T, U> & other) const {return value >= other.get_value();}

		bool isTrue() const{return _isTrue(value);}

		T operator=(const T v) {return (value = v);}
		const NumericType<T, U> & operator=(const NumericType<T, U> & other){value = other.get_value(); return *this;}

	private:
		bool isEqu(const Variable & v) const{
			__DO_NUM_EVAL__(==);
		}
		bool isNeq(const Variable & v) const{
			__DO_NUM_EVAL__(!=);
		}
		bool isLess(const Variable & v) const{
			__DO_NUM_EVAL__(<);
		}
		bool isLeq(const Variable & v) const{
			__DO_NUM_EVAL__(<=);
		}
		bool isGre(const Variable & v) const{
			__DO_NUM_EVAL__(>);
		}
		bool isGeq(const Variable & v) const{
			__DO_NUM_EVAL__(>=);
		}

		bool _isTrue() const{return (bool)value;}

		T value;
};
