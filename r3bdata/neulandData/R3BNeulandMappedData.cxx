#include "R3BNeulandMappedData.h"

R3BNeulandMappedData::R3BNeulandMappedData(UShort_t clock,
                                           UShort_t tacData,
                                           UShort_t stopT,
                                           UShort_t qdcData,
                                           Int_t plane,
                                           Int_t paddle,
                                           Int_t side)
    : fClock(clock)
    , fTacData(tacData)
    , fStopT(stopT)
    , fQdcData(qdcData)
    , fPlane(plane)
    , fPaddle(paddle)
    , fSide(side)
{
}

std::ostream& operator<<(std::ostream& os, const R3BNeulandMappedData& mappedData)
{
    os << "R3BNeulandMappedData:"
       << "    Clock " << mappedData.GetClock() << "    TacData " << mappedData.GetTacData() << "    StopT "
       << mappedData.GetStopT() << "    QdcData " << mappedData.GetQdcData() << "    Plane " << mappedData.GetPlane()
       << "    Paddle " << mappedData.GetPaddle() << "    Side " << mappedData.GetSide() << std::endl;
    return os;
}

void R3BNeulandMappedData::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandMappedData)
