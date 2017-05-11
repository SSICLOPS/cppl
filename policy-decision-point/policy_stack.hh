#include "__policy_stack.hh"
#include "pdp_policy_stack_processor.hh"
#include "policy_stack_evaluator.hh"
#include "ccppl_decompressor.hh"

typedef __PolicyStack<PDPPolicyStackProcessor<PolicyStackEvaluator, CcpplDecompressor>> PolicyStack;
