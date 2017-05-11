#include "simple_reason_printer.hh"

#include<queue>

#include<iostream>

#include "ID.hh"
#include "function.hh"

void SimpleReasonPrinter::print(bool result, vector<PolicyStackEvaluatorNode> reason){
	if (result){
		queue<PolicyStackEvaluatorNode> workQueue;
		workQueue.push(reason[1]);

		while(!workQueue.empty()){
			PolicyStackEvaluatorNode pspn = workQueue.front();
			switch(pspn.type){
				case PolicyStackEvaluatorNodeType::AND:
					workQueue.push(reason[pspn.reason[0]]);
					workQueue.push(reason[pspn.reason[1]]);
					break;
				case PolicyStackEvaluatorNodeType::OR:
					workQueue.push(choose(reason, pspn));
					break;
				case PolicyStackEvaluatorNodeType::RELATION:
					reasonList.push_back(pspn.reason[0]);
					break;
			}
			workQueue.pop();
		}
	}

}


string SimpleReasonPrinter::printReasonToString(){
	string str = "";
	for (id_type reasonId = 0; reasonId < reasonList.size(); ++reasonId){
		//vector<int64_t> variableIds = mRelationSet->getRelationVariableIds(reasonList[reasonId]);
		const Relation * relation = mRelationSet->getRelation(reasonList[reasonId]);
		const Variable * vLHS, * vRHS;
		//vLHS = mVariableSet->getVariableById(variableIds[0]);
		vLHS = mRelationSet->getVariable(relation->getLeftID());
		str += _variableToString(vLHS);

		str += _relationTypeToString(relation->getType());

		if (relation->getRightID() != std::numeric_limits<id_type>::max()){
			//vRHS = mVariableSet->getVariableById(variableIds[1]);
			vRHS = mRelationSet->getVariable(relation->getRightID());
			str += _variableToString(vRHS);
		}

		str += '\n';
	}

	return str;
}

string SimpleReasonPrinter::_relationToString(id_type relationId, string str_surrounding){
	string str = "";
	//vector<int64_t> variableIds = mRelationSet->getRelationVariableIds(relationId);
	//Variable vLHS = mVariableSet->getVariableById(variableIds[0]);
	//RelationSetType type = mRelationSet->getRelationType(relationId);
	const Relation * relation = mRelationSet->getRelation(relationId);
	const Variable * vLHS = mRelationSet->getVariable(relation->getLeftID());
	Relation::Types type = relation->getType();
	if (vLHS->get_type() == Variable::FUNCTION){
		if (type == Relation::Types::IS_TRUE){
			str += _variableToString(vLHS, str_surrounding);
		}
		else if (type == Relation::Types::IS_FALSE){
			str += "!" + _variableToString(vLHS, str_surrounding);
		}
	}
	else{
		str += _variableToString(vLHS, str_surrounding) + _relationTypeToString(type);
		if (relation->getRightID() != numeric_limits<id_type>::max()){
			const Variable * vRHS = mRelationSet->getVariable(relation->getRightID());
			str +=  _variableToString(vRHS, str_surrounding);
		}
	}
	return str;
}

string SimpleReasonPrinter::_printReasonToJSON_AUX(id_type reasonId){
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
		for (id_type reasonId = 1; reasonId < reasonList.size(); ++reasonId){
			out << ",\n" << _printReasonToJSON_AUX(reasonId);
		}
	}
	out << "\n] }";

	return out.str();
}
#endif//PRINT_REASON_TO_CONSOLE

string SimpleReasonPrinter::_relationTypeToString(Relation::Types t){
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

string SimpleReasonPrinter::_variableToString(const Variable * v, string str_surrounding){
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
