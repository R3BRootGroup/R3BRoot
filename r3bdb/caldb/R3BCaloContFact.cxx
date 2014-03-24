/////////////////////////////////////////////////////////////
//
//  R3BCaloContFact
//
//  Factory for the parameter containers
//
/////////////////////////////////////////////////////////////
#include "R3BCaloContFact.h"

#include "R3BCaloGeometryPar.h"               // for R3BCaloGeometryPar
#include "FairParSet.h"                 // for FairParSet
#include "FairRuntimeDb.h"              // for FairRuntimeDb

#include "Riosfwd.h"                    // for ostream
#include "TList.h"                      // for TList
#include "TString.h"                    // for TString

#include <string.h>                     // for strcmp, NULL
#include <iostream>                     // for operator<<, basic_ostream, etc

using namespace std;

ClassImp(R3BCaloContFact);

static R3BCaloContFact gR3BCaloContFact;

R3BCaloContFact::R3BCaloContFact()
{
  // Constructor (called when the library is loaded)
  fName="R3BCaloContFact";
  fTitle="Tutorial factory for parameter containers";
  setAllContainers();
  FairRuntimeDb::instance()->addContFactory(this);
}

void R3BCaloContFact::setAllContainers()
{
  /** Creates the Container objects with all accepted contexts and adds them to
   *  the list of containers.*/

  FairContainer* p1 = new FairContainer("CaloGeometryPar", "Califa Geometry Parameters",
                                        "TestDefaultContext");
  p1->addContext("TestNonDefaultContext");
  containers->Add(p1);
}

FairParSet* R3BCaloContFact::createContainer(FairContainer* c)
{
  /** Calls the constructor of the corresponding parameter container.
   * For an actual context, which is not an empty string and not the default context
   * of this container, the name is concatinated with the context. */

  const char* name=c->GetName();
  cout << "-I-R3BCaloContFact::createContainer " << name << endl;
  FairParSet* p=NULL;

  if (strcmp(name,"CaloGeometryPar")==0) {
    p=new R3BCaloGeometryPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
    // Set Arguments needed for SQL versioning managment
    p->SetVersion(0);
    p->SetDbEntry(0);
    p->SetLogTitle(name);
  }

  return p;
}
