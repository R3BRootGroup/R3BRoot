
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
: R3BHit(ID, x, y, ELoss, tdiff)
{
}

ClassImp(R3BTofdHitData)


