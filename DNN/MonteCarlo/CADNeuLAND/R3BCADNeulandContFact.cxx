#include "R3BCADNeulandContFact.h"
#include "FairRuntimeDb.h"
#include "R3BCADNeulandGeoPar.h"
#include "R3BCADNeulandNeutron2DPar.h"

static R3BCADNeulandContFact gR3BCADNeulandContFact;

R3BCADNeulandContFact::R3BCADNeulandContFact()
{
    fName = "R3BCADNeulandContFact";
    fTitle = "Factory for Neuland parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCADNeulandContFact::setAllContainers()
{
    FairContainer* p1 = new FairContainer("R3BCADNeulandGeoPar", "Neuland Geometry Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 =
        new FairContainer("R3BCADNeulandNeutron2DPar", "Neuland Neutron 2D Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
}

FairParSet* R3BCADNeulandContFact::createContainer(FairContainer* c)
{
    const char* name = c->GetName();

    if (strcmp(name, "R3BCADNeulandGeoPar") == 0)
    {
        return new R3BCADNeulandGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    if (strcmp(name, "R3BCADNeulandNeutron2DPar") == 0)
    {
        return new R3BCADNeulandNeutron2DPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return nullptr;
}

ClassImp(R3BCADNeulandContFact)
