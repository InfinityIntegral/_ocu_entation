#ifndef SGDMCPPMEMBER_H
#define SGDMCPPMEMBER_H

#include <SGXString.h>
#include <SGLVector.h>
#include <SGLSet.h>

class CompareStringsByLength {
public:
    bool operator()(const SGXString& a, const SGXString& b) const;
};

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
    SGLSet<SGXString, CompareStringsByLength> parameters;
    SGLVector<SGXString> description;
    SGLVector<SGXString> notes;
    SGLVector<SGXString> warnings;
};

#endif // SGDMCPPMEMBER_H
