//*-- AUTHOR : Denis Bertini
//*-- Created : 21/06/2005

/////////////////////////////////////////////////////////////
//
//  R3BPassiveContFact
//
//  Factory for the parameter containers in libPassive
//
/////////////////////////////////////////////////////////////

#include <iostream>
#include <iomanip>

using namespace std;

#include "R3BPassiveContFact.h"
#include "FairRuntimeDb.h"
#include "R3BGeoPassivePar.h"

ClassImp(R3BPassiveContFact)

static R3BPassiveContFact gR3BPassiveContFact;

R3BPassiveContFact::R3BPassiveContFact() {
  // Constructor (called when the library is loaded)
  fName="R3BPassiveContFact";
  fTitle="Factory for parameter containers in libPassive";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BPassiveContFact::setAllContainers() {
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers for the STS library.*/


    FairContainer* p= new FairContainer("R3BGeoPassivePar",
                                          "Passive Geometry Parameters",
                                          "TestDefaultContext");
    p->addContext("TestNonDefaultContext");

    containers->Add(p);

}

FairParSet* R3BPassiveContFact::createContainer(FairContainer* c) {
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

  const char* name=c->GetName();
  FairParSet *p=NULL;
  if (strcmp(name,"R3BGeoPassivePar")==0) {
    p=new R3BGeoPassivePar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  }
  return p;
}

