#include <stdint.h>

#define __USE_VIRTUAL_FUNC__

//If set to 1 all relations in the relation stack start with a bit that indicates
//if a equal relation with two next relation symbols is following or not
//s.t. these relations can be encoded by using just 1 bit and all others consume 1 bit more
#define OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES 0

typedef uint64_t id_type;
typedef uint64_t offset_t;
typedef uint64_t pol_def_version_t;
typedef uint16_t ccppl_package_len_type;
