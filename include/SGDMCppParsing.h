#ifndef SGDMCPPPARSING_H
#define SGDMCPPPARSING_H

class SGXString;
template <typename T> class SGLVector;
template <typename T1, typename T2> class SGLPair;
class SGDMCppParsing {
public:
    SGDMCppParsing() = delete;
    static void getFilesList();
    static SGLVector<SGXString>* filesList;
    static int currentFileNumber;
    static void processNextFile();
    static SGLVector<SGLPair<SGXString, SGXString>>* classList;
    static int currentClassNumber;
    static void processNextClass();
    static SGLVector<SGLPair<SGXString, SGXString>>* membersList;
    static void linkInheritance();
    static int currentMemberNumber;
    static void processNextMember();
    static bool isCppKeyword(const SGXString& s);
};

#endif // SGDMCPPPARSING_H
