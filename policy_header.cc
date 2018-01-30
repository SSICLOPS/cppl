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

#include "policy_header.hh"

ostream& operator<<(ostream &out, PolicyHeader &policyHeader) {
    #if DEBUG_POLICY_GENERATION
        out << policyHeader.asString;
    #endif
    return out;
}

void PolicyHeader::store()  {
    //add the header version to the binary
    policyBinary.push_back(version, bitsForVersion);
}

//print the size of the header
void PolicyHeader::printSize()  {
    //DEBUG output for header size
    cout << "Header size: " << (int16_t) bitsForVersion << endl;
}
