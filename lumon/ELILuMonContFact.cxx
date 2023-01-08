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

//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009

/////////////////////////////////////////////////////////////
//
//  ELILuMonContFact
//
//  Factory for the parameter containers in libELILuMon
//
/////////////////////////////////////////////////////////////

#include "ELILuMonContFact.h"

//#include "ELILuMonParRootFileIo.h"
//#include "ELILuMonParAsciiFileIo.h"
#include "ELIGeoLuMonPar.h"
//#include "ELILuMonDigiPar.h"

#include "FairRuntimeDb.h"
//#include "CbmParTest.h"
#include "FairParAsciiFileIo.h"
#include "FairParRootFileIo.h"

#include "TClass.h"

#include <iomanip>
#include <iostream>

using std::cout;
using std::endl;

ClassImp(ELILuMonContFact)

    static ELILuMonContFact gELILuMonContFact;

ELILuMonContFact::ELILuMonContFact()
{
    // Constructor (called when the library is loaded)
    fName = "ELILuMonContFact";
    fTitle = "Factory for parameter containers in libELILuMon";
    setAllContainers();
    FairRuntimeDb::instance()->addContFactory(this);
}

void ELILuMonContFact::setAllContainers()
{
    /** Creates the Container objects with all accepted contexts and adds them to
     *  the list of containers for the STS library.*/
    /*
     FairContainer* p1= new FairContainer("ELILuMonDigiPar",
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
}

FairParSet* ELILuMonContFact::createContainer(FairContainer* c)
{
    /** LuMonls the constructor of the corresponding parameter container.
     * For an actual context, which is not an empty string and not the default context
     * of this container, the name is concatinated with the context. */
    /*
    const char* name=c->GetName();
    cout << " -I container name " << name << endl;
    FairParSet* p=0;
    if (strcmp(name,"ELILuMonDigiPar")==0) {
      p=new ELILuMonDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }
    if (strcmp(name,"CbmGeoStsPar")==0) {
      p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    }
    return p;
    */
    return NULL;
}

void ELILuMonContFact::activateParIo(FairParIo* io)
{
    // activates the input/output class for the parameters
    // needed by the Sts
    /*
    if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
      ELILuMonParRootFileIo* p=new ELILuMonParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
      io->setDetParIo(p);
    }
    if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
      ELILuMonParAsciiFileIo* p=new ELILuMonParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
      io->setDetParIo(p);
      }
    */
}
