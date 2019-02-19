#ifndef R3BNEULANDTACQUILAMAPPEDDATA_H
#define R3BNEULANDTACQUILAMAPPEDDATA_H

#include "TObject.h"
#include <iostream>

class R3BNeulandTacquilaMappedData : public TObject
{
  public:
    R3BNeulandTacquilaMappedData() = default;
    R3BNeulandTacquilaMappedData(UShort_t clock,
                                 UShort_t tacData,
                                 UShort_t stopT,
                                 UShort_t qdcData,
                                 Int_t plane,
                                 Int_t paddle,
                                 Int_t side);

    UShort_t GetClock() const { return fClock; }
    UShort_t GetTacData() const { return fTacData; }
    UShort_t GetStopT() const { return fStopT; }
    UShort_t GetQdcData() const { return fQdcData; }
    Int_t GetPlane() const { return fPlane; }
    Int_t GetPaddle() const { return fPaddle; }
    Int_t GetSide() const { return fSide; }

    void Print(const Option_t*) const override;

  private:
    UShort_t fClock;
    UShort_t fTacData;
    UShort_t fStopT;
    UShort_t fQdcData;
    Int_t fPlane;
    Int_t fPaddle;
    Int_t fSide;

    ClassDefOverride(R3BNeulandTacquilaMappedData, 1)
};

std::ostream& operator<<(std::ostream&, const R3BNeulandTacquilaMappedData&); // Support easy printing

#endif // R3BNEULANDTACQUILAMAPPEDDATA_H
