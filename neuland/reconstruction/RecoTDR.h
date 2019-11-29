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

#ifndef NEULAND_RECOTDR_H
#define NEULAND_RECOTDR_H

#include "ReconstructionEngine.h"
#include <functional>

class R3BNeulandNeutron2DPar;

namespace Neuland
{
    class RecoTDR : public ReconstructionEngine
    {
      public:
        RecoTDR();
        void Init() override;
        R3BNeulandNeutron2DPar* Get2DPar() const { return fPar; }
        std::vector<R3BNeulandNeutron> GetNeutrons(const std::vector<R3BNeulandCluster*>&) const override;
        void SetBeam(Double_t EBeamMeV, Double_t BetaBeam);
        void SortClustersByRValue(std::vector<R3BNeulandCluster*>&) const;
        void FilterClustersByEnergyDeposit(std::vector<R3BNeulandCluster*>&) const;
        void FilterClustersByBeta(std::vector<R3BNeulandCluster*>&) const;
        void FilterClustersByElasticScattering(std::vector<R3BNeulandCluster*>&) const;
        void PrioritizeTimeWiseFirstCluster(std::vector<R3BNeulandCluster*>&) const;
        UInt_t FindNumberOfNeutrons(std::vector<R3BNeulandCluster*>&) const;

      private:
        R3BNeulandNeutron2DPar* fPar; // non-owning
        Double_t fEBeamMeV;
        Double_t fBetaBeam;
        Double_t fClusterMinE;
    };
} // namespace Neuland

#endif // NEULAND_RECOTDR_H
