#pragma once

//If set to 1 all relations in the relation stack start with a bit that indicates
//if a equal relation with two next relation symbols is following or not
//s.t. these relations can be encoded by using just 1 bit and all others consume 1 bit more
#define OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES 0

// Number of bits used to encode the relation type in the relation stack
#define RELATION_TYPE_LEN_IN_BITS           3
// Number of bits used to encode the variable type in the variable stack
#define VARIABLE_TYPE_LEN_IN_BITS           4

// Number of bits used to represent the position of a redundant relation
#define SPECIFIC_RELATION_ID_LEN_IN_BITS    8
// Number of bits used to represent the position of a redundant variable
#define SPECIFIC_VARIABLE_ID_LEN_IN_BITS    8


// Sanity checks
#if RELATION_TYPE_LEN_IN_BITS > 255
  #error RELATION_TYPE_LEN_IN_BITS > 255
#endif
#if VARIABLE_TYPE_LEN_IN_BITS > 255
  #error VARIABLE_TYPE_LEN_IN_BITS > 255
#endif
#if SPECIFIC_RELATION_ID_LEN_IN_BITS > 255
  #error SPECIFIC_RELATION_ID_LEN_IN_BITS > 255
#endif
#if SPECIFIC_VARIABLE_ID_LEN_IN_BITS > 255
  #error SPECIFIC_VARIABLE_ID_LEN_IN_BITS > 255
#endif

#define CHOOSE_FIRST_OR_BRANCH_BY_TRUE

#define CONVERT_TO_DNF
