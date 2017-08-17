// R3BsfiDigitizer.h 25. 08. 2017.
// Created by: Aron Kripko

#ifndef R3BSFIDIGITISER_H
#define R3BSFIDIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BFi4HitItem.h"
#include <TRandom3.h>


class TClonesArray;
class TH1F;
class TH2F;



class R3BsfiDigitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BsfiDigitizer();
  
  
  R3BsfiDigitizer(Double_t esigma, Double_t tsigma);


  /** Destructor **/
  ~R3BsfiDigitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();
  
  void SetEnergyResolution(Double_t e);
  void SetTimeResolution(Double_t t);
  

  protected:
  
	  TClonesArray* fPoints;
	  
	  TClonesArray* fMCTrack; 
	  
	  TClonesArray* Hits;
	  
  private:
  
	TRandom3* prnd;
	Double_t esigma;
	Double_t tsigma;
	Int_t NumOfFibers;
	
	
   ClassDef(R3BsfiDigitizer,1);
  
};

#endif
