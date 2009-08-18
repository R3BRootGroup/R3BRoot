#ifndef R3BMODULE_H
#define R3BMODULE_H 1


#include "FairModule.h"

#include "TObject.h"
#include "TVector3.h"

class R3BModule : public FairModule
{
 public:

  /** Default constructor **/
  R3BModule();


  /** Constructor with arguments
   **/
  R3BModule(const char* Name, const char* title, Bool_t Active=kFALSE);


  /** Copy constructor **/
 // R3BModule(const R3BModule& point) { *this = point; };


  /** Destructor **/
  virtual ~R3BModule();

  ClassDef(R3BModule,1)

};

#endif
