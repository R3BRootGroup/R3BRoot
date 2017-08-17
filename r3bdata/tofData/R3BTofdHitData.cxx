
#include "R3BTofdHitData.h"

// for the data analysis of the Tofd detector. 
// Introduced by M.Heil, May 2016


R3BTofdHitData::R3BTofdHitData()
{
}

R3BTofdHitData::R3BTofdHitData(	Double_t t,
						     Double_t x,
						     Double_t y,
						     Double_t Z,
						     Double_t tdiff,
						     Double_t ELoss,
						     Double_t ID
						     )
    : fTime_ns(t) 
    , fXpos_cm(x)
    , fYpos_cm(y)
    , fZ(Z)
    , fTimeDiff2Ref_ns( tdiff )
    , fEnergyLoss( ELoss )
    , fDetectorNumber(ID)
{
}

R3BTofdHitData::R3BTofdHitData(const R3BTofdHitData& right)
    : fTime_ns(right.fTime_ns) 
    , fXpos_cm(right.fXpos_cm)
    , fYpos_cm(right.fYpos_cm)
    , fZ(right.fZ)
    , fTimeDiff2Ref_ns( right.fTimeDiff2Ref_ns )
    , fEnergyLoss( right.fEnergyLoss )
    , fDetectorNumber(right.fDetectorNumber)
{
}

ClassImp(R3BTofdHitData)


