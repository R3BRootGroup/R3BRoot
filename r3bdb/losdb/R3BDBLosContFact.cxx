/////////////////////////////////////////////////////////////
//
//  R3BDBLosContFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BDBLosContFact.h"

#include "R3BLosCalPar.h"               // for R3BLandGeometryPar
#include "FairParSet.h"                 // for FairParSet
#include "FairRuntimeDb.h"              // for FairRuntimeDb

#include "Riosfwd.h"                    // for ostream
#include "TList.h"                      // for TList
#include "TString.h"                    // for TString

#include <string.h>                     // for strcmp, NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using namespace std;

ClassImp(R3BDBLosContFact);

static R3BDBLosContFact gR3BDBLosContFact;

R3BDBLosContFact::R3BDBLosContFact()
{
  // Constructor (called when the library is loaded)
  fName="R3BDBLosContFact";
  fTitle="Tutorial factory for parameter containers";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BDBLosContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers.*/

  FairContainer* p1 = new FairContainer("LosCalPar", "LOS Calibration Parameters",
                                        "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");
  containers->Add(p1);
}

FairParSet* R3BDBLosContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

  const char* name=c->GetName();
  cout << "-I-R3BDBLosContFact::createContainer " << name << endl;
  FairParSet* p=NULL;

  if (strcmp(name,"LosCalPar")==0) {
    p=new R3BLosCalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    // Set Arguments needed for SQL versioning managment
    //p->SetVersion(0);
    //p->SetDbEntry(0);
    //p->SetLogTitle(name);
  }

  return p;
}
