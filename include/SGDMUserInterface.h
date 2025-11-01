#ifndef SGDMUSERINTERFACE_H
#define SGDMUSERINTERFACE_H

class SGWInput;
class SGWLabel;
class SGWBackground;
class SGDMUserInterface {
public:
    SGDMUserInterface() = delete;
    static void initialise();
    static SGWBackground* pageBackground;
    static SGWInput* sourceInput;
    static SGWInput* destinationInput;
    static SGWLabel* sourceWarning;
    static SGWLabel* destinationWarning;
    static void checkSourcePath();
    static void checkDestinationPath();
    static void generateMD();
    static void generateHTML();
    static bool checkFolderValidity();
    static void getStoredPaths();
    static void setStoredPaths();
};

#endif // SGDMUSERINTERFACE_H
