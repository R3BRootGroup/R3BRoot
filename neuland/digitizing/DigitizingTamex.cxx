#include "DigitizingTamex.h"
#include <iostream>

namespace Neuland
{
    void Tamex::Channel::AddHit(Double_t mcTime, Double_t mcLight, Double_t dist)
    {
        std::cout << "Tamex::Channel::AddHit Not Yet Implemented " << mcTime << " " << mcLight << " " << dist
                  << std::endl;
    }

    bool Tamex::Channel::HasFired() const
    {
        std::cout << "Tamex::Channel::HasFired Not Yet Implemented " << std::endl;
        return false;
    }

    Double_t Tamex::Channel::GetQDC() const
    {
        std::cout << "Tamex::Channel::GetQDC Not Yet Implemented " << std::endl;
        return 0;
    }

    Double_t Tamex::Channel::GetTDC() const
    {
        std::cout << "Tamex::Channel::GetTDC Not Yet Implemented " << std::endl;
        return 0;
    }

    Double_t Tamex::Channel::GetEnergy() const
    {
        std::cout << "Tamex::Channel::GetEnergy Not Yet Implemented " << std::endl;
        return 0;
    }

    std::unique_ptr<Digitizing::Channel> DigitizingTamex::BuildChannel()
    {
        return std::unique_ptr<Digitizing::Channel>(new Tamex::Channel());
    }

} // namespace Neuland
