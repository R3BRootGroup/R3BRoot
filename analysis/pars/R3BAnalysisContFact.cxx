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

#include "R3BAnalysisContFact.h"
#include "R3BIncomingIDPar.h"
#include "R3BLogger.h"
#include "R3BMSOffsetPar.h"
#include "R3BTGeoPar.h"

#include <FairLogger.h>
#include <FairRuntimeDb.h>

#include <TClass.h>

static R3BAnalysisContFact gR3BAnalysisContFact;

R3BAnalysisContFact::R3BAnalysisContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BAnalysisContFact";
    fTitle = "Factory for parameter containers needed for the online and offline analysis";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BAnalysisContFact::setAllContainers()
{
    auto p1 = std::make_unique<FairContainer>("IncomingIDPar", "Incoming ID parameters", "IncomingIDParContext");
    p1->addContext("IncomingIDParContext");
    containers->Add(p1.release());

    auto p2 = std::make_unique<FairContainer>("MSOffsetPar", "MSOffsetPar Offset Parameters", "MSOffsetParContext");
    p2->addContext("MSOffsetParContext");
    containers->Add(p2.release());
}

FairParSet* R3BAnalysisContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "IncomingIDPar")
    {
        p = new R3BIncomingIDPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "MSOffsetPar")
    {
        p = new R3BMSOffsetPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BAnalysisContFact)
