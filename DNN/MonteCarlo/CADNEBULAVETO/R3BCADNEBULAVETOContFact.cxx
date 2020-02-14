#include "R3BCADNEBULAVETOContFact.h"
#include "FairRuntimeDb.h"
#include "R3BCADNEBULAVETOGeoPar.h"
#include "R3BCADNEBULAVETONeutron2DPar.h"

static R3BCADNEBULAVETOContFact gR3BCADNEBULAVETOContFact;

R3BCADNEBULAVETOContFact::R3BCADNEBULAVETOContFact()
{
    fName = "R3BCADNEBULAVETOContFact";
    fTitle = "Factory for NEBULAVETO parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCADNEBULAVETOContFact::setAllContainers()
{
    FairContainer* p1 = new FairContainer("R3BCADNEBULAVETOGeoPar", "NEBULAVETO Geometry Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 =
        new FairContainer("R3BCADNEBULAVETONeutron2DPar", "NEBULAVETO Neutron 2D Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
}

FairParSet* R3BCADNEBULAVETOContFact::createContainer(FairContainer* c)
{
    const char* name = c->GetName();

    if (strcmp(name, "R3BCADNEBULAVETOGeoPar") == 0)
    {
        return new R3BCADNEBULAVETOGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    if (strcmp(name, "R3BCADNEBULAVETONeutron2DPar") == 0)
    {
        return new R3BCADNEBULAVETONeutron2DPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return nullptr;
}

ClassImp(R3BCADNEBULAVETOContFact)
