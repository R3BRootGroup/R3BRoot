/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef NEULAND_DIGITIZING_TACQUILA_H
#define NEULAND_DIGITIZING_TACQUILA_H

#include "R3BDigitizingChannel.h"
#include "R3BShared.h"
#include "TRandom3.h"
#include "Validated.h"
#include <RtypesCore.h>
#include <memory>
#include <vector>

namespace R3B::Digitizing::Neuland::TacQuila
{
    struct Params
    {
        double fPMTThresh;             // [MeV]
        double fSaturationCoefficient; //
        Bool_t fExperimentalDataIsCorrectedForSaturation;
        double fTimeRes;         // time + Gaus(0., fTimeRes) [ns]
        double fEResRel;         // Gaus(e, fEResRel * e) []
        double fIntegrationTime; // [ns]
        std::shared_ptr<TRandom3> fRnd;

        Params();
    };

    extern const Params TACQUILA_DEFAULT_PARAM;
    class Channel : public Digitizing::AbstractChannel
    {
      public:
        Channel(const Channel&) = delete;
        Channel(Channel&&) = delete;
        auto operator=(const Channel&) -> Channel& = delete;
        auto operator=(Channel&&) -> Channel& = delete;
        explicit Channel(Side, const TacQuila::Params& = TACQUILA_DEFAULT_PARAM);
        ~Channel() override = default;
        void add_signal(Signal newHit) override;
        auto HasFired() -> bool override;
        auto GetQDC() -> double;
        auto GetTDC() -> double;
        auto GetEnergy() -> double;

      private:
        static constexpr double DEFAULT_LAMBDA = 1. / 2.1;
        const TacQuila::Params& par;
        std::vector<Signal> fPMTHits;
        mutable Validated<std::vector<Signal>::const_iterator> cachedFirstHitOverThresh;
        mutable Validated<double> cachedQDC;
        mutable Validated<double> cachedTDC;
        mutable Validated<double> cachedEnergy;

        void extra_reset() override;

        // NOTE: Some expensive calculations and random distributions are cached
        // so they do not need to be recomputed every time a Getter is called
        void construct_hits(Hits& signals) override;

        auto BuildQDC() -> double;
        auto BuildTDC() -> double;
        auto BuildEnergy() -> double;
        auto FindThresholdExceedingHit() const -> std::vector<Signal>::const_iterator;
    };

} // namespace R3B::Digitizing::Neuland::TacQuila

#endif // NEULAND_DIGITIZING_TACQUILA_H
