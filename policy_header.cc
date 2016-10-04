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
