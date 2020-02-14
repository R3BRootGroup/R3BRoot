#include "R3BNeulandCADHit.h"

constexpr Double_t c = 29.97924580000000105;   // cm/ns
constexpr Double_t c2 = 898.75517873681758374; // cm²/ns²

R3BNeulandCADHit::R3BNeulandCADHit(Int_t paddle,
                             Double_t TdcL,
                             Double_t TdcR,
                             Double_t time,
                             Double_t QdcL,
                             Double_t QdcR,
                             Double_t energy,
                             const TVector3& pos,
                             const TVector3& pix)
    : fPaddle(paddle)
    , fTdcL(TdcL)
    , fTdcR(TdcR)
    , fT(time)
    , fQdcL(QdcL)
    , fQdcR(QdcR)
    , fE(energy)
    , fPosition(pos)
    , fPixel(pix)
{
}

Double_t R3BNeulandCADHit::GetBeta() const { return GetPosition().Mag() / (GetT() * c); }

Double_t R3BNeulandCADHit::GetEToF(const Double_t mass) const
{
    const Double_t v2 = GetPosition().Mag2() / std::pow(GetT(), 2); // cm²/ns²
    const Double_t gamma = 1. / std::sqrt(1. - (v2 / c2));
    return (gamma - 1.) * mass;
}

std::ostream& operator<<(std::ostream& os, const R3BNeulandCADHit& hit)
{
    os << "R3BNeulandCADHit: NeuLAND Hit in Paddle " << hit.GetPaddle() << std::endl
       << "    TdcL: " << hit.GetTdcL() << "    TdcR: " << hit.GetTdcR() << "    Time: " << hit.GetT() << std::endl
       << "    QdcL: " << hit.GetQdcL() << "    QdcR: " << hit.GetQdcR() << "    Energy: " << hit.GetE() << std::endl
       << "    Position XYZ: " << hit.GetPosition().X() << "    " << hit.GetPosition().Y() << "    "
       << hit.GetPosition().Z() << std::endl;
    return os;
}

void R3BNeulandCADHit::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandCADHit)
