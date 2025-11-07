#include <SGDMUserInterface.h>
#include <SGWTextLabel.h>
#include <SGWTextInput.h>
#include <SGWWidget.h>
#include <SGWHorizontalAlignment.h>
#include <SGWPageBackground.h>
#include <SGWTextButton.h>
#include <SGXFileSystem.h>
#include <SGWBackground.h>
#include <SGWInput.h>
#include <SGWLabel.h>
#include <SGWNotify.h>
#include <SGDMDocumentationParsing.h>
#include <SGDMResultsPage.h>
#include <SGXFile.h>
#include <SGDMCppParsing.h>

SGWBackground* SGDMUserInterface::pageBackground = nullptr;
SGWInput* SGDMUserInterface::sourceInput = nullptr;
SGWLabel* SGDMUserInterface::sourceWarning = nullptr;
SGWInput* SGDMUserInterface::destinationInput = nullptr;
SGWLabel* SGDMUserInterface::destinationWarning = nullptr;

void SGDMUserInterface::initialise(){
    SGDMUserInterface::pageBackground = new SGWPageBackground(SGWWidget::parentWidget, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 8);
    new SGWTextLabel(SGDMUserInterface::pageBackground, "project folder path:", 0.0f, 0.5f, 0.0f, 0.5f, 1.0f, -1.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, false);
    SGDMUserInterface::sourceInput = new SGWTextInput(SGDMUserInterface::pageBackground, &SGDMUserInterface::checkSourcePath, 0.0f, 0.5f, 0.0f, 1.5f, 1.0f, -1.0f, 0.0f, 1.0f);
    SGDMUserInterface::sourceWarning = new SGWTextLabel(SGDMUserInterface::pageBackground, "this folder must exist", 0.0f, 0.5f, 0.0f, 2.5f, 1.0f, -1.0f, 0.0f, 0.75f, SGWHorizontalAlignment::Left, true);
    new SGWTextLabel(SGDMUserInterface::pageBackground, "output folder path:", 0.0f, 0.5f, 0.0f, 3.5f, 1.0f, -1.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, false);
    SGDMUserInterface::destinationInput = new SGWTextInput(SGDMUserInterface::pageBackground, &SGDMUserInterface::checkDestinationPath, 0.0f, 0.5f, 0.0f, 4.5f, 1.0f, -1.0f, 0.0f, 1.0f);
    SGDMUserInterface::destinationWarning = new SGWTextLabel(SGDMUserInterface::pageBackground, "this folder must be empty or nonexistent", 0.0f, 0.5f, 0.0f, 5.5f, 1.0f, -1.0f, 0.0f, 1.0f, SGWHorizontalAlignment::Left, true);
    new SGWTextButton(SGDMUserInterface::pageBackground, "generate Markdown", &SGDMUserInterface::generateMD, 0.5f, -4.0f, 0.0f, 7.0f, 0.0f, 8.0f, 0.0f, 1.0f);
    new SGWTextButton(SGDMUserInterface::pageBackground, "generate HTML", &SGDMUserInterface::generateHTML, 0.5f, -4.0f, 0.0f, 8.5f, 0.0f, 8.0f, 0.0f, 1.0f);
    SGDMUserInterface::getStoredPaths();
}

void SGDMUserInterface::checkSourcePath(){
    if(SGXFileSystem::folderExists((*SGDMUserInterface::sourceInput).getTextAsString()) == false){
        (*SGDMUserInterface::sourceInput).setInvalid(true);
        (*SGDMUserInterface::sourceWarning).setItemVisibility(true);
    }
    else{
        (*SGDMUserInterface::sourceInput).setInvalid(false);
        (*SGDMUserInterface::sourceWarning).setItemVisibility(false);
    }
}

void SGDMUserInterface::checkDestinationPath(){
    if(SGXFileSystem::folderExists((*SGDMUserInterface::destinationInput).getTextAsString()) == true && SGXFileSystem::getFolderSize((*SGDMUserInterface::destinationInput).getTextAsString()) != 0ll){
        (*SGDMUserInterface::destinationInput).setInvalid(true);
        (*SGDMUserInterface::destinationWarning).setItemVisibility(true);
    }
    else{
        (*SGDMUserInterface::destinationInput).setInvalid(false);
        (*SGDMUserInterface::destinationWarning).setItemVisibility(false);
    }
}

bool SGDMUserInterface::checkFolderValidity(){
    if(SGXFileSystem::folderExists((*SGDMUserInterface::sourceInput).getTextAsString()) == false){
        SGWNotify::notify("project folder does not exist, not ok");
        return false;
    }
    if(SGXFileSystem::folderExists((*SGDMUserInterface::destinationInput).getTextAsString()) == true && SGXFileSystem::getFolderSize((*SGDMUserInterface::destinationInput).getTextAsString()) != 0ll){
        SGWNotify::notify("output folder has contents, not ok");
        return false;
    }
    SGDMUserInterface::setStoredPaths();
    SGDMResultsPage::showPage();
    SGDMDocumentationParsing::sourcePath = (*SGDMUserInterface::sourceInput).getTextAsString();
    SGDMDocumentationParsing::destinationPath = (*SGDMUserInterface::destinationInput).getTextAsString();
    if(SGDMDocumentationParsing::sourcePath.at(SGDMDocumentationParsing::sourcePath.length() - 1) != '/'){SGDMDocumentationParsing::sourcePath += "/";}
    return true;
}

void SGDMUserInterface::generateMD(){
    if(SGDMUserInterface::checkFolderValidity() == false){return;}
    SGDMDocumentationParsing::isWritingHTML = false;
    SGDMCppParsing::startParsingOperation();
}

void SGDMUserInterface::generateHTML(){
    if(SGDMUserInterface::checkFolderValidity() == false){return;}
    SGDMDocumentationParsing::isWritingHTML = true;
    SGDMCppParsing::startParsingOperation();
}

void SGDMUserInterface::getStoredPaths(){
    const SGXString filePath = SGXFileSystem::joinFilePaths(SGXFileSystem::configFilePath, "paths.sg");
    if(SGXFileSystem::fileExists(filePath) == false){return;}
    {
        const SGXFile fileReader(filePath);
        (*SGDMUserInterface::sourceInput).setTextFromString(fileReader.readString());
        (*SGDMUserInterface::destinationInput).setTextFromString(fileReader.readString());
    }
}

void SGDMUserInterface::setStoredPaths(){
    const SGXString filePath = SGXFileSystem::joinFilePaths(SGXFileSystem::configFilePath, "paths.sg");
    if(SGXFileSystem::fileExists(filePath) == true){SGXFileSystem::permanentDeleteFile(filePath);}
    SGXFileSystem::createFile(filePath);
    {
        const SGXFile fileWriter(filePath);
        fileWriter.writeString((*SGDMUserInterface::sourceInput).getTextAsString());
        fileWriter.writeString((*SGDMUserInterface::destinationInput).getTextAsString());
    }
}
