#ifndef R3BMTOFDIGITIZER_H
#define R3BMTOFDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BmTofDigiPar.h"
#include "R3BmTofDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BmTofDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BmTofDigitizer();


  /** Destructor **/
  ~R3BmTofDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  //R3BmTofDigi* AddHit(Int_t ntmul,Double_t ntfx,Double_t ntfy,Double_t ntft,Double_t ntfpath,Double_t ntfpx,Double_t ntfpy,
  //Double_t ntfpz);
  R3BmTofDigi* AddHit(Int_t ntmul,Double_t ntfx,Double_t ntfy,Double_t ntft,Double_t ntfpath,Double_t ntfpx,Double_t ntfpy,
  Double_t ntfpz, Double_t ntfe);


  protected:
  TClonesArray* fmTofPoints;
  TClonesArray* fmTofMCTrack; 
  TClonesArray* fmTofDigi;


  // Parameter class
  R3BmTofDigiPar* fmTofDigiPar;

  //- Control Hitograms
  
  TH1F * NtfXhis;
  TH1F * fMult;
  TH1F * fEloss;

  Int_t eventNomTof;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BmTofDigitizer,1);
  
};

#endif
