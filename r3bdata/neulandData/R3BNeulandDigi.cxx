#include "R3BNeulandDigi.h"

static const Double_t c = 29.97924580000000105;   // cm/ns
static const Double_t c2 = 898.75517873681758374; // cm²/ns²

std::ostream& operator<<(std::ostream& os, const R3BNeulandDigi& digi)
{
    os << "R3BNeulandDigi: NeuLAND Digi in Paddle " << digi.GetPaddle() << std::endl
       << "    TdcL: " << digi.GetTdcL() << "    TdcR: " << digi.GetTdcR() << "    Time: " << digi.GetT() << std::endl
       << "    QdcL: " << digi.GetQdcL() << "    QdcR: " << digi.GetQdcR() << "    Energy: " << digi.GetE() << std::endl
       << "    Position XYZ: " << digi.GetPosition().X() << "    " << digi.GetPosition().Y() << "    "
       << digi.GetPosition().Z() << std::endl;
    return os;
}

Double_t R3BNeulandDigi::GetBeta() const { return GetPosition().Mag() / (GetT() * c); }

Double_t R3BNeulandDigi::GetEToF(const Double_t mass) const
{
    const Double_t v2 = GetPosition().Mag2() / std::pow(GetT(), 2); // cm²/ns²
    const Double_t gamma = 1. / std::sqrt(1. - (v2 / c2));
    return (gamma - 1.) * mass;
}

void R3BNeulandDigi::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandDigi)
