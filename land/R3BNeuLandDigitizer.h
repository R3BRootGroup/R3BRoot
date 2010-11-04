#ifndef R3BNEULANDDIGITIZER_H
#define R3BNEULANDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;


class R3BNeuLandDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BNeuLandDigitizer();


  /** Destructor **/
  ~R3BNeuLandDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();

  protected:
  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 


  //- Control Hitograms
  TH1F *h_time;
  TH1F *h_xAv;
  TH1F *h_yAv;
  TH1F *h_eDepTot;

  private:
  virtual void SetParContainers();

 
  ClassDef(R3BNeuLandDigitizer,1);
  
};

#endif
