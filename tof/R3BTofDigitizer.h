#ifndef R3BTOFDIGITIZER_H
#define R3BTOFDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BTofDigiPar.h"
#include "R3BTofDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BTofDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BTofDigitizer();


  /** Destructor **/
  ~R3BTofDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BTofDigi* AddHit(Int_t tfmul,Double_t tfwx_p1,Double_t tfwy_p1,Double_t tfwt_p1);


  protected: 
  TClonesArray* fTofDigi;
  TClonesArray* fTofPoints;
  TClonesArray* fTofMCTrack;

  // Parameter class
  R3BTofDigiPar* fTofDigiPar;

  //- Control Hitograms

  
  TH1F * TOFelosshis;
  
  TH1F * TfwXhis;
  TH1F * TfwYhis;
  TH1F * TfwThis;
  
  Int_t eventNoTof;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BTofDigitizer,1);
  
};

#endif
