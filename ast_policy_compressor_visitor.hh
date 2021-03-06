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
