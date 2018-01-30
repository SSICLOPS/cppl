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
