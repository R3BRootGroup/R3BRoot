// -----------------------------------------------------------------------------
// -----                        R3BPrimPart                                -----
// -----                 Created 12-07-2011 by D.Kresan                    -----
// -----------------------------------------------------------------------------

#ifndef R3BPRIMPART
#define R3BPRIMPART

#include "TObject.h"
#include "TVector3.h"



class R3BPrimPart : public TObject
{
 public:
  R3BPrimPart();
  R3BPrimPart(Int_t pdg, Double_t px, Double_t py, Double_t pz,
	      Double_t x, Double_t y, Double_t z, Double_t t,
	      Double_t A, Double_t M);
  virtual ~R3BPrimPart();

  Int_t GetPdgCode() const;
  void Momentum(TVector3 &mom) const;
  void Position(TVector3 &pos) const;
  Double_t GetT() const;
  Double_t GetA() const;
  Double_t GetM() const;
  Double_t GetE() const;
  Double_t GetM2() const;
  Double_t GetBeta() const;
  Double_t GetGamma() const;

  void SetPz(const Double_t& pz);
  void SetA(const Double_t& a);
  void SetM(const Double_t& m);

 private:
  Int_t fPdg;
  Double_t fPx;
  Double_t fPy;
  Double_t fPz;
  Double_t fX;
  Double_t fY;
  Double_t fZ;
  Double_t fT;
  Double_t fA;
  Double_t fM;

 public:
  ClassDef(R3BPrimPart, 1)
};



#endif
