#ifndef R3BROOT_R3BNEULANDMULTIPLICITY_H
#define R3BROOT_R3BNEULANDMULTIPLICITY_H

#include "TObject.h"
#include <array>
#include <iostream>

static constexpr int NEULAND_MAX_MULT = 6 + 1;

class R3BNeulandMultiplicity : public TObject
{
  public:
    using MultiplicityProbabilities = std::array<double, NEULAND_MAX_MULT>;

    R3BNeulandMultiplicity();
    R3BNeulandMultiplicity(MultiplicityProbabilities mult);
    ~R3BNeulandMultiplicity() override = default;

    void Print(const Option_t* = "") const override;

    MultiplicityProbabilities m;
    int GetMultiplicity() const;

    ClassDefOverride(R3BNeulandMultiplicity, 1)
};

std::ostream& operator<<(std::ostream&, const R3BNeulandMultiplicity&); // Support easy printing

#endif // R3BROOT_R3BNEULANDMULTIPLICITY_H
