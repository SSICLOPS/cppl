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

def readfile(file):
    with open(file, 'r') as f:
        data = f.readlines()
        data_formatted = "".join( data )
    return data_formatted

pd_mem = readfile( base_path + "../../examples/example_paper/policy_definition.json" )
pd_funchandler_mem = readfile( base_path + "../../policy-decision-point/func_handler_23.so" )
np_mem = readfile( base_path + "../../examples/example_paper/policy_nodeparams_1.json" )
npr_mem = readfile( base_path + "../../examples/example_paper/policy_noderuntimeparams_1.json" )

cppl.init()

try:
    policy = cppl.Policy( base_path + "../../examples/example_paper/policy_textual.cppl" )
    print 'policy: {}'.format( policy.get_string() )
    pd = cppl.PolicyDefinition( pd_mem, pd_funchandler_mem, inmemory=True )
    compressed_policy_raw = policy.compress( pd )
    # with open("/tmp/compressed_from_mem.ccppl", "w") as f:
    #     f.write( compressed_policy_raw )

    compressed_policy = cppl.CompressedPolicy( compressed_policy_raw, pd, inmemory=True )
    np = cppl.NodeParameters( np_mem, npr_mem, pd, inmemory=True )
    ret_eval, reason = compressed_policy.evaluate( np, with_reason=True )
    if ret_eval:
        msg = 'Policy fulfilled'
    else:
        msg = 'Policy unsatisfied'
    print 'Eval result: {}'.format( msg )
    print reason
finally:
    cppl.cleanup()
