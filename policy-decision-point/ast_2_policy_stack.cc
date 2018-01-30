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

#include "ast_2_policy_stack.hh"
#include "equal.hh"
#include "not_equal.hh"
#include "less.hh"
#include "less_equal.hh"
#include "greater.hh"
#include "greater_equal.hh"
#include "is_true.hh"
#include "is_false.hh"
#include "basic_type.hh"
#include "function.hh"
#include "ID.hh"

void Ast2PolicyStack::visit(AstOperation &op){
	if (op.type == AstOperationType::NOT)
		throw "The not operation should have been eliminated, since we here only support & and |";

	if (op.type == AstOperationType::OR || op.type == AstOperationType::AND){
		for (uint8_t i = 0; i < op.getNumberOfChildren(); ++i)
			op.getChild(i)->accept(*this);

		StackOperation stackOp;
		if (op.type == AstOperationType::OR){
			stackOp.type = PolicyStackOperationType::OR;
		}
		else{
			stackOp.type = PolicyStackOperationType::AND;
		}

		policyStack.push(std::move(stackOp));
	}
	else if (op.type == AstOperationType::ELIMINATED_NOT){
		assert(op.getNumberOfChildren() == 1);
		op.getChild(0)->accept(*this);
	}
	else{
		op.getChild(0)->accept(*this);
		if (op.type != AstOperationType::IS_TRUE && op.type != AstOperationType::IS_FALSE){
			assert(op.getNumberOfChildren() == 2);
			op.getChild(1)->accept(*this);
		}

		Relation * relation = NULL;
		if (op.type == AstOperationType::EQUAL){
#ifdef __DEBUG__
			std::cout<<_left<<" = "<<_right<<std::endl;
#endif
			relation = new Equal(_left, _right);
		}
		else if (op.type == AstOperationType::NEQ){
#ifdef __DEBUG__
			std::cout<<_left<<" != "<<_right<<std::endl;
#endif
			relation = new NotEqual(_left, _right);
		}
		else if (op.type == AstOperationType::LESS){
#ifdef __DEBUG__
			std::cout<<_left<<" < "<<_right<<std::endl;
#endif
			relation = new Less(_left, _right);
		}
		else if (op.type == AstOperationType::LEQ){
#ifdef __DEBUG__
			std::cout<<_left<<" <= "<<_right<<std::endl;
#endif
			relation = new LessEqual(_left, _right);
		}
		else if (op.type == AstOperationType::GREATER){
#ifdef __DEBUG__
			std::cout<<_left<<" > "<<_right<<std::endl;
#endif
			relation = new Greater(_left, _right);
		}
		else if (op.type == AstOperationType::GEQ){
#ifdef __DEBUG__
			std::cout<<_left<<" >= "<<_right<<std::endl;
#endif
			relation = new GreaterEqual(_left, _right);
		}
		else if (op.type == AstOperationType::IS_TRUE){
#ifdef __DEBUG__
			std::cout<<_left<<" = true"<<std::endl;
#endif
			relation = new IsTrue(_left);
		}
		else if (op.type == AstOperationType::IS_FALSE){
#ifdef __DEBUG__
			std::cout<<_left<<" = false"<<std::endl;
#endif
			relation = new IsFalse(_left);
		}
		else
			throw "Convert for this RelationSetType is not implemented";

		StackOperation stackOp;
		id_type relationId = relationSet.getRelationID(*relation);
		if (relationId == std::numeric_limits<id_type>::max()){//a normal, new relation
			relationSet.addRelation(relation);
			stackOp.type = PolicyStackOperationType::NEXT_RELATION;
		}
		else{
			delete relation;
			stackOp.type = PolicyStackOperationType::SPECIFIC_RELATION;
			stackOp.relationId = relationId;
		}
		policyStack.push(std::move(stackOp));

		_left = _right = std::numeric_limits<id_type>::max();
	}
}

Variable * AstConstant2Variable(const AstConstant & astConst){
	Variable * var = NULL;

	if (astConst.type == AstValueType::UINT8){
#ifdef __DEBUG__
		std::cout<<"AstConstant: UINT8"<<std::endl;
#endif
		var = new Uint8(boost::get<int64_t>(astConst.value));
	}
	else if (astConst.type == AstValueType::UINT16){
#ifdef __DEBUG__
		std::cout<<"AstConstant: UINT16"<<std::endl;
#endif
		var = new Uint16(boost::get<int64_t>(astConst.value));
	}
	else if (astConst.type == AstValueType::UINT32){
#ifdef __DEBUG__
		std::cout<<"AstConstant: UINT32"<<std::endl;
#endif
		var = new Uint32(boost::get<int64_t>(astConst.value));
	}
	else if (astConst.type == AstValueType::INT8){
#ifdef __DEBUG__
		std::cout<<"AstConstant: INT8"<<std::endl;
#endif
		var = new Int8(boost::get<int64_t>(astConst.value));
	}
	else if (astConst.type == AstValueType::INT16){
#ifdef __DEBUG__
		std::cout<<"AstConstant: INT16"<<std::endl;
#endif
		var = new Int16(boost::get<int64_t>(astConst.value));
	}
	else if (astConst.type == AstValueType::INT32){
#ifdef __DEBUG__
		std::cout<<"AstConstant: INT32"<<std::endl;
#endif
		var = new Int32(boost::get<int64_t>(astConst.value));
	}
	else if (astConst.type == AstValueType::INT64){
#ifdef __DEBUG__
		std::cout<<"AstConstant: INT64"<<std::endl;
#endif
		var = new Int64(boost::get<int64_t>(astConst.value));
	}
	else if (astConst.type == AstValueType::Float){
#ifdef __DEBUG__
		std::cout<<"AstConstant: Float"<<std::endl;
#endif
		var = new Double(boost::get<double>(astConst.value));
	}
	else if (astConst.type == AstValueType::Boolean){
#ifdef __DEBUG__
		std::cout<<"AstConstant: Boolean"<<std::endl;
#endif
		var = new Boolean(boost::get<bool>(astConst.value));
	}
	else if (astConst.type == AstValueType::String){
#ifdef __DEBUG__
		std::cout<<"AstConstant: String"<<std::endl;
#endif
		var = new String(boost::get<string>(astConst.value));
	}
	else if (astConst.type == AstValueType::EnumValue){
#ifdef __DEBUG__
		std::cout<<"AstConstant: EnumValue"<<std::endl;
#endif
		var = new Enum(boost::get<int64_t>(astConst.value), astConst.enumValuePosition);
	}

	return var;
}

void Ast2PolicyStack::visit(AstConstant & astConstant){
	id_type * pOpID = &_left;
	if (_left != std::numeric_limits<id_type>::max())
		pOpID = &_right;
	if (_right != std::numeric_limits<id_type>::max())
		throw "astConstant: only relations with less than two variables are supported";

	Variable * var = AstConstant2Variable(astConstant);
	if (var->get_type() != Variable::Types::BOOLEAN){
		*pOpID = relationSet.getVariableID(*var);
	}

	if (*pOpID == std::numeric_limits<id_type>::max()){
		*pOpID = relationSet.addVariable(var);
	}
	else
		delete var;
}

void Ast2PolicyStack::visit(AstId & astId){
#ifdef __DEBUG__
	std::cout<<astId.name<<std::endl;
#endif
	id_type * pOpID = &_left;
	if (_left != std::numeric_limits<id_type>::max())
		pOpID = &_right;
	if (_right != std::numeric_limits<id_type>::max())
		throw "AstId: only relations with less than two variables are supported";

	ID * id = new ID(NULL, astId.position);
	//*pOpID = relationSet.getVariableID(*id);

	//if (*pOpID == std::numeric_limits<id_type>::max()){
		*pOpID = relationSet.addVariable(id);
	//}
	//else
	//	delete id;
}

void Ast2PolicyStack::visit(AstFunction & astFunction){
	id_type * pOpID = &_left;
	if (_left != std::numeric_limits<id_type>::max())
		pOpID = &_right;
	if (_right != std::numeric_limits<id_type>::max())
		throw "AstFunction: only relations with less than two variables are supported";

	Function * function = new Function(astFunction.position, NULL, NULL);
	for (auto it = astFunction.parameters.begin(); it != astFunction.parameters.end();++it){
		function->addParam(AstConstant2Variable(**it));
	}
	*pOpID = relationSet.getVariableID(*function);

	if (*pOpID == std::numeric_limits<id_type>::max()){
		*pOpID = relationSet.addVariable(function);
	}
	else
		delete function;
}

void Ast2PolicyStack::visit(Ast & ast){
	policyStack.push({PolicyStackOperationType::SPECIFIC_RELATION, std::numeric_limits<id_type>::max()});
	ast.root->accept(*this);
}
