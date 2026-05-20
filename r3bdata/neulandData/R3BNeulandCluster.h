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

#ifndef R3BNEULANDCLUSTER_H
#define R3BNEULANDCLUSTER_H

#include "R3BNeulandHit.h"
#include "TObject.h"
#include <Math/Vector3Dfwd.h>
#include <Rtypes.h>
#include <RtypesCore.h>
#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>

class R3BNeulandCluster : public TObject
{
  public:
    R3BNeulandCluster() = default;
    explicit R3BNeulandCluster(const R3BNeulandHit& hit)
        : fHits({ hit })
    {
    }
    R3BNeulandCluster(std::vector<R3BNeulandHit>::iterator begin, std::vector<R3BNeulandHit>::iterator end)
        : fHits(begin, end)
    {
    }
    explicit R3BNeulandCluster(std::vector<R3BNeulandHit> hits)
        : fHits(std::move(hits))
    {
    }

    void Clear(Option_t* /*unused*/) override
    {
        fHits.clear();
        fHits.shrink_to_fit();
    }

    [[nodiscard]] auto GetHits() const -> const std::vector<R3BNeulandHit>& { return fHits; }
    [[nodiscard]] auto GetFirstHit() const -> R3BNeulandHit;
    [[nodiscard]] auto GetLastHit() const -> R3BNeulandHit;
    [[nodiscard]] auto GetMaxEnergyHit() const -> R3BNeulandHit;
    [[nodiscard]] auto GetForemostHit() const -> R3BNeulandHit;
    [[nodiscard]] auto GetPosition() const -> ROOT::Math::XYZVector;
    [[nodiscard]] auto GetT() const -> double;
    [[nodiscard]] auto GetE() const -> double;
    [[nodiscard]] auto GetBeta() const -> double { return GetFirstHit().GetBeta(); }
    [[nodiscard]] auto GetEToF() const -> double { return GetFirstHit().GetEToF(); }
    [[nodiscard]] auto GetSize() const -> std::size_t { return fHits.size(); }
    [[nodiscard]] auto GetEnergyCentroid() const -> ROOT::Math::XYZVector;
    [[nodiscard]] auto GetEnergyMoment() const -> double;
    [[nodiscard]] auto GetRCluster(double beta) const -> double;
    [[nodiscard]] auto GetRECluster(double ekin) const -> double;

    void Print(const Option_t* /*option*/) const override;

    auto operator==(const R3BNeulandCluster& cluster) const -> bool { return this->GetHits() == cluster.GetHits(); }

  private:
    std::vector<R3BNeulandHit> fHits;

    ClassDefOverride(R3BNeulandCluster, 1)
};

auto operator<<(std::ostream&, const R3BNeulandCluster&) -> std::ostream&; // Support easy printing

#endif // R3BNEULANDCLUSTER_H
