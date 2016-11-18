#include "R3BNeulandNeutron.h"
#include <cmath>

static const Double_t massNeutron = 939.565379;   // MeV/c²
static const Double_t c = 29.9792458;             // cm/ns
static const Double_t c2 = 898.75517873681758374; // cm²/ns²

Double_t R3BNeulandNeutron::GetGamma() const
{
    const Double_t v2 = GetPosition().Mag2() / std::pow(GetT(), 2); // cm²/ns²
    return 1. / std::sqrt(1. - (v2 / c2));
}

TVector3 R3BNeulandNeutron::GetP() const { return GetPosition() * (GetGamma() * massNeutron / GetT() / c); }

Double_t R3BNeulandNeutron::GetEtot() const { return GetGamma() * massNeutron; }

Double_t R3BNeulandNeutron::GetEkin() const { return (GetGamma() - 1.) * massNeutron; }

std::ostream& operator<<(std::ostream& os, const R3BNeulandNeutron& digi)
{
    os << "R3BNeulandNeutron: XYZTE    " << digi.GetPosition().X() << "    " << digi.GetPosition().Y() << "    "
       << digi.GetPosition().Z() << "    " << digi.GetT() << "    " << digi.GetEkin() << std::endl;
    return os;
}

void R3BNeulandNeutron::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandNeutron)
