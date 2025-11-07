#ifndef SGDMDOCUMENTATIONPARSING_H
#define SGDMDOCUMENTATIONPARSING_H

#include <SGXString.h>

template <typename T> class SGLVector;
class SGXString;
class SGDMCppClass;
class SGDMDocumentationParsing {
public:
    static bool isWritingHTML; // true - HTML, false - MD
    static SGXString sourcePath;
    static SGXString destinationPath;
    static void findDocFiles();
    static SGLVector<SGXString>* docFilesList;
    static void parseNextDocFile();
    static void parseDocFile(const SGXString& fileContents);
    static int currentDocFileNumber;
    static void verifyClass();
    static void verifyNextClass();
    static SGLVector<const SGDMCppClass*>* verifyClassList;
    static int currentVerifyingClass;
};

#endif // SGDMDOCUMENTATIONPARSING_H
