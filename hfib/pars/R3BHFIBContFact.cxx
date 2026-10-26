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

#include "R3BHFIBContFact.h"

#include "FairLogger.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "R3BHFIBTotCalPar.h"
#include "R3BTGeoPar.h"

#include "TClass.h"

static R3BHFIBContFact gR3BHFIBContFact;

R3BHFIBContFact::R3BHFIBContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BHFIBContFact";
    fTitle = "Factory for parameter containers in libR3BHFIB";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BHFIBContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them
    // to the list of containers for the HFIB library.

    FairContainer* p1 = new FairContainer("HFIBTotCalPar", "HFIB Calibration Parameters", "HFIBCalParContext");
    p1->addContext("HFIBCalParContext");
    containers->Add(p1);
}

FairParSet* R3BHFIBContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context.
     */

    const std::string name(c->GetName());
    LOG(info) << "R3BHFIBContFact: Create container name: " << name;
    FairParSet* p = 0;
    if (name == "HFIBTotCalPar")
    {
        p = new R3BHFIBTotCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BHFIBContFact);
