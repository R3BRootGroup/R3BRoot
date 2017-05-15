//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009
//*-- Modified: 16/12/2016, P.Cabanelas

/////////////////////////////////////////////////////////////
//
//  R3BCalifaContFact
//
//  Factory for the parameter containers in libR3BCalifa
//
/////////////////////////////////////////////////////////////


#include "R3BCalifaContFact.h"

//#include "R3BCaloParRootFileIo.h"
//#include "R3BCaloParAsciiFileIo.h"
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

ClassImp(R3BCalifaContFact)

static R3BCalifaContFact gR3BCalifaContFact;

R3BCalifaContFact::R3BCalifaContFact() {
  // Constructor (called when the library is loaded)
  fName="R3BCalifaContFact";
  fTitle="Factory for parameter containers in libR3BCalifa";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCalifaContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
   /*
    FairContainer* p1= new FairContainer("R3BCalifaDigiPar",
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

FairParSet* R3BCalifaContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  /*
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"R3BCalifaDigiPar")==0) {
    p=new R3BCalifaDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
  */
  return NULL;
}

void  R3BCalifaContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BCalifaParRootFileIo* p=new R3BCalifaParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BCalifaParAsciiFileIo* p=new R3BCalifaParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}

