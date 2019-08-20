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

/////////////////////////////////////////////////////////////
//
//  R3BFiberContFact
//
//  Factory for the parameter containers in libR3BFiber
//
/////////////////////////////////////////////////////////////

#include "R3BFiberContFact.h"

//#include "R3BGeoTofPar.h" //still to do
#include "R3BBunchedFiberHitPar.h"

#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"
#include "FairRuntimeDb.h"

#include "TClass.h"

#include <iomanip>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(R3BFiberContFact)

    static R3BFiberContFact gR3BFiberContFact;

R3BFiberContFact::R3BFiberContFact()
{
    // Constructor (called when the library is loaded)
    fName = "R3BFiberContFact";
    fTitle = "Factory for parameter containers in libR3BFiber";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void R3BFiberContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers for the STS library.*/
    /*
     FairContainer* p1= new FairContainer("R3BFiberDigiPar",
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
    FairContainer* p1 = new FairContainer("Fi3aHitPar", "Fi3a Hit Parameters", "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");
    containers->Add(p1);

    FairContainer* p2 = new FairContainer("Fi3bHitPar", "Fi3b Hit Parameters", "TestDefaultContext");
    p2->addContext("TestNonDefaultContext");
    containers->Add(p2);

    FairContainer* p3 = new FairContainer("Fi7HitPar", "Fi7 Hit Parameters", "TestDefaultContext");
    p3->addContext("TestNonDefaultContext");
    containers->Add(p3);

    FairContainer* p4 = new FairContainer("Fi8HitPar", "Fi8 Hit Parameters", "TestDefaultContext");
    p4->addContext("TestNonDefaultContext");
    containers->Add(p4);

    FairContainer* p5 = new FairContainer("Fi10HitPar", "Fi10 Hit Parameters", "TestDefaultContext");
    p5->addContext("TestNonDefaultContext");
    containers->Add(p5);

    FairContainer* p6 = new FairContainer("Fi11HitPar", "Fi11 Hit Parameters", "TestDefaultContext");
    p6->addContext("TestNonDefaultContext");
    containers->Add(p6);

    FairContainer* p7 = new FairContainer("Fi12HitPar", "Fi12 Hit Parameters", "TestDefaultContext");
    p7->addContext("TestNonDefaultContext");

    containers->Add(p7);
    FairContainer* p8 = new FairContainer("Fi13HitPar", "Fi13 Hit Parameters", "TestDefaultContext");
    p8->addContext("TestNonDefaultContext");
    containers->Add(p8);

    FairContainer* p9 = new FairContainer("Fi1aHitPar", "Fi1a Hit Parameters", "TestDefaultContext");
    p9->addContext("TestNonDefaultContext");
    containers->Add(p9);

    FairContainer* p10 = new FairContainer("Fi1bHitPar", "Fi1b Hit Parameters", "TestDefaultContext");
    p10->addContext("TestNonDefaultContext");
    containers->Add(p10);
}

FairParSet* R3BFiberContFact::createContainer(FairContainer* c)
{
    /** Fiberls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */

    const char* name = c->GetName();
    cout << " -I container name " << name << endl;
    FairParSet* p = 0;
    /*
    if (strcmp(name,"R3BFiberDigiPar")==0) {
      p=new R3BFiberDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }
    if (strcmp(name,"CbmGeoStsPar")==0) {
      p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }*/
    //  if (strcmp(name,"FiberdHitPar")==0) {
    p = new R3BBunchedFiberHitPar(c->getConcatName().Data(), c->GetTitle(), c->getContext());
    //  }
    return p;
}

void R3BFiberContFact::activateParIo(FairParIo* io)
{
    // activates the input/output class for the parameters
    // needed by the Sts
    /*
    if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
      R3BFiberParRootFileIo* p=new R3BFiberParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
      io->setDetParIo(p);
    }
    if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
      R3BFiberParAsciiFileIo* p=new R3BFiberParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
      io->setDetParIo(p);
      }
    */
}
