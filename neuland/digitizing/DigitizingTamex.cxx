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

#include <algorithm>
#include <iostream>

#include "FairLogger.h"

namespace Neuland
{

    Tamex::Params::Params()
        : fPMTThresh(1.)                // [MeV]
        , fSaturationCoefficient(0.012) //
        , fExperimentalDataIsCorrectedForSaturation(true)
        , fTimeRes(0.15) // time + Gaus(0., fTimeRes) [ns]
        , fEResRel(0.05) // Gaus(e, fEResRel * e) []
        , fRnd(new TRandom3())
    {
    }

    Tamex::Channel::Channel(const Tamex::Params& p)
        : par(p)
    {
    }

    void Tamex::Channel::AddHit(Double_t mcTime, Double_t mcLight, Double_t dist)
    {
        fPMTHits.emplace_back(mcTime, mcLight, dist);

        cachedQDC.push_back(Validated<Double_t>());
        cachedTDC.push_back(Validated<Double_t>());
        cachedEnergy.push_back(Validated<Double_t>());
    }

    bool Tamex::Channel::HasFired() const
    {
        Bool_t hasFired = false;
        if (fPMTHits.size() == 0)
        {
            return false;
        }
        else
        {
            // hasFired = (fPMTHits.back().light > par.fPMTThresh);
            hasFired = std::any_of(
                fPMTHits.begin(), fPMTHits.end(), [this](Digitizing::PMTHit e) { return e.light > par.fPMTThresh; });
        }
        return hasFired;
    }

    Int_t Tamex::Channel::GetNHits() const { return fPMTHits.size(); }

    Double_t Tamex::Channel::GetQDC(UShort_t index) const
    {
        if (!HasFired())
        {
            LOG(ERROR) << "Error: Cannot get QDC values from unfired NeuLAND paddle!";
            return 0;
        }

        if (!cachedQDC[index].valid())
        {
            // get light depostion
            Double_t l = fPMTHits[index].light;

            // apply PMT saturation
            l = l / (1. + par.fSaturationCoefficient * l);

            // apply energy smearing
            l = par.fRnd->Gaus(l, par.fEResRel * l);

            // set qdc to zero if below PMT threshold
            l = (l > par.fPMTThresh) ? l : 0.0;

            cachedQDC[index].set(l);
        }
        return cachedQDC[index].get();
    }

    Double_t Tamex::Channel::GetTDC(UShort_t index) const
    {
        if (!HasFired())
        {
            LOG(ERROR) << "Error: Cannot get TDC values from unfired NeuLAND paddle!";
            return 0;
        }
        if (!cachedTDC[index].valid())
        {
            cachedTDC[index].set(fPMTHits.back().time + par.fRnd->Gaus(0., par.fTimeRes));
        }

        return cachedTDC[index].get();
    }

    Double_t Tamex::Channel::GetEnergy(UShort_t index) const
    {
        if (!HasFired())
        {
            LOG(ERROR) << "Error: Cannot get energy values from unfired NeuLAND paddle!";
            return 0;
        }

        if (!cachedEnergy[index].valid())
        {
            Double_t e = GetQDC(index);

            // Apply reverse saturation
            if (par.fExperimentalDataIsCorrectedForSaturation)
            {
                e = e / (1. - par.fSaturationCoefficient * e);
            }
            // Apply reverse attenuation
            e = e * exp((2. * (Digitizing::Paddle::gHalfLength)) * Digitizing::Paddle::gAttenuation / 2.);
            cachedEnergy[index].set(e);
        }
        return cachedEnergy[index].get();
    }

    DigitizingTamex::DigitizingTamex()
        : fTmP(Tamex::Params())
    {
    }

    std::unique_ptr<Digitizing::Channel> DigitizingTamex::BuildChannel()
    {
        return std::unique_ptr<Digitizing::Channel>(new Tamex::Channel(fTmP));
    }

} // namespace Neuland
