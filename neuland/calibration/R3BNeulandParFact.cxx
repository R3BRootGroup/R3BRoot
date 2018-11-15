/////////////////////////////////////////////////////////////
//
//  R3BNeulandParFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BNeulandParFact.h"
#include "FairLogger.h"
#include "FairParSet.h"    // for FairParSet
#include "FairRuntimeDb.h" // for FairRuntimeDb
#include "R3BNeulandHitPar.h"
#include "R3BNeulandQCalPar.h"
#include "TList.h"   // for TList
#include "TString.h" // for TString

//#include <string.h> // for strcmp, NULL

static R3BNeulandParFact gR3BNeulandParFact;

R3BNeulandParFact::R3BNeulandParFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BNeulandParFact";
    fTitle = "Tutorial factory for parameter containers"; // TODO
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BNeulandParFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers.*/

    FairContainer* p1 = new FairContainer("NeulandHitPar", "NeuLAND Modul Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);
    FairContainer* p2 = new FairContainer("NeulandQCalPar", "NeuLAND Pedestal Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
}

FairParSet* R3BNeulandParFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    LOG(INFO) << "R3BNeulandParFact::createContainer : " << name << FairLogger::endl;
    FairParSet* p = NULL;

    if (strcmp(name, "NeulandHitPar") == 0)
    {
        p = new R3BNeulandHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "NeulandQCalPar") == 0)
    {
        p = new R3BNeulandQCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BNeulandParFact);
