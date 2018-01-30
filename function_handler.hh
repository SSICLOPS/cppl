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

#include <string>
#include <vector>

#include "policy_definition.hh"
#include "variable_set.hh"

class PolicyDefinition;
struct Variable;

class FunctionHandler  {
    public:
        FunctionHandler(PolicyDefinition &policyDefinition) : policyDefinition(policyDefinition) {}
        bool processFunction(const string &funcName, const vector<Variable> &funcParams);

    private:
        PolicyDefinition &policyDefinition;
};
