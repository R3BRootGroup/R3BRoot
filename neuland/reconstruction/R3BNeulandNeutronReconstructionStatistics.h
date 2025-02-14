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
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include "TCAConnector.h"
#include <TH1.h>
#include <iostream>

class R3BNeulandNeutronReconstructionStatistics : public FairTask
{
  public:
    explicit R3BNeulandNeutronReconstructionStatistics(TString primary = "NeulandNeutronClusters",
                                                       TString secondary = "NeulandPrimaryClusters",
                                                       TString predicted = "NeulandSecondaryClusters",
                                                       std::ostream& out = std::cout);

  protected:
    auto Init() -> InitStatus override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    TCAInputConnector<R3BNeulandCluster> fPrimaryClusters;
    TCAInputConnector<R3BNeulandCluster> fSecondaryClusters;
    TCAInputConnector<R3BNeulandNeutron> fPredictedNeutrons;
    TString fPredictedName;
    TH1D* fhTP;
    TH1D* fhFP;
    TH1D* fhFN;
    TH1D* fhTN;
    TH1D* fhF1;
    int fTP;
    int fFP;
    int fFN;
    int fTN;
    std::ostream& fOut;
    std::vector<int> fMult;

  public:
    ClassDefOverride(R3BNeulandNeutronReconstructionStatistics, 0);
};

#endif // R3BROOT_R3BNEULANDNEUTRONRECONSTRUCTIONSTATISTICS_H
