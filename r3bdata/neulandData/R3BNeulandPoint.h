#ifndef R3BNEULANDPOINT_H
#define R3BNEULANDPOINT_H

#include "FairMCPoint.h"
#include "TObject.h"
#include "TVector3.h"
#include <iostream>

class R3BNeulandPoint : public FairMCPoint
{

  public:
    R3BNeulandPoint()
        : FairMCPoint()
        , fLightYield(0)
    {
    }

    R3BNeulandPoint(const Int_t trackID,
                    const Int_t detID,
                    const TVector3& pos,
                    const TVector3& mom,
                    const Double_t tof,
                    const Double_t length,
                    const Double_t eLoss,
                    const UInt_t EventId,
                    const Double_t lightYield)
        : FairMCPoint(trackID, detID, pos, mom, tof, length, eLoss, EventId)
        , fLightYield(lightYield)
    {
    }

    R3BNeulandPoint(const FairMCPoint& point, const Double_t lightYield)
        : FairMCPoint(point)
        , fLightYield(lightYield)
    {
    }

    TVector3 GetMomentum() const;
    TVector3 GetPosition() const;
    Int_t GetPaddle() const { return GetDetectorID(); }
    Double_t GetLightYield() const { return fLightYield; }

    void Print(const Option_t*) const override;

  protected:
    Double_t fLightYield;

  public:
    ClassDefOverride(R3BNeulandPoint, 1)
};

std::ostream& operator<<(std::ostream&, const R3BNeulandPoint&); // Support easy printing

#endif // R3BNEULANDPOINT_H
