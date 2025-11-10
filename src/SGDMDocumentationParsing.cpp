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
#include <SGLSet.h>
#include <SGDMCppMember.h>
#include <SGLUnorderedSet.h>
#include <SGDMSgmlGeneration.h>

bool SGDMDocumentationParsing::isWritingHTML = false;
SGXString SGDMDocumentationParsing::sourcePath = "";
SGXString SGDMDocumentationParsing::destinationPath = "";
SGLVector<SGXString>* SGDMDocumentationParsing::docFilesList = nullptr;
int SGDMDocumentationParsing::currentDocFileNumber = 0;
SGLVector<SGDMCppClass*>* SGDMDocumentationParsing::verifyClassList = nullptr;
int SGDMDocumentationParsing::currentVerifyingClass = 0;
SGLUnorderedMap<SGXString, SGXString, SGLEqualsTo<SGXString>, SGLHash<SGXString>> SGDMDocumentationParsing::findReplaceDatabase;
SGLUnorderedSet<SGXString, SGLEqualsTo<SGXString>, SGLHash<SGXString>> SGDMDocumentationParsing::autoLinkIgnore;

void SGDMDocumentationParsing::findDocFiles(){
    SGDMResultsPage::updateInfo("searching for SGDoc files");
    SGDMDocumentationParsing::docFilesList = new SGLVector<SGXString>();
    SGLArray<SGXString> files = SGXFileSystem::getFilesListWithExtensionRecursive(SGDMDocumentationParsing::sourcePath + "SGDoc_source", "sgdoc");
    for(int i=0; i<files.length(); i++){
        (*SGDMDocumentationParsing::docFilesList).pushBack(files.at(i));
    }
    SGDMDocumentationParsing::currentDocFileNumber = 0;
    SGDMDocumentationParsing::findReplaceDatabase = SGLUnorderedMap<SGXString, SGXString, SGLEqualsTo<SGXString>, SGLHash<SGXString>>();
    SGDMDocumentationParsing::autoLinkIgnore = SGLUnorderedSet<SGXString, SGLEqualsTo<SGXString>, SGLHash<SGXString>>();
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
        const SGXString& currentDirective = list.at(i);
        currentDirective.cleanWhitespace();
        if(currentDirective == "" || currentDirective == " "){continue;}
        if(currentDirective.length() > 16 && currentDirective.substringLeft(16) == "@SG_PROJECTNAME "){
            SGDMSgmlGeneration::projectName = currentDirective.substringRight(currentDirective.length() - 16);
        }
        else if(currentDirective.length() > 17 && currentDirective.substringLeft(17) == "@SG_GITHUBPREFIX "){
            SGDMSgmlGeneration::githubPrefixLink = currentDirective.substringRight(currentDirective.length() - 17);
        }
        else if(currentDirective.length() > 15 && currentDirective.substringLeft(15) == "@SG_NOAUTOLINK "){
            SGDMDocumentationParsing::autoLinkIgnore.insert(currentDirective.substringRight(currentDirective.length() - 15));
        }
        else if(currentDirective.length() > 18 && currentDirective.substringLeft(18) == "@SG_FINDREPSOURCE "){
            SGDMDocumentationParsing::addFindReplacePair(currentDirective.substringRight(currentDirective.length() - 18));
        }
        else if(currentDirective.length() > 14 && currentDirective.substringLeft(14) == "@SG_CPP_CLASS "){
            const SGXString className = currentDirective.substringRight(currentDirective.length() - 14);
            if(SGDMCppClass::allClasses.contains(className) == false){SGDMResultsPage::addWarning(SGXString("Selecting nonexistent class ") + className);}
            else{currentClass = className;}
        }
        else if(currentDirective.length() > 17 && currentDirective.substringLeft(17) == "@SG_SHORT_DESCRP "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding short description to nonexistent class ") + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).briefDescription = SGDMDocumentationParsing::resolveSGDocDirectives(currentDirective.substringRight(currentDirective.length() - 17));}
        }
        else if(currentDirective.length() > 16 && currentDirective.substringLeft(16) == "@SG_LONG_DESCRP "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding long description to nonexistent class ") + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).detailedDescription = SGDMDocumentationParsing::resolveSGDocDirectives(currentDirective.substringRight(currentDirective.length() - 16));}
        }
        else if(currentDirective.length() > 10 && currentDirective.substringLeft(10) == "@SG_IMPLE "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding implementation info to nonexistent class ") + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).implementationDetails = SGDMDocumentationParsing::resolveSGDocDirectives(currentDirective.substringRight(currentDirective.length() - 10));}
        }
        else if(currentDirective.length() > 16 && currentDirective.substringLeft(16) == "@SG_PRIVATE_API "){
            const SGXString memberName = currentDirective.substringRight(currentDirective.length() - 16);
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Marking private member in nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(memberName) == false){SGDMResultsPage::addWarning(SGXString("Selecting nonexistent member ") + memberName + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(memberName).isPrivateAPI = true;}
        }
        else if(currentDirective.length() > 9 && currentDirective.substringLeft(9) == "@SG_FUNC "){
            const SGXString memberName = currentDirective.substringRight(currentDirective.length() - 9);
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Selecting member in nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(memberName) == false){SGDMResultsPage::addWarning(SGXString("Selecting nonexistent member ") + memberName + " in class " + currentClass);}
            else{currentMember = memberName;}
        }
        else if(currentDirective.length() > 9 && currentDirective.substringLeft(9) == "@SG_NOTE "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding note to nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(currentMember) == false){SGDMResultsPage::addWarning(SGXString("Adding note to nonexistent member ") + currentMember + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(currentMember).notes.pushBack(SGDMDocumentationParsing::resolveSGDocDirectives(currentDirective.substringRight(currentDirective.length() - 9)));}
        }
        else if(currentDirective.length() > 9 && currentDirective.substringLeft(9) == "@SG_WARN "){
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding warning to nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(currentMember) == false){SGDMResultsPage::addWarning(SGXString("Adding warning to nonexistent member ") + currentMember + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(currentMember).warnings.pushBack(SGDMDocumentationParsing::resolveSGDocDirectives(currentDirective.substringRight(currentDirective.length() - 9)));}
        }
        else{
            if(SGDMCppClass::allClasses.contains(currentClass) == false){SGDMResultsPage::addWarning(SGXString("Adding description to nonexistent class ") + currentClass);}
            else if(SGDMCppClass::allClasses.at(currentClass).members.contains(currentMember) == false){SGDMResultsPage::addWarning(SGXString("Adding description to nonexistent member ") + currentMember + " in class " + currentClass);}
            else{SGDMCppClass::allClasses.at(currentClass).members.at(currentMember).description.pushBack(SGDMDocumentationParsing::resolveSGDocDirectives(currentDirective));}
        }
    }
}

void SGDMDocumentationParsing::verifyClass(){
    SGDMResultsPage::updateInfo("preparing verification...");
    SGDMDocumentationParsing::verifyClassList = new SGLVector<SGDMCppClass*>();
    for(SGLUnorderedMap<SGXString, SGDMCppClass, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::Iterator i = SGDMCppClass::allClasses.begin(); i != SGDMCppClass::allClasses.end(); i++){
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
        SGXTimer::singleCall(0.0f, &SGDMSgmlGeneration::generateSgml);
        return;
    }
    SGDMCppClass& currentClass = (*(*SGDMDocumentationParsing::verifyClassList).at(SGDMDocumentationParsing::currentVerifyingClass));
    SGDMDocumentationParsing::currentVerifyingClass++;
    SGDMResultsPage::updateInfo(SGXString("verifying ") + SGXString::intToString(SGDMDocumentationParsing::currentVerifyingClass) + " of " + SGXString::intToString((*SGDMDocumentationParsing::verifyClassList).length()) + " C++ classes");
    
    if(currentClass.briefDescription == ""){SGDMResultsPage::addWarning(currentClass.className + " missing brief description");}
    if(currentClass.detailedDescription == ""){SGDMResultsPage::addWarning(currentClass.className + " missing detailed description");}
    if(currentClass.implementationDetails == ""){SGDMResultsPage::addWarning(currentClass.className + " missing implementation info");}
    for(SGLUnorderedMap<SGXString, SGDMCppMember, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::Iterator i = currentClass.members.begin(); i != currentClass.members.end(); i++){
        SGDMCppMember& currentMember = i.value();
        if(currentMember.isPrivateAPI == true){continue;}
        if(currentMember.fullDeclaration.at(currentMember.fullDeclaration.length() - 1) != ';'){SGDMResultsPage::addWarning(SGXString("improperly terminated declaration ") + currentMember.fullDeclaration + " in " + currentClass.className);}
        if(currentMember.description.length() == 0){SGDMResultsPage::addWarning(currentMember.fullDeclaration + " in " + currentClass.className + " missing description");}
        SGLVector<SGXString*> memberInfo;
        for(int j=0; j<currentMember.description.length(); j++){
            memberInfo.pushBack(&currentMember.description.at(j));
        }
        for(int j=0; j<currentMember.notes.length(); j++){
            memberInfo.pushBack(&currentMember.notes.at(j));
        }
        for(int j=0; j<currentMember.warnings.length(); j++){
            memberInfo.pushBack(&currentMember.warnings.at(j));
        }
        for(SGLSet<SGXString, CompareStringsByLength>::ConstIterator j = currentMember.parameters.constBegin(); j != currentMember.parameters.constEnd(); j++){
            bool parameterFound = false;
            const SGXString searchString = SGXString("$") + (*j);
            const SGXString replaceString = SGXString("@SG_ML_B") + SGXString::intToStringBase16((*j).length()).fillLeftToLength(2, '0') + (*j);
            for(int k=0; k<currentMember.description.length(); k++){
                if(currentMember.description.at(k).contains(searchString) == true){parameterFound = true;}
            }
            if(parameterFound == false){SGDMResultsPage::addWarning(SGXString("parameter ") + (*j) + " not found in docs of member " + currentMember.fullDeclaration + " of class " + currentClass.className);}
            for(int k=0; k<memberInfo.length(); k++){
                while((*memberInfo.at(k)).contains(searchString)){
                    (*memberInfo.at(k)).replace(searchString, replaceString);
                }
            }
        }
        for(int j=0; j<memberInfo.length(); j++){
            if((*memberInfo.at(j)).contains(SGXChar('$'))){SGDMResultsPage::addWarning(SGXString("parameter indicator token found in docs of member ") + currentMember.fullDeclaration + " of class " + currentClass.className + ":\n" + (*memberInfo.at(j)));}
        }
    }
    SGXTimer::singleCall(0.0f, &SGDMDocumentationParsing::verifyNextClass);
}

SGXString SGDMDocumentationParsing::resolveSGDocDirectives(const SGXString &directive){
    SGXString unresolvedDirective = directive;
    SGXString resolvedDirective = "";
    while(unresolvedDirective != ""){
        const int nextDirectiveIndex = unresolvedDirective.findFirstFromLeft("@SG_");
        if(nextDirectiveIndex == -1){
            resolvedDirective += unresolvedDirective;
            unresolvedDirective = "";
            continue;
        }
        if(nextDirectiveIndex != 0){
            resolvedDirective += unresolvedDirective.substringLeft(nextDirectiveIndex);
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - nextDirectiveIndex);
            continue;
        }
        int directiveEnd = unresolvedDirective.length();
        for(int i=1; i<unresolvedDirective.length(); i++){
            if(unresolvedDirective.at(i).isEnglishAlphanumeric() == false && unresolvedDirective.at(i) != '_'){
                directiveEnd = i;
                break;
            }
        }
        SGXString extractedDirective = unresolvedDirective.substringLeft(directiveEnd);
        unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - directiveEnd);
        if(extractedDirective.length() <= 12 || extractedDirective.substringLeft(12) != "@SG_FINDREP_"){SGDMResultsPage::addWarning(SGXString("cannot resolve SGDoc directive ") + extractedDirective + " because it is not find replace");}
        else{
            extractedDirective = extractedDirective.substringRight(extractedDirective.length() - 12);
            if(SGDMDocumentationParsing::findReplaceDatabase.contains(extractedDirective) == false){SGDMResultsPage::addWarning(SGXString("nonexistent find replace directive ") + extractedDirective);}
            else{
                resolvedDirective += SGDMDocumentationParsing::findReplaceDatabase.at(extractedDirective);
            }
        }
    }
    unresolvedDirective = resolvedDirective;
    resolvedDirective = "";
    while(unresolvedDirective != ""){
        const int index = unresolvedDirective.findFirstFromLeft("undefined behaviour");
        if(index == -1){
            resolvedDirective += unresolvedDirective;
            unresolvedDirective = "";
            continue;
        }
        if(index > 0){
            resolvedDirective += unresolvedDirective.substringLeft(index);
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - index);
            continue;
        }
        resolvedDirective += "@SG_ML_L131Fundefined behaviour../tutorials/undefinedbehaviour";
        unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - 19);
    }
    unresolvedDirective = resolvedDirective;
    resolvedDirective = "";
    while(unresolvedDirective != ""){
        const int index = unresolvedDirective.findFirstFromLeft("SG");
        if(index == -1){
            resolvedDirective += unresolvedDirective;
            unresolvedDirective = "";
            continue;
        }
        if(index > 0){
            resolvedDirective += unresolvedDirective.substringLeft(index);
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - index);
            continue;
        }
        if(unresolvedDirective.length() > 6 && unresolvedDirective.substringLeft(6) == "SG_ML_"){
            resolvedDirective += unresolvedDirective.substringLeft(6);
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - 6);
            continue;
        }
        int endIndex = unresolvedDirective.length();
        for(int i=0; i<unresolvedDirective.length(); i++){
            if(unresolvedDirective.at(i).isEnglishAlphanumeric() == false && unresolvedDirective.at(i)  != ':' && unresolvedDirective.at(i) != '_'){
                endIndex = i;
                break;
            }
        }
        const SGXString autoLinkTarget = unresolvedDirective.substringLeft(endIndex);
        unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - endIndex);
        if(SGDMDocumentationParsing::autoLinkIgnore.contains(autoLinkTarget) == true){
            resolvedDirective += autoLinkTarget;
            continue;
        }
        if(SGDMCppClass::allClasses.contains(autoLinkTarget) == true){
            resolvedDirective += (SGXString("@SG_ML_L") + SGXString::intToStringBase16(endIndex).fillLeftToLength(2, SGXChar('0')) + SGXString::intToStringBase16(endIndex).fillLeftToLength(2, SGXChar('0')) + autoLinkTarget + autoLinkTarget.getLowerLanguageAware().replace(SGXChar(':'), SGXChar('_')));
            continue;
        }
        if(autoLinkTarget.contains("::")){
            const SGXString className = autoLinkTarget.substringLeft(autoLinkTarget.findFirstFromRight("::"));
            const SGXString functionName = autoLinkTarget.substringRight(autoLinkTarget.length() - autoLinkTarget.findFirstFromRight("::") - 2);
            if(SGDMCppClass::allClasses.contains(className) == false){
                resolvedDirective += autoLinkTarget;
                SGDMResultsPage::addWarning(SGXString("cannot autolink to member ") + functionName + " of nonexistent class " + className);
                continue;
            }
            bool memberFound = false;
            for(SGLUnorderedMap<SGXString, SGDMCppMember, SGLEqualsTo<SGXString>, SGLHash<SGXString>>::ConstIterator i = SGDMCppClass::allClasses.at(className).members.constBegin(); i != SGDMCppClass::allClasses.at(className).members.constEnd(); i++){
                if(i.value().functionName == functionName){
                    const SGXString link = className.getLowerLanguageAware().replace(SGXChar(':'), SGXChar('_')) + "#" + i.value().normalisedSignature;
                    resolvedDirective += (SGXString("@SG_ML_L") + SGXString::intToStringBase16(endIndex).fillLeftToLength(2, SGXChar('0')) + SGXString::intToStringBase16(link.length()).fillLeftToLength(2, SGXChar('0')) + autoLinkTarget + link);
                    memberFound = true;
                }
            }
            if(memberFound == true){continue;}
            resolvedDirective += autoLinkTarget;
            SGDMResultsPage::addWarning(SGXString("cannot autolink to nonexistent member ") + functionName + " in class " + className);
            continue;
        }
        resolvedDirective += autoLinkTarget;
        SGDMResultsPage::addWarning(SGXString("cannot unresolve autolink target: ") + autoLinkTarget);
    }
    return resolvedDirective;
}

void SGDMDocumentationParsing::addFindReplacePair(const SGXString &findReplaceDirective){
    const int x = findReplaceDirective.findFirstFromLeft(SGXChar(' '));
    SGDMDocumentationParsing::findReplaceDatabase.insert(findReplaceDirective.substringLeft(x), findReplaceDirective.substringRight(findReplaceDirective.length() - x - 1));
}
