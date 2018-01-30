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

#include <stack>

#include "__policy_stack.hh"
#include "relation_set.hh"
#include "binary.hh"
#include "policy_definition.hh"

using namespace std;

class Ast2Ccppl{
	public:
		Ast2Ccppl(stack_type & policyStack, RelationSet & relationSet, void * no_use = nullptr)
			:_policyStack(policyStack), _relationSet(relationSet)
		{
		}

		Binary * compress(string uncompressedCppl,
				string policyDefinitionFile,
				bool traceParsingEnabled = false,
				bool taceScanningEnabled = false);

		Binary * compress(string uncompressedCppl,
				const PolicyDefinition & policyDefinition,
				bool traceParsingEnabled = false,
				bool traceScanningEnabled = false);

		bool compressToFile(string uncompressedCppl,
				string policyDefinitionFile,
				string outputFile,
				bool traceParsingEnabled = false,
				bool taceScanningEnabled = false);

		bool compressToFile(string uncompressedCppl,
				const PolicyDefinition & policyDefinition,
				string outputFile,
				bool traceParsingEnabled = false,
				bool traceScanningEnabled = false);
	private:
		string getFile(string filename);
		Binary * _Compress(string uncompressedCppl,
				const PolicyDefinition & policyDefinitionFile,
				bool traceParsingEnabled,
				bool taceScanningEnabled);
		stack_type & _policyStack;
		RelationSet & _relationSet;
};
