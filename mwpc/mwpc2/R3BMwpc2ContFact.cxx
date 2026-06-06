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
#include "R3BMwpc2CalPar.h"
#include "R3BMwpc2ContFact.h"
#include "R3BTGeoPar.h"

static R3BMwpc2ContFact gR3BMwpc2ContFact;

R3BMwpc2ContFact::R3BMwpc2ContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BMwpc2ContFact";
    fTitle = "Factory for parameter containers in libR3BMwpc2";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BMwpc2ContFact::setAllContainers()
{
    FairContainer* p1 = new FairContainer("mwpc2CalPar", "Mwpc2 Cal Parameters", "Mwpc2CalParContext");
    p1->addContext("Mwpc2CalParContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("Mwpc2GeoPar", "Mwpc2 geometry parameters", "GeometryParameterContext");
    p2->addContext("GeometryParameterContext");
    containers->Add(p2);
}

FairParSet* R3BMwpc2ContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "mwpc2CalPar")
    {
        p = new R3BMwpc2CalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "Mwpc2GeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BMwpc2ContFact)
