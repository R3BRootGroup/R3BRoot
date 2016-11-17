/////////////////////////////////////////////////////////////
//
//  R3BTContFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BTContFact.h"

#include "R3BTGeoPar.h"
#include "FairParSet.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "Riosfwd.h"
#include "TList.h"
#include "TString.h"

#include <string.h>

using namespace std;

ClassImp(R3BTContFact);

static R3BTContFact gR3BTContFact;

R3BTContFact::R3BTContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BTContFact";
    fTitle = "Tutorial factory for parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BTContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers.*/

    FairContainer* p1 = new FairContainer("mTofGeoPar", "mTOF geometry parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);
}

FairParSet* R3BTContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    LOG(INFO) << "R3BTContFact::createContainer : " << name << FairLogger::endl;
    FairParSet* p = NULL;

    if (strcmp(name, "mTofGeoPar") == 0)
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return p;
}
