// ---------------------------------------------------------
// -----                 R3BStarTrackInfoHit          -----
// -----          Created 09-07-2014 by M.Labiche      -----
// -----                 from R3BLandPmt class         ----- 
//                        Use for calibration
//
// ---------------------------------------------------------

#include "R3BStarTrackInfoHit.h"

R3BStarTrackInfoHit::R3BSrarTrackInfoHit()
    : fModuleId(0)
    , fAsicId(0)
    , fSide(0)
    , fStripId(0)
    , fEnergy(0.)    
    , fTime(0.)
{
}

R3BStarTrackInfoHit::R3BStarTrackInfoHit(Int_t moduleId, Int_t asicId, Int_t side, Int_t stripId, Double_t energy, Int_t time)
    : fModuleId(moduleId)
    , fAsicId(asicId)
    , fSide(side)
    , fStripId(stripId)
    , fEnergy(energy)    
    , fTime(time)
{
}

R3BStarTrackInfoHit::~R3BStarTrackInfoHit()
{
}

ClassImp(R3BStarTrackInfoHit)
