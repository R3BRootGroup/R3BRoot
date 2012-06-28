//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009

/////////////////////////////////////////////////////////////
//
//  R3BSTaRTraContFact
//
//  Factory for the parameter containers in libR3BSTaRTra
//
/////////////////////////////////////////////////////////////


#include "R3BSTaRTraContFact.h"

//#include "R3BSTaRTraParRootFileIo.h"
//#include "R3BSTaRTraParAsciiFileIo.h"
#include "R3BGeoSTaRTraPar.h"
//#include "R3BSTaRTraDigiPar.h"

#include "FairRuntimeDb.h"
//#include "CbmParTest.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(R3BSTaRTraContFact)

static R3BSTaRTraContFact gR3BSTaRTraContFact;

R3BSTaRTraContFact::R3BSTaRTraContFact() {
  // Constructor (called when the library is loaded)
  fName="R3BSTaRTraContFact";
  fTitle="Factory for parameter containers in libR3BSTaRTra";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BSTaRTraContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
   /*
    FairContainer* p1= new FairContainer("R3BSTaRTraDigiPar",
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

FairParSet* R3BSTaRTraContFact::createContainer(FairContainer* c) {
  /** Trals the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  /*
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"R3BSTaRTraDigiPar")==0) {
    p=new R3BSTaRTraDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
  */
  return NULL;
}

void  R3BSTaRTraContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BSTaRTraParRootFileIo* p=new R3BSTaRTraParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BSTaRTraParAsciiFileIo* p=new R3BSTaRTraParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}

