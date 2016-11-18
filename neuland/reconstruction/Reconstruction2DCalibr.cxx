#include "Reconstruction2DCalibr.h"

#include <numeric>
#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "FairRtdbRun.h"
#include "R3BNeulandNeutron2DPar.h"

namespace Neuland
{
    Reconstruction2DCalibr::Reconstruction2DCalibr(const Double_t EToFMin,
                                                   const Double_t EToFMax,
                                                   const Double_t EDepMin)
        : fEToFMin(EToFMin)
        , fEToFMax(EToFMax)
        , fEDepMin(EDepMin)
    {
    }

    void Reconstruction2DCalibr::Init()
    {
        FairRuntimeDb* rtdb = FairRuntimeDb::instance();
        if (!rtdb)
        {
            LOG(FATAL) << "Reconstruction2DCalibr: No FairRuntimeDb!" << FairLogger::endl;
        }

        fPar = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");
        if (!fPar)
        {
            LOG(FATAL) << "Reconstruction2DCalibr: No R3BNeulandNeutron2DPar!" << FairLogger::endl;
        }

        // FIXME: FairRuntimeDB needs to be forced to load the Data from the second file with Run Id 1
        rtdb->initContainers(rtdb->getCurrentRun()->getRunId(), 1);
        fPar->printParams();
    }

    std::vector<R3BNeulandNeutron> Reconstruction2DCalibr::GetNeutrons(
        const std::vector<R3BNeulandCluster*>& clusters) const
    {
        const Double_t Etot = std::accumulate(clusters.begin(),
                                              clusters.end(),
                                              Double_t(0.),
                                              [](const Double_t a, const R3BNeulandCluster* b)
                                              {
                                                  return a + b->GetE();
                                              });

        // Get Number of neutrons using the previously calibrated cuts stored in the parameter file
        const UInt_t nNeutrons = fPar->GetNeutronMultiplicity(Etot, clusters.size());

        std::vector<R3BNeulandCluster*> filteredclusters;
        std::copy_if(clusters.begin(),
                     clusters.end(),
                     std::back_inserter(filteredclusters),
                     [&](const R3BNeulandCluster* a)
                     {
                         return a->GetFirstDigi().GetEToF() > fEToFMin && a->GetFirstDigi().GetEToF() < fEToFMax &&
                                a->GetE() > fEDepMin;
                     });

        std::sort(filteredclusters.begin(),
                  filteredclusters.end(),
                  [](const R3BNeulandCluster* a, const R3BNeulandCluster* b)
                  {
                      return a->GetFirstDigi().GetEToF() > b->GetFirstDigi().GetEToF();
                  });

        std::vector<R3BNeulandNeutron> neutrons;
        for (UInt_t n = 0; n < nNeutrons && n < filteredclusters.size(); n++)
        {
            const R3BNeulandDigi digi = filteredclusters.at(n)->GetFirstDigi();
            neutrons.push_back(R3BNeulandNeutron(digi.GetT(), digi.GetPosition()));
        }
        return neutrons;
    }
}
