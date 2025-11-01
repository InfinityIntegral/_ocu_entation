#include <SGDMResultsPage.h>
#include <SGWBackground.h>
#include <SGWLabel.h>
#include <SGWWidget.h>
#include <SGWPageBackground.h>
#include <SGWTextLabel.h>
#include <SGWHorizontalAlignment.h>
#include <SGWSequentialScrollView.h>
#include <SGWTextButton.h>
#include <SGDMCppClass.h>
#include <SGLUnorderedMap.h>
#include <SGLEqualsTo.h>
#include <SGLHash.h>

SGWBackground* SGDMResultsPage::pageBackground = nullptr;
SGWLabel* SGDMResultsPage::currentInfo = nullptr;
SGWLabel* SGDMResultsPage::warningLabel = nullptr;
bool SGDMResultsPage::hasWarning = false;
SGWWidget* SGDMResultsPage::warningList = nullptr;

void SGDMResultsPage::showPage(){
    SGWBackground::enable(SGDMResultsPage::pageBackground, &SGDMResultsPage::initialisePage, nullptr);
}

SGWBackground* SGDMResultsPage::initialisePage(){
    SGWBackground* bg = new SGWPageBackground(SGWWidget::parentWidget, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 8);
    SGDMResultsPage::currentInfo = new SGWTextLabel(bg, "starting processing...", 0.0f, 0.5f, 0.0f, 0.5f, 1.0f, -1.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, false);
    SGDMResultsPage::hasWarning = false;
    SGDMResultsPage::warningLabel = new SGWTextLabel(bg, "warnings: (none)", 0.0f, 0.5f, 0.0f, 2.0f, 1.0f, -1.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, false);
    SGDMResultsPage::warningList = new SGWSequentialScrollView(bg, 0.0f, 0.5f, 0.0f, 3.0f, 1.0f, -1.0f, 1.0f, -5.0f, 0.0f, 0.5f, 8);
    return bg;
}

void SGDMResultsPage::updateInfo(const SGXString &x){
    (*SGDMResultsPage::currentInfo).setTextFromString(x);
}

void SGDMResultsPage::addWarning(const SGXString &x){
    if(SGDMResultsPage::hasWarning == false){
        SGDMResultsPage::hasWarning = true;
        (*SGDMResultsPage::warningLabel).setTextFromString("warnings:");
    }
    new SGWTextLabel(SGDMResultsPage::warningList, x + "\n", 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, false);
}

void SGDMResultsPage::exitPage(){
    SGWBackground::disable(SGDMResultsPage::pageBackground);
    SGDMResultsPage::terminate();
}

void SGDMResultsPage::showExitButton(){
    new SGWTextButton(SGDMResultsPage::pageBackground, "done", &SGDMResultsPage::exitPage, 0.5f, -1.5f, 1.0f, -1.5f, 0.0f, 3.0f, 0.0f, 1.0f);
}

void SGDMResultsPage::terminate(){
    if(SGDMCppClass::allClasses != nullptr){
        for(SGLUnorderedMap<SGXString, SGDMCppClass*, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::ConstIterator i=(*SGDMCppClass::allClasses).constBegin(); i != (*SGDMCppClass::allClasses).constEnd(); i++){
            delete i.value();
        }
    }
    delete SGDMCppClass::allClasses;
    SGDMCppClass::allClasses = nullptr;
}
