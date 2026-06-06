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

//
//  R3BMusliContFact
//
//  Factory for the parameter containers in libR3BMusli
//

#include "R3BMusliContFact.h"

#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "R3BLogger.h"
#include "R3BMusliCalPar.h"
#include "R3BMusliHitPar.h"
#include "R3BTGeoPar.h"

#include "TClass.h"

static R3BMusliContFact gR3BMusliContFact;

R3BMusliContFact::R3BMusliContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BMusliContFact";
    fTitle = "Factory for parameter containers in libR3BMusli";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BMusliContFact::setAllContainers()
{
    auto* p1 = new FairContainer("musliCalPar", "MUSLI Cal Parameters", "MusliCalParContext");
    p1->addContext("MusliCalParContext");

    containers->Add(p1);

    auto* p2 = new FairContainer("musliHitPar", "MUSLI Hit Parameters", "MusliHitParContext");
    p2->addContext("MusliHitParContext");

    containers->Add(p2);

    auto* p3 = new FairContainer("MusliGeoPar", "Musli geometry parameters", "GeometryParameterContext");
    p3->addContext("GeometryParameterContext");

    containers->Add(p3);
}

FairParSet* R3BMusliContFact::createContainer(FairContainer* c)
{
    const char* name = c->GetName();
    R3BLOG(info, name);
    FairParSet* p = 0;
    if (strcmp(name, "musliCalPar") == 0)
    {
        p = new R3BMusliCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "musliHitPar") == 0)
    {
        p = new R3BMusliHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "MusliGeoPar") == 0)
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BMusliContFact);
