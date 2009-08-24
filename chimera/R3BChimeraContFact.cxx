//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009

/////////////////////////////////////////////////////////////
//
//  R3BChimeraContFact
//
//  Factory for the parameter containers in libR3BChimera
//
/////////////////////////////////////////////////////////////


#include "R3BChimeraContFact.h"

//#include "R3BChimeraParRootFileIo.h"
//#include "R3BChimeraParAsciiFileIo.h"
#include "R3BGeoChimeraPar.h"
//#include "R3BChimeraDigiPar.h"

#include "FairRuntimeDb.h"
//#include "CbmParTest.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(R3BChimeraContFact)

static R3BChimeraContFact gR3BChimeraContFact;

R3BChimeraContFact::R3BChimeraContFact() {
  // Constructor (called when the library is loaded)
  fName="R3BChimeraContFact";
  fTitle="Factory for parameter containers in libR3BChimera";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BChimeraContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
   /*
    FairContainer* p1= new FairContainer("R3BChimeraDigiPar",
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

FairParSet* R3BChimeraContFact::createContainer(FairContainer* c) {
  /** Chimerals the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  /*
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"R3BChimeraDigiPar")==0) {
    p=new R3BChimeraDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
  */
  return NULL;
}

void  R3BChimeraContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BChimeraParRootFileIo* p=new R3BChimeraParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BChimeraParAsciiFileIo* p=new R3BChimeraParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}

