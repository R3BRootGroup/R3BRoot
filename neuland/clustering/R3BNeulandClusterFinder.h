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

#pragma once

/**
 * NeuLAND cluster finder task
 * @author Jan Mayer
 *
 * For each event, get the R3BNeulandHits and group them into R3BNeulandClusters using the Neuland Clustering Engine.
 *   Input:  Branch NeulandHits    = TClonesArray("R3BNeulandDigi")
 *   Output: Branch NeulandClusters = TClonesArray("R3BNeulandCluster")
 *
 */

#include "ClusteringEngine.h"
#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandHit.h"
#include <R3BIOConnector.h>

constexpr auto CLUSTER_FINDER_DISTANCE = 7.5; // cm

class R3BNeulandClusterFinder : public FairTask
{
  public:
    explicit R3BNeulandClusterFinder(double dist_x = 1. * CLUSTER_FINDER_DISTANCE,
                                     double dist_y = 1. * CLUSTER_FINDER_DISTANCE,
                                     double dist_z = 2. * CLUSTER_FINDER_DISTANCE,
                                     double dist_t = 1.,
                                     std::string_view input = "NeulandHits",
                                     std::string_view output = "NeulandClusters");

  private:
    Neuland::ClusteringEngine<R3BNeulandHit> fClusteringEngine;
    R3B::InputVectorConnector<R3BNeulandHit> fDigis;
    R3B::OutputVectorConnector<R3BNeulandCluster> fClusters;

    std::vector<R3BNeulandHit> neuland_hits_buffer_;
    std::vector<std::vector<R3BNeulandHit>> clustered_hits_buffer_;

    auto Init() -> InitStatus override;
    void Exec(Option_t* /*option*/) override;
    ClassDefOverride(R3BNeulandClusterFinder, 0);
};
