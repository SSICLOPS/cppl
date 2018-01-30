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
