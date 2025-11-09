#include <SGDMCppClass.h>
#include <SGDMCppMember.h>
#include <SGXString.h>
#include <SGLUnorderedMap.h>
#include <SGLEqualsTo.h>
#include <SGLHash.h>

SGLUnorderedMap<SGXString, SGDMCppClass, SGLEqualsTo<SGXString>, SGLHash<SGXString>> SGDMCppClass::allClasses;

bool CompareStringsByLength::operator()(const SGXString& a, const SGXString& b) const {
    if(a.length() != b.length()){return (a.length() > b.length());}
    return (a < b);
}
