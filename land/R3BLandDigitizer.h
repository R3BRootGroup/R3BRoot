#ifndef R3BLANDDIGITIZER_H
#define R3BLANDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;


class R3BLandDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BLandDigitizer();


  /** Destructor **/
  ~R3BLandDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();

  protected:
  TClonesArray* fLandPoints;
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BLandDigitizer,1);
  
};

#endif
