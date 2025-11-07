#ifndef SGDMCPPCLASS_H
#define SGDMCPPCLASS_H

#include <SGXString.h>
#include <SGLVector.h>
#include <SGDMCppMember.h>
#include <SGLUnorderedMap.h>
#include <SGLEqualsTo.h>
#include <SGLHash.h>

class SGDMCppClass {
public:
    SGDMCppClass() = default;
    static SGLUnorderedMap<SGXString, SGDMCppClass, SGLEqualsTo<SGXString>, SGLHash<SGXString>> allClasses;
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
    SGLUnorderedMap<SGXString, SGDMCppMember, SGLEqualsTo<SGXString>, SGLHash<SGXString>> members;
    SGXString implementationDetails;
};

#endif // SGDMCPPCLASS_H
