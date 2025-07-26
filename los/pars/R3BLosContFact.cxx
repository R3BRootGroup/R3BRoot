/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BLosContFact.h"

#include <FairLogger.h>
#include <FairRuntimeDb.h>

#include "R3BLogger.h"
#include "R3BLosHitPar.h"
#include "R3BTGeoPar.h"

#include <TClass.h>

static R3BLosContFact gR3BLosContFact;

R3BLosContFact::R3BLosContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BLosContFact";
    fTitle = "Factory for parameter containers needed for Los";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BLosContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the Los library.

    auto p1 = new FairContainer("LosHitPar", "Los Hit parameters", "LosHitParContext");
    p1->addContext("LosHitParContext");
    containers->Add(p1);

    auto p2 = new FairContainer("LosGeoPar", "Los geometry parameters", "GeometryParameterContext");
    p2->addContext("GeometryParameterContext");
    containers->Add(p2);
}

FairParSet* R3BLosContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "LosHitPar")
    {
        p = new R3BLosHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "LosGeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BLosContFact)
