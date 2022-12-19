#include "R3BSci2ContFact.h"

#include "FairLogger.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "R3BSci2HitPar.h"
#include "TClass.h"

static R3BSci2ContFact gR3BSci2ContFact;

R3BSci2ContFact::R3BSci2ContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BSci2ContFact";
    fTitle = "Factory for parameter containers needed for Sci2";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BSci2ContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the STS library.

    FairContainer* p1 = new FairContainer("Sci2HitPar", "Sci2 Hit parameters", "Sci2HitParContext");
    p1->addContext("Sci2HitParContext");
    containers->Add(p1);
}

FairParSet* R3BSci2ContFact::createContainer(FairContainer* c)
{
    // Trals the constructor of the corresponding parameter container.
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.

    const char* name = c->GetName();
    LOG(info) << "R3BSci2ContFact: Create container name: " << name;
    FairParSet* p = 0;
    if (strcmp(name, "Sci2HitPar") == 0)
    {
        p = new R3BSci2HitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BSci2ContFact);
