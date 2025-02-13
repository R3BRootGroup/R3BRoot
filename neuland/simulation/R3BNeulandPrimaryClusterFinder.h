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
