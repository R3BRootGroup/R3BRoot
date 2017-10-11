//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009

/////////////////////////////////////////////////////////////
//
//  R3BStartrackContFact
//
//  Factory for the parameter containers in libR3BStartrack
//
/////////////////////////////////////////////////////////////


#include "R3BStartrackContFact.h"

//#include "R3BStartrackParRootFileIo.h"
//#include "R3BStartrackParAsciiFileIo.h"
#include "R3BGeoStartrackPar.h"
//#include "R3BStartrackHitPar.h"

#include "FairRuntimeDb.h"
//#include "CbmParTest.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(R3BStartrackContFact)

static R3BStartrackContFact gR3BStartrackContFact;

R3BStartrackContFact::R3BStartrackContFact() {
  // Constructor (called when the library is loaded)
  fName="R3BStartrackContFact";
  fTitle="Factory for parameter containers in libR3BStartrack";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BStartrackContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
   /*
    FairContainer* p1= new FairContainer("R3BStartrackHitPar",
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

FairParSet* R3BStartrackContFact::createContainer(FairContainer* c) {
  /** Trals the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  /*
  const char* name=c->GetName();
  cout << " -I container name " << name << endl;
  FairParSet* p=0;
  if (strcmp(name,"R3BStartrackHitPar")==0) {
    p=new R3BStartrackHitPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
  */
  return NULL;
}

void  R3BStartrackContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BStartrackParRootFileIo* p=new R3BStartrackParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BStartrackParAsciiFileIo* p=new R3BStartrackParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}

