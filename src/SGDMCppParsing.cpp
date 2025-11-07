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
#include <SGDMCppMember.h>

int SGDMCppParsing::currentFileNumber = 0;
SGLVector<SGXString>* SGDMCppParsing::filesList = nullptr;
SGLVector<SGLPair<SGXString, SGXString>>* SGDMCppParsing::classList = nullptr;
int SGDMCppParsing::currentClassNumber = 0;
SGLVector<SGLPair<SGXString, SGXString>>* SGDMCppParsing::membersList = nullptr;
int SGDMCppParsing::currentMemberNumber = 0;

void SGDMCppParsing::startParsingOperation(){
    SGDMResultsPage::updateInfo("searching for C++ files");
    SGDMCppParsing::getFilesList();
}

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
        SGDMCppClass::allClasses = SGLUnorderedMap<SGXString, SGDMCppClass, SGLEqualsTo<SGXString>, SGLHash<SGXString>>();
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
            else{
                SGXString preimplementation = classContents.substringLeft(classContents.findFirstFromLeft(SGXChar('{')));
                if(preimplementation.contains("Iterator")){
                    const SGXString implementation = classContents.substringRight(classContents.length() - preimplementation.length());
                    preimplementation = preimplementation.substringLeft(preimplementation.findFirstFromRight(SGXChar('<'))) + preimplementation.substringRight(preimplementation.length() - preimplementation.findFirstFromRight(SGXChar('>')) - 1);
                    (*SGDMCppParsing::classList).pushBack(SGLPair<SGXString, SGXString>(currentFile, preimplementation + implementation));
                }
            }
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
    
    SGDMCppClass classStruct;
    classStruct.headerPath = currentClassFileName.substringRight(currentClassFileName.length() - SGDMDocumentationParsing::sourcePath.length());
    classStruct.sourcePath = classStruct.headerPath.replace("include", "src").replace(".h", ".cpp");
    if(SGXFileSystem::fileExists(SGDMDocumentationParsing::sourcePath + classStruct.sourcePath) == false){classStruct.sourcePath = "";}
    classStruct.moduleName = SGXFileSystem::getParentName(SGXFileSystem::getParentPath(currentClassFileName));
    const SGXString preimplementation = currentClass.substringLeft(currentClass.findFirstFromLeft(SGXChar('{')));
    SGLArray<SGXString> declarationElements = preimplementation.splitCustomSeparator(SGXChar(' '));
    int indexOfClassKeyword = 0;
    for(int i=0; i<declarationElements.length(); i++){
        if(declarationElements.at(i) == "class"){
            indexOfClassKeyword = i;
            break;
        }
    }
    if(declarationElements.at(indexOfClassKeyword + 1).contains("_DLL") == false){classStruct.className = declarationElements.at(indexOfClassKeyword + 1);}
    else{classStruct.className = declarationElements.at(indexOfClassKeyword + 2);}
    classStruct.className.cleanWhitespace();
    classStruct.templateInfo = currentClass.substringLeft(currentClass.findFirstFromLeft("class "));
    classStruct.templateInfo.cleanWhitespace();
    if(preimplementation.contains(" : ") == false){classStruct.parentClass = "";}
    else{
        classStruct.parentClass = preimplementation.substringRight(preimplementation.length() - preimplementation.findFirstFromRight("public ") - 7);
        classStruct.parentClass.cleanWhitespace();
    }
    classStruct.briefDescription = "";
    classStruct.detailedDescription = "";
    classStruct.implementationDetails = "";
    SGDMCppClass::allClasses.insert(classStruct.className, classStruct);
    
    const SGXString implementation = currentClass.substring(currentClass.findFirstFromLeft(SGXChar('{')) + 1, currentClass.findFirstFromRight(SGXChar('}')) - 1 - currentClass.findFirstFromLeft(SGXChar('{')));
    SGLArray<SGXString> members = implementation.splitCustomSeparator(SGXChar('\n'));
    const SGXString className = classStruct.className;
    SGLArray<SGXString> excluded(
        className + "(const " + className + "&",
        className + "(" + className + "&&",
        className + "& operator=(",
        SGXString("~") + className + "()",
        "public:",
        "protected:",
        "private:",
        "{",
        "}",
        "class ",
        "friend "
    );
    int index = -1;
    for(int i=0; i<members.length(); i++){
        members.at(i).cleanWhitespace();
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
        if(members.at(index).length() >= 11 && members.at(index).substringRight(11) == " = default;"){isExcluded = true;}
        if(members.at(index).length() >= 10 && members.at(index).substringRight(10) == " = delete;"){isExcluded = true;}
        if(isExcluded == true){continue;}
        if(members.at(index) == ""){continue;}
        if(members.at(index).length() > 3 && members.at(index).substringRight(3) == "{};"){members.at(index) = members.at(index).substringLeft(members.at(index).length() - 6) + ";";}
        (*SGDMCppParsing::membersList).pushBack(SGLPair<SGXString, SGXString>(className, members.at(index)));
    }
    
    SGXTimer::singleCall(0.0f, &SGDMCppParsing::processNextClass);
}

void SGDMCppParsing::linkInheritance(){
    SGDMResultsPage::updateInfo("processing inheritance hierarchy");
    for(SGLUnorderedMap<SGXString, SGDMCppClass, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::Iterator i = SGDMCppClass::allClasses.begin(); i != SGDMCppClass::allClasses.end(); i++){
        if(i.value().parentClass != ""){
            SGDMCppClass::allClasses.at(i.value().parentClass).childrenClass.pushBack(i.key());
        }
    }
    SGXTimer::singleCall(0.0f, &SGDMCppParsing::processNextMember);
}

void SGDMCppParsing::processNextMember(){
    if(SGDMCppParsing::currentMemberNumber == (*SGDMCppParsing::membersList).length()){
        SGDMResultsPage::updateInfo("done parsing C++ members");
        delete SGDMCppParsing::membersList;
        SGDMCppParsing::membersList = nullptr;
        SGDMCppParsing::currentMemberNumber = 0;
        SGXTimer::singleCall(0.0f, &SGDMDocumentationParsing::findDocFiles);
        return;
    }
    
    const SGXString currentClass = (*SGDMCppParsing::membersList).at(SGDMCppParsing::currentMemberNumber).first;
    const SGXString currentMember = (*SGDMCppParsing::membersList).at(SGDMCppParsing::currentMemberNumber).second;
    SGDMCppParsing::currentMemberNumber++;
    SGDMResultsPage::updateInfo(SGXString("parsing ") + SGXString::intToString(SGDMCppParsing::currentMemberNumber) + " of " + SGXString::intToString((*SGDMCppParsing::membersList).length()) + " members");
    currentMember.cleanWhitespace();
    
    SGDMCppMember memberStruct;
    memberStruct.fullDeclaration = currentMember;
    memberStruct.isPrivateAPI = false;
    memberStruct.description = SGLVector<SGXString>();
    memberStruct.warnings = SGLVector<SGXString>();
    memberStruct.notes = SGLVector<SGXString>();
    if(currentMember.contains("static ") == true){memberStruct.isStatic = true;}
    else{memberStruct.isStatic = false;}
    if(currentMember.contains(" = 0;") == true){memberStruct.isPureVirtual = true;}
    else{memberStruct.isPureVirtual = false;}
    if(currentMember.findFirstFromLeft(SGXChar('(')) > 0 && currentMember.at(currentMember.findFirstFromLeft(SGXChar('(')) - 1) != ' '){memberStruct.isFunction = true;}
    else{memberStruct.isFunction = false;}
    SGXString s = "";
    SGLVector<SGXString> identifiers;
    for(int i=0; i<currentMember.length(); i++){
        if(currentMember.at(i).isEnglishAlphanumeric() == true){s += currentMember.at(i);}
        else{
            if(s.length() >= 8 && s.substringLeft(8) == "operator" && (
                (currentMember.at(i) != '(' && currentMember.at(i) != ')' && currentMember.at(i) != ' ') || ((s.contains("()") == false && currentMember.at(i) == '(' && currentMember.at(i + 1) == ')') || (s.contains("()") == false && currentMember.at(i - 1) == '(' && currentMember.at(i) == ')')))){
                s += currentMember.at(i);
                continue;
            }
            if(s != ""){identifiers.pushBack(s);}
            s = "";
        }
    }
    for(int i=0; i<identifiers.length(); i++){
        if(isCppKeyword(identifiers.at(i)) == false){
            memberStruct.functionName = identifiers.at(i);
            break;
        }
    }
    if(currentMember.length() > currentClass.length() + 1 && currentMember.substringLeft(currentClass.length() + 1) == currentClass + "("){memberStruct.functionName = currentClass;}
    s = "";
    SGXString signature = "";
    for(int i=0; i<currentMember.length(); i++){
        if(currentMember.at(i).isEnglishAlphanumeric() == true){s += currentMember.at(i);}
        else if(s != ""){
            if(SGDMCppParsing::isCppKeyword(s) == true || s == memberStruct.functionName){signature += (s + "_");}
            s = "";
        }
        if(currentMember.at(i) == '*'){signature += "ptr_";}
        else if(currentMember.at(i) == '&'){signature += "ref_";}
        else if(currentMember.at(i) == '(' && currentMember.at(i+1) == ')'){
            i++;
            signature += "func_";
        }
        else if(currentMember.at(i) == ':' && currentMember.at(i+1) == ':'){
            i++;
            signature += "mc_";
        }
        else if(currentMember.at(i) == '+' && currentMember.at(i+1) == '+'){
            i++;
            signature += "increment_";
        }
        else if(currentMember.at(i) == '-' && currentMember.at(i+1) == '-'){
            i++;
            signature += "decrement_";
        }
        else if(currentMember.at(i) == '=' && currentMember.at(i+1) == '='){
            i++;
            signature += "eq_";
        }
        else if(currentMember.at(i) == '!' && currentMember.at(i+1) == '='){
            i++;
            signature += "neq_";
        }
        else if(currentMember.at(i) == '<' && currentMember.at(i+1) == '='){
            i++;
            signature += "leq_";
        }
        else if(currentMember.at(i) == '>' && currentMember.at(i+1) == '='){
            i++;
            signature += "geq_";
        }
        else if(currentMember.at(i) == '+'){signature += "plus_";}
        else if(currentMember.at(i) == '-'){signature += "minus_";}
        else if(currentMember.at(i) == '='){signature += "assign_";}
        else if(currentMember.at(i) == '<'){signature += "lt_";}
        else if(currentMember.at(i) == '>'){signature += "gt_";}
        else if(currentMember.at(i) == '/'){signature += "div_";}
    }
    memberStruct.normalisedSignature = signature;
    SGDMCppClass::allClasses.at(currentClass).members.insert(memberStruct.fullDeclaration, memberStruct);
    
    SGXTimer::singleCall(0.0f, &SGDMCppParsing::processNextMember);
}

bool SGDMCppParsing::isCppKeyword(const SGXString &s){
    if(s.at(0).isEnglishUppercase() == true){return true;}
    if(s == "nodiscard"){return true;}
    if(s == "bool"){return true;}
    if(s == "char"){return true;}
    if(s == "const"){return true;}
    if(s == "double"){return true;}
    if(s == "enum"){return true;}
    if(s == "float"){return true;}
    if(s == "friend"){return true;}
    if(s == "inline"){return true;}
    if(s == "int"){return true;}
    if(s == "long"){return true;}
    if(s == "operator"){return true;}
    if(s == "short"){return true;}
    if(s == "signed"){return true;}
    if(s == "static"){return true;}
    if(s == "struct"){return true;}
    if(s == "unsigned"){return true;}
    if(s == "virtual"){return true;}
    if(s == "void"){return true;}
    return false;
}
