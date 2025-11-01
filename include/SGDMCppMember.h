#ifndef SGDMCPPMEMBER_H
#define SGDMCPPMEMBER_H

#include <SGXString.h>
#include <SGLVector.h>

class SGDMCppMember {
    SGDMCppMember();
    SGXString functionSignature;
    SGXString functionName;
    SGXString normalisedSignature;
    SGXString description;
    SGXString implementationDetails;
    SGXString timeComplexity;
    SGLVector<SGXString> notes;
    SGLVector<SGXString> warnings;
};

#endif // SGDMCPPMEMBER_H
