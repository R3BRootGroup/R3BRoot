/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BDMContFact.h"

#include "FairLogger.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "R3BDMTotCalPar.h"
#include "R3BTGeoPar.h"

#include "TClass.h"

static R3BDMContFact gR3BDMContFact;

R3BDMContFact::R3BDMContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BDMContFact";
    fTitle = "Factory for parameter containers in libR3BDM";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BDMContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them
    // to the list of containers for the DM library.

    FairContainer* p1 = new FairContainer("DMTotCalPar", "DM Calibration Parameters", "DMCalParContext");
    p1->addContext("DMCalParContext");
    containers->Add(p1);
}

FairParSet* R3BDMContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context.
     */

    const std::string name(c->GetName());
    LOG(info) << "R3BDMContFact: Create container name: " << name;
    FairParSet* p = 0;
    if (name == "DMTotCalPar")
    {
        p = new R3BDMTotCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BDMContFact);
