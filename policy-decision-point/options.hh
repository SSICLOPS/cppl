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

#include <stdint.h>

#define __USE_VIRTUAL_FUNC__

//If set to 1 all relations in the relation stack start with a bit that indicates
//if a equal relation with two next relation symbols is following or not
//s.t. these relations can be encoded by using just 1 bit and all others consume 1 bit more
#define OPTIMIZE_NEXT_RELATION_EQUATION_OCCURENCES 0

#define CHOOSE_FIRST_OR_BRANCH_BY_TRUE
#define PRINT_REASON_TO_CONSOLE

#define EVALUATION_OUTPUT 1

typedef uint64_t id_type;
typedef uint64_t offset_t;
typedef uint64_t pol_def_version_t;
typedef uint16_t ccppl_package_len_type;
