#ifndef R3BFRSSCICALITEM_H
#define R3BFRSSCICALITEM_H

#include "TObject.h"

class R3BFrsSciTofCalData : public TObject
{
  public:
    // Default Constructor
    R3BFrsSciTofCalData();

    // Standard Constructor
    R3BFrsSciTofCalData(UShort_t, UShort_t, UShort_t, Float_t, Float_t, Double_t, Double_t, Double_t);

    // Destructor
    virtual ~R3BFrsSciTofCalData() {}

    // Getters
    inline const UShort_t& GetDetIdSta() const { return fDetIdSta; }
    inline const UShort_t& GetDetIdSto() const { return fDetIdSto; }
    inline const Float_t& GetCalPosStaMm() const { return fCalPosStaMm; }
    inline const Float_t& GetCalPosStoMm() const { return fCalPosStoMm; }
    inline const Double_t& GetRawTofNs() const { return fRawTofNs; }
    inline const Double_t& GetCalTofNs() const { return fCalTofNs; }
    inline const Double_t& GetBeta() const { return fBeta; }

  private:
    UShort_t fRank;
    UShort_t fDetIdSta;
    UShort_t fDetIdSto;
    Float_t fCalPosStaMm;
    Float_t fCalPosStoMm;
    Double_t fRawTofNs;
    Double_t fCalTofNs;
    Double_t fBeta;

  public:
    ClassDef(R3BFrsSciTofCalData, 2)
};

#endif
