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

#include <FairRuntimeDb.h>
#include <TClass.h>
#include <string>

#include "R3BLogger.h"
#include "R3BMwpc0CalPar.h"
#include "R3BMwpc0ContFact.h"
#include "R3BTGeoPar.h"

static R3BMwpc0ContFact gR3BMwpc0ContFact;

R3BMwpc0ContFact::R3BMwpc0ContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BMwpc0ContFact";
    fTitle = "Factory for parameter containers in libR3BMwpc0";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BMwpc0ContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them to
    // the list of containers for the Mwpc0 library.

    FairContainer* p1 = new FairContainer("mwpc0CalPar", "Mwpc0 Cal Parameters", "Mwpc0CalParContext");
    p1->addContext("Mwpc0CalParContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("Mwpc0GeoPar", "Mwpc0 geometry parameters", "GeometryParameterContext");
    p2->addContext("GeometryParameterContext");
    containers->Add(p2);
}

FairParSet* R3BMwpc0ContFact::createContainer(FairContainer* c)
{
    // Trals the constructor of the corresponding parameter container.
    // For an actual context, which is not an empty string and not the default context
    // of this container, the name is concatinated with the context.

    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "mwpc0CalPar")
    {
        p = new R3BMwpc0CalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "Mwpc0GeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BMwpc0ContFact)
