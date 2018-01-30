# Copyright 2015-2018 RWTH Aachen University
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import cppl
import os, inspect

base_path = os.path.dirname(os.path.abspath(inspect.stack()[0][1])) + "/"


cppl.init()

try:
    policy = cppl.Policy( base_path + "../../examples/example_paper/policy_textual.cppl" )
    print 'policy: {}'.format( policy.get_string() )
    pd = cppl.PolicyDefinition( base_path + "../../examples/example_paper/policy_definition.json", base_path + "../../policy-decision-point/func_handler_23.so" )
    policy.compress_to_file( "/tmp/compressed.ccppl", pd )

    compressed_policy = cppl.CompressedPolicy( "/tmp/compressed.ccppl", pd)
    np = cppl.NodeParameters( base_path + "../../examples/example_paper/policy_nodeparams_1.json", base_path + "../../examples/example_paper/policy_noderuntimeparams_1.json", pd )
    ret_eval, reason = compressed_policy.evaluate( np, with_reason=True )
    if ret_eval:
        msg = 'Policy fulfilled'
    else:
        msg = 'Policy unsatisfied'
    print 'Eval result: {}'.format( msg )
    print reason
finally:
    cppl.cleanup()
