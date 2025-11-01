#ifndef SGDMCPPCLASS_H
#define SGDMCPPCLASS_H

#include <SGXString.h>
#include <SGLVector.h>

class SGDMCppMember;
class SGDMCppClass {
    SGDMCppClass();
    static SGLVector<SGDMCppClass*> listOfClasses;
    static SGXString projectName;
    SGXString headerPath;
    SGXString sourcePath;
    SGXString headerName;
    SGXString moduleName;
    SGDMCppClass* parentClass;
    SGLVector<SGDMCppClass*> childrenClass;
    SGXString briefDescription;
    SGXString detailedDescription;
    SGLVector<SGDMCppMember*> members;
    SGXString implementationDetails;
};

#endif // SGDMCPPCLASS_H
