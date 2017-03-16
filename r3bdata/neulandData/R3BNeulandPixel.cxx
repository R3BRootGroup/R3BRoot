#include "R3BNeulandPixel.h"

std::ostream& operator<<(std::ostream& os, const R3BNeulandPixel& pixel)
{
    os << "R3BNeulandPixel: E: " << pixel.GetE() << "   T: " << pixel.GetT() << std::endl
       << "      Position XYZ: " << pixel.GetX() << "    " << pixel.GetY() << "    " << pixel.GetZ() << std::endl;
    return os;
}

void R3BNeulandPixel::Print(const Option_t*) const { std::cout << *this; }

ClassImp(R3BNeulandPixel)
