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

#include "Riosfwd.h" // for ostream
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

void R3BTCalContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers.*/

    FairContainer* p1 = new FairContainer("LandTCalPar", "NeuLAND TCAL Calibration Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("LosTCalPar", "LOS TCAL Calibration Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);

    FairContainer* p3 = new FairContainer("TofdTCalPar", "TOFD TCAL Calibration Parameters", "TestDefaultContext");
    p3->addContext("TestNonDefaultContext");
    containers->Add(p3);
}

FairParSet* R3BTCalContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    LOG(INFO) << "R3BTCalContFact::createContainer : " << name << FairLogger::endl;
    FairParSet* p = NULL;

    if (strcmp(name, "LandTCalPar") == 0)
    {
        p = new R3BTCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "LosTCalPar") == 0)
    {
        p = new R3BTCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "TofdTCalPar") == 0)
    {
        p = new R3BTCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return p;
}
