#ifndef R3BFI7DIGITIZER_H
#define R3BFI7DDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BFi7DigiPar.h"
#include "R3BFi7Digi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BFi7Digitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BFi7Digitizer();


  /** Destructor **/
  ~R3BFi7Digitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BFi7Digi* AddHit(Int_t gf1mul,Double_t gf1_01x,Int_t gf2mul,Double_t gf2_01x);


  protected:
  TClonesArray* fFi7Points;
  TClonesArray* fFi7MCTrack; 
  TClonesArray* fFi7Digi;

  // Parameter class
  R3BFi7DigiPar* fFi7DigiPar;

  //- Control Hitograms

  
  Int_t eventNoFi7;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BFi7Digitizer,1);
  
};

#endif
