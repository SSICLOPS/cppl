#pragma once

#include "ast.hh"
#include "policy_stack.hh"
#include "policy_definition.hh"
#include "relation_set.hh"

#include "debug.hh"

//Visitor that prints the AST in a nice way
class AstPolicyCompressorVisitor : public AstVisitor  {
    public:
        //the policy definition is used to get the types of IDs, since they are initially unknown after the parsing
        AstPolicyCompressorVisitor(PolicyDefinition &policyDefinition, PolicyStack &policyStack, RelationSet &relationSet) 
            : policyDefinition(policyDefinition), policyStack(policyStack), relationSet(relationSet)
        {}

        void visit(AstOperation &);
        void visit(AstConstant &);
        void visit(AstId &);
        void visit(AstFunction &);
        void visit(Ast &);

    private:
        //stack that contains the policy in RPN
        PolicyStack &policyStack;
        //bit set for the relations
        RelationSet &relationSet;
        //policy definition with variables
        PolicyDefinition &policyDefinition;

        //return values for the relation children
        //can only be one of the following types
        AstId *resultId;
        AstValue *resultValue;
};
