#ifndef R3BNEULANDDIGI_H
#define R3BNEULANDDIGI_H

#include <iostream>

#include "TObject.h"
#include "TVector3.h"

class R3BNeulandDigi : public TObject
{
  public:
    R3BNeulandDigi()
        : fPaddle(0)
        , fTdcL(0)
        , fTdcR(0)
        , fT(0)
        , fQdcL(0)
        , fQdcR(0)
        , fE(0)
        , fPosition(TVector3(0, 0, 0))
    {
    }

    R3BNeulandDigi(const Int_t paddle,
                   const Double_t TdcL,
                   const Double_t TdcR,
                   const Double_t time,
                   const Double_t QdcL,
                   const Double_t QdcR,
                   const Double_t energy,
                   const TVector3& pos)
        : fPaddle(paddle)
        , fTdcL(TdcL)
        , fTdcR(TdcR)
        , fT(time)
        , fQdcL(QdcL)
        , fQdcR(QdcR)
        , fE(energy)
        , fPosition(pos)
    {
    }

    // Rule of zero: Non-ownership-managing classes should not have custom
    // destructors, copy/move constructors or copy/move assignment operators.

    // TODO: #ROOT6: Declare functions overriding virtual functions overrider
    void Print(const Option_t*) const; // override

    void SetTdcR(const Double_t time) { fTdcR = time; }
    Double_t GetTdcR() const { return fTdcR; }

    void SetTdcL(const Double_t time) { fTdcL = time; }
    Double_t GetTdcL() const { return fTdcL; }

    void SetT(const Double_t time) { fT = time; }
    Double_t GetT() const { return fT; }

    void SetQdcR(const Double_t energy) { fQdcR = energy; }
    Double_t GetQdcR() const { return fQdcR; }

    void SetQdcL(const Double_t energy) { fQdcL = energy; }
    Double_t GetQdcL() const { return fQdcL; }

    void SetE(const Double_t energy) { fE = energy; }
    Double_t GetE() const { return fE; }

    void SetPaddle(const Int_t paddle) { fPaddle = paddle; }
    Int_t GetPaddle() const { return fPaddle; }

    void SetPosition(const TVector3& position) { fPosition = position; }
    TVector3 GetPosition() const { return fPosition; }

    Double_t GetBeta() const;
    Double_t GetEToF(const Double_t mass = 939.565379) const;

    bool operator==(const R3BNeulandDigi& b) const
    {
        return this->GetPaddle() == b.GetPaddle() && this->GetT() == b.GetT() && this->GetE() == b.GetE() &&
               this->GetPosition() == b.GetPosition();
    }

  protected:
    Int_t fPaddle;
    Double_t fTdcL;
    Double_t fTdcR;
    Double_t fT;
    Double_t fQdcL;
    Double_t fQdcR;
    Double_t fE;
    TVector3 fPosition;

  public:
    ClassDef(R3BNeulandDigi, 1)
};

// Support easy printing with R3BNeulandDigi mydigi; cout << mydigi;
std::ostream& operator<<(std::ostream&, const R3BNeulandDigi&);

#endif // R3BNEULANDDIGI_H
