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
    bool isPrivateAPI;
    SGXString functionName;
    SGXString normalisedSignature;
    SGXString fullDeclaration;
    SGLVector<SGXString> description;
    SGLVector<SGXString> notes;
    SGLVector<SGXString> warnings;
};

#endif // SGDMCPPMEMBER_H
