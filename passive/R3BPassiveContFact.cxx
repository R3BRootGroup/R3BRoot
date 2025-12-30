/******************************************************************************
 *   Copyright (C) 2009 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2009-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BPassiveContFact.h"
#include "R3BGeoPassivePar.h"
#include "R3BLogger.h"
#include "R3BTGeoPar.h"

#include <TClass.h>

static R3BPassiveContFact gR3BPassiveContFact;

R3BPassiveContFact::R3BPassiveContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BPassiveContFact";
    fTitle = "Factory for parameter containers in libR3BPassive";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BPassiveContFact::setAllContainers()
{
    auto p1 = new FairContainer("R3BGeoPassivePar", "Passive Geometry Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    auto p2 = new FairContainer("TargetGeoPar", "Target geometry parameters", "GeometryParameterContext");
    p2->addContext("GeometryParameterContext");
    containers->Add(p2);
}

FairParSet* R3BPassiveContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "R3BGeoPassivePar")
    {
        p = new R3BGeoPassivePar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "TargetGeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BPassiveContFact)
