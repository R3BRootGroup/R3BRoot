// -------------------------------------------------------------------------
// -----                    R3BFieldContFact header file               -----
// -------------------------------------------------------------------------


/** R3BFieldContFact.h
 ** @author
 ** @since 20.02.2006
 ** @version 1.0
 **
 ** Parameter container factory for field parameter container
 **/


#ifndef R3BFIELDCONTFACT_H
#define R3BFIELDCONTFACT_H 1


#include "FairContFact.h"


class FairContainer;
class FairParSet;


class R3BFieldContFact : public FairContFact
{

 public:

  /** Constructor **/
  R3BFieldContFact();


  /** Destructor **/
  ~R3BFieldContFact();


  /** Create containers
   ** Creates the requested parameter sets (R3BFieldPar) 
   **/
  FairParSet* createContainer(FairContainer* container);


 private:

  /** Set all containers  
   ** Creates container objects with all accepted contexts and adds them
   ** to the list of containers for the field library. 
   **/
  void SetAllContainers();


  ClassDef(R3BFieldContFact,1);

};

#endif
