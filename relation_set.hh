#pragma once

#include <iostream>
#include <map>
#include <vector>

#include "ast.hh"
#include "binary.hh"
#include "policy_definition.hh"
#include "variable_set.hh"
#include "colors.hh"

#include <boost/variant.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "debug.hh"
#include "options.hh"

class VariableSet;

using namespace std;

enum class RelationSetType {
    EQUAL,
    NEQ,
    LESS,
    LEQ,
    GREATER,
    GEQ,
    IS_TRUE,
    IS_FALSE,
    END, //Just used to signal that there are no more relations
};

struct Relation  {
    RelationSetType type;
    int64_t variableIds[2]; //-1 if next variable is used; -2 if free
    bool result;
};

//used for the relation processing
typedef boost::variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t> NumberVariant;

//functions used to convert the integer variant to int64_t
struct toInt64_functor: boost::static_visitor<int64_t> {
    template <class T> int64_t operator()(const T& v) const {
        // Lexical cast has many optimizations including optimizations for situations that usually 
        // occur in generic programming, like std::string to std::string or arithmetic type to arithmetic type conversion. 
        return boost::numeric_cast<int64_t>(v);
    }
};

// Throws `boost::bad_lexical_cast` if value of the variant is not convertible to `long double`
template <class Variant> int64_t toInt64(const Variant& v) {
    return boost::apply_visitor(toInt64_functor(), v);
}

//stores a vector of all binary relations for the policy
class RelationSet {
    public:
        //inits the relation type to bit string map
        RelationSet(PolicyDefinition &policyDefinition, VariableSet &variableSet, Binary &policyBinary) 
            : policyDefinition(policyDefinition), variableSet(variableSet), policyBinary(policyBinary)
        {
            relationTypeToInt[RelationSetType::EQUAL] = 0;
            intToRelationType[0] = RelationSetType::EQUAL;

            relationTypeToInt[RelationSetType::NEQ] = 1;
            intToRelationType[1] = RelationSetType::NEQ;

            relationTypeToInt[RelationSetType::LESS] = 2;
            intToRelationType[2] = RelationSetType::LESS;

            relationTypeToInt[RelationSetType::LEQ] = 3;
            intToRelationType[3] = RelationSetType::LEQ;

            relationTypeToInt[RelationSetType::GREATER] = 4;
            intToRelationType[4] = RelationSetType::GREATER;

            relationTypeToInt[RelationSetType::GEQ] = 5;
            intToRelationType[5] = RelationSetType::GEQ;

            relationTypeToInt[RelationSetType::IS_TRUE] = 6;
            intToRelationType[6] = RelationSetType::IS_TRUE;

            relationTypeToInt[RelationSetType::IS_FALSE] = 7;
            intToRelationType[7] = RelationSetType::IS_FALSE;

            //initially clear the current relation (used for the policy generation)
            currentRelation.variableIds[0] = -2; //indicates that its free
            currentRelation.variableIds[1] = -2;

            #if DEBUG_POLICY_GENERATION
                legend = string(MAGENTA) + "RelType 000:= 001:!= 010:< 011:<= 100:> 101:>= 110:=1 111:=0 " + string(GREEN) + "0:NEXT_VAR " + string(YELLOW) + "1:SPEC_VAR ";
            #endif
        }

        //first addRelationElement is called two times for the LHS and RHS of the relation
        void addRelationElement(AstConstant &);
        void addRelationElement(AstId &);
        void addRelationElement(AstFunction &);

        //adds the variable to the bit set
        void addVariable(Variable var);

        //then add the type of the relation
        //returns the offset from the start of the relation set to this new relation
        int64_t addType(RelationSetType);

        //add the relation to relations
        void push_back(Relation relation);

        friend ostream& operator<<(ostream &out, RelationSet &relationSet);

        //get the next 3 bits from the binary policy and interrepts them as relation set type
        RelationSetType getType();

        //determines the relation set size and directly processes the relations s.t. their structure is stored in a vector 
        //returns the starting offset of the variable set
        //can't directly evaluate the relations, since we dont't now the starting offset of the variable set before we determined
        //the relations set size
        void parse(uint64_t relationSetOffset, uint64_t numberOfRelations);

        //processes the relations by evaluating the internal relation set structure
        //called after parseRelationSet(), since then the variabel set is initialized
        void process();

        //stores the relation set in the binary
        void store();

        //color coded string of the policy
        //DEBUG output
        #if DEBUG_POLICY_GENERATION
            string asString;
            string legend;
        #endif

        uint64_t getVariableSetOffset();
        uint64_t getNumberOfVariables();

        //consts for the relation set
        static const uint8_t bitsForRelationType = RELATION_TYPE_LEN_IN_BITS;
        static const uint8_t bitsForSpecificRelationId = SPECIFIC_RELATION_ID_LEN_IN_BITS; //max ID from start which can be used for the specific relations feature

        bool getRelationResult(uint64_t relationId)  {
            return relations.at(relationId).result;
        }

        //print the calculated relation set size
        void printSize();

    private:
        //used to get the signitare of functions
        PolicyDefinition &policyDefinition;

        //the relation variables are stored during the relation creation
        VariableSet &variableSet;

        Binary &policyBinary;

        //used during the policy generation to store the current relation
        Relation currentRelation;

        //return the Variable value as boost variant
        //used for the relation processing
        NumberVariant variableToNumberVariant(Variable var);

        //number of relations is used to determine the real relation set size
        uint64_t numberOfVariables; //calculated by parseRelationSet
        uint64_t variableSetOffset; //calculated by parseRelationSet

        //converts the AST constant node into a variable set element
        Variable toVariable(AstConstant &constant); //also compressed integers to the smallest necessary storage size
        Variable toVariableByType(AstConstant &constant, VariableSetType type);

        //map that stores for each operation type it bit sequence as string
        //e.g. RelationSetType::EQUAL -> "000" (MSB left)
        map<RelationSetType, int> relationTypeToInt;
        map<int, RelationSetType> intToRelationType;
        vector<Relation> relations;

        //used for the size calculation
        uint64_t relationSetSize = 0;
};
