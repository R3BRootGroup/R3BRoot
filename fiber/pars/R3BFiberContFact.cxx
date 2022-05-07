/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

/////////////////////////////////////////////////////////////
//
//  R3BFiberContFact
//
//  Factory for the parameter containers in libR3BFiber
//
/////////////////////////////////////////////////////////////

#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "R3BFiberContFact.h"
#include "R3BLogger.h"
#include "R3BBunchedFiberHitPar.h"
#include "R3BFiberMAPMTHitPar.h"
#include "R3BFiberMappingPar.h"

#include "TClass.h"

static R3BFiberContFact gR3BFiberContFact;

R3BFiberContFact::R3BFiberContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BFiberContFact";
    fTitle = "Factory for parameter containers in libR3BFiber";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BFiberContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers for the STS library.*/
    /*
     FairContainer* p1= new FairContainer("R3BFiberDigiPar",
                                           "Sts Digitisation Parameters",
                                           "TestDefaultContext");
     p1->addContext("TestNonDefaultContext");
     containers->Add(p1);
     */
    FairContainer* p1 = new FairContainer("Fi3aHitPar", "Fi3a Hit Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("Fi3bHitPar", "Fi3b Hit Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);

    FairContainer* p3 = new FairContainer("Fi7HitPar", "Fi7 Hit Parameters", "TestDefaultContext");
    p3->addContext("TestNonDefaultContext");
    containers->Add(p3);

    FairContainer* p4 = new FairContainer("Fi8HitPar", "Fi8 Hit Parameters", "TestDefaultContext");
    p4->addContext("TestNonDefaultContext");
    containers->Add(p4);

    FairContainer* p5 = new FairContainer("Fi10HitPar", "Fi10 Hit Parameters", "TestDefaultContext");
    p5->addContext("TestNonDefaultContext");
    containers->Add(p5);

    FairContainer* p6 = new FairContainer("Fi11HitPar", "Fi11 Hit Parameters", "TestDefaultContext");
    p6->addContext("TestNonDefaultContext");
    containers->Add(p6);

    FairContainer* p7 = new FairContainer("Fi12HitPar", "Fi12 Hit Parameters", "TestDefaultContext");
    p7->addContext("TestNonDefaultContext");

    containers->Add(p7);
    FairContainer* p8 = new FairContainer("Fi13HitPar", "Fi13 Hit Parameters", "TestDefaultContext");
    p8->addContext("TestNonDefaultContext");
    containers->Add(p8);

    FairContainer* p9 = new FairContainer("Fi1aHitPar", "Fi1a Hit Parameters", "TestDefaultContext");
    p9->addContext("TestNonDefaultContext");
    containers->Add(p9);

    FairContainer* p10 = new FairContainer("Fi1bHitPar", "Fi1b Hit Parameters", "TestDefaultContext");
    p10->addContext("TestNonDefaultContext");
    containers->Add(p10);

    FairContainer* p11 = new FairContainer("Fi30HitPar", "Fi30 Hit Parameters", "TestDefaultContext");
    p11->addContext("TestNonDefaultContext");
    containers->Add(p11);

    FairContainer* p12 = new FairContainer("Fi31HitPar", "Fi31 Hit Parameters", "TestDefaultContext");
    p12->addContext("TestNonDefaultContext");
    containers->Add(p12);

    FairContainer* p13 = new FairContainer("Fi32HitPar", "Fi32 Hit Parameters", "TestDefaultContext");
    p13->addContext("TestNonDefaultContext");
    containers->Add(p13);

    FairContainer* p14 = new FairContainer("Fi33HitPar", "Fi33 Hit Parameters", "TestDefaultContext");
    p14->addContext("TestNonDefaultContext");
    containers->Add(p14);

    FairContainer* p15 = new FairContainer("Fi23aHitPar", "Fi23a Hit Parameters", "TestDefaultContext");
    p15->addContext("TestNonDefaultContext");
    containers->Add(p15);

    FairContainer* p16 = new FairContainer("Fi23bHitPar", "Fi23b Hit Parameters", "TestDefaultContext");
    p16->addContext("TestNonDefaultContext");
    containers->Add(p16);

    auto p17 = new FairContainer("Fi30MappingPar", "Fi30 Mapping Parameters", "TestDefaultContext");
    p17->addContext("TestNonDefaultContext");
    containers->Add(p17);

    auto p18 = new FairContainer("Fi31MappingPar", "Fi31 Mapping Parameters", "TestDefaultContext");
    p18->addContext("TestNonDefaultContext");
    containers->Add(p18);

    auto p19 = new FairContainer("Fi32MappingPar", "Fi32 Mapping Parameters", "TestDefaultContext");
    p19->addContext("TestNonDefaultContext");
    containers->Add(p19);

    auto p20 = new FairContainer("Fi33MappingPar", "Fi33 Mapping Parameters", "TestDefaultContext");
    p20->addContext("TestNonDefaultContext");
    containers->Add(p20);
}

FairParSet* R3BFiberContFact::createContainer(FairContainer* c)
{
    /** Fiberls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    R3BLOG(INFO, "Create container name " << name);
    
    FairParSet* p = 0;
    if (
        strcmp(name, "Fi10MappingPar") == 0 || strcmp(name, "Fi11MappingPar") == 0 ||
        strcmp(name, "Fi12MappingPar") == 0 || strcmp(name, "Fi13MappingPar") == 0 ||
        strcmp(name, "Fi23aMappingPar") == 0 || strcmp(name, "Fi23bMappingPar") == 0 ||
        strcmp(name, "Fi30MappingPar") == 0 || strcmp(name, "Fi31MappingPar") == 0 ||
        strcmp(name, "Fi32MappingPar") == 0 || strcmp(name, "Fi33MappingPar") == 0)
    {
        p = new R3BFiberMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "Fi30HitPar") == 0 || strcmp(name, "Fi31HitPar") == 0 || strcmp(name, "Fi32HitPar") == 0 ||
             strcmp(name, "Fi33HitPar") == 0 || strcmp(name, "Fi23aHitPar") == 0 || strcmp(name, "Fi23bHitPar") == 0)
    {
        p = new R3BFiberMAPMTHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else
    {
        p = new R3BBunchedFiberHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BFiberContFact);
