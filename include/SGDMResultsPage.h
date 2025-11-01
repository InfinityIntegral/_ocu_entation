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
    static bool hasWarning;
    static SGWWidget* warningList;
    static void updateInfo(const SGXString& x);
    static void addWarning(const SGXString& x);
    static void exitPage();
    static void showExitButton();
    static void terminate();
};

#endif // SGDMRESULTSPAGE_H
