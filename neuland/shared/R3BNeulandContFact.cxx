#include "R3BNeulandContFact.h"
#include "FairRuntimeDb.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandNeutron2DPar.h"

static R3BNeulandContFact gR3BNeulandContFact;

R3BNeulandContFact::R3BNeulandContFact()
{
    fName = "R3BNeulandContFact";
    fTitle = "Factory for Neuland parameter containers";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BNeulandContFact::setAllContainers()
{
    FairContainer* p1 = new FairContainer("R3BNeulandGeoPar", "Neuland Geometry Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 =
        new FairContainer("R3BNeulandNeutron2DPar", "Neuland Neutron 2D Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
}

FairParSet* R3BNeulandContFact::createContainer(FairContainer* c)
{
    const char* name = c->GetName();

    if (strcmp(name, "R3BNeulandGeoPar") == 0)
    {
        return new R3BNeulandGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    if (strcmp(name, "R3BNeulandNeutron2DPar") == 0)
    {
        return new R3BNeulandNeutron2DPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return nullptr;
}

ClassImp(R3BNeulandContFact)
