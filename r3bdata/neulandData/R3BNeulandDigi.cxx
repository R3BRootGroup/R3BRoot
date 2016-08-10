#include "R3BNeulandDigi.h"

using namespace std;

ostream& operator<<(ostream& os, const R3BNeulandDigi& digi)
{
    os << "R3BNeulandDigi: NeuLAND Digi in Paddle " << digi.GetPaddle() << std::endl
       << "    TdcL: " << digi.GetTdcL() << "    TdcR: " << digi.GetTdcR() << "    Time: " << digi.GetT() << std::endl
       << "    QdcL: " << digi.GetQdcL() << "    QdcR: " << digi.GetQdcR() << "    Energy: " << digi.GetE() << std::endl
       << "    Position XYZ: " << digi.GetPosition().X() << "    " << digi.GetPosition().Y() << "    "
       << digi.GetPosition().Z() << std::endl;
    return os;
}

void R3BNeulandDigi::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandDigi)
