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

/////////////////////////////////////////////////////////////
//
//  R3BFiberContFact
//
//  Factory for the parameter containers in libR3BFiber
//
/////////////////////////////////////////////////////////////

#include <FairLogger.h>
#include <FairRuntimeDb.h>

#include "R3BBunchedFiberHitPar.h"
#include "R3BFiberContFact.h"
#include "R3BFiberMAPMTHitPar.h"
#include "R3BFiberMappingPar.h"
#include "R3BLogger.h"
#include "R3BTGeoPar.h"

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
    // Basic name of the fiber detectors, add more if needed
    std::vector<std::string> detectors = { "Fi3a", "Fi3b", "Fi7",  "Fi8",  "Fi10", "Fi11", "Fi12",  "Fi13",
                                           "Fi1a", "Fi1b", "Fi30", "Fi31", "Fi32", "Fi33", "Fi23a", "Fi23b" };

    for (const auto& det : detectors)
    {
        std::string name = det + "GeoPar";
        std::string title = det + " Geometry Parameters";

        auto* cont = new FairContainer(name.c_str(), title.c_str(), "GeometryParContext");
        cont->addContext("GeometryParContext");
        containers->Add(cont);
    }

    for (const auto& det : detectors)
    {
        std::string name = det + "MappingPar";
        std::string title = det + " Mapping Parameters";

        auto* cont = new FairContainer(name.c_str(), title.c_str(), "MappingParContext");
        cont->addContext("MappingParContext");
        containers->Add(cont);
    }

    for (const auto& det : detectors)
    {
        std::string name = det + "HitPar";
        std::string title = det + " Hit Parameters";

        auto* cont = new FairContainer(name.c_str(), title.c_str(), "HitParContext");
        cont->addContext("HitParContext");
        containers->Add(cont);
    }
}

FairParSet* R3BFiberContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "Fi7GeoPar" || name == "Fi8GeoPar" || name == "Fi10GeoPar" || name == "Fi11GeoPar" ||
        name == "Fi12GeoPar" || name == "Fi13GeoPar" || name == "Fi23aGeoPar" || name == "Fi23bGeoPar" ||
        name == "Fi30GeoPar" || name == "Fi31GeoPar" || name == "Fi32GeoPar" || name == "Fi33GeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "Fi10MappingPar" || name == "Fi11MappingPar" || name == "Fi12MappingPar" ||
             name == "Fi13MappingPar" || name == "Fi23aMappingPar" || name == "Fi23bMappingPar" ||
             name == "Fi30MappingPar" || name == "Fi31MappingPar" || name == "Fi32MappingPar" ||
             name == "Fi33MappingPar")
    {
        p = new R3BFiberMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "Fi30HitPar" || name == "Fi31HitPar" || name == "Fi32HitPar" || name == "Fi33HitPar" ||
             name == "Fi23aHitPar" || name == "Fi23bHitPar")
    {
        p = new R3BFiberMAPMTHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else
    {
        p = new R3BBunchedFiberHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BFiberContFact)
