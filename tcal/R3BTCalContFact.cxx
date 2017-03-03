/////////////////////////////////////////////////////////////
//
//  R3BTCalContFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BTCalContFact.h"

#include "R3BTCalPar.h"    // for R3BLandGeometryPar
#include "FairParSet.h"    // for FairParSet
#include "FairRuntimeDb.h" // for FairRuntimeDb
#include "FairLogger.h"

#include "TList.h"   // for TList
#include "TString.h" // for TString

#include <string.h> // for strcmp, NULL

using namespace std;

ClassImp(R3BTCalContFact);

static R3BTCalContFact gR3BTCalContFact;

R3BTCalContFact::R3BTCalContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BTCalContFact";
    fTitle = "Tutorial factory for parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BTCalContFact::addContainer(TString name, TString description)
{
	auto container = new FairContainer(name, description,
	    "TestDefaultContext");
	container->addContext("TestNonDefaultContext");
	containers->Add(container);
}

void R3BTCalContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds
     *  them to the list of containers.*/
    addContainer("LandTCalPar", "NeuLAND TCAL Calibration Parameters");
    addContainer("LosTCalPar", "LOS TCAL Calibration Parameters");
    addContainer("TofdTCalPar", "TOFD TCAL Calibration Parameters");
    addContainer("StrawtubesTCalPar", "Strawtubes TCAL Calibration Parameters");
    addContainer("Fi1TCalPar", "FI1 TCAL Calibration Parameters");
    addContainer("Fi5TCalPar", "FI5 TCAL Calibration Parameters");
    addContainer("Fi6TCalPar", "FI6 TCAL Calibration Parameters");
    addContainer("PtofTCalPar", "PTOF TCAL Calibration Parameters");
}

FairParSet* R3BTCalContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    LOG(INFO) << "R3BTCalContFact::createContainer : " << name
        << FairLogger::endl;

    vector<const char *> containerNames;
    containerNames.push_back("LandTCalPar");
    containerNames.push_back("LosTCalPar");
    containerNames.push_back("TofdTCalPar");
    containerNames.push_back("StrawtubesTCalPar");
    containerNames.push_back("Fi1TCalPar");
    containerNames.push_back("Fi5TCalPar");
    containerNames.push_back("Fi6TCalPar");
    containerNames.push_back("PtofTCalPar");

    bool found = false;
    for (auto containerName : containerNames) {
        if (strncmp(name, containerName, strlen(containerName)) == 0) {
            found = true;
            break;
        }
    }

    if (found == true) {
        return new R3BTCalPar(c->getConcatName().Data(), c->GetTitle(),
            c->getContext());
    } else {
        return nullptr;
    }
}
