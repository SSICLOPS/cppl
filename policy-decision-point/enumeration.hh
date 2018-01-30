// Copyright 2015-2018 RWTH Aachen University
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
