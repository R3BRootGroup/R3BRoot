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

// -------------------------------------------------------------------------
// -----                    R3BFieldContFact source file               -----
// -----                   Created 20/02/06  by V. Friese              -----
// -------------------------------------------------------------------------

#include "R3BFieldContFact.h"
#include "R3BFieldPar.h"
#include <R3BLogger.h>

#include <FairParSet.h>
#include <FairRuntimeDb.h>

#include <iostream>
#include <string>

static R3BFieldContFact gR3BFieldContFact;

// -----   Constructor   ---------------------------------------------------
R3BFieldContFact::R3BFieldContFact()
{
    fName = "R3BFieldContFact";
    fTitle = "Factory for field parameter containers";
    SetAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

// -----   Create containers   ---------------------------------------------
FairParSet* R3BFieldContFact::createContainer(FairContainer* container)
{
    const std::string name(container->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* set = nullptr;

    if (name == "R3BFieldPar")
    {
        set = new R3BFieldPar(container->getConcatName().Data(), container->GetTitle(), container->getContext());
    }
    return set;
}

// -----   Set all containers (private)   ----------------------------------
void R3BFieldContFact::SetAllContainers()
{
    auto* container = new FairContainer("R3BFieldPar", "Field parameter container", "Default field");
    containers->Add(container);
}

ClassImp(R3BFieldContFact)
