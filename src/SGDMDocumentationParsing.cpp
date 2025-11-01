#include <SGDMDocumentationParsing.h>
#include <SGDMResultsPage.h>
#include <SGXString.h>

bool SGDMDocumentationParsing::isWritingHTML = false;

void SGDMDocumentationParsing::startParsingOperation(){
    SGDMResultsPage::updateInfo("testing info");
    SGDMResultsPage::addWarning("warning 1");
    SGDMResultsPage::addWarning("warning 2");
    SGDMResultsPage::addWarning("warning 3");
    SGDMResultsPage::showExitButton();
}
