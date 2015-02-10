/////////////////////////////////////////////////////////////
//
//  R3BDBLandContFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BDBLandContFact.h"

#include "R3BLandCalPar.h"               // for R3BLandGeometryPar
#include "FairParSet.h"                 // for FairParSet
#include "FairRuntimeDb.h"              // for FairRuntimeDb

#include "Riosfwd.h"                    // for ostream
#include "TList.h"                      // for TList
#include "TString.h"                    // for TString

#include <string.h>                     // for strcmp, NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using namespace std;

ClassImp(R3BDBLandContFact);

static R3BDBLandContFact gR3BDBLandContFact;

R3BDBLandContFact::R3BDBLandContFact()
{
  // Constructor (called when the library is loaded)
  fName="R3BDBLandContFact";
  fTitle="Tutorial factory for parameter containers";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BDBLandContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers.*/

  FairContainer* p1 = new FairContainer("LandCalPar", "land Calibration Parameters",
                                        "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");
  containers->Add(p1);
}

FairParSet* R3BDBLandContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

  const char* name=c->GetName();
  cout << "-I-R3BDBLandContFact::createContainer " << name << endl;
  FairParSet* p=NULL;

  if (strcmp(name,"LandCalPar")==0) {
    p=new R3BLandCalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    // Set Arguments needed for SQL versioning managment
    //p->SetVersion(0);
    //p->SetDbEntry(0);
    //p->SetLogTitle(name);
  }

  return p;
}
