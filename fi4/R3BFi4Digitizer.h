// R3BFi4Digitizer.h 07. 08. 2017.
// Created by: Aron Kripko

#ifndef R3BFi4DIGITISER_H
#define R3BFI4DIGITISER_H 1


#include "FairTask.h"
#include <map>
#include <string>
#include "R3BFi4HitItem.h"
#include <TRandom3.h>


class TClonesArray;
class TH1F;
class TH2F;



class R3BFi4Digitizer : public FairTask
{

 public:

  /** Default constructor **/  
  R3BFi4Digitizer();
  
  
  R3BFi4Digitizer(Double_t esigma, Double_t tsigma, Double_t ysigma);


  /** Destructor **/
  ~R3BFi4Digitizer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  virtual void Reset();
  
  void SetEnergyResolution(Double_t e);
  void SetTimeResolution(Double_t t);
  void SetYPositionResolution(Double_t y);
  

  protected:
  
	  TClonesArray* fFi4Points;
	  TClonesArray* fFi6Points;
	  TClonesArray* fFi5Points;
	  
	  TClonesArray* fMCTrack; 
	  
	  TClonesArray* fFi4Hits;
	  TClonesArray* fFi6Hits;
	  TClonesArray* fFi5Hits;
	  
  private:
  
	TRandom3* prnd;
	Double_t esigma;
	Double_t tsigma;
	Double_t ysigma;
	
	
   ClassDef(R3BFi4Digitizer,1);
  
};

#endif
