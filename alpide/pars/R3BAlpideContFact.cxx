/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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
// -----            R3BAlpideContFact source file             -----
// -----    Created 11/02/2022 by J.L. Rodriguez-Sanchez      -----
// ----- Factory for the parameter containers in libR3BAlpide -----
//-----------------------------------------------------------------

#include "FairRuntimeDb.h"

#include "R3BAlpideContFact.h"
#include "R3BAlpideMappingPar.h"
#include "R3BLogger.h"

#include "R3BTGeoPar.h"

#include "TClass.h"

static R3BAlpideContFact gR3BAlpideContFact;

R3BAlpideContFact::R3BAlpideContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BAlpideContFact";
    fTitle = "Factory for parameter containers in libR3BAlpide";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BAlpideContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the Alpide library.

    FairContainer* p1 = new FairContainer("AlpideGeoPar", "ALPIDE Geometry Parameters", "GeometryParContext");
    p1->addContext("GeometryParContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("alpideMappingPar", "ALPIDE Mapping Parameters", "AlpideMappingParContext");
    p2->addContext("AlpideMappingParContext");
    containers->Add(p2);
}

FairParSet* R3BAlpideContFact::createContainer(FairContainer* c)
{
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.
    const char* name = c->GetName();
    R3BLOG(info, "Create container name: " << name);
    FairParSet* p = 0;
    if (strcmp(name, "AlpideGeoPar") == 0)
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "alpideMappingPar") == 0)
    {
        p = new R3BAlpideMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BAlpideContFact);
