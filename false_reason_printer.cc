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

void FalseReasonPrinter::print(bool result, vector<PolicyStackProcessorNode> reason){
	if (!result){
		PolicyStackProcessorNode pspn = reason[1];
		while (pspn.type == PolicyStackProcessorNodeType::AND && pspn.reason[1] == 0){
			pspn = reason[pspn.reason[0]];
		}

		reasonTree.push_back(pspn);

		for (uint64_t current = 0; current != reasonTree.size(); ++current){
			if (reasonTree[current].type != PolicyStackProcessorNodeType::RELATION){
				pspn = reason[reasonTree[current].reason[0]];
				while (pspn.type == PolicyStackProcessorNodeType::AND && pspn.reason[1] == 0)
					pspn = reason[pspn.reason[0]];

				reasonTree.push_back(pspn);
				reasonTree[current].reason[0] = reasonTree.size() - 1;

				pspn = reason[reasonTree[current].reason[1]];
				while (pspn.type == PolicyStackProcessorNodeType::AND && pspn.reason[1] == 0)
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
	stack<PolicyStackProcessorNode> workStack;
	ostringstream oss;
	PolicyStackProcessorNode closeParenthesis;
	closeParenthesis.type = PolicyStackProcessorNodeType::OR;
	closeParenthesis.reason[0] = -2; //means a ")" should be outputed
	
	PolicyStackProcessorNode pspn = reasonTree[0];
	while (pspn.type != PolicyStackProcessorNodeType::RELATION){
		oss<<"(";
		workStack.push(closeParenthesis);
		workStack.push(pspn);
		pspn = reasonTree[pspn.reason[0]];
	}

	oss<<_relationToString(pspn.reason[0]);

	while(!workStack.empty()){
		pspn = workStack.top();
		workStack.pop();

		if (pspn.reason[0] == -2){// when the first child id is -2, means ")" should be printed
			oss<<")";
		}
		else{
			if (pspn.type == PolicyStackProcessorNodeType::OR)
				oss << " OR ";
			else
				oss << " AND ";

			pspn = reasonTree[pspn.reason[1]];
			while (pspn.type != PolicyStackProcessorNodeType::RELATION){
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

string FalseReasonPrinter::_relationToString(uint64_t relationId){
	string str = "";
	vector<int64_t> variableIds = mRelationSet->getRelationVariableIds(relationId);
	Variable vLHS = mVariableSet->getVariableById(variableIds[0]);
	RelationSetType type = mRelationSet->getRelationType(relationId);
	if (vLHS.isFunction()){
		if (type == RelationSetType::IS_TRUE){
			str += _variableToString(vLHS);
		}
		else if (type == RelationSetType::IS_FALSE){
			str += "!" + _variableToString(vLHS);
		}
	}
	else{
		str += _variableToString(vLHS) + _relationTypeToString(type);
		if (variableIds[1] >= 0){
			Variable vRHS = mVariableSet->getVariableById(variableIds[1]);
			str +=  _variableToString(vRHS);
		}
	}
	return str;
}

string FalseReasonPrinter::_relationTypeToString(RelationSetType t){
	string str = "";
	switch(t){
		case RelationSetType::EQUAL:
			str = "=";
			break;
		case RelationSetType::NEQ:
			str = "!=";
			break;
		case RelationSetType::GEQ:
			str = ">=";
			break;
		case RelationSetType::LEQ:
			str = "<=";
			break;
		case RelationSetType::GREATER:
			str = ">";
			break;
		case RelationSetType::LESS:
			str = "<";
			break;
		case RelationSetType::IS_TRUE:
			str = "=true";
			break;
		case RelationSetType::IS_FALSE:
			str = "=false";
			break;
		case RelationSetType::END:
			break;
	};

	return str;
}

string FalseReasonPrinter::_variableToString(Variable v, string str_surrounding){
	//string str;
	ostringstream oss;
	if (v.isReference()){
		oss<< mPolicyDefinition->getIdName(v.idInPolicyDef);
		if (v.isFunction()){
			oss<< "( " << _variableToString(v.funcParams->at(0), str_surrounding);
			for (vector<Variable>::iterator it = v.funcParams->begin() + 1; it != v.funcParams->end(); ++it){
				oss << ", " << _variableToString(*it, str_surrounding);
			}

			oss<< " )";
		}
	}
	else {
		switch (v.type){
			case VariableSetType::STRING:
				oss<< str_surrounding <<*(v.value.asString)<< str_surrounding;
				break;
			case VariableSetType::BOOLEAN:
				if (v.value.asBoolean)
					oss<< "true";
				else
					oss<< "false";
				break;
			//ENUM_VALUE not used
			case VariableSetType::INT8:
				//uint8_t is defined as char. print directly will meet problem.
				oss<< (int)(v.value.asInt8);
				break;
			case VariableSetType::INT16:
				oss<< v.value.asInt16;
				break;
			case VariableSetType::INT32:
				oss<< v.value.asInt32;
				break;
			case VariableSetType::UINT8:
				//uint8_t is defined as char. print directly will meet problem.
				oss<< (unsigned int)(v.value.asUInt8);
				break;
			case VariableSetType::UINT16:
				oss<< v.value.asUInt16;
				break;
			case VariableSetType::UINT32:
				oss<< v.value.asUInt32;
				break;
			default:
				break;
		}
	}
	return oss.str();
}
