#include <SGDMResultsPage.h>
#include <SGWBackground.h>
#include <SGWLabel.h>
#include <SGWWidget.h>
#include <SGWPageBackground.h>
#include <SGWTextLabel.h>
#include <SGWHorizontalAlignment.h>
#include <SGWSequentialScrollView.h>
#include <SGWTextButton.h>
#include <SGWSequentialLongLabel.h>

SGWBackground* SGDMResultsPage::pageBackground = nullptr;
SGWLabel* SGDMResultsPage::currentInfo = nullptr;
SGWLabel* SGDMResultsPage::warningLabel = nullptr;
SGWWidget* SGDMResultsPage::warningList = nullptr;

void SGDMResultsPage::showPage(){
    SGWBackground::enable(SGDMResultsPage::pageBackground, &SGDMResultsPage::initialisePage, nullptr);
}

SGWBackground* SGDMResultsPage::initialisePage(){
    SGWBackground* bg = new SGWPageBackground(SGWWidget::parentWidget, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 8);
    SGDMResultsPage::currentInfo = new SGWTextLabel(bg, "starting processing...", 0.0f, 0.5f, 0.0f, 0.5f, 1.0f, -1.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, false);
    SGDMResultsPage::warningLabel = new SGWTextLabel(bg, "warnings: (none)", 0.0f, 0.5f, 0.0f, 2.0f, 1.0f, -1.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, false);
    SGDMResultsPage::warningList = new SGWSequentialScrollView(bg, 0.0f, 0.5f, 0.0f, 3.0f, 1.0f, -1.0f, 1.0f, -5.0f, 0.0f, 0.5f, 8);
    return bg;
}

void SGDMResultsPage::updateInfo(const SGXString &x){
    (*SGDMResultsPage::currentInfo).setTextFromString(x);
}

void SGDMResultsPage::addWarning(const SGXString &x){
    new SGWSequentialLongLabel(SGDMResultsPage::warningList, SGXString('\n') + x, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
    (*SGDMResultsPage::warningLabel).setTextFromString(SGXString("warnings: (") + SGXString::intToString((*SGDMResultsPage::warningList).getChildren().length()) + ")");
}

void SGDMResultsPage::exitPage(){
    SGWBackground::disable(SGDMResultsPage::pageBackground);
}

void SGDMResultsPage::showExitButton(){
    new SGWTextButton(SGDMResultsPage::pageBackground, "done", &SGDMResultsPage::exitPage, 0.5f, -1.5f, 1.0f, -1.5f, 0.0f, 3.0f, 0.0f, 1.0f);
}
