#ifndef R3BDCHDIGITIZER_H
#define R3BDCHDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BDchDigiPar.h"
#include "R3BDchDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BDchDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BDchDigitizer();


  /** Destructor **/
  ~R3BDchDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BDchDigi* AddHit();


  protected:
  TClonesArray* fDchPoints;
  TClonesArray* fDchMCTrack; 
  TClonesArray* fDchDigi;

  // Parameter class
  R3BDchDigiPar* fDchDigiPar;

  //- Control Hitograms
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BDchDigitizer,1);
  
};

#endif
