#ifndef SGDMDOCUMENTATIONPARSING_H
#define SGDMDOCUMENTATIONPARSING_H

#include <SGXString.h>
#include <SGLUnorderedMap.h>
#include <SGLEqualsTo.h>
#include <SGLHash.h>
#include <SGLUnorderedSet.h>

template <typename T> class SGLVector;
class SGXString;
class SGDMCppClass;
class SGDMDocumentationParsing {
public:
    static SGXString sourcePath;
    static SGXString destinationPath;
    static void findDocFiles();
    static SGLVector<SGXString>* docFilesList;
    static void parseNextDocFile();
    static void parseDocFile(const SGXString& fileContents);
    static int currentDocFileNumber;
    static void verifyClass();
    static void verifyNextClass();
    static SGLVector<SGDMCppClass*>* verifyClassList;
    static int currentVerifyingClass;
    static SGXString resolveSGDocDirectives(const SGXString& directive);
    static SGLUnorderedMap<SGXString, SGXString, SGLEqualsTo<SGXString>, SGLHash<SGXString>> findReplaceDatabase;
    static void addFindReplacePair(const SGXString& findReplaceDirective);
    static SGLUnorderedSet<SGXString, SGLEqualsTo<SGXString>, SGLHash<SGXString>> autoLinkIgnore;
};

#endif // SGDMDOCUMENTATIONPARSING_H
