#ifndef R3BLANDDIGITIZER_H
#define R3BLANDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;


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
  TClonesArray* fLandMCTrack; 

  //- Table data 
//  const Int_t npaddles = 200;  //number of paddles
//  const Int_t nplanes = 10;  //number of planes


  //- Control Hitograms
  TH1F *hPMl;
  TH1F *hPMr;
  TH1F *hTotalEnergy;
  TH1F *hTotalLight;
  TH1F *hMult;
  TH1F *hParticle;
  TH1F *hPaddleEnergy;
  TH1F *hFirstEnergy;

  TH1F *hDeltaPx1;
  TH1F *hDeltaPy1;
  TH1F *hDeltaPz1;
  TH1F *hDeltaPx2;
  TH1F *hDeltaPy2;
  TH1F *hDeltaPz2;
  
  TH2F *hElossLight;
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BLandDigitizer,1);
  
};

#endif
