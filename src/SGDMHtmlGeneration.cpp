#include <SGDMHtmlGeneration.h>
#include <SGXString.h>
#include <SGDMResultsPage.h>
#include <SGLVector.h>
#include <SGLArray.h>
#include <SGXFileSystem.h>
#include <SGDMDocumentationParsing.h>
#include <SGXTimer.h>
#include <SGXFile.h>
#include <SGDMSgmlGeneration.h>
#include <SGXChar.h>

SGLVector<SGXString>* SGDMHtmlGeneration::sgmlFiles = nullptr;
int SGDMHtmlGeneration::currentSgmlFile = 0;

void SGDMHtmlGeneration::generateHtml(){
    SGDMResultsPage::updateInfo("preparing HTML generation");
    SGDMHtmlGeneration::sgmlFiles = new SGLVector<SGXString>();
    SGLArray<SGXString> filesInFileSystem = SGXFileSystem::getFilesListWithExtensionRecursive(SGDMDocumentationParsing::sourcePath + "SGML_source", "sgml");
    for(int i=0; i<filesInFileSystem.length(); i++){
        (*SGDMHtmlGeneration::sgmlFiles).pushBack(filesInFileSystem.at(i).substringRight(filesInFileSystem.at(i).length() - SGDMDocumentationParsing::sourcePath.length()));
    }
    SGDMHtmlGeneration::currentSgmlFile = 0;
    if(SGXFileSystem::folderExists(SGDMDocumentationParsing::destinationPath) == false){SGXFileSystem::createFolder(SGDMDocumentationParsing::destinationPath);}
    SGLArray<SGXString> sourceFolders = SGXFileSystem::getFoldersList(SGDMDocumentationParsing::sourcePath + "SGML_source");
    for(int i=0; i<sourceFolders.length(); i++){
        const SGXString subfolder = SGXFileSystem::joinFilePaths(SGDMDocumentationParsing::destinationPath, sourceFolders.at(i).substringRight(sourceFolders.at(i).length() - SGDMDocumentationParsing::sourcePath.length() - 11));
        if(SGXFileSystem::folderExists(subfolder) == false){SGXFileSystem::createFolder(subfolder);}
    }
    SGXTimer::singleCall(0.0f, &SGDMHtmlGeneration::generateHtmlForNextClass);
}

void SGDMHtmlGeneration::generateHtmlForNextClass(){
    if(SGDMHtmlGeneration::currentSgmlFile == (*SGDMHtmlGeneration::sgmlFiles).length()){
        SGDMResultsPage::updateInfo("done generating HTML");
        delete SGDMHtmlGeneration::sgmlFiles;
        SGDMHtmlGeneration::sgmlFiles = nullptr;
        SGDMHtmlGeneration::currentSgmlFile = 0;
        SGDMResultsPage::showExitButton();
        return;
    }
    const SGXString currentFilePath = (*SGDMHtmlGeneration::sgmlFiles).at(SGDMHtmlGeneration::currentSgmlFile);
    SGDMHtmlGeneration::currentSgmlFile++;
    SGDMResultsPage::updateInfo(SGXString("generating ") + SGXString::intToString(SGDMHtmlGeneration::currentSgmlFile) + " of " + SGXString::intToString((*SGDMHtmlGeneration::sgmlFiles).length()) + " HTML files");
    const SGXString fileContents = SGDMHtmlGeneration::convertSgmlToHtml(SGXFile::readAllText(SGDMDocumentationParsing::sourcePath + currentFilePath), currentFilePath);
    SGXFile::writeAllText(SGDMDocumentationParsing::destinationPath + currentFilePath.substringRight(currentFilePath.length() - 11).replace("sgml", "html"), fileContents);
    SGXTimer::singleCall(0.0f, &SGDMHtmlGeneration::generateHtmlForNextClass);
}

SGXString SGDMHtmlGeneration::convertSgmlToHtml(const SGXString &sgml, const SGXString& filePath){
    SGXString output = "";
    SGXString pageName = "";
    if(sgml.contains("@SG_ML_PAGENAME ")){
        const int index = sgml.findFirstFromLeft("@SG_ML_PAGENAME ") + 16;
        const int endIndex = sgml.findFirstFromLeftCustomStart(SGXChar('\n'), index);
        pageName = sgml.substring(index, endIndex - index);
    }
    else{SGDMResultsPage::addWarning(filePath + " has no page name specified");}
    pageName = SGXString("<title>") + pageName + "</title>";
    output += R"RAW_STRING(
<!DOCTYPE html>
<html lang="en-SG">
<head>
<meta charset="UTF-8">
)RAW_STRING";
    output += pageName;
    output += R"RAW_STRING(
<link rel="icon" href="/icon.png" type="image/png">
<meta property="og:image" content="/icon.png">
<meta name="twitter:image" content="/icon.png">
<link rel="apple-touch-icon" href="/icon.png">
<script type="application/ld+json">
{
    "@context": "https://schema.org",
    "@type": "WebSite",
    "url": "https://infinityintegral.github.io",
    "image": "/icon.png"
}
</script>
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
@font-face {
	font-family: "SingScript.sg";
	src: url("/SingScript.sg")format("truetype");
	font-weight: normal;
	font-style: normal;
}

:root {
	--c0: rgb(0, 0, 0);
	--c1: rgb(64, 0, 50);
	--c2: rgb(128, 0, 100);
	--c3: rgb(191, 0, 150);
	--c4: rgb(255, 0, 200);
	--c5: rgb(255, 64, 214);
	--c6: rgb(255, 128, 227);
	--c7: rgb(255, 191, 241);
	--c8: rgb(255, 255, 255);
}

*{
    font-family: "SingScript.sg";
	max-width: 100%;
	word-break: break-word;
	-webkit-text-size-adjust: 100%;
	-ms-text-size-adjust: 100%;
}

.title {
	color: var(--c4);
	font-size: 3.5rem;
	margin: 0;
	font-weight: normal;
}
.title::selection {
	color: var(--c8);
	background-color: var(--c4);
	font-size: 3.5rem;
	margin: 0;
	font-weight: normal;
}

.header {
	color: var(--c4);
	font-size: 2.5rem;
	margin: 0;
	font-weight: normal;
}
.header::selection {
	color: var(--c8);
	background-color: var(--c4);
	font-size: 2.625rem;
	margin: 0;
	font-weight: normal;
}

.description {
	color: var(--c4);
	font-size: 1.75rem;
	margin: 0;
	white-space: pre-wrap;
}
.description::selection {
	color: var(--c8);
	background-color: var(--c4);
	font-size: 1.75rem;
	margin: 0;
	white-space: pre-wrap;
}

.note {
	color: var(--c4);
	font-size: 1.75rem;
	margin: 0.25rem;
	white-space: pre-wrap;
    border: 0.25rem solid var(--c7);
    padding: 0.25rem;
}
.note::selection {
	color: var(--c8);
	background-color: var(--c4);
	font-size: 1.75rem;
	margin: 0.25rem;
	white-space: pre-wrap;
    border: 0.25rem solid var(--c7);
    padding: 0.25rem;
}

.warning {
	color: var(--c4);
	font-size: 1.75rem;
	margin: 0.25rem;
	white-space: pre-wrap;
    border: 0.25rem solid var(--c4);
    padding: 0.25rem;
}
.warning::selection {
	color: var(--c8);
	background-color: var(--c4);
	font-size: 1.75rem;
	margin: 0.25rem;
	white-space: pre-wrap;
    border: 0.25rem solid var(--c4);
    padding: 0.25rem;
}

.link {
	color: var(--c2);
	text-decoration: underline;
}
.link:hover {
	color: var(--c2);
	background-color: var(--c7);
	text-decoration: underline;
}
.link:active {
	color: var(--c2);
	background-color: var(--c6);
	text-decoration: underline;
}
.link::selection {
	color: var(--c8);
	background-color: var(--c4);
	text-decoration: underline;
}

.button {
	color: var(--c8);
	background-color: var(--c4);
	font-size: 1.75rem;
	border: none;
}
.button:hover {
	color: var(--c7);
	background-color: var(--c3);
	font-size: 1.75rem;
	border: none;
}
.button:active {
	color: var(--c6);
	background-color: var(--c2);
	font-size: 1.75rem;
	border: none;
}
.button::selection {
	color: var(--c4);
	background-color: var(--c8);
	font-size: 1.75rem;
	border: none;
}

.input {
	color: var(--c8);
	background-color: var(--c4);
	font-size: 1.75rem;
	border: none;
	outline: none;
	text-align: center;
}
.input:hover {
	color: var(--c7);
	background-color: var(--c3);
	font-size: 1.75rem;
	border: none;
	outline: none;
	text-align: center;
}
.input:focus {
	color: var(--c6);
	background-color: var(--c2);
	font-size: 1.75rem;
	border: none;
	outline: none;
	text-align: center;
}
.input::selection {
	color: var(--c4);
	background-color: var(--c8);
	font-size: 1.75rem;
	border: none;
	outline: none;
	text-align: center;
}

.highlight {
    animation: highlightFlash 2s ease-out;
}

@keyframes highlightFlash {
    0% {background-color: var(--c6);}
    100% {background-color: var(--c8);}
}
</style>
<script>
let m = (new Date()).getMonth() + 1;
if(m >= 7 && m <= 9){
    document.documentElement.style.setProperty("--c0", "rgb(0, 0, 0)");
    document.documentElement.style.setProperty("--c1", "rgb(60, 9, 14)");
    document.documentElement.style.setProperty("--c2", "rgb(119, 19, 27)");
    document.documentElement.style.setProperty("--c3", "rgb(179, 28, 41)");
    document.documentElement.style.setProperty("--c4", "rgb(238, 37, 54)");
    document.documentElement.style.setProperty("--c5", "rgb(242, 92, 104)");
    document.documentElement.style.setProperty("--c6", "rgb(247, 146, 155)");
    document.documentElement.style.setProperty("--c7", "rgb(251, 201, 205)");
    document.documentElement.style.setProperty("--c8", "rgb(255, 255, 255)");
}
function highlightSection(){
    let tag = window.location.hash.substring(1);
    if(tag == ""){return;}
    let element = document.getElementById(tag);
    if(element == null){return;}
    element.classList.add("highlight");
    element.addEventListener("animationend", () => {element.classList.remove("highlight");}, {once: true});
}
window.addEventListener("DOMContentLoaded", highlightSection);
window.addEventListener("hashchange", highlightSection);
</script>
</head>
<body style="margin-left: 1em;">
<p class="description" style="color: rgb(238, 37, 54); text-align: center; font-size: 2.5rem">sincerely <img src="/flag.png" style="vertical-align: middle; height: 2.5rem; image-rendering: auto;"> Singaporean</p>
<div style="width: 100%; height: 0.25rem; background-color: var(--c8);"></div>
<div><button class="button" style="width: 100%;" onclick="location.href='/'">Project 05524F.sg</button></div>
<div style="width: 100%; height: 0.25rem; background-color: var(--c8);"></div>
)RAW_STRING";
    if(SGDMSgmlGeneration::projectName != ""){
        output += SGXString("<div><button class=\"button\" style=\"width: 100%;\" onclick=\"location.href='../readme/readme'\">") + SGDMSgmlGeneration::projectName + " Documentation</button></div>";
        output += "<div style=\"width: 100%; height: 0.25rem; background-color: var(--c8);\"></div>";
    }
    if(SGDMSgmlGeneration::projectName == "SGEXTN"){
        output += "<p class=\"warning\">If you have not done so, read this full <a class=\"link\" href=\"../tutorials/full\">tutorial</a> on how to use SGEXTN to build an application.</p>";
        output += "<div style=\"width: 100%; height: 0.75rem; background-color: var(--c8);\"></div>";
    }
    
    SGLArray<SGXString> directives = sgml.splitCustomSeparator(SGXChar('\n'));
    for(int i=0; i<directives.length(); i++){
        SGXString currentDirective = directives.at(i);
        if((currentDirective.length() > 16 && currentDirective.substringLeft(16) == "@SG_ML_PAGENAME ") || (currentDirective.length() > 15 && currentDirective.substringLeft(15) == "@SG_ML_COMMENT ")){}
        else if(currentDirective == "@SG_ML_BLANKLINE"){output += "<div style=\"width: 100%; height: 1.75rem; background-color: var(--c8);\"></div>\n";}
        else if(currentDirective.length() > 13 && currentDirective.substringLeft(13) == "@SG_ML_TITLE "){output += SGXString("<h1 class=\"title\">") + SGDMHtmlGeneration::resolveInlineSgmlDirectives(currentDirective.substringRight(currentDirective.length() - 13)) + "</h1>\n";}
        else if(currentDirective.length() > 14 && currentDirective.substringLeft(14) == "@SG_ML_HEADER_"){
            const int spaceIndex = currentDirective.findFirstFromLeft(SGXChar(' '));
            const SGXString tag = currentDirective.substring(14, spaceIndex - 14);
            currentDirective = currentDirective.substringRight(currentDirective.length() - spaceIndex - 1);
            output += SGXString("<h2 class=\"header\" id=\"") + tag + "\">" +  SGDMHtmlGeneration::resolveInlineSgmlDirectives(currentDirective) + "</h2>\n";
        }
        else if(currentDirective.length() > 12 && currentDirective.substringLeft(12) == "@SG_ML_NOTE "){output += SGXString("<p class=\"note\">") + SGDMHtmlGeneration::resolveInlineSgmlDirectives(currentDirective.substringRight(currentDirective.length() - 12)) + "</p>\n";}
        else if(currentDirective.length() > 12 && currentDirective.substringLeft(12) == "@SG_ML_WARN "){output += SGXString("<p class=\"warning\">") + SGDMHtmlGeneration::resolveInlineSgmlDirectives(currentDirective.substringRight(currentDirective.length() - 12)) + "</p>\n";}
        else{output += SGXString("<p class=\"description\">") + SGDMHtmlGeneration::resolveInlineSgmlDirectives(currentDirective) + "</p>\n";}
    }
    
    output += R"RAW_STRING(
<div style="width: 100%; height: 1.75rem; background-color: var(--c8);"></div>
<div style="width: 100%; height: 0.25rem; background-color: var(--c4);"></div>
<div style="width: 100%; height: 0.25rem; background-color: var(--c8);"></div>
<p class="description">©2025 05524F.sg (Singapore)</p>
<p class="description"><a class="link" href="/contact">contact 05524F / report a bug / make a suggestion</a></p>
<p class="description"><a class="link" href="/values">about 05524F SINGAPORE values</a></p>
<p class="description"><a class="link" href="/projects">list of 05524F projects</a></p>
<div style="width: 100%; height: 0.25rem; background-color: var(--c8);"></div>
<div style="width: 100%; height: 0.25rem; background-color: var(--c4);"></div>
<div style="width: 100%; height: 0.25rem; background-color: var(--c8);"></div>
</body>
</html>
)RAW_STRING";
    return output;
}

SGXString SGDMHtmlGeneration::resolveInlineSgmlDirectives(const SGXString &sgml){
    SGXString unresolvedDirective = sgml;
    SGXString resolvedDirective = "";
    while(unresolvedDirective != ""){
        const int nextDirectiveIndex = unresolvedDirective.findFirstFromLeft("@SG_");
        if(nextDirectiveIndex == -1){
            resolvedDirective += unresolvedDirective;
            unresolvedDirective = "";
            continue;
        }
        if(nextDirectiveIndex > 0){
            resolvedDirective += unresolvedDirective.substringLeft(nextDirectiveIndex);
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - nextDirectiveIndex);
            continue;
        }
        if(unresolvedDirective.substringLeft(8) == "@SG_ML_B"){
            const int boldLength = unresolvedDirective.substring(8, 3).parseToIntBase16(nullptr);
            const SGXString boldText = SGXString("<b>") + unresolvedDirective.substring(11, boldLength) + "</b>";
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - boldLength - 11);
            resolvedDirective += boldText;
        }
        else if(unresolvedDirective.substringLeft(8) == "@SG_ML_L"){
            const int textLength = unresolvedDirective.substring(8, 3).parseToIntBase16(nullptr);
            const int linkLength = unresolvedDirective.substring(11, 3).parseToIntBase16(nullptr);
            const SGXString linkText = SGXString("<a class=\"link\" href=\"") + unresolvedDirective.substring(14 + textLength, linkLength) + "\">" + unresolvedDirective.substring(14, textLength) + "</a>";
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - textLength - linkLength - 14);
            resolvedDirective += linkText;
        }
        else{
            resolvedDirective += unresolvedDirective.substringLeft(8);
            unresolvedDirective = unresolvedDirective.substringRight(unresolvedDirective.length() - 8);
            SGDMResultsPage::addWarning(SGXString("cannot resolve SGML directive") + unresolvedDirective.substringLeft(8));
            continue;
        }
    }
    return resolvedDirective;
}
