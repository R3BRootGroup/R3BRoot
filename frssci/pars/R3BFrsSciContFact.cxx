/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2024-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ----------------------------------------------------------------
// -----            R3BFrsSciContFact source file             -----
// -----         Created 20/01/2024 by A. Chatillon           -----
// ----- Factory for the parameter containers in libR3BFrsSci -----
//-----------------------------------------------------------------

#include <FairRuntimeDb.h>

#include "R3BFrsSciCalPar.h"
#include "R3BFrsSciContFact.h"
#include "R3BFrsSciTcalPar.h"
#include "R3BLogger.h"

#include <TClass.h>

static R3BFrsSciContFact gR3BFrsSciContFact;

R3BFrsSciContFact::R3BFrsSciContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BFrsSciContFact";
    fTitle = "Factory for parameter containers needed for Sci2";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BFrsSciContFact::setAllContainers()
{
    auto p1 = new FairContainer(
        "FrsSciTcalPar", "FrsSci Tcal parameters for VFTX time calibration in ns", "FrsSciTcalParContext");
    p1->addContext("FrsSciTcalParContext");
    containers->Add(p1);

    auto p2 = new FairContainer("FrsSciCalPar",
                                "FrsSci Cal parameters: multi hit to single hit + params for position and Tofs",
                                "FrsSciCalParContext");
    p2->addContext("FrsSciCalParContext");
    containers->Add(p2);
}

FairParSet* R3BFrsSciContFact::createContainer(FairContainer* c)
{
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "FrsSciTcalPar")
    {
        p = new R3BFrsSciTcalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "FrsSciCalPar")
    {
        p = new R3BFrsSciCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BFrsSciContFact)
