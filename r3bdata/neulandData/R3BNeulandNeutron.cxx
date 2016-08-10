#include "R3BNeulandNeutron.h"

using namespace std;

ostream& operator<<(ostream& os, const R3BNeulandNeutron& digi)
{
    os << "R3BNeulandNeutron: XYZTE    " << digi.GetPosition().X() << "    " << digi.GetPosition().Y() << "    "
       << digi.GetPosition().Z() << "    " << digi.GetT() << "    " << digi.GetE() << std::endl;
    return os;
}

void R3BNeulandNeutron::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandNeutron)
