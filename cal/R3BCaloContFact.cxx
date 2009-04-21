//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009

/////////////////////////////////////////////////////////////
//
//  R3BCaloContFact
//
//  Factory for the parameter containers in libR3BCalo
//
/////////////////////////////////////////////////////////////


#include "R3BCaloContFact.h"

//#include "R3BCaloParRootFileIo.h"
//#include "R3BCaloParAsciiFileIo.h"
#include "R3BGeoCaloPar.h"
//#include "R3BCaloDigiPar.h"

#include "FairRuntimeDb.h"
//#include "CbmParTest.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(R3BCaloContFact)

static R3BCaloContFact gR3BCaloContFact;

R3BCaloContFact::R3BCaloContFact() {
  // Constructor (called when the library is loaded)
  fName="R3BCaloContFact";
  fTitle="Factory for parameter containers in libR3BCalo";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCaloContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
   /*
    FairContainer* p1= new FairContainer("R3BCaloDigiPar",
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

FairParSet* R3BCaloContFact::createContainer(FairContainer* c) {
  /** Calols the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  /*
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"R3BCaloDigiPar")==0) {
    p=new R3BCaloDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
  */
  return NULL;
}

void  R3BCaloContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BCaloParRootFileIo* p=new R3BCaloParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BCaloParAsciiFileIo* p=new R3BCaloParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}

