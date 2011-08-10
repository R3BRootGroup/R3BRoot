#ifndef R3BDCH2PDIGITIZER_H
#define R3BDCH2PDDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BDch2pDigiPar.h"
#include "R3BDch2pDigi.h"

class TClonesArray;
class TObjectArray;
class TH1F;
class TH2F;



class R3BDch2pDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BDch2pDigitizer();


  /** Destructor **/
  ~R3BDch2pDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();


  R3BDch2pDigi* AddHit(Int_t pd1mul,Double_t Pdx1_p1,Double_t Pdy1_p1,Double_t Pdx1_p2,Double_t Pdy1_p2,
Int_t pd2mul,Double_t Pdx2_p1,Double_t Pdy2_p1,Double_t Pdx2_p2,Double_t Pdy2_p2);


  protected:
  TClonesArray* fDch2pPoints;
  TClonesArray* fDch2pMCTrack; 
  TClonesArray* fDch2pDigi;

  // Parameter class
  R3BDch2pDigiPar* fDch2pDigiPar;

  //- Control Hitograms
  
  Int_t eventNoDch;
  
  
  private:
  virtual void SetParContainers();

 
  ClassDef(R3BDch2pDigitizer,1);
  
};

#endif
