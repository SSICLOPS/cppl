#include <limits>
#include <cstring>
#include <boost/utility/binary.hpp>
#include <vector>

#include "ccppl_decompressor.hh"
#include "policy_definition_manager.hh"
#include "policy_definition.hh"
#include "equal.hh"
#include "not_equal.hh"
#include "greater.hh"
#include "less.hh"
#include "greater_equal.hh"
#include "less_equal.hh"
#include "is_true.hh"
#include "is_false.hh"

#include "basic_type.hh"
#include "function.hh"
#include "enumeration.hh"
#include "ID.hh"

#define __PARSE_RELATION_WITH_TWO_VAR__(REL) leftID = getVariableID(binary, numberofVariables);\
											 rightID = getVariableID(binary, numberofVariables);\
											 relation = new REL(leftID, rightID)

void CcpplDecompressor::decompress(const void * data, uint16_t len, std::stack<StackOperation> & policyStack, RelationSet & relationSet){
	Binary binary(NULL);
	binary.read_from_mem(data,len);

	//read header
	uint16_t version = binary.next(16);
	_version = version;
	const PolicyDefinition * policyDefinition = PolicyDefinitionManager::getInstance()->getPolicyDefinition(version);
	if (policyDefinition == NULL)
		throw "can not find policy definition with version number: " + std::to_string(version);

	doDecompress(binary, policyDefinition, policyStack, relationSet);
}

void CcpplDecompressor::decompress(Binary & binary,
		const PolicyDefinition * policyDefinition,
		std::stack<StackOperation> & policyStack,
		RelationSet & relationSet)
{
	//read header
	uint16_t version = binary.next(16);
	_version = version;

	doDecompress(binary, policyDefinition, policyStack, relationSet);
}

void CcpplDecompressor::doDecompress(Binary & binary,
		const PolicyDefinition * policyDefinition,
		std::stack<StackOperation> & policyStack,
		RelationSet & relationSet)
{
	//parse policy stack
	StackOperation stackOp;
	std::vector<StackOperation> workOpVector;
	std::stack<id_type> workOpStack;

	while(true){
		stackOp.type = getStackOperationType(binary);
		stackOp.relationId = std::numeric_limits<id_type>::max();

		if (stackOp.type == PolicyStackOperationType::NEXT_RELATION){
			workOpStack.push(workOpVector.size());
			//stackOp.relationId = numberOfRelations++;
		}
		else if (stackOp.type == PolicyStackOperationType::SPECIFIC_RELATION){
			uint8_t firstBit = binary.next(1);
			if (firstBit == 0)
				break;
			else
				stackOp.relationId = binary.next(BitsFor::SPECIFIC_RELATION_ID);
		}

		//policyStack.push(stackOp);
		workOpVector.push_back(std::move(stackOp));
	}

	uint64_t numberOfRelations = 0;
	while (!workOpStack.empty()){
		workOpVector[workOpStack.top()].relationId = numberOfRelations++;
		workOpStack.pop();
	}

	for (auto it = workOpVector.begin(); it != workOpVector.end();++it)
		policyStack.push(std::move(*it));

	//parse relation set
	uint64_t numberofVariables = 0;
	id_type leftID = 0;
	id_type rightID = 0;
	for (;numberOfRelations != 0; --numberOfRelations){
#if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
		uint64_t compressedBit = binary.next(1);
		if (compressedBit == 0){
			leftID = numberofVariables++;
			rightID = numberofVariables++;
			Equal * equ = new Equal(leftID,rightID);
			relationSet.addRelation(equ);
		}
		else{
#endif
			Relation * relation;
			switch(getRelationType(binary)){
				case Relation::Types::EQUAL:
					__PARSE_RELATION_WITH_TWO_VAR__(Equal);
					break;
				case Relation::Types::NEQ:
					__PARSE_RELATION_WITH_TWO_VAR__(NotEqual);
					break;
				case Relation::Types::LESS:
					__PARSE_RELATION_WITH_TWO_VAR__(Less);
					break;
				case Relation::Types::LEQ:
					__PARSE_RELATION_WITH_TWO_VAR__(LessEqual);
					break;
				case Relation::Types::GREATER:
					__PARSE_RELATION_WITH_TWO_VAR__(Greater);
					break;
				case Relation::Types::GEQ:
					__PARSE_RELATION_WITH_TWO_VAR__(GreaterEqual);
					break;
				case Relation::Types::IS_TRUE:
					leftID = getVariableID(binary, numberofVariables);
					relation = new IsTrue(leftID);
					break;
				case Relation::Types::IS_FALSE:
					leftID = getVariableID(binary, numberofVariables);
					relation = new IsFalse(leftID);
					break;
			};
			relationSet.addRelation(relation);
#if OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES
		}
#endif
	}

	//parse variable set
	
	uint8_t bitsForVariablePosition = ceil(log2(policyDefinition->getVarNum()));
	id_type lastUsedVariableId = 0;
	for (;numberofVariables != 0; --numberofVariables){
		Variable * var;

		Variable::Types type = getVariableType(binary);
		if (type == Variable::Types::ID){
			id_type id;
			id = binary.next(bitsForVariablePosition);
			var = new ID(nodeParameters, id);
			lastUsedVariableId = id;
		}
		else if (type == Variable::Types::ENUM_VALUE){
			uint8_t bitsForEnumPosition;
			uint8_t offset;
			bitsForEnumPosition = ceil(log2(policyDefinition->getEnumElemNum(lastUsedVariableId)));
			if (bitsForEnumPosition == 0)
				throw "parse enum error";
			offset = binary.next(bitsForEnumPosition);
			var = new Enum(lastUsedVariableId, offset);
		}
		else if (type == Variable::Types::FUNCTION){
			id_type enum_pos = 0;
			id_type id;
			id = binary.next(bitsForVariablePosition);
			const FuncEntry * funcEntry = static_cast<const FuncEntry *>(policyDefinition->queryByID(id));
			//-----------------------------------------------------------------------------------------------------
			//Function * func = new Function((func_handler_type)funcEntry->handler, nodeParameters);
			Function * func = new Function(id, funcEntry->handler, nodeParameters);
			//-----------------------------------------------------------------------------------------------------
			for (auto it = funcEntry->para_list.begin(); it != funcEntry->para_list.end(); ++it){
				Variable::Types type = PolDefEntryType2VarType(*it);
				var = createBasicVariableFromBin(binary, type);
				if (var == NULL){
					if (type == Variable::Types::ENUM_VALUE){
						uint8_t bitsForEnumPosition = ceil(log2(policyDefinition->getEnumElemNum(funcEntry->enum_para_list->at(enum_pos))));
						uint8_t offset = binary.next(bitsForEnumPosition);
						var = new Enum(funcEntry->enum_para_list->at(enum_pos++), offset);
					}
					else{
						throw "unkown function parameter type";
					}
				}
				func->addParam(var);
			}
			var = func;
		}
		else{
			var = createBasicVariableFromBin(binary, type);
			if (var == NULL)
				throw "unkown variable type";
		}

		relationSet.addVariable(var);
	}
}

Variable * CcpplDecompressor::createBasicVariableFromBin(Binary & binary, Variable::Types type){
	Variable * var = NULL;

	if (type ==  Variable::Types::BOOLEAN){
		var = new Boolean(binary.next(1));
	}
	else if (type ==  Variable::Types::STRING){
		std::string str;
		char nextChar;
		while ((nextChar = binary.next(8)) != '\0')
			str += nextChar;
		var = new String(str);
	}
	else if (type ==  Variable::Types::UINT64){
		var = new Uint64(binary.next(64));
	}
	else if (type ==  Variable::Types::UINT32){
		var = new Uint32(binary.next(32));
	}
	else if (type ==  Variable::Types::UINT16){
		var = new Uint16(binary.next(16));
	}
	else if (type ==  Variable::Types::UINT8){
		var = new Uint8(binary.next(8));
	}
	else if (type ==  Variable::Types::INT64){
		var = new Int64(binary.next(64));
	}
	else if (type ==  Variable::Types::INT32){
		var = new Int32(binary.next(32));
	}
	else if (type ==  Variable::Types::INT16){
		var = new Int16(binary.next(16));
	}
	else if (type ==  Variable::Types::INT8){
		var = new Int8(binary.next(8));
	}
	else if (type ==  Variable::Types::DOUBLE){
		uint64_t doubleIntValue;
		doubleIntValue = binary.next(64);
		double d;
		std::memcpy(&d, &doubleIntValue, sizeof(double));
		var = new Double(d);
	}

	return var;
}

Variable::Types CcpplDecompressor::PolDefEntryType2VarType(PDEntryTypes type){
	switch(type){
		case PDEntryTypes::UINT64:
			return Variable::Types::UINT64;
		case PDEntryTypes::UINT32:
			return Variable::Types::UINT32;
		case PDEntryTypes::UINT16:
			return Variable::Types::UINT16;
		case PDEntryTypes::UINT8:
			return Variable::Types::UINT8;
		case PDEntryTypes::INT64:
			return Variable::Types::INT64;
		case PDEntryTypes::INT32:
			return Variable::Types::INT32;
		case PDEntryTypes::INT16:
			return Variable::Types::INT16;
		case PDEntryTypes::INT8:
			return Variable::Types::UINT8;
		case PDEntryTypes::BOOLEAN:
			return Variable::Types::BOOLEAN;
		case PDEntryTypes::STRING:
			return Variable::Types::STRING;
		case PDEntryTypes::DOUBLE:
			return Variable::Types::DOUBLE;
		case PDEntryTypes::FUNCTION:
			return Variable::Types::FUNCTION;
		case PDEntryTypes::ENUM_VALUE:
			return Variable::Types::ENUM_VALUE;
	};
}

Variable::Types CcpplDecompressor::getVariableType(Binary & binary){
	uint8_t type = binary.next(BitsFor::VARIABLE_TYPE);

	if (type == BOOST_BINARY(0000))
		return Variable::Types::BOOLEAN;
	else if (type == BOOST_BINARY(0001))
		return Variable::Types::ID;
	else if (type == BOOST_BINARY(0010))
		return Variable::Types::STRING;
	else if (type == BOOST_BINARY(0011))
		return Variable::Types::ENUM_VALUE;
	else if (type == BOOST_BINARY(0100))
		return Variable::Types::FUNCTION;
	else if (type == BOOST_BINARY(0101))
		return Variable::Types::INT64;
	else if (type == BOOST_BINARY(0110))
		return Variable::Types::INT32;
	else if (type == BOOST_BINARY(0111))
		return Variable::Types::INT16;
	else if (type == BOOST_BINARY(1000))
		return Variable::Types::INT8;
	else if (type == BOOST_BINARY(1001))
		return Variable::Types::UINT32;
	else if (type == BOOST_BINARY(1010))
		return Variable::Types::UINT16;
	else if (type == BOOST_BINARY(1011))
		return Variable::Types::UINT8;
	else if (type == BOOST_BINARY(1100))
		return Variable::Types::DOUBLE;
}	

Relation::Types CcpplDecompressor::getRelationType(Binary & binary){
	uint8_t type = binary.next(BitsFor::RELATION_TYPE);

	if (type == BOOST_BINARY(000))
		return Relation::Types::EQUAL;
	else if (type == BOOST_BINARY(001))
		return Relation::Types::NEQ;
	else if (type == BOOST_BINARY(010))
		return Relation::Types::LESS;
	else if (type == BOOST_BINARY(011))
		return Relation::Types::LEQ;
	else if (type == BOOST_BINARY(100))
		return Relation::Types::GREATER;
	else if (type == BOOST_BINARY(101))
		return Relation::Types::GEQ;
	else if (type == BOOST_BINARY(110))
		return Relation::Types::IS_TRUE;
	else if (type == BOOST_BINARY(111))
		return Relation::Types::IS_FALSE;
}

PolicyStackOperationType CcpplDecompressor::getStackOperationType(Binary & binary){
	uint8_t type = binary.next(BitsFor::STACK_OPERATION_TYPE);

	if (type == BOOST_BINARY(00))
		return PolicyStackOperationType::NEXT_RELATION;
	else if (type == BOOST_BINARY(01))
		return PolicyStackOperationType::SPECIFIC_RELATION;
	else if (type == BOOST_BINARY(10))
		return PolicyStackOperationType::OR;
	else if (type == BOOST_BINARY(11))
		return PolicyStackOperationType::AND;
}

id_type CcpplDecompressor::getVariableID(Binary & binary, uint64_t & var_num){
	uint8_t firstBit = binary.next(1);
	if (firstBit == 0){//NEXT_VARIABLE
		return var_num++;
	}
	else{//SPECIFIC_VARIABLE
		if (var_num == 0)
			throw "Invalid policy binary: SPECIFIC_VARIABLE (allows only to step back) is used before any vaiable is defined!";

		return binary.next(BitsFor::SPECIFIC_VARIABLE_ID);
	}
}
