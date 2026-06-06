/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <FairRuntimeDb.h>
#include <TClass.h>
#include <string>

#include "R3BLogger.h"
#include "R3BSci2ContFact.h"
#include "R3BSci2HitPar.h"

static R3BSci2ContFact gR3BSci2ContFact;

R3BSci2ContFact::R3BSci2ContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BSci2ContFact";
    fTitle = "Factory for parameter containers needed for Sci2";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BSci2ContFact::setAllContainers()
{
    auto* p1 = new FairContainer("Sci2HitPar", "Sci2 Hit parameters", "Sci2HitParContext");
    p1->addContext("Sci2HitParContext");
    containers->Add(p1);
}

FairParSet* R3BSci2ContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "Sci2HitPar")
    {
        p = new R3BSci2HitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BSci2ContFact)
