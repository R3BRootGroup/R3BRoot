#ifndef R3BNEULANDCADHIT_H
#define R3BNEULANDCADHIT_H

#include "TObject.h"
#include "TVector3.h"
#include <iostream>

class R3BNeulandCADHit : public TObject
{
  public:
    R3BNeulandCADHit() = default;
    R3BNeulandCADHit(Int_t paddle,
                  Double_t TdcL,
                  Double_t TdcR,
                  Double_t time,
                  Double_t QdcL,
                  Double_t QdcR,
                  Double_t energy,
                  const TVector3& pos,
                  const TVector3& pix);

    Int_t GetPaddle() const { return fPaddle; }
    Double_t GetTdcR() const { return fTdcR; }
    Double_t GetTdcL() const { return fTdcL; }
    Double_t GetT() const { return fT; }
    Double_t GetQdcR() const { return fQdcR; }
    Double_t GetQdcL() const { return fQdcL; }
    Double_t GetE() const { return fE; }
    TVector3 GetPosition() const { return fPosition; }
    TVector3 GetPixel() const { return fPixel; }

    Double_t GetBeta() const;
    Double_t GetEToF(Double_t mass = 939.565379) const;

    void Print(const Option_t*) const override;

    bool operator==(const R3BNeulandCADHit& b) const
    {
        return this->GetPaddle() == b.GetPaddle() && this->GetT() == b.GetT() && this->GetE() == b.GetE() &&
               this->GetPosition() == b.GetPosition();
    }

  private:
    Int_t fPaddle;
    Double_t fTdcL;
    Double_t fTdcR;
    Double_t fT;
    Double_t fQdcL;
    Double_t fQdcR;
    Double_t fE;
    TVector3 fPosition;
    TVector3 fPixel;

    ClassDefOverride(R3BNeulandCADHit, 1)
};

std::ostream& operator<<(std::ostream&, const R3BNeulandCADHit&); // Support easy printing

#endif // R3BNEULANDHIT_H
