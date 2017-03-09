/////////////////////////////////////////////////////////////
//
//  R3BPtofParFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BPtofParFact.h"

#include "R3BPtofHitPar.h"

#include "FairParSet.h"    // for FairParSet
#include "FairRuntimeDb.h" // for FairRuntimeDb
#include "FairLogger.h"

#include "Riosfwd.h" // for ostream
#include "TList.h"   // for TList
#include "TString.h" // for TString

//#include <string.h> // for strcmp, NULL

using namespace std;

static R3BPtofParFact gR3BPtofParFact;

R3BPtofParFact::R3BPtofParFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BPtofParFact";
    fTitle = "Tutorial factory for parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BPtofParFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers.*/

    FairContainer* p1 = new FairContainer("PtofHitPar", "Ptof Hit Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);
}

FairParSet* R3BPtofParFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    LOG(INFO) << "R3BPtofParFact::createContainer : " << name << FairLogger::endl;
    FairParSet* p = NULL;

    if (strcmp(name, "PtofHitPar") == 0)
    {
        p = new R3BPtofHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    
    return p;
}

ClassImp(R3BPtofParFact)