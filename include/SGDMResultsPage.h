#ifndef SGDMRESULTSPAGE_H
#define SGDMRESULTSPAGE_H

class SGWBackground;
class SGWLabel;
class SGWWidget;
class SGXString;
class SGDMResultsPage {
public:
    static SGWBackground* pageBackground;
    static void showPage();
    static SGWBackground* initialisePage();
    static SGWLabel* currentInfo;
    static SGWLabel* warningLabel;
    static SGWWidget* warningList;
    static void updateInfo(const SGXString& x);
    static void addWarning(const SGXString& x);
    static void exitPage();
    static void showExitButton();
};

#endif // SGDMRESULTSPAGE_H
