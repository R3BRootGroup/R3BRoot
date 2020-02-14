// Inclusion of its own header:
#include "R3BCADNEBULAVETOPoint.h"

using namespace std;

TVector3 R3BCADNEBULAVETOPoint::GetMomentum() const
{
    TVector3 v;
    // Momentum(v); // TODO: #fairroot: FairMCPoint not const
    v.SetXYZ(fPx, fPy, fPz);
    return v;
}

TVector3 R3BCADNEBULAVETOPoint::GetPosition() const
{
    TVector3 v;
    Position(v);
    return v;
}

ostream& operator<<(ostream& os, const R3BCADNEBULAVETOPoint& point)
{
    os << "R3BCADNEBULAVETOPoint: NeuLAND Point in Paddle " << point.GetPaddle() << " with light yield "
       << point.GetLightYield() << std::endl;
    // TODO: #fairroot: Move this to FairMCPoint
    os << "... with FairMCPoint for track " << point.GetTrackID() << std::endl;
    os << "    Position XYZ: " << point.GetPosition().X() << "    " << point.GetPosition().X() << "    "
       << point.GetPosition().Z() << std::endl;
    os << "    Momentum XYZ: " << point.GetMomentum().X() << "    " << point.GetMomentum().X() << "    "
       << point.GetMomentum().Z() << std::endl;
    os << "    Time: " << point.GetTime() << "    Length: " << point.GetLength()
       << "    Energy loss: " << point.GetEnergyLoss() << std::endl;
    return os;
}

void R3BCADNEBULAVETOPoint::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BCADNEBULAVETOPoint)
