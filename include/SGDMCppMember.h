#ifndef SGDMCPPMEMBER_H
#define SGDMCPPMEMBER_H

#include <SGXString.h>
#include <SGLVector.h>

class SGDMCppMember {
public:
    SGDMCppMember() = default;
    bool isFunction;
    bool isStatic;
    bool isPureVirtual;
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
