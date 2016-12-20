#ifndef R3BNEULANDCLUSTERMON_H
#define R3BNEULANDCLUSTERMON_H 1

/** Neuland Cluster Monitor
 * @author Jan Mayer
 * @since  10.06.2015
 *
 *  Input:  Clusters. Should work both on Clusters from Monte Carlo simulations
 *          as well as experimental data on digi level.
 *  Output: Soon: Vairous diagrams.
 *          Currently: A 3D scatterplot for each event.
 */

#include "FairTask.h"
#include <vector>
#include "R3BNeulandCluster.h"

class TClonesArray;
class TH1D;
class TH2D;
class TH3D;

class R3BNeulandClusterMon : public FairTask
{
  public:
    R3BNeulandClusterMon(const Option_t* option = "");
    ~R3BNeulandClusterMon();

    InitStatus Init();
    void Exec(Option_t* option);
    void Finish();
    void Reset();

  private:
    TClonesArray* fNeulandClusters;
    std::vector<R3BNeulandCluster> fVectorClusters;

    Bool_t fIs3DTrackEnabled;
    TH3D* fh3;

    TH1D* fhClusters;
    TH1D* fhClusterSize;
    TH1D* fhClusterEnergy;
    TH1D* fhClusterEToF;
    TH1D* fhClusterTime;
    TH2D* fhClusterNumberVSEnergy;
    TH2D* fhClusterEToFVSEnergy;
    TH2D* fhClusterEToFVSTime;
    TH2D* fhClusterEVSTime;

    TH2D* fhClusterForemostMinusCentroidVSEnergy;
    TH2D* fhClusterForemostMinusMaxEnergyDigiPosVSEnergy;

    TH2D* fhClusterCentroidMinusFirstDigiPosVSEnergy;
    TH2D* fhClusterMaxEnergyDigiMinusFirstDigiPosVSEnergy;
    TH2D* fhClusterMaxEnergyDigiMinusCentroidVSEnergy;
    TH2D* fhClusterEnergyMomentVSEnergy;

    ClassDef(R3BNeulandClusterMon, 0);
};

#endif // R3BNEULANDCLUSTERMON_H
