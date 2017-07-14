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
