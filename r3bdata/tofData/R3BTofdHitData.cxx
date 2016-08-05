
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
						     Double_t tdiff
						     )
    : fTime_ns( t ) 
    , fXpos_cm(x)
    , fYpos_cm(y)
    , fZ(Z)
    , fTimeDiff2Ref_ns( tdiff )
{
}

ClassImp(R3BTofdHitData)


