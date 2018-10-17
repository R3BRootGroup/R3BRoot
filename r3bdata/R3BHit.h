// -----------------------------------------------------------------------------
// -----                              R3BHit                               -----
// -----                 Created on 01.03.2018 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#ifndef R3BHIT_H
#define R3BHIT_H

#include "TObject.h"

class R3BHit : public TObject {
public:
    R3BHit();
    R3BHit(Int_t detId, Double_t x, Double_t y, Double_t eloss, Double_t time);
    virtual ~R3BHit();
    
    inline const Int_t& GetDetId() const { return fDetId; }
    inline const Double_t& GetX() const { return fX; }
    inline const Double_t& GetY() const { return fY; }
    inline const Double_t& GetEloss() const { return fEloss; }
    inline const Double_t& GetTime() const { return fTime; }
    
    inline void SetEloss(const Double_t& eloss) { fEloss = eloss; }
    inline void SetTime(const Double_t& time) { fTime = time; }
    
protected:
    Int_t fDetId;
    Double_t fX;
    Double_t fY;
    Double_t fEloss;
    Double_t fTime;
    
public:
    ClassDef(R3BHit, 1)
};

#endif
