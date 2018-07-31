//*-- AUTHOR : Denis Bertini
//*-- Created : 20/03/2009
//*-- Modified: 29/05/2018, J.L. Rodriguez

/////////////////////////////////////////////////////////////
//
//  R3BTraContFact
//
//  Factory for the parameter containers in libR3BTra
//
/////////////////////////////////////////////////////////////


#include "R3BTraContFact.h"

//#include "R3BTraParRootFileIo.h"
//#include "R3BTraParAsciiFileIo.h"
#include "R3BGeoTraPar.h"
//#include "R3BTraDigiPar.h"

#include "FairRuntimeDb.h"
//#include "CbmParTest.h"
#include "FairParRootFileIo.h"
#include "FairParAsciiFileIo.h"
#include "FairLogger.h"

#include "R3BAmsStripCalPar.h"

#include "TClass.h"



static R3BTraContFact gR3BTraContFact;



R3BTraContFact::R3BTraContFact()
{
  // Constructor (called when the library is loaded)
  fName="R3BTraContFact";
  fTitle="Factory for parameter containers in libR3BTra";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}



void R3BTraContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/
   /*
    FairContainer* p1= new FairContainer("R3BTraDigiPar",
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

  FairContainer* p1= new FairContainer("amsStripCalPar",
                                       "AMS Strip Parameters",
                                       "AMSCalParContext");
  p1->addContext("AMSCalParContext");
  
  containers->Add(p1);

}



FairParSet* R3BTraContFact::createContainer(FairContainer* c)
{
  /** Trals the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */
  /*
  const char* name=c->GetName();
  LOG(INFO) << "container name " << name << FairLogger::endl;
  FairParSet* p=0;
  if (strcmp(name,"R3BTraDigiPar")==0) {
    p=new R3BTraDigiPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  if (strcmp(name,"CbmGeoStsPar")==0) {
    p=new CbmGeoStsPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
  */

  const char* name=c->GetName();
  LOG(INFO) << "R3BTraContFact: Create container name: " << name << FairLogger::endl;
  FairParSet* p=0;
  if (strcmp(name,"amsStripCalPar")==0){
    p=new R3BAmsStripCalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;


 // return NULL;
}



void  R3BTraContFact::activateParIo(FairParIo* io)
{
  // activates the input/output class for the parameters
  // needed by the Sts
  /*
  if (strcmp(io->IsA()->GetName(),"FairParRootFileIo")==0) {
    R3BTraParRootFileIo* p=new R3BTraParRootFileIo(((FairParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"FairParAsciiFileIo")==0) {
    R3BTraParAsciiFileIo* p=new R3BTraParAsciiFileIo(((FairParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
    }
  */
}



ClassImp(R3BTraContFact)
