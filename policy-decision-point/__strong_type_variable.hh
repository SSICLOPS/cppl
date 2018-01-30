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

		bool _sameAs(const Variable & v) const{
			__DO_EVAL__(==);
		}

	private:	
		bool operator==(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator!=(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator<(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator<=(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator>(const __StrongTypeVariable<T, U> &) const{return false;}
		bool operator>=(const __StrongTypeVariable<T, U> &) const{return false;}
};
