
#include "R3BFi4HitItem.h"

// for the data analysis of the s438b PSPX detectors. 
// Introduced by Ralf, Jan 2016
// adapted by Joachim March 2016

R3BFi4HitItem::R3BFi4HitItem()
    : fDetector(0)
    , fFiber_n(0)
    , fFiber_Xpos(0)
    , fFiber_Ypos(0)
    , fFiber_Zpos(0)
    , fEnergy_top(0)
    , fEnergy_bottom(0)
    , fTime_top(0)
    , fTime_bottom(0)
{
}

R3BFi4HitItem::R3BFi4HitItem(	UInt_t detector, 
				UInt_t fiber,
				Float_t xpos,
				Float_t ypos,
				Float_t zpos,
				Float_t energy_top,
				Float_t energy_bottom,
				Float_t time_top,
				Float_t time_bottom)
: fDetector(detector)
  , fFiber_n(fiber)
  , fFiber_Xpos(xpos)
  , fFiber_Ypos(ypos)
  , fFiber_Zpos(zpos)
  , fEnergy_top(energy_top)
  , fEnergy_bottom(energy_bottom)
  , fTime_top(time_top)
  , fTime_bottom(time_bottom)
  
{
}

ClassImp(R3BFi4HitItem)
