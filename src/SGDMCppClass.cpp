#include <SGDMCppClass.h>
#include <SGXString.h>

SGLUnorderedMap<SGXString, SGDMCppClass*, SGLEqualsTo<SGXString>, SGLHash<SGXString>>* SGDMCppClass::allClasses = nullptr;
