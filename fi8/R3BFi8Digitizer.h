#ifndef R3BFI8DIGITIZER_H
#define R3BFI8DDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BFi8DigiPar.h"
#include "R3BFi8Digi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BFi8Digitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BFi8Digitizer();


  /** Destructor **/
  ~R3BFi8Digitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BFi8Digi* AddHit(Int_t gf1mul,Double_t gf1_01x,Int_t gf2mul,Double_t gf2_01x);


  protected:
  TClonesArray* fFi8Points;
  TClonesArray* fFi8MCTrack; 
  TClonesArray* fFi8Digi;

  // Parameter class
  R3BFi8DigiPar* fFi8DigiPar;

  //- Control Hitograms

  
  Int_t eventNoFi8;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BFi8Digitizer,1);
  
};

#endif
