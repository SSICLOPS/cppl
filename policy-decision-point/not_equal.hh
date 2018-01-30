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

#include "variable.hh"
#include "relation.hh"

class NotEqual:public Relation{
	public:
		NotEqual(id_type lid, id_type rid):Relation(Relation::Types::NEQ, lid, rid){}
	private:
		bool _doEval(const Variable * lv, const Variable * rv){return *lv != *rv;}
};
