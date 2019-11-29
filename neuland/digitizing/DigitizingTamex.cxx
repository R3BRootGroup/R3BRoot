/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

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
