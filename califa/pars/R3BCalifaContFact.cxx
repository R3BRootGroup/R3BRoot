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

#include <FairLogger.h>
#include <FairParAsciiFileIo.h>
#include <FairParRootFileIo.h>
#include <FairRuntimeDb.h>

#include "R3BCalifaContFact.h"
#include "R3BCalifaCrystalCalPar.h"
#include "R3BCalifaCrystalPars4Sim.h"
#include "R3BCalifaMappingPar.h"
#include "R3BCalifaTotCalPar.h"
#include "R3BLogger.h"
#include "R3BTGeoPar.h"

#include <TClass.h>

static R3BCalifaContFact gR3BCalifaContFact;

R3BCalifaContFact::R3BCalifaContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BCalifaContFact";
    fTitle = "Factory for parameter containers in libR3BCalifa";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCalifaContFact::setAllContainers()
{
    // Creates the Container objects with all accepted contexts and adds them
    // to the list of containers for the CALIFA library.

    auto p1 = new FairContainer("califaCrystalCalPar", "Califa Calibration Parameters", "CalifaCalParContext");
    p1->addContext("CalifaCalParContext");
    containers->Add(p1);

    auto p2 = new FairContainer("califaMappingPar", "Califa Mapping Parameters", "CalifaMappingContext");
    p2->addContext("CalifaMappingContext");
    containers->Add(p2);

    auto p3 = new FairContainer("califaCrystalPars4Sim", "Califa Crystal Parameters for Sim", "CalifaSimParContext");
    p3->addContext("CalifaSimParContext");
    containers->Add(p3);

    auto p4 = new FairContainer("CalifaTotCalPar", "Califa Tot Cal parameters", "CalifaTotCalParContext");
    p4->addContext("CalifaTotCalParContext");
    containers->Add(p4);

    auto p5 = new FairContainer("CalifaGeoPar", "Califa geometry parameters", "GeometryParameterContext");
    p5->addContext("GeometryParameterContext");
    containers->Add(p5);
}

FairParSet* R3BCalifaContFact::createContainer(FairContainer* c)
{
    /** Calls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatenated with the context.
     */
    const std::string name(c->GetName());
    R3BLOG(info, "Create container name: " << name.c_str());

    FairParSet* p = nullptr;
    if (name == "califaCrystalCalPar")
    {
        p = new R3BCalifaCrystalCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "califaMappingPar")
    {
        p = new R3BCalifaMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "califaCrystalPars4Sim")
    {
        p = new R3BCalifaCrystalPars4Sim(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "CalifaTotCalPar")
    {
        p = new R3BCalifaTotCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (name == "CalifaGeoPar")
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BCalifaContFact)
