#ifndef R3BCADVETOPOINT_H
#define R3BCADVETOPOINT_H

// C++ headers:
#include <iostream>

// ROOT headers:
#include "TObject.h"
#include "TVector3.h"

// FAIR headers:
#include "FairMCPoint.h"

class R3BCADVETOPoint : public FairMCPoint
{

  public:
    R3BCADVETOPoint()
        : FairMCPoint()
        , fLightYield(0)
    {
    }

    R3BCADVETOPoint(const Int_t trackID,
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

    R3BCADVETOPoint(const FairMCPoint& point, const Double_t lightYield)
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
    ClassDefOverride(R3BCADVETOPoint, 1)
};

std::ostream& operator<<(std::ostream&, const R3BCADVETOPoint&); // Support easy printing

#endif // R3BVETOPOINT_H
