//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009

/////////////////////////////////////////////////////////////
//
//  R3BXBallContFact
//
//  Factory for the parameter containers in libR3BXBall
//
/////////////////////////////////////////////////////////////

#include "TClass.h"

#include "FairRuntimeDb.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairLogger.h"

#include "R3BXBallContFact.h"
//#include "R3BXBallParRootFileIo.h"
//#include "R3BXBallParAsciiFileIo.h"
#include "R3BGeoXBallPar.h"
//#include "R3BXBallDigiPar.h"



static R3BXBallContFact gR3BXBallContFact;



R3BXBallContFact::R3BXBallContFact()
{
  // Constructor (called when the library is loaded)
  fName = "R3BXBallContFact";
  fTitle = "Factory for parameter containers in libR3BXBall";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}



void R3BXBallContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
   /*
    FairContainer* p1= new FairContainer("R3BXBallDigiPar",
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



FairParSet* R3BXBallContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  /*
  const char* name=c->GetName();
  LOG(INFO) << "container name " << name;
  FairParSet* p=0;
  if (strcmp(name,"R3BXBallDigiPar")==0) {
    p=new R3BXBallDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
  */
  return NULL;
}



void  R3BXBallContFact::activateParIo(FairParIo* io)
{
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BXBallParRootFileIo* p=new R3BXBallParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BXBallParAsciiFileIo* p=new R3BXBallParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}



ClassImp(R3BXBallContFact)
