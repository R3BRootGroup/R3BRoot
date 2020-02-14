#include "R3BNeulandCADNeutron.h"
#include <cmath>

static const Double_t massNeutron = 939.565379;   // MeV/c²
static const Double_t c = 29.9792458;             // cm/ns
static const Double_t c2 = 898.75517873681758374; // cm²/ns²

R3BNeulandCADNeutron::R3BNeulandCADNeutron(Int_t paddle, Double_t time, const TVector3& pos, const TVector3& pix)
    : fPaddle(paddle)
    , fT(time)
    , fPosition(pos)
    , fPixel(pix)
{
}

R3BNeulandCADNeutron::R3BNeulandCADNeutron(const R3BNeulandCADHit& hit)
    : fPaddle(hit.GetPaddle())
    , fT(hit.GetT())
    , fPosition(hit.GetPosition())
    , fPixel(hit.GetPixel())
{
}

R3BNeulandCADNeutron::R3BNeulandCADNeutron(const R3BNeulandCADCluster& cluster)
    : R3BNeulandCADNeutron(cluster.GetFirstHit())
{
}

Double_t R3BNeulandCADNeutron::GetGamma() const
{
    const Double_t v2 = GetPosition().Mag2() / std::pow(GetT(), 2); // cm²/ns²
    return 1. / std::sqrt(1. - (v2 / c2));
}

TVector3 R3BNeulandCADNeutron::GetP() const { return GetPosition() * (GetGamma() * massNeutron / GetT() / c); }

Double_t R3BNeulandCADNeutron::GetEtot() const { return GetGamma() * massNeutron; }

Double_t R3BNeulandCADNeutron::GetEkin() const { return (GetGamma() - 1.) * massNeutron; }

std::ostream& operator<<(std::ostream& os, const R3BNeulandCADNeutron& digi)
{
    os << "R3BNeulandCADNeutron: XYZTE    " << digi.GetPosition().X() << "    " << digi.GetPosition().Y() << "    "
       << digi.GetPosition().Z() << "    " << digi.GetT() << "    " << digi.GetEkin() << std::endl;
    return os;
}

void R3BNeulandCADNeutron::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandCADNeutron)
