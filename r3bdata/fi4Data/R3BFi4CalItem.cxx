
#include "R3BFi4CalItem.h"

// for the data analysis of the s438b Fi4 detectors. 
// Introduced by Ralf, Jan 2016
// adapted by Joachim March 2016


R3BFi4CalItem::R3BFi4CalItem()
    : fDetector(0)
    , fFiber_n(0)
    , fMPPC_top(0)
    , fMPPC_bottom(0)
    , fEnergy_top(0)
    , fEnergy_bottom(0)
    , fTime_top(0)
    , fTime_bottom(0)
{
}

R3BFi4CalItem::R3BFi4CalItem(	UChar_t detector, 
				UInt_t fiber_n,
				UInt_t mppc_top,
				UInt_t mppc_bottom,
				Float_t energy_top,
				Float_t energy_bottom,
				Float_t time_top,
				Float_t time_bottom)
: fDetector(detector)
  , fFiber_n(fiber_n)
  , fMPPC_top(mppc_top)
  , fMPPC_bottom(mppc_bottom)
  , fEnergy_top(energy_top)
  , fEnergy_bottom(energy_bottom)
  , fTime_top(time_top)
  , fTime_bottom(time_bottom)
  
{
}

ClassImp(R3BFi4CalItem)
