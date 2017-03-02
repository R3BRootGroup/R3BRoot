// ---------------------------------------------------------
// -----               R3BStarTrackInfoHit            -----
// -----          Created 09-04-2014 by M.Labiche      -----
// -----             Based on R3BLandPmt class         -----        
// ---------------------------------------------------------

#ifndef R3BSTARTRACKINFOHIT
#define R3BSTARTRACKINFOHIT

#include "TObject.h"

class R3BStarTrackInfoHit : public TObject
{
  public:
    R3BStarTrackInfoHit();

    R3BStarTrackInfoHit(Int_t moduleId, Int_t asicId, Int_t side, Int_t stripId, Double_t energy, Int_t time);

    virtual ~R3BStarTrackInfoHit();

    inline Int_t GetModuleId() const
    {
        return fModuleId;
    }
    inline Int_t GetAsicId() const
    {
        return fAsicId;
    }
    inline Int_t GetSide() const
    {
        return fSide;
    }
    inline Int_t GetStripId() const
    {
        return fStripId;
    }
    inline Double_t GetEnergy() const
    {
        return fEnergy;
    }
    inline Int_t GetTime() const
    {
        return fTime;
    }


  private:
    Int_t fModuleId;
    Int_t fAsicId;
    Int_t fSide;
    Int_t fStripId;
    Double_t fEnergy;
    Int_t fTime;

  public:
    ClassDef(R3BStarTrackInfoHit, 1);
};

#endif
