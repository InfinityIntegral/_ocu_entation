#ifndef SGDMCPPCLASS_H
#define SGDMCPPCLASS_H

#include <SGXString.h>
#include <SGLVector.h>
#include <SGDMCppMember.h>

template <typename K, typename V, typename EqualityCheck, typename HashFunction> class SGLUnorderedMap;
template <typename T> class SGLEqualsTo;
template <typename T> class SGLHash;
class SGDMCppClass {
public:
    SGDMCppClass() = default;
    static SGLUnorderedMap<SGXString, SGDMCppClass*, SGLEqualsTo<SGXString>, SGLHash<SGXString>>* allClasses;
    static SGXString projectName;
    SGXString headerPath;
    SGXString sourcePath;
    SGXString moduleName;
    SGXString className;
    SGXString templateInfo;
    SGXString parentClass;
    SGLVector<SGXString> childrenClass;
    SGXString briefDescription;
    SGXString detailedDescription;
    SGLVector<SGDMCppMember> members;
    SGXString implementationDetails;
};

#endif // SGDMCPPCLASS_H
