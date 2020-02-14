#include "R3BCADNEBULAContFact.h"
#include "FairRuntimeDb.h"
#include "R3BCADNEBULAGeoPar.h"
#include "R3BCADNEBULANeutron2DPar.h"

static R3BCADNEBULAContFact gR3BCADNEBULAContFact;

R3BCADNEBULAContFact::R3BCADNEBULAContFact()
{
    fName = "R3BCADNEBULAContFact";
    fTitle = "Factory for NEBULA parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCADNEBULAContFact::setAllContainers()
{
    FairContainer* p1 = new FairContainer("R3BCADNEBULAGeoPar", "NEBULA Geometry Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 =
        new FairContainer("R3BCADNEBULANeutron2DPar", "NEBULA Neutron 2D Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
}

FairParSet* R3BCADNEBULAContFact::createContainer(FairContainer* c)
{
    const char* name = c->GetName();

    if (strcmp(name, "R3BCADNEBULAGeoPar") == 0)
    {
        return new R3BCADNEBULAGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    if (strcmp(name, "R3BCADNEBULANeutron2DPar") == 0)
    {
        return new R3BCADNEBULANeutron2DPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return nullptr;
}

ClassImp(R3BCADNEBULAContFact)
