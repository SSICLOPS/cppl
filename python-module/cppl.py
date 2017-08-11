import cppl_cpp_python_bridge as cppl_bridge

error = cppl_bridge.error

def init():
    cppl_bridge.init()

def cleanup():
    cppl_bridge.cleanup()

class PolicyDefinition(object):
    """docstring for PolicyDefinition"""
    def __init__(self, policyDefinitionPath, functionHandlerPath, inmemory=False):
        super(PolicyDefinition, self).__init__()
        if inmemory:
            self.pd_id = cppl_bridge.read_policy_definition( policyDefinitionPath, functionHandlerPath)
        else:
            self.pd_id = cppl_bridge.read_policy_definition_from_file( policyDefinitionPath, functionHandlerPath)

    def get_id(self):
        return self.pd_id

    def __del__(self):
        cppl_bridge.policy_definition_free( self.get_id() )

class NodeParameters(object):
    """docstring for NodeParameters"""
    def __init__(self, nodeParametersPath, nodeRuntimeParametersPath, policyDefinition, inmemory=False):
        # sanity checks
        if type(policyDefinition) != PolicyDefinition:
            raise TypeError

        super(NodeParameters, self).__init__()
        if inmemory:
            self.np_id = cppl_bridge.read_node_parameters( nodeParametersPath, nodeRuntimeParametersPath, policyDefinition.get_id() )
        else:
            self.np_id = cppl_bridge.read_node_parameters_from_file( nodeParametersPath, nodeRuntimeParametersPath, policyDefinition.get_id() )

    def get_id(self):
        return self.np_id

    def __del__(self):
        cppl_bridge.node_parameters_free( self.get_id() )

class Policy(object):
    """docstring for Policy"""
    def __init__(self, policyPath=None, inmemory=False):
        super(Policy, self).__init__()
        if inmemory:
            self.string = policyPath
        else
            self.string = cppl_bridge.read_policy_from_file( policyPath )

    def get_string(self):
        return self.string

    def compress(self, policyDefinition):
        # sanity checks
        if type(policyDefinition) != PolicyDefinition:
            raise TypeError

        ret = cppl_bridge.compress_policy(self.get_string(), policyDefinition.get_id())
        return ret

    def compress_to_file(self, destPath, policyDefinition):
        # sanity checks
        if type(policyDefinition) != PolicyDefinition:
            raise TypeError

        ret = cppl_bridge.compress_policy_to_file(self.get_string(), policyDefinition.get_id(), destPath)

    # def __del__(self):
    #     # Nothing todo as only a string is passed from C++ to Python
    #     cppl_bridge.compressed_cppl_free( self.get_id() )

class CompressedPolicy(object):
    """docstring for CompressedPolicy"""
    def __init__(self, compressedPolicyPath, policyDefinition, inmemory=False):
        # sanity checks
        if type(policyDefinition) != PolicyDefinition:
            raise TypeError

        super(CompressedPolicy, self).__init__()
        if inmemory:
            self.ccppl_id = cppl_bridge.read_compressed_cppl( compressedPolicyPath, policyDefinition.get_id() )
        else:
            self.ccppl_id = cppl_bridge.read_compressed_cppl_from_file( compressedPolicyPath, policyDefinition.get_id() )
        self.policyDefinition = policyDefinition

    def get_id(self):
        return self.ccppl_id

    def __del__(self):
        cppl_bridge.compressed_cppl_free( self.get_id() )

    def evaluate(self, nodeParameters, with_reason=True):
        # sanity checks
        if type( nodeParameters ) != NodeParameters:
            raise TypeError

        ret_eval = cppl_bridge.evaluate(self.get_id(), nodeParameters.get_id() )

        reason = None
        if with_reason:
            reason = cppl_bridge.get_reason( self.get_id(), self.policyDefinition.get_id() )

        return ret_eval, reason
