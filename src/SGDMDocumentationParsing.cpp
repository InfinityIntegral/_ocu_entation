#include <SGDMDocumentationParsing.h>
#include <SGDMResultsPage.h>
#include <SGXString.h>
#include <SGDMCppParsing.h>

bool SGDMDocumentationParsing::isWritingHTML = false;
SGXString SGDMDocumentationParsing::sourcePath = "";
SGXString SGDMDocumentationParsing::destinationPath = "";

void SGDMDocumentationParsing::startParsingOperation(){
    SGDMResultsPage::updateInfo("searching for C++ files");
    SGDMCppParsing::getFilesList();
}
