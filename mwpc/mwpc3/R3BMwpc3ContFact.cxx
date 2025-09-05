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

// ---------------------------------------------------------
// -----		R3BMwpc3ContFact 	         -----
// -----      Created 11/10/2019 by G.García Jiménez   -----
// ---------------------------------------------------------

#include <FairRuntimeDb.h>
#include <TClass.h>
#include <string>

#include "R3BLogger.h"
#include "R3BMwpc3CalPar.h"
#include "R3BMwpc3ContFact.h"
#include "R3BTGeoPar.h"

static R3BMwpc3ContFact gR3BMwpc3ContFact;

R3BMwpc3ContFact::R3BMwpc3ContFact()
{
    /* ---- Constructor (called when the library is loaded) ---- */
    fName = "R3BMwpc3ContFact";
    fTitle = "Factory for parameter containers in libR3BMwpc3";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BMwpc3ContFact::setAllContainers()
{
    auto* p1 = new FairContainer("mwpc3CalPar", "Mwpc3 Cal Parameters", "Mwpc3CalParContext");
    p1->addContext("Mwpc3CalParContext");
    containers->Add(p1);

    auto* p2 = new FairContainer("Mwpc3GeoPar", "Mwpc3 geometry parameters", "GeometryParameterContext");
    p2->addContext("GeometryParameterContext");
    containers->Add(p2);
}

FairParSet* R3BMwpc3ContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "mwpc3CalPar")
    {
        p = new R3BMwpc3CalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "Mwpc3GeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BMwpc3ContFact)
