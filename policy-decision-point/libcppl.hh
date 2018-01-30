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

#include "options.hh"
#include "binary.hh"
#include "policy_definition.hh"
#include "node_parameter.hh"
#include "policy_stack.hh"
#include "reason_printer.hh"

using namespace std;

/**
 * Set up stuff for cppl such as directory in /tmp/.
 */
int cppl_init();

/**
 * Cleanup of cppl related temporary data.
 */
int cppl_cleanup();



/* Reading and writing of cppl related data */

/**
 * Obtain an uncompressed cppl policy from filesystem.
 */
string cppl_read_policy_from_file(const string & cpplFilePath);

/**
 * Compress an uncompressed cppl policy and write the result to a file.
 */
bool cppl_compress_policy_to_file(const string & policy,
        const PolicyDefinition & policyDefinition,
        const string & outputFile,
        bool traceParsingEnabled = false,
        bool traceScanningEnabled = false);

/**
 * Compress an uncompressed cppl policy.
 */
Binary * cppl_compress_policy(const string & policy,
        const PolicyDefinition & policyDefinition,
        bool traceParsingEnabled = false,
        bool traceScanningEnabled = false);

/**
 * Read policy definition and corresponding function handler library from disk.
 */
PolicyDefinition * cppl_read_policy_definition_from_file(const string & policyDefinitionFile,
        const string & functionHandlerLibPath = "");

/**
 * Get policy definition and corresponding function handler from memory.
 */
PolicyDefinition * cppl_read_policy_definition(const string & policyDefinition,
        const Binary * functionHandler = NULL);

/**
 * Get node parameters from disk.
 */
NodeParameters * cppl_read_node_parameters_from_file(const string & nodeParamsPath,
        const string & nodeRuntimeParamsPath,
        const PolicyDefinition * policyDefinition);

/**
 * Get node parameters from memory.
 */
NodeParameters * cppl_read_node_parameters(const string & nodeParams,
        const string & nodeRuntimeParams,
        const PolicyDefinition * policyDefinition);

/**
 * Get compressed cppl policy from file.
 */
PolicyStack * cppl_read_compressed_cppl_from_file(const string & ccpplFilePath,
        const PolicyDefinition * policyDefinition);

/**
 * Get compressed cppl policy from memory.
 */
PolicyStack * cppl_read_compressed_cppl(Binary & binary,
        const PolicyDefinition * policyDefinition);



/* Policy evaluation */

/**
 * @return True of the nodeParameters fulfill the policy, False otherwise.
 */
bool cppl_evaluate(PolicyStack * policyStack, const NodeParameters * NodeParameters);

/**
 * Get a string that describes the reason of the policy results. Only use after cppl_evaluate.
 *
 * Note that if the policy is not fulfilled, cppl_get_reason will require significantly
 * more time compared to a fulfilled policy. Thus, checking unfulfilled policies for
 * a reason can significantly can result in significant performance drops.
 */
string cppl_get_reason(PolicyStack * policyStack, const PolicyDefinition* policyDefinition);

