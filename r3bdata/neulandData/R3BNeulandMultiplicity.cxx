#include "R3BNeulandMultiplicity.h"
#include <algorithm>

R3BNeulandMultiplicity::R3BNeulandMultiplicity()
    : m({})
{
}

R3BNeulandMultiplicity::R3BNeulandMultiplicity(MultiplicityProbabilities mult)
    : m(mult)
{
}

std::ostream& operator<<(std::ostream& os, const R3BNeulandMultiplicity& mult)
{
    os << "R3BNeulandMultiplicity:";
    for (const auto n : mult.m)
    {
        os << "\t" << n;
    }
    os << std::endl;
    return os;
}

void R3BNeulandMultiplicity::Print(const Option_t*) const { std::cout << *this; }

int R3BNeulandMultiplicity::GetMultiplicity() const
{
    return std::distance(m.cbegin(), std::max_element(m.cbegin(), m.cend()));
}

ClassImp(R3BNeulandMultiplicity)
