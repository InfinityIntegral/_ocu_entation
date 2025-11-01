#include <SGDMCppParsing.h>
#include <SGLVector.h>
#include <SGXString.h>
#include <SGDMResultsPage.h>
#include <SGXFileSystem.h>
#include <SGLStack.h>
#include <SGDMDocumentationParsing.h>
#include <SGXTimer.h>
#include <SGXFile.h>
#include <SGLPair.h>
#include <SGDMCppClass.h>
#include <SGLUnorderedMap.h>
#include <SGLEqualsTo.h>
#include <SGLHash.h>
#include <SGLArray.h>

int SGDMCppParsing::currentFileNumber = 0;
SGLVector<SGXString>* SGDMCppParsing::filesList = nullptr;
SGLVector<SGLPair<SGXString, SGXString>>* SGDMCppParsing::classList = nullptr;
int SGDMCppParsing::currentClassNumber = 0;
SGLVector<SGLPair<SGXString, SGXString>>* SGDMCppParsing::membersList = nullptr;

void SGDMCppParsing::getFilesList(){
    SGDMResultsPage::updateInfo("searching for C++ files");
    SGDMCppParsing::filesList = new SGLVector<SGXString>();
    SGLStack<SGXString> foldersToProcess;
    foldersToProcess.push(SGDMDocumentationParsing::sourcePath);
    while(foldersToProcess.length() > 0){
        const SGXString currentFolder = foldersToProcess.top();
        foldersToProcess.pop();
        if(SGXFileSystem::getFileNameNoExtension(currentFolder).contains("private_api")){continue;}
        SGLArray<SGXString> filesInThisFolder = SGXFileSystem::getFilesListWithExtension(currentFolder, "h");
        for(int i=0; i<filesInThisFolder.length(); i++){
            const SGXString fileName = SGXFileSystem::getFileNameNoExtension(filesInThisFolder.at(i));
            if(fileName.length() <= 2 || fileName.substringLeft(2) != "SG"){continue;}
            (*SGDMCppParsing::filesList).pushBack(filesInThisFolder.at(i));
        }
        SGLArray<SGXString> foldersInThisFolder = SGXFileSystem::getFoldersList(currentFolder);
        for(int i=0; i<foldersInThisFolder.length(); i++){
            foldersToProcess.push(foldersInThisFolder.at(i));
        }
    }
    SGDMCppParsing::classList = new SGLVector<SGLPair<SGXString, SGXString>>();
    SGXTimer::singleCall(0.0f, &SGDMCppParsing::processNextFile);
}

void SGDMCppParsing::processNextFile(){
    if(SGDMCppParsing::currentFileNumber == (*SGDMCppParsing::filesList).length()){
        SGDMResultsPage::updateInfo("done parsing C++ files");
        delete SGDMCppParsing::filesList;
        SGDMCppParsing::filesList = nullptr;
        SGDMCppParsing::currentFileNumber = 0;
        SGDMCppParsing::membersList = new SGLVector<SGLPair<SGXString, SGXString>>();
        SGDMCppClass::allClasses = new SGLUnorderedMap<SGXString, SGDMCppClass*, SGLEqualsTo<SGXString>, SGLHash<SGXString>>();
        SGXTimer::singleCall(0.0f, &SGDMCppParsing::processNextClass);
        return;
    }
    const SGXString currentFile = (*SGDMCppParsing::filesList).at(SGDMCppParsing::currentFileNumber);
    SGDMCppParsing::currentFileNumber++;
    SGDMResultsPage::updateInfo(SGXString("parsing ") + SGXString::intToString(SGDMCppParsing::currentFileNumber) + " of " + SGXString::intToString((*SGDMCppParsing::filesList).length()) + " files");
    
    SGXString fileContents = SGXFile::readAllText(currentFile);
    while(fileContents.findFirstFromLeft("class ") != -1){
        bool isForwardDeclared = false;
        int nextStart = 0;
        for(int i=fileContents.findFirstFromLeft("class "); i<fileContents.length(); i++){
            if(fileContents.at(i) == ';'){
                isForwardDeclared = true;
                nextStart = i+1;
                break;
            }
            if(fileContents.at(i) == '\n'){break;}
        }
        if(isForwardDeclared == false){
            const int startIndex = fileContents.findFirstFromRightCustomStart(SGXChar('\n'), fileContents.findFirstFromLeft("class "));
            int indentLevel = 0;
            int endIndex = 0;
            for(int i=startIndex; i<fileContents.length(); i++){
                if(fileContents.at(i) == '{'){indentLevel++;}
                else if(fileContents.at(i) == '}'){
                    indentLevel--;
                    if(indentLevel == 0){
                        endIndex = i;
                        nextStart = i + 1;
                        break;
                    }
                }
            }
            const SGXString classContents = fileContents.substring(startIndex + 1, endIndex - startIndex);
            if(classContents.substringLeft(classContents.findFirstFromLeft(SGXChar('{'))).count(SGXChar('<')) <= 1){(*SGDMCppParsing::classList).pushBack(SGLPair<SGXString, SGXString>(currentFile, classContents));}
        }
        fileContents = fileContents.substringRight(fileContents.length() - nextStart);
    }

    SGXTimer::singleCall(0.0f, &SGDMCppParsing::processNextFile);
}

void SGDMCppParsing::processNextClass(){
    if(SGDMCppParsing::currentClassNumber == (*SGDMCppParsing::classList).length()){
        SGDMResultsPage::updateInfo("done parsing C++ classes");
        delete SGDMCppParsing::classList;
        SGDMCppParsing::classList = nullptr;
        SGDMCppParsing::currentClassNumber = 0;
        SGXTimer::singleCall(0.0f, &SGDMCppParsing::linkInheritance);
        return;
    }
    const SGXString currentClass = (*SGDMCppParsing::classList).at(SGDMCppParsing::currentClassNumber).second;
    const SGXString currentClassFileName = (*SGDMCppParsing::classList).at(SGDMCppParsing::currentClassNumber).first;
    SGDMCppParsing::currentClassNumber++;
    SGDMResultsPage::updateInfo(SGXString("parsing ") + SGXString::intToString(SGDMCppParsing::currentClassNumber) + " of " + SGXString::intToString((*SGDMCppParsing::classList).length()) + " classes");
    
    SGDMCppClass* classStruct = new SGDMCppClass();
    (*classStruct).headerPath = currentClassFileName.substringRight(currentClassFileName.length() - SGDMDocumentationParsing::sourcePath.length());
    (*classStruct).sourcePath = (*classStruct).headerPath.replace("include", "src").replace(".h", ".cpp");
    if(SGXFileSystem::fileExists(SGDMDocumentationParsing::sourcePath + (*classStruct).sourcePath) == false){(*classStruct).sourcePath = "";}
    (*classStruct).moduleName = SGXFileSystem::getParentName(SGXFileSystem::getParentPath(currentClassFileName));
    const SGXString preimplementation = currentClass.substringLeft(currentClass.findFirstFromLeft(SGXChar('{')));
    SGLArray<SGXString> declarationElements = preimplementation.splitCustomSeparator(SGXChar(' '));
    int indexOfClassKeyword = 0;
    for(int i=0; i<declarationElements.length(); i++){
        if(declarationElements.at(i) == "class"){
            indexOfClassKeyword = i;
            break;
        }
    }
    if(declarationElements.at(indexOfClassKeyword + 1).contains("_DLL") == false){(*classStruct).className = declarationElements.at(indexOfClassKeyword + 1);}
    else{(*classStruct).className = declarationElements.at(indexOfClassKeyword + 2);}
    (*classStruct).className.removeLeadingTrailingWhitespace();
    (*classStruct).templateInfo = currentClass.substringLeft(currentClass.findFirstFromLeft("class "));
    (*classStruct).templateInfo.removeLeadingTrailingWhitespace();
    if(preimplementation.contains(SGXChar(':')) == false){(*classStruct).parentClass = "";}
    else{
        (*classStruct).parentClass = preimplementation.substringRight(preimplementation.length() - preimplementation.findFirstFromRight("public ") - 7);
        (*classStruct).parentClass.removeLeadingTrailingWhitespace();
    }
    (*SGDMCppClass::allClasses).insert((*classStruct).className, classStruct);
    
    const SGXString implementation = currentClass.substring(currentClass.findFirstFromLeft(SGXChar('{')) + 1, currentClass.findFirstFromRight(SGXChar('}')) - 1 - currentClass.findFirstFromLeft(SGXChar('{')));
    SGLArray<SGXString> members = implementation.splitCustomSeparator(SGXChar('\n'));
    const SGXString className = (*classStruct).className;
    SGLArray<SGXString> excluded(
        className + "(const " + className + "&",
        className + "(" + className + "&&",
        className + "& operator=(",
        SGXString("~") + className + "()",
        "public:",
        "protected:",
        "private:",
        "{",
        "}"
    );
    int index = -1;
    for(int i=0; i<members.length(); i++){
        members.at(i).removeLeadingTrailingWhitespace();
    }
    while(index < members.length()-1){
        index++;
        if(members.at(index).length() > 5 && members.at(index).substringLeft(5) == "enum "){
            SGXString enumString = members.at(index);
            while(members.at(index) != "};"){
                index++;
                enumString += (SGXString(" ") + members.at(index));
            }
            (*SGDMCppParsing::membersList).pushBack(SGLPair<SGXString, SGXString>(className, enumString));
            continue;
        }
        bool isExcluded = false;
        for(int i=0; i<excluded.length(); i++){
            if(members.at(index).length() >= excluded.at(i).length() && members.at(index).substringLeft(excluded.at(i).length()) == excluded.at(i)){isExcluded = true;}
        }
        if(isExcluded == true){continue;}
        if(members.at(index) == ""){continue;}
        if(members.at(index).length() > 3 && members.at(index).substringRight(3) == "{};"){members.at(index) = members.at(index).substringLeft(members.at(index).length() - 6) + ";";}
        (*SGDMCppParsing::membersList).pushBack(SGLPair<SGXString, SGXString>(className, members.at(index)));
    }
    
    SGXTimer::singleCall(0.0f, &SGDMCppParsing::processNextClass);
}

void SGDMCppParsing::linkInheritance(){
    SGDMResultsPage::updateInfo("processing inheritance hierarchy");
    for(SGLUnorderedMap<SGXString, SGDMCppClass*, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::Iterator i = (*SGDMCppClass::allClasses).begin(); i != (*SGDMCppClass::allClasses).end(); i++){
        if((*i.value()).parentClass != ""){
            (*(*SGDMCppClass::allClasses).at((*i.value()).parentClass)).childrenClass.pushBack(i.key());
        }
    }
}
