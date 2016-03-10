/////////////////////////////////////////////////////////////
//
//  R3BNeulandParFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BNeulandParFact.h"

#include "R3BNeulandHitPar.h"    // for R3BLandGeometryPar
#include "FairParSet.h"    // for FairParSet
#include "FairRuntimeDb.h" // for FairRuntimeDb
#include "FairLogger.h"

#include "Riosfwd.h" // for ostream
#include "TList.h"   // for TList
#include "TString.h" // for TString

//#include <string.h> // for strcmp, NULL

using namespace std;

static R3BNeulandParFact gR3BNeulandParFact;

R3BNeulandParFact::R3BNeulandParFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BNeulandParFact";
    fTitle = "Tutorial factory for parameter containers"; //TODO
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BNeulandParFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers.*/

    FairContainer* p1 = new FairContainer("NeulandPar", "NeuLAND Modul Parameters", "TestDefaultContext"); //TODO
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);
}

FairParSet* R3BNeulandParFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    LOG(INFO) << "R3BNeulandParFact::createContainer : " << name << FairLogger::endl;
    FairParSet* p = NULL;

    if (strcmp(name, "NeulandPar") == 0) //TODO
    {
        p = new R3BNeulandHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BNeulandParFact);