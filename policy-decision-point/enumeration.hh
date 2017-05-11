#pragma once

#include "options.hh"
#include "__strong_type_variable.hh"

#include <iostream>

class Enum:public __StrongTypeVariable<Enum, Variable::Types::ENUM_VALUE>{
	public:
	Enum(): id(0), offset(0){}
	Enum(id_type i, offset_t o): id(i), offset(o){}
	Enum(const Enum & e): id(e.get_id()), offset(e.get_offset()){}

	inline id_type get_id() const {return id;}
	inline offset_t get_offset() const {return offset;}

	bool operator==(const Enum & e) const{return id == e.get_id() && offset == e.get_offset();}
	bool operator!=(const Enum & e) const{return offset != e.get_offset() || id != e.get_id();}
	bool operator>(const Enum & e) const{return id == e.get_id() && offset > e.get_offset();}
	bool operator>=(const Enum & e) const{return id == e.get_id() && offset >= e.get_offset();}
	bool operator<(const Enum & e) const{return id== e.get_id() && offset < e.get_offset();}
	bool operator<=(const Enum & e) const{return id == e.get_id() && offset <= e.get_offset();}

	bool isTrue() const {return false;}

	const Enum & operator=(const Enum & other){id = other.get_id();offset = other.get_offset(); return *this;}
	private:
		id_type id;
		offset_t offset;
};
