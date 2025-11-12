#ifndef SGDMHTMLGENERATION_H
#define SGDMHTMLGENERATION_H

class SGXString;
template <typename T> class SGLVector;
class SGDMHtmlGeneration {
public:
    SGDMHtmlGeneration() = delete;
    static void generateHtml();
    static void generateHtmlForNextClass();
    static SGXString convertSgmlToHtml(const SGXString& sgml, const SGXString& filePath);
    static SGLVector<SGXString>* sgmlFiles;
    static int currentSgmlFile;
    static SGXString resolveInlineSgmlDirectives(const SGXString& sgml);
};

#endif // SGDMHTMLGENERATION_H
