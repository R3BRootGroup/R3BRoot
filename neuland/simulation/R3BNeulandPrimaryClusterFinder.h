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

#ifndef R3BROOT_R3BNEULANDPRIMARYCLUSTERFINDER_H
#define R3BROOT_R3BNEULANDPRIMARYCLUSTERFINDER_H

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include <R3BIOConnector.h>

class R3BNeulandPrimaryClusterFinder : public FairTask
{
  public:
    explicit R3BNeulandPrimaryClusterFinder(std::string_view primaryHits = "NeulandPrimaryHits",
                                            std::string_view clusters = "NeulandClusters",
                                            std::string_view primaryClusters = "NeulandPrimaryClusters",
                                            std::string_view secondaryClusters = "NeulandSecondaryClusters");

    ~R3BNeulandPrimaryClusterFinder() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandPrimaryClusterFinder(const R3BNeulandPrimaryClusterFinder&) = delete;
    R3BNeulandPrimaryClusterFinder(R3BNeulandPrimaryClusterFinder&&) = delete;
    auto operator=(const R3BNeulandPrimaryClusterFinder&) -> R3BNeulandPrimaryClusterFinder& = delete;
    auto operator=(R3BNeulandPrimaryClusterFinder&&) -> R3BNeulandPrimaryClusterFinder& = delete;

  protected:
    auto Init() -> InitStatus override;

  public:
    void Exec(Option_t* /*option*/) override;

  private:
    R3B::InputVectorConnector<R3BNeulandHit> fPrimaryHits;
    R3B::InputVectorConnector<R3BNeulandCluster> fClusters;

    R3B::OutputVectorConnector<R3BNeulandCluster> fPrimaryClusters;
    R3B::OutputVectorConnector<R3BNeulandCluster> fSecondaryClusters;

    ClassDefOverride(R3BNeulandPrimaryClusterFinder, 1);
};

#endif // R3BROOT_R3BNEULANDPRIMARYCLUSTERFINDER_H
