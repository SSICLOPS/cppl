#pragma once

//#include<typeinfo>
#include<stdint.h>
#include<string>

#include "__strong_type_variable.hh"

template<typename T, Variable::Types U>
class StrongTypeVariable: public __StrongTypeVariable<StrongTypeVariable<T, U>, U >{
	public:
		StrongTypeVariable<T, U>(){}
		StrongTypeVariable<T, U>(const T & v):value(v){}
		StrongTypeVariable<T, U>(const StrongTypeVariable<T, U> & v):value(v.get_value()){}

		T get_value() const {return value;}
		void set_value(const T & v){value = v;}

		bool operator==(const StrongTypeVariable<T, U> & other) const {return value == other.get_value();}
		bool operator!=(const StrongTypeVariable<T, U> & other) const {return value != other.get_value();}
		bool operator<(const StrongTypeVariable<T, U> & other) const {return value < other.get_value();}
		bool operator<=(const StrongTypeVariable<T, U> & other) const {return value <= other.get_value();}
		bool operator>(const StrongTypeVariable<T, U> & other) const {return value > other.get_value();}
		bool operator>=(const StrongTypeVariable<T, U> & other) const {return value >= other.get_value();}

		bool isTrue() const{return _isTrue(value);}

		T operator=(const T v) {return (value = v);}
		const StrongTypeVariable<T, U> & operator=(const StrongTypeVariable<T, U> & other){value = other.get_value(); return *this;}

	private:
		template<typename V> inline bool _isTrue(const V & v) const {return v;}
		T value;
};

template<> template<>
inline bool StrongTypeVariable<std::string, Variable::Types::STRING>::_isTrue<std::string>(const std::string & v) const{
	return v.size();
}

typedef StrongTypeVariable<bool, Variable::Types::BOOLEAN> Boolean;
typedef StrongTypeVariable<std::string, Variable::Types::STRING> String;
