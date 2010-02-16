

#ifndef R3BNEUTRONTRACK_H
#define R3BNEUTRONTRACK_H


#include "TObject.h"
#include "TVector3.h"



class R3BNeutronTrack : public TObject
{

 public:

  /** Default constructor **/
  R3BNeutronTrack();
  R3BNeutronTrack(Int_t paddle, Double_t tdcR, Double_t tdcL,
	      Double_t qdcR, Double_t qdcL);

  /** Copy constructor **/
  R3BNeutronTrack(const R3BNeutronTrack& point) { *this = point; };

  /** Destructor **/
  virtual ~R3BNeutronTrack();

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;



  void SetTdcR(Double_t time){fTdcR = time;}
  Double_t GetTdcR(){return fTdcR;}

  void SetTdcL(Double_t time){fTdcL = time;}
  Double_t GetTdcL(){return fTdcL;}

  void SetQdcR(Double_t charge){fQdcR = charge;}
  Double_t GetQdcR(){return fQdcR;}

  void SetQdcL(Double_t charge){fQdcL = charge;}
  Double_t GetQdcL(){return fQdcL;}

  void SetPaddleNr(Int_t paddleNb){fPaddleNb = paddleNb;}
  Int_t GetPaddleNr(){return fPaddleNb;}



 protected:

  Int_t fPaddleNb;
  Double32_t fTdcL;
  Double32_t fTdcR;
  Double32_t fQdcL;
  Double32_t fQdcR;

  ClassDef(R3BNeutronTrack,1)

};

#endif
