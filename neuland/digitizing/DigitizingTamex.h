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

#ifndef R3BROOT_DIGITIZINGTAMEX_H
#define R3BROOT_DIGITIZINGTAMEX_H

#include "DigitizingEngine.h"

namespace Neuland
{
    namespace Tamex
    {
        class Channel : public Digitizing::Channel
        {
          public:
            Channel() = default;
            ~Channel() override = default;
            void AddHit(Double_t mcTime, Double_t mcLight, Double_t dist) override;
            bool HasFired() const override;
            Double_t GetQDC() const override;
            Double_t GetTDC() const override;
            Double_t GetEnergy() const override;
        };

    } // namespace Tamex

    class DigitizingTamex : public DigitizingEngine
    {
      public:
        DigitizingTamex() = default;
        ~DigitizingTamex() override = default;
        std::unique_ptr<Digitizing::Channel> BuildChannel() override;
    };
} // namespace Neuland

#endif // R3BROOT_DIGITIZINGTAMEX_H
