#ifndef R3BNEULANDCADNEUTRON_H
#define R3BNEULANDCADNEUTRON_H

#include "R3BNeulandCADCluster.h"
#include "R3BNeulandCADHit.h"
#include "TObject.h"
#include "TVector3.h"
#include <iostream>

class R3BNeulandCADNeutron : public TObject
{
  public:
    R3BNeulandCADNeutron() = default;
    R3BNeulandCADNeutron(Int_t paddle, Double_t time, const TVector3& pos, const TVector3& pix);
    explicit R3BNeulandCADNeutron(const R3BNeulandCADHit&);
    explicit R3BNeulandCADNeutron(const R3BNeulandCADCluster&);

    Int_t GetPaddle() const { return fPaddle; }
    Double_t GetT() const { return fT; }
    TVector3 GetPosition() const { return fPosition; }
    TVector3 GetPixel() const { return fPixel; }

    Double_t GetEtot() const;
    Double_t GetEkin() const;
    Double_t GetGamma() const;
    TVector3 GetP() const;

    void Print(const Option_t*) const override;

  private:
    Int_t fPaddle;
    Double_t fT;
    TVector3 fPosition;
    TVector3 fPixel;

    ClassDefOverride(R3BNeulandCADNeutron, 1)
};

std::ostream& operator<<(std::ostream&, const R3BNeulandCADNeutron&); // Support easy printing

#endif // R3BNEULANDNEUTRON_H
