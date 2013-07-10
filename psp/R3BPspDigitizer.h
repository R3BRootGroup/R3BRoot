#ifndef R3BPSPDIGITIZER_H
#define R3BPSPDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BPspDigiPar.h"
#include "R3BPspDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BPspDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BPspDigitizer();


  /** Destructor **/
  ~R3BPspDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  
  R3BPspDigi* AddHit(Int_t psp3mul,Double_t psp3x,Double_t psp3y,Double_t psp3e);


  protected:
  TClonesArray* fPspPoints;
  TClonesArray* fPspMCTrack; 
  TClonesArray* fPspDigi;


  // Parameter class
  R3BPspDigiPar* fPspDigiPar;

  //- Control Hitograms
  
//   TH1F* PSPXhis;
  
  Int_t eventNoPsp;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BPspDigitizer,1);
  
};

#endif
