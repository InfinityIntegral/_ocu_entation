#ifndef SGDMSGMLGENERATION_H
#define SGDMSGMLGENERATION_H

#include <SGXString.h>

template <typename T1, typename T2> class SGLPair;
template <typename T> class SGLVector;
class SGDMCppClass;
class SGDMCppMember;
class SGDMSgmlGeneration {
public:
    SGDMSgmlGeneration() = delete;
    static SGLVector<SGDMCppClass*>* generationSources;
    static void generateSgml();
    static void generateForNextClass();
    static int currentGenerationSource;
    static SGXString githubPrefixLink;
    static SGXString createLink(const SGXString& linkText, const SGXString& link);
    static SGXString projectName;
    static SGXString autoLinkMemberName(const SGDMCppMember* member, const SGXString& className);
};

class MemberOrdering {
public:
    bool operator()(const SGDMCppMember* a, const SGDMCppMember* b);
};

class MemberPairOrdering {
public:
    bool operator()(const SGLPair<const SGDMCppMember*, SGXString>& a, const SGLPair<const SGDMCppMember*, SGXString>& b);
};

#endif // SGDMSGMLGENERATION_H
