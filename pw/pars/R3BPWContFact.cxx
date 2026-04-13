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

#include "R3BPWContFact.h"

#include "FairLogger.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "R3BPWHitPar.h"
#include "R3BPWTotCalPar.h"
#include "R3BTGeoPar.h"

#include "TClass.h"

static R3BPWContFact gR3BPWContFact;

R3BPWContFact::R3BPWContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BPWContFact";
    fTitle = "Factory for parameter containers in libR3BPW";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BPWContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them
    // to the list of containers for the PW library.

    FairContainer* p1 = new FairContainer("PWTotCalPar", "PW Calibration Parameters", "PWCalParContext");
    p1->addContext("PWCalParContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("PWHitPar", "PW Hit Parameters", "PWHitParContext");
    p2->addContext("PWHitParContext");
    containers->Add(p2);
}

FairParSet* R3BPWContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context.
     */

    const std::string name(c->GetName());
    LOG(info) << "R3BPWContFact: Create container name: " << name;
    FairParSet* p = 0;
    if (name == "PWTotCalPar")
    {
        p = new R3BPWTotCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "PWHitPar")
    {
        p = new R3BPWHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BPWContFact);
