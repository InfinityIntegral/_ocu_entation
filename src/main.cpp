#include <SGEXTN_EntryPoint.h>
#include <SGDMUserInterface.h>
#include <SGXString.h>
#include <SGXCentral.h>
#include <SGDMResultsPage.h>

namespace{
void app_init(){
    SGXCentral::applicationName = "_ocu_entation";
    SGXCentral::applicationVersion = "v1.0.0";
    SGXCentral::organisationName = "05524F.sg (Singapore)";
    SGXCentral::folderName = "_ocu_entation";
    SGXCentral::customInitialise = &SGDMUserInterface::initialise;
}
}

int main(int argc, char** argv){
    SGEXTN(argc, argv, &app_init);
}
