#pragma once

#include "ast.hh"
#include "policy_definition.hh"
#include <algorithm>
#include <boost/lexical_cast.hpp>

//Visitor that preprocesses the AST
class AstPreprocessorVisitor : public AstVisitor  {
    public:
        //the policy definition is used to get the types of IDs, since they are initially unknown after the parsing
        AstPreprocessorVisitor(const PolicyDefinition &policyDefinition) : policyDefinition(policyDefinition) {}

        void visit(AstOperation &);
        void visit(AstConstant &);
        void visit(AstId &);
        void visit(AstFunction &);
        void visit(Ast &);

    private:
        const PolicyDefinition &policyDefinition;

        const static uint64_t maxNumberOfChildren = 2;

        //fields used to save results of a visit
        AstValueType resultType; //used for type checking, IDs are replaced with their real value
        AstOperationType parentType; //used to flip booleans with NOT parent
        uint64_t nodeCounter;
        bool flipMeaning; //used to eliminate NOTs by moving them into the relations
};
