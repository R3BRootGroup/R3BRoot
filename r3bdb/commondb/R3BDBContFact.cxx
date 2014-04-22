/////////////////////////////////////////////////////////////
//
//  R3BDBContFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BDBContFact.h"

#include "FairParSet.h"                 // for FairParSet
#include "FairRuntimeDb.h"              // for FairRuntimeDb

#include "Riosfwd.h"                    // for ostream
#include "TList.h"                      // for TList
#include "TString.h"                    // for TString

#include <string.h>                     // for strcmp, NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using namespace std;

ClassImp(R3BDBContFact);

static R3BDBContFact gR3BDBContFact;



R3BDBContFact::R3BDBContFact()
{
  // Constructor (called when the library is loaded)
  fName="R3BDBContFact";
  fTitle="Tutorial factory for parameter containers";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BDBContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers.*/

  FairContainer* p1 = new FairContainer("CalPar", "land Calibration Parameters",
                                        "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");
  containers->Add(p1);
}

FairParSet* R3BDBContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

  const char* name=c->GetName();
  cout << "-I-R3BDBContFact::createContainer " << name << endl;
  FairParSet* p=NULL;

  if (strcmp(name,"CalPar")==0) {
    //p=new R3BCalPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    // Set Arguments needed for SQL versioning managment
    //p->SetVersion(0);
	// p->SetDbEntry(0);
    //p->SetLogTitle(name);
  }

  return p;
}
