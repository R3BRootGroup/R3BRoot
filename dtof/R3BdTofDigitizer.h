// dTofDigitzer 07 aug 2017 created by Alicia Wongel

#ifndef R3BDTOFDDIGITISER_H
#define R3BDTOFDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BTofdHitData.h"

class TClonesArray;
class TH1F;
class TH2F;
class TRandom3;



class R3BdTofDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BdTofDigitizer();


  /** Destructor **/
  ~R3BdTofDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  /** Setters for sigmas **/
  void SetSigma_y( Float_t sigma_y ) {fsigma_y = sigma_y;}
  void SetSigma_t( Float_t sigma_t ) {fsigma_t = sigma_t;}
  void SetSigma_ELoss( Float_t sigma_ELoss ) {fsigma_ELoss = sigma_ELoss;}

  protected:
  TClonesArray* fdTofPoints;
  TClonesArray* fMCTrack; 
  TClonesArray* fdTofHits;
  TRandom3* fRnd;
  TH1F* fHist1;
  TH1F* fHist2;
  TH1F* fHist3;
    
  Float_t fsigma_y;
  Float_t fsigma_t;
  Float_t fsigma_ELoss;
  
 
  ClassDef(R3BdTofDigitizer,1);
  
};

#endif
