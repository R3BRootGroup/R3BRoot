#include "R3BNeulandCalData.h"

R3BNeulandCalData::R3BNeulandCalData(Int_t barId, Int_t side, Double_t time, Int_t qdc)
    : fBarId(barId)
    , fSide(side)
    , fTime(time)
    , fQdc(qdc)
{
}

std::ostream& operator<<(std::ostream& os, const R3BNeulandCalData& calData)
{
    os << "R3BNeulandCalData: BarID " << calData.GetBarId() << "    Side " << calData.GetSide() << "    Time "
       << calData.GetTime() << "    QDC " << calData.GetQdc() << std::endl;
    return os;
}

void R3BNeulandCalData::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandCalData)
