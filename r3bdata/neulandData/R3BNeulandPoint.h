#ifndef R3BNEULANDPOINT_H
#define R3BNEULANDPOINT_H

#include <iostream>

#include "TObject.h"
#include "TVector3.h"

#include "FairMCPoint.h"

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
                    const TVector3 pos,
                    const TVector3 mom,
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

    // Rule of zero: Non-ownership-managing classes should not have custom
    // destructors, copy/move constructors or copy/move assignment operators.

    // TODO: #ROOT6: Declare functions overrriding virtual functions overrride
    void Print(const Option_t*) const;

    TVector3 GetMomentum() const;
    TVector3 GetPosition() const;

    void SetPaddle(const Int_t paddle) { SetDetectorID(paddle); }
    Int_t GetPaddle() const { return GetDetectorID(); }

    void SetLightYield(const Double_t light) { fLightYield = light; }
    Double_t GetLightYield() const { return fLightYield; }

  protected:
    Double_t fLightYield;

  public:
    ClassDef(R3BNeulandPoint, 1)
};

// Support easy printing with R3BNeulandPoint mypoint; cout << mypoint;
std::ostream& operator<<(std::ostream&, const R3BNeulandPoint&);

#endif // R3BNEULANDPOINT_H
