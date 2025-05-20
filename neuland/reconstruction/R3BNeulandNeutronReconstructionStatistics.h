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

#ifndef R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H
#define R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H

#include "FairTask.h"
#include "R3BDataMonitor.h"
#include "R3BIOConnector.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include <Rtypes.h>
#include <RtypesCore.h>
#include <TH1.h>
#include <string>
#include <string_view>
#include <vector>

class R3BNeulandNeutronReconstructionStatistics : public FairTask
{
  public:
    explicit R3BNeulandNeutronReconstructionStatistics(std::string_view primary = "NeulandNeutronClusters",
                                                       std::string_view secondary = "NeulandPrimaryClusters",
                                                       std::string_view predicted = "NeulandSecondaryClusters");

  protected:
    auto Init() -> InitStatus override;
    void Finish() override;

  public:
    void Exec(Option_t* /*option*/) override;

  private:
    int true_positive_ = 0;
    int false_positive_ = 0;
    int false_negative_ = 0;
    int true_negative_ = 0;

    R3B::InputVectorConnector<R3BNeulandCluster> primary_clusters_;
    R3B::InputVectorConnector<R3BNeulandCluster> secondary_clusters_;
    R3B::InputVectorConnector<R3BNeulandNeutron> predicted_neutrons_;
    std::string predicted_name_;

    TH1D* hist_true_positive_ = nullptr;
    TH1D* hist_false_positive_ = nullptr;
    TH1D* hist_false_negative_ = nullptr;
    TH1D* hist_true_negative_ = nullptr;
    TH1D* hist_f1_value_ = nullptr;

    std::vector<int> multiplicities_;
    R3B::DataMonitor data_monitor_;

  public:
    ClassDefOverride(R3BNeulandNeutronReconstructionStatistics, 1);
};

#endif // R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H
