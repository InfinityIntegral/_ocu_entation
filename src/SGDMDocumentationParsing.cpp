#include <SGDMDocumentationParsing.h>
#include <SGDMResultsPage.h>
#include <SGXString.h>
#include <SGDMCppParsing.h>
#include <SGXFileSystem.h>
#include <SGLArray.h>
#include <SGLVector.h>
#include <SGXTimer.h>
#include <SGXFile.h>
#include <SGDMCppClass.h>
#include <SGXDebug.h>
#include <SGLUnorderedMap.h>
#include <SGLEqualsTo.h>
#include <SGLHash.h>

bool SGDMDocumentationParsing::isWritingHTML = false;
SGXString SGDMDocumentationParsing::sourcePath = "";
SGXString SGDMDocumentationParsing::destinationPath = "";
SGLVector<SGXString>* SGDMDocumentationParsing::docFilesList = nullptr;
int SGDMDocumentationParsing::currentDocFileNumber = 0;
SGLVector<const SGDMCppClass*>* SGDMDocumentationParsing::verifyClassList = nullptr;
int SGDMDocumentationParsing::currentVerifyingClass = 0;

void SGDMDocumentationParsing::findDocFiles(){
    SGDMResultsPage::updateInfo("searching for SGDoc files");
    SGDMDocumentationParsing::docFilesList = new SGLVector<SGXString>();
    SGLArray<SGXString> files = SGXFileSystem::getFilesListWithExtensionRecursive(SGDMDocumentationParsing::sourcePath, "sgdoc");
    for(int i=0; i<files.length(); i++){
        (*SGDMDocumentationParsing::docFilesList).pushBack(files.at(i));
    }
    SGDMDocumentationParsing::currentDocFileNumber = 0;
    SGXTimer::singleCall(0.0f, &SGDMDocumentationParsing::parseNextDocFile);
}

void SGDMDocumentationParsing::parseNextDocFile(){
    if(SGDMDocumentationParsing::currentDocFileNumber == (*SGDMDocumentationParsing::docFilesList).length()){
        SGDMResultsPage::updateInfo("done parsing SGDoc files");
        delete SGDMDocumentationParsing::docFilesList;
        SGDMDocumentationParsing::docFilesList = nullptr;
        SGDMDocumentationParsing::currentDocFileNumber = 0;
        SGXTimer::singleCall(0.0f, &SGDMDocumentationParsing::verifyClass);
        return;
    }
    const SGXString currentFile = (*SGDMDocumentationParsing::docFilesList).at(SGDMDocumentationParsing::currentDocFileNumber);
    SGDMDocumentationParsing::currentDocFileNumber++;
    SGDMResultsPage::updateInfo(SGXString("parsing ") + SGXString::intToString(SGDMDocumentationParsing::currentDocFileNumber) + " of " + SGXString::intToString((*SGDMDocumentationParsing::docFilesList).length()) + " files");
    SGDMDocumentationParsing::parseDocFile(SGXFile::readAllText(currentFile));
    SGXTimer::singleCall(0.0f, &SGDMDocumentationParsing::parseNextDocFile);
}

void SGDMDocumentationParsing::parseDocFile(const SGXString& fileContents){
    SGLArray<SGXString> list = fileContents.splitCustomSeparator(SGXChar('\n'));
    SGXString currentClass = "";
    SGXString currentMember = "";
    for(int i=0; i<list.length(); i++){
        SGXString currentDirective = list.at(i);
        currentDirective.cleanWhitespace();
        if(currentDirective == "" || currentDirective == " "){continue;}
        if(currentDirective.length() > 14 && currentDirective.substringLeft(14) == "@SG_CPP_CLASS "){
            SGXString className = currentDirective.substringRight(currentDirective.length() - 14);
            if(SGDMCppClass::allClasses.contains(className) == false){SGDMResultsPage::addWarning(SGXString("Selecting nonexistent class ") + className);}
            else{currentClass = className;}
        }
        else if(currentDirective.length() > 17 && currentDirective.substringLeft(17) == "@SG_SHORT_DESCRP "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding short description to nonexistent class ") + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).briefDescription = currentDirective.substringRight(currentDirective.length() - 17);}
        }
        else if(currentDirective.length() > 16 && currentDirective.substringLeft(16) == "@SG_LONG_DESCRP "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding long description to nonexistent class ") + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).detailedDescription = currentDirective.substringRight(currentDirective.length() - 16);}
        }
        else if(currentDirective.length() > 10 && currentDirective.substringLeft(10) == "@SG_IMPLE "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding implementation info to nonexistent class ") + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).implementationDetails = currentDirective.substringRight(currentDirective.length() - 10);}
        }
        else if(currentDirective.length() > 16 && currentDirective.substringLeft(16) == "@SG_PRIVATE_API "){
            SGXString memberName = currentDirective.substringRight(currentDirective.length() - 16);
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Marking private member in nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(memberName) == false){SGDMResultsPage::addWarning(SGXString("Selecting nonexistent member ") + memberName + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(memberName).isPrivateAPI = true;}
        }
        else if(currentDirective.length() > 9 && currentDirective.substringLeft(9) == "@SG_FUNC "){
            SGXString memberName = currentDirective.substringRight(currentDirective.length() - 9);
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Selecting member in nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(memberName) == false){SGDMResultsPage::addWarning(SGXString("Selecting nonexistent member ") + memberName + " in class " + currentClass);}
            else{currentMember = memberName;}
        }
        else if(currentDirective.length() > 9 && currentDirective.substringLeft(9) == "@SG_NOTE "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding note to nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(currentMember) == false){SGDMResultsPage::addWarning(SGXString("Adding note to nonexistent member ") + currentMember + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(currentMember).notes.pushBack(currentDirective.substringRight(currentDirective.length() - 9));}
        }
        else if(currentDirective.length() > 9 && currentDirective.substringLeft(9) == "@SG_WARN "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding warning to nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(currentMember) == false){SGDMResultsPage::addWarning(SGXString("Adding warning to nonexistent member ") + currentMember + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(currentMember).warnings.pushBack(currentDirective.substringRight(currentDirective.length() - 9));}
        }
        else{
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding description to nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(currentMember) == false){SGDMResultsPage::addWarning(SGXString("Adding description to nonexistent member ") + currentMember + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(currentMember).description.pushBack(currentDirective);}
        }
    }
}

void SGDMDocumentationParsing::verifyClass(){
    SGDMResultsPage::updateInfo("preparing verification...");
    SGDMDocumentationParsing::verifyClassList = new SGLVector<const SGDMCppClass*>();
    for(SGLUnorderedMap<SGXString, SGDMCppClass, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::ConstIterator i = SGDMCppClass::allClasses.constBegin(); i != SGDMCppClass::allClasses.constEnd(); i++){
        (*SGDMDocumentationParsing::verifyClassList).pushBack(&i.value());
    }
    SGDMDocumentationParsing::currentVerifyingClass = 0;
    SGXTimer::singleCall(0.0f, &SGDMDocumentationParsing::verifyNextClass);
}

void SGDMDocumentationParsing::verifyNextClass(){
    if(SGDMDocumentationParsing::currentVerifyingClass == (*SGDMDocumentationParsing::verifyClassList).length()){
        SGDMResultsPage::updateInfo("done verifying documentation");
        delete SGDMDocumentationParsing::verifyClassList;
        SGDMDocumentationParsing::verifyClassList = nullptr;
        SGDMDocumentationParsing::currentVerifyingClass = 0;
        return;
    }
    const SGDMCppClass& currentClass = (*(*SGDMDocumentationParsing::verifyClassList).at(SGDMDocumentationParsing::currentVerifyingClass));
    SGDMDocumentationParsing::currentVerifyingClass++;
    SGDMResultsPage::updateInfo(SGXString("verifying ") + SGXString::intToString(SGDMDocumentationParsing::currentVerifyingClass) + " of " + SGXString::intToString((*SGDMDocumentationParsing::verifyClassList).length()) + " C++ classes");
    
    if(currentClass.briefDescription == ""){SGDMResultsPage::addWarning(currentClass.className + " missing brief description");}
    if(currentClass.detailedDescription == ""){SGDMResultsPage::addWarning(currentClass.className + " missing detailed description");}
    if(currentClass.implementationDetails == ""){SGDMResultsPage::addWarning(currentClass.className + " missing implementation info");}
    for(SGLUnorderedMap<SGXString, SGDMCppMember, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::ConstIterator i = currentClass.members.constBegin(); i != currentClass.members.constEnd(); i++){
        const SGDMCppMember& currentMember = i.value();
        if(currentMember.isPrivateAPI == false && currentMember.description.length() == 0){SGDMResultsPage::addWarning(currentMember.fullDeclaration + " in " + currentClass.className + " missing description");}
    }
    
    SGXTimer::singleCall(0.0f, &SGDMDocumentationParsing::verifyNextClass);
}
