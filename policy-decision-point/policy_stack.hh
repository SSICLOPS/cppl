#include "__policy_stack.hh"
#include "pdp_policy_stack_processor.hh"
#include "policy_stack_evaluator.hh"
#include "ccppl_decompressor.hh"
#include "ast_2_ccppl.hh"

typedef __PolicyStack<PDPPolicyStackProcessor<PolicyStackEvaluator, CcpplDecompressor>> PolicyStack;
typedef __PolicyStack<Ast2Ccppl> PolicyStackCompress;
