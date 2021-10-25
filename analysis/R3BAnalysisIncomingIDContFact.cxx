
#include "R3BAnalysisIncomingIDContFact.h"

#include "FairLogger.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "R3BIncomingIDPar.h"
#include "R3BTGeoPar.h"
#include "TClass.h"

static R3BAnalysisIncomingIDContFact gR3BAnalysisIncomingIDContFact;

R3BAnalysisIncomingIDContFact::R3BAnalysisIncomingIDContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BAnalysisIncomingIDContFact";
    fTitle = "Factory for parameter containers needed for AnalysisIncomingID";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BAnalysisIncomingIDContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the STS library.

    FairContainer* p1 = new FairContainer("IncomingIDPar", "Incoming ID parameters", "IncomingIDParContext");
    p1->addContext("IncomingIDParContext");
    containers->Add(p1);
}

FairParSet* R3BAnalysisIncomingIDContFact::createContainer(FairContainer* c)
{
    // Trals the constructor of the corresponding parameter container.
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.

    const char* name = c->GetName();
    LOG(INFO) << "R3BIncomingIDContFact: Create container name: " << name;
    FairParSet* p = 0;
    if (strcmp(name, "IncomingIDPar") == 0)
    {
        p = new R3BIncomingIDPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BAnalysisIncomingIDContFact);
