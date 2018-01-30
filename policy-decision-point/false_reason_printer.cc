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

#include "false_reason_printer.hh"

#include<queue>

#include "ID.hh"
#include "function.hh"

void FalseReasonPrinter::print(bool result, vector<PolicyStackEvaluatorNode> reason){
	if (!result){
		PolicyStackEvaluatorNode pspn = reason[1];
		while (pspn.type == PolicyStackEvaluatorNodeType::AND && pspn.reason[1] == std::numeric_limits<id_type>::max()){
			pspn = reason[pspn.reason[0]];
		}

		reasonTree.push_back(pspn);

		for (uint64_t current = 0; current != reasonTree.size(); ++current){
			if (reasonTree[current].type != PolicyStackEvaluatorNodeType::RELATION){
				pspn = reason[reasonTree[current].reason[0]];
				while (pspn.type == PolicyStackEvaluatorNodeType::AND && pspn.reason[1] == std::numeric_limits<id_type>::max())
					pspn = reason[pspn.reason[0]];

				reasonTree.push_back(pspn);
				reasonTree[current].reason[0] = reasonTree.size() - 1;

				pspn = reason[reasonTree[current].reason[1]];
				while (pspn.type == PolicyStackEvaluatorNodeType::AND && pspn.reason[1] == std::numeric_limits<id_type>::max())
					pspn = reason[pspn.reason[0]];

				reasonTree.push_back(pspn);
				reasonTree[current].reason[1] = reasonTree.size() - 1;
			}
		}
	}
}

void FalseReasonPrinter::printDNF(){
	unsigned long i = 0;
	if (mDNF.size() == 0)
		return;

	while(true){
		std::cout<<"( ";
		set<uint64_t>::iterator it = mDNF[i].begin();
		while(true){
			std::cout<< _relationToString(*it);
			++it;
			if (it != mDNF[i].end())
				std::cout<<" && ";
			else
				break;
		}
		std::cout<<" )";
		++i;
		if (i != mDNF.size())
			std::cout<<" || ";
		else
			break;
	}
	std::cout<<std::endl;
}

string FalseReasonPrinter::printReasonToString(){
	stack<PolicyStackEvaluatorNode> workStack;
	ostringstream oss;
	PolicyStackEvaluatorNode closeParenthesis;
	closeParenthesis.type = PolicyStackEvaluatorNodeType::OR;
	closeParenthesis.reason[0] = std::numeric_limits<id_type>::max() - 2; //means a ")" should be outputed
	
	PolicyStackEvaluatorNode pspn = reasonTree[0];
	while (pspn.type != PolicyStackEvaluatorNodeType::RELATION){
		oss<<"(";
		workStack.push(closeParenthesis);
		workStack.push(pspn);
		pspn = reasonTree[pspn.reason[0]];
	}

	oss<<_relationToString(pspn.reason[0]);

	while(!workStack.empty()){
		pspn = workStack.top();
		workStack.pop();

		if (pspn.reason[0] == std::numeric_limits<id_type>::max() - 2){// when the first child id is -2, means ")" should be printed
			oss<<")";
		}
		else{
			if (pspn.type == PolicyStackEvaluatorNodeType::OR)
				oss << " OR ";
			else
				oss << " AND ";

			pspn = reasonTree[pspn.reason[1]];
			while (pspn.type != PolicyStackEvaluatorNodeType::RELATION){
				oss << "(";
				workStack.push(closeParenthesis);
				workStack.push(pspn);
				pspn = reasonTree[pspn.reason[0]];
			}

			oss<<_relationToString(pspn.reason[0]);
		}
	}

	oss<<endl;

	return oss.str();
}

string FalseReasonPrinter::_relationToString(id_type relationId){
	string str = "";
	//vector<int64_t> variableIds = mRelationSet->getRelationVariableIds(relationId);
	//Variable vLHS = mVariableSet->getVariableById(variableIds[0]);
	//RelationSetType type = mRelationSet->getRelationType(relationId);
	const Relation * relation = mRelationSet->getRelation(relationId);
	const Variable * vLHS = mRelationSet->getVariable(relation->getLeftID());
	Relation::Types type = relation->getType();
	if (vLHS->get_type() == Variable::FUNCTION){
		if (type == Relation::Types::IS_TRUE){
			str += _variableToString(vLHS);
		}
		else if (type == Relation::Types::IS_FALSE){
			str += "!" + _variableToString(vLHS);
		}
	}
	else{
		str += _variableToString(vLHS) + _relationTypeToString(type);
		if (relation->getRightID() != numeric_limits<id_type>::max()){
			const Variable * vRHS = mRelationSet->getVariable(relation->getRightID());
			str +=  _variableToString(vRHS);
		}
	}
	return str;
}

string FalseReasonPrinter::_relationTypeToString(Relation::Types t){
	string str = "";
	switch(t){
		case Relation::Types::EQUAL:
			str = "=";
			break;
		case Relation::Types::NEQ:
			str = "!=";
			break;
		case Relation::Types::GEQ:
			str = ">=";
			break;
		case Relation::Types::LEQ:
			str = "<=";
			break;
		case Relation::Types::GREATER:
			str = ">";
			break;
		case Relation::Types::LESS:
			str = "<";
			break;
		case Relation::Types::IS_TRUE:
			str = "=true";
			break;
		case Relation::Types::IS_FALSE:
			str = "=false";
			break;
	};

	return str;
}

string FalseReasonPrinter::_variableToString(const Variable * v, string str_surrounding){
	//string str;
	ostringstream oss;
	if (v->get_type() == Variable::Types::FUNCTION){
		const Function * func = (const Function *)v;
		oss<<mPolicyDefinition->getNameByID(func->get_id())<<"( ";
		if (func->getParameterNum() > 0){
			oss<< _variableToString(func->getParameter(0), str_surrounding);
			for (id_type i = 1; i < func->getParameterNum(); ++i){
				oss<<", "<<_variableToString(func->getParameter(i), str_surrounding);
			}
		}
		oss<< " )";
	}
	else if (v->get_type() == Variable::Types::ENUM_VALUE){
		const Enum * e = (const Enum *)v;
		const CommonEnumEntry * cee = (const CommonEnumEntry *)(mPolicyDefinition->queryByID(e->get_id()));
		cee->print(oss, e->get_offset());
	}
	else {
		switch (v->get_type()){
			case Variable::Types::STRING:
				oss<< str_surrounding <<((const String *)v)->get_value()<< str_surrounding;
				break;
			case Variable::Types::BOOLEAN:
				if (((const Boolean *)v)->get_value())
					oss<< "true";
				else
					oss<< "false";
				break;
			//ENUM_VALUE not used
			case Variable::Types::INT8:
				//uint8_t is defined as char. print directly will meet problem.
				oss<< (int)(((const Int8 *)v)->get_value());
				break;
			case Variable::Types::INT16:
				oss<< ((const Int16 *)v)->get_value();
				break;
			case Variable::Types::INT32:
				oss<< ((const Int32 *)v)->get_value();
				break;
			case Variable::Types::INT64:
				oss<< ((const Int64 *)v)->get_value();
				break;
			case Variable::Types::UINT8:
				//uint8_t is defined as char. print directly will meet problem.
				oss<< (unsigned int)(((const Uint8 *)v)->get_value());
				break;
			case Variable::Types::UINT16:
				oss<< ((const Uint16 *)v)->get_value();
				break;
			case Variable::Types::UINT32:
				oss<< ((const Uint32 *)v)->get_value();;
				break;
			case Variable::Types::ID:
				oss<<mPolicyDefinition->getNameByID(((const ID *)v)->getID());
				break;
			case Variable::Types::DOUBLE:
				oss<< ((const Double *)v)->get_value();
			default:
				break;
		}
	}
	return oss.str();
}
