#include "R3BFrsSciContFact.h"
#include "R3BFrsSciTcalPar.h"

#include "FairLogger.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "TClass.h"

static R3BFrsSciContFact gR3BFrsSciContFact;

R3BFrsSciContFact::R3BFrsSciContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BFrsSciContFact";
    fTitle = "Factory for parameter containers needed for Sci2";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BFrsSciContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the STS library.

    FairContainer* p1 = new FairContainer(
        "FrsSciTcalPar", "FrsSci Tcal parameters for VFTX time caliration in ns", "FrsSciTcalParContext");
    p1->addContext("FrsSciTcalParContext");
    containers->Add(p1);
}

FairParSet* R3BFrsSciContFact::createContainer(FairContainer* c)
{
    // Trals the constructor of the corresponding parameter container.
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.

    const char* name = c->GetName();
    LOG(info) << "R3BFrsSciContFact: Create container name: " << name;
    FairParSet* p = 0;
    if (strcmp(name, "FrsSciTcalTofPar") == 0)
    {
        p = new R3BFrsSciTcalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BFrsSciContFact);
