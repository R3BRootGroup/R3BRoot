/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009
//*-- Modified: 29/05/2018, J.L. Rodriguez

//-----------------------------------------------------------
//  R3BTraContFact                                      -----
//  Factory for the parameter containers in libR3BTra   -----
//-----------------------------------------------------------

#include "R3BTraContFact.h"
#include "R3BGeoTraPar.h"
//#include "R3BTraDigiPar.h"

#include "FairLogger.h"
#include "FairRuntimeDb.h"

#include "R3BAmsMappingPar.h"
#include "R3BAmsStripCalPar.h"
#include "R3BTGeoPar.h"

#include "TClass.h"

static R3BTraContFact gR3BTraContFact;

R3BTraContFact::R3BTraContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BTraContFact";
    fTitle = "Factory for parameter containers in libR3BTra";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BTraContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers for the STS library.*/
    /*
     FairContainer* p1= new FairContainer("R3BTraDigiPar",
                                           "Sts Digitisation Parameters",
                                           "TestDefaultContext");
     p1->addContext("TestNonDefaultContext");

     FairContainer* p2= new FairContainer("CbmGeoStsPar",
                                           "Sts Geometry Parameters",
                                           "TestDefaultContext");
     p2->addContext("TestNonDefaultContext");

     containers->Add(p1);
     containers->Add(p2);
     */

    FairContainer* p1 = new FairContainer("amsStripCalPar", "AMS Strip Parameters", "AMSCalParContext");
    p1->addContext("AMSCalParContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("amsMappingPar", "AMS Mapping Parameters", "AmsMappingContext");
    p2->addContext("AmsMappingContext");
    containers->Add(p2);

    FairContainer* p3 = new FairContainer("amsGeoPar", "AMS Geometry Parameters", "GeometryParameterContext");
    p3->addContext("GeometryParameterContext");
    containers->Add(p3);
}

FairParSet* R3BTraContFact::createContainer(FairContainer* c)
{
    /** Trals the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */
    /*
    const char* name=c->GetName();
    LOG(INFO) << "container name " << name;
    FairParSet* p=0;
    if (strcmp(name,"R3BTraDigiPar")==0) {
      p=new R3BTraDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }
    if (strcmp(name,"CbmGeoStsPar")==0) {
      p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }
    return p;
    */

    const char* name = c->GetName();
    LOG(INFO) << "R3BTraContFact: Create container name: " << name;
    FairParSet* p = 0;
    if (strcmp(name, "amsStripCalPar") == 0)
    {
        p = new R3BAmsStripCalPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "amsMappingPar") == 0)
    {
        p = new R3BAmsMappingPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    else if (strcmp(name, "amsGeoPar") == 0)
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }
    return p;
}

ClassImp(R3BTraContFact);
