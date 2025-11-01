#ifndef SGDMDOCUMENTATIONPARSING_H
#define SGDMDOCUMENTATIONPARSING_H

#include <SGXString.h>

class SGDMDocumentationParsing {
public:
    static bool isWritingHTML; // true - HTML, false - MD
    static void startParsingOperation();
    static SGXString sourcePath;
    static SGXString destinationPath;
};

#endif // SGDMDOCUMENTATIONPARSING_H
