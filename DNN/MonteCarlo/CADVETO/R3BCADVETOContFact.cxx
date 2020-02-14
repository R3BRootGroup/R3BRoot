#include "R3BCADVETOContFact.h"
#include "FairRuntimeDb.h"
#include "R3BCADVETOGeoPar.h"
#include "R3BCADVETONeutron2DPar.h"

static R3BCADVETOContFact gR3BCADVETOContFact;

R3BCADVETOContFact::R3BCADVETOContFact()
{
    fName = "R3BCADVETOContFact";
    fTitle = "Factory for VETO parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCADVETOContFact::setAllContainers()
{
    FairContainer* p1 = new FairContainer("R3BCADVETOGeoPar", "VETO Geometry Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 =
        new FairContainer("R3BCADVETONeutron2DPar", "VETO Neutron 2D Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
}

FairParSet* R3BCADVETOContFact::createContainer(FairContainer* c)
{
    const char* name = c->GetName();

    if (strcmp(name, "R3BCADVETOGeoPar") == 0)
    {
        return new R3BCADVETOGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    if (strcmp(name, "R3BCADVETONeutron2DPar") == 0)
    {
        return new R3BCADVETONeutron2DPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return nullptr;
}

ClassImp(R3BCADVETOContFact)
