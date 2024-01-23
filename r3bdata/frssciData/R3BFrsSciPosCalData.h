#ifndef R3BFRSSCIPOSCALITEM_H
#define R3BFRSSCIPOSCALITEM_H

#include "TObject.h"

class R3BFrsSciPosCalData : public TObject
{
  public:
    // Default Constructor
    R3BFrsSciPosCalData();

    // Standard Constructor
    R3BFrsSciPosCalData(UShort_t, Double_t, Float_t, Float_t);

    // Destructor
    virtual ~R3BFrsSciPosCalData() {}

    // Getters
    inline const UShort_t& GetDetector() const { return fDetector; }
    inline const Double_t& GetRawTimeNs() const { return fRawTimeNs; }
    inline const Float_t& GetRawPosNs() const { return fRawPosNs; }
    inline const Float_t& GetCalPosMm() const { return fCalPosMm; }

  private:
    UShort_t fDetector;
    Double_t fRawTimeNs; // 0.5 * (Tright + Tleft) after selection of the multiplicity
    Float_t fRawPosNs;   // Tright - Tleft: x increasing for right to left
    Float_t fCalPosMm;   // calibrated position in Mm

  public:
    ClassDef(R3BFrsSciPosCalData, 2)
};

#endif
