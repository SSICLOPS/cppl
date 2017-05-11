#include "simple_reason_printer.hh"

#include<queue>

#include<iostream>

//#ifdef CONVERT_TO_DNF
//void SimpleReasonPrinter::print(bool result, vector<PolicyStackProcessorNode> reason, DNFConverterNode & dnf){
//#else
void SimpleReasonPrinter::print(bool result, vector<PolicyStackProcessorNode> reason){
//#endif
	if (result){
		queue<PolicyStackProcessorNode> workQueue;
		workQueue.push(reason[1]);

		while(!workQueue.empty()){
			PolicyStackProcessorNode pspn = workQueue.front();
			switch(pspn.type){
				case PolicyStackProcessorNodeType::AND:
					workQueue.push(reason[pspn.reason[0]]);
					workQueue.push(reason[pspn.reason[1]]);
					break;
				case PolicyStackProcessorNodeType::OR:
					workQueue.push(choose(reason, pspn));
					break;
				case PolicyStackProcessorNodeType::RELATION:
					reasonList.push_back(pspn.reason[0]);
					break;
			}
			workQueue.pop();
		}
	}

//#ifdef CONVERT_TO_DNF
	//mDNF = dnf;
//#endif
}

//#ifdef CONVERT_TO_DNF
//void SimpleReasonPrinter::printDNF(){
	//unsigned long i = 0;
	//if (mDNF.size() == 0)
		//return;

	//while(true){
		//std::cout<<"( ";
		//set<uint64_t>::iterator it = mDNF[i].begin();
		//while(true){
			//std::cout<< _relationToString(*it);
			//++it;
			//if (it != mDNF[i].end())
				//std::cout<<" && ";
			//else
				//break;
		//}
		//std::cout<<" )";
		//++i;
		//if (i != mDNF.size())
			//std::cout<<" || ";
		//else
			//break;
	//}
	//std::cout<<std::endl;
//}
//#endif

string SimpleReasonPrinter::printReasonToString(){
	string str = "";
	for (uint64_t reasonId = 0; reasonId < reasonList.size(); ++reasonId){
		vector<int64_t> variableIds = mRelationSet->getRelationVariableIds(reasonList[reasonId]);
		Variable vLHS, vRHS;
		vLHS = mVariableSet->getVariableById(variableIds[0]);
		str += _variableToString(vLHS);

		str += _relationTypeToString(mRelationSet->getRelationType(reasonList[reasonId]));

		if (variableIds[1] >= 0){
			vRHS = mVariableSet->getVariableById(variableIds[1]);
			str += _variableToString(vRHS);
		}

		str += '\n';
	}

	return str;
}

string SimpleReasonPrinter::_relationToString(uint64_t relationId, string str_surrounding){
	string str = "";
	vector<int64_t> variableIds = mRelationSet->getRelationVariableIds(relationId);
	Variable vLHS = mVariableSet->getVariableById(variableIds[0]);
	RelationSetType type = mRelationSet->getRelationType(relationId);
	if (vLHS.isFunction()){
		if (type == RelationSetType::IS_TRUE){
			str += _variableToString(vLHS, str_surrounding);
		}
		else if (type == RelationSetType::IS_FALSE){
			str += "!" + _variableToString(vLHS, str_surrounding);
		}
	}
	else{
		str += _variableToString(vLHS, str_surrounding) + _relationTypeToString(type);
		if (variableIds[1] >= 0){
			Variable vRHS = mVariableSet->getVariableById(variableIds[1]);
			str +=  _variableToString(vRHS, str_surrounding);
		}
	}
	return str;
}

string SimpleReasonPrinter::_printReasonToJSON_AUX(uint64_t reasonId){
	string str = "";
	str +=  "\"";
	str += _relationToString(reasonList[reasonId], "\\\"");
	str += "\"";

	return str;
}

void SimpleReasonPrinter::printReasonToJSON(string filename){
	ofstream of(filename);
	if (!of.is_open())
		throw runtime_error("cannot open file" + filename);

	of << "{\n" <<"\"reasons\":[\n";
	if (reasonList.size() > 0){
		of << _printReasonToJSON_AUX(0);
		for (uint64_t reasonId = 1; reasonId < reasonList.size(); ++reasonId){
			of << ",\n" << _printReasonToJSON_AUX(reasonId);
		}
	}
	of << "\n] }";

	of.close();
}

#ifdef PRINT_REASON_TO_CONSOLE
string SimpleReasonPrinter::printReasonToJSON(){
	ostringstream out;
	out << "{\n" <<"\"reasons\":[\n";
	if (reasonList.size() > 0){
		out << _printReasonToJSON_AUX(0);
		for (uint64_t reasonId = 1; reasonId < reasonList.size(); ++reasonId){
			out << ",\n" << _printReasonToJSON_AUX(reasonId);
		}
	}
	out << "\n] }";

	return out.str();
}
#endif//PRINT_REASON_TO_CONSOLE

string SimpleReasonPrinter::_relationTypeToString(RelationSetType t){
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

string SimpleReasonPrinter::_variableToString(Variable v, string str_surrounding){
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
