//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009

/////////////////////////////////////////////////////////////
//
//  R3BLandContFact
//
//  Factory for the parameter containers in libR3BLand
//
/////////////////////////////////////////////////////////////


#include "R3BLandContFact.h"

//#include "R3BLandParRootFileIo.h"
//#include "R3BLandParAsciiFileIo.h"
#include "R3BGeoLandPar.h"
#include "R3BLandDigiPar.h"

#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairLogger.h"

#include "TClass.h"

#include <iostream>
#include <iomanip>

using std::cout;
using std::endl;

ClassImp(R3BLandContFact)

static R3BLandContFact gR3BLandContFact;

R3BLandContFact::R3BLandContFact() {
  // Constructor (called when the library is loaded)
  fName="R3BLandContFact";
  fTitle="Factory for parameter containers in libR3BLand";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BLandContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/

    FairContainer* p1= new FairContainer("R3BLandDigiPar",
                                          "Land Digitisation Parameters",
                                          "TestDefaultContext");
    p1->addContext("TestNonDefaultContext");


    containers->Add(p1);
}

FairParSet* R3BLandContFact::createContainer(FairContainer* c) {
  /** Landls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

  const char* name=c->GetName();
  LOG(INFO) << "R3BLandContFact: Create container name: " << name << FairLogger::endl;
  FairParSet* p=0;
  if (strcmp(name,"R3BLandDigiPar")==0) {
    p=new R3BLandDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;

}

void  R3BLandContFact::activateParIo(FairParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BLandParRootFileIo* p=new R3BLandParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BLandParAsciiFileIo* p=new R3BLandParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}

