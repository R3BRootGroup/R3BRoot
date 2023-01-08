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

//*-- AUTHOR : D. Kresan
//*-- Created : 18/05/2015

/////////////////////////////////////////////////////////////
//
//  R3BFi23aContFact
//
//  Factory for the parameter containers in libR3BmTof
//
/////////////////////////////////////////////////////////////

#include "R3BFi23aContFact.h"

//#include "R3BmTofParRootFileIo.h"
//#include "R3BmTofParAsciiFileIo.h"
//#include "R3BmTofDigiPar.h"
#include "R3BTGeoPar.h"

#include "FairRuntimeDb.h"
//#include "CbmParTest.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"

#include "TClass.h"

#include <iomanip>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(R3BFi23aContFact)

    static R3BFi23aContFact gR3BFi23aContFact;

R3BFi23aContFact::R3BFi23aContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BFi23aContFact";
    fTitle = "Factory for parameter containers in libR3BdTof";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BFi23aContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers for the STS library.*/
    /*
     FairContainer* p1= new FairContainer("R3BmTofDigiPar",
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

    FairContainer* p2 = new FairContainer("fi23aGeoPar", "Fi23a geometry parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);
}

FairParSet* R3BFi23aContFact::createContainer(FairContainer* c)
{
    /** mTofls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    cout << " -I container name " << name << endl;
    FairParSet* p = 0;
    /*if (strcmp(name,"R3BmTofDigiPar")==0) {
      p=new R3BmTofDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }
    if (strcmp(name,"CbmGeoStsPar")==0) {
      p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }*/

    if (strcmp(name, "fi23aGeoPar") == 0)
    {
        p = new R3BTGeoPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    }

    return p;
}

void R3BFi23aContFact::activateParIo(FairParIo* io)
{
    // activates the input/output class for the parameters
    // needed by the Sts
    /*
    if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
      R3BmTofParRootFileIo* p=new R3BmTofParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
      io->setDetParIo(p);
    }
    if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
      R3BmTofParAsciiFileIo* p=new R3BmTofParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
      io->setDetParIo(p);
      }
    */
}
