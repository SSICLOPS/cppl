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
