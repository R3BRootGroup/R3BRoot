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

// ---------------------------------------------------------------
// -----            R3BSsdContFact source file               -----
// -----    Created 29/05/2018 by J.L. Rodriguez-Sanchez     -----
// ----- Factory for the parameter containers in libR3BSsd   -----
//----------------------------------------------------------------

#include <FairLogger.h>
#include <FairRuntimeDb.h>

#include "R3BGeoTraPar.h"
#include "R3BSsdContFact.h"

#include "R3BAmsMappingPar.h"
#include "R3BAmsStripCalPar.h"

#include "R3BFootCalPar.h"
#include "R3BFootHitPar.h"
#include "R3BFootMappingPar.h"

#include "R3BLogger.h"
#include "R3BTGeoPar.h"

#include <TClass.h>

static R3BSsdContFact gR3BSsdContFact;

R3BSsdContFact::R3BSsdContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BSsdContFact";
    fTitle = "Factory for parameter containers in libR3BSsd";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BSsdContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the SSD library.

    auto p1 = new FairContainer("amsMappingPar", "AMS Mapping Parameters", "AmsMappingParContext");
    p1->addContext("AmsMappingParContext");
    containers->Add(p1);

    auto p2 = new FairContainer("amsStripCalPar", "AMS StripCal Parameters", "AmsCalParContext");
    p2->addContext("AmsCalParContext");
    containers->Add(p2);

    auto p3 = new FairContainer("amsGeoPar", "AMS Geometry Parameters", "GeometryParameterContext");
    p3->addContext("GeometryParameterContext");
    containers->Add(p3);

    auto p4 = new FairContainer("footMappingPar", "FOOT Mapping Parameters", "FootMappingParContext");
    p4->addContext("FootMappingParContext");
    containers->Add(p4);

    auto p5 = new FairContainer("footCalPar", "FOOT StripCal Parameters", "FootCalParContext");
    p5->addContext("FootCalParContext");
    containers->Add(p5);

    auto p6 = new FairContainer("footHitPar", "FOOT Hit Parameters", "FootHitParContext");
    p6->addContext("FootHitParContext");
    containers->Add(p6);

    auto p7 = new FairContainer("footGeoPar", "FOOT Geometry Parameters", "GeometryParameterContext");
    p7->addContext("GeometryParameterContext");
    containers->Add(p7);
}

FairParSet* R3BSsdContFact::createContainer(FairContainer* c)
{
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.

    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "amsMappingPar")
    {
        p = new R3BAmsMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "amsStripCalPar")
    {
        p = new R3BAmsStripCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "footMappingPar")
    {
        p = new R3BFootMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "footCalPar")
    {
        p = new R3BFootCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "footHitPar")
    {
        p = new R3BFootHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "amsGeoPar" || name == "footGeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BSsdContFact)
