#ifndef R3B_NEULAND_NEUTRON_RECONSTRUCTION_H
#define R3B_NEULAND_NEUTRON_RECONSTRUCTION_H

/** Neuland Neutron Reconstruction
 * @author Jan Mayer
 *
 * Work in progress
 */

#include "FairTask.h"
#include "R3BNeulandCluster.h"
#include "R3BNeulandNeutron.h"
#include "ReconstructionEngine.h"
#include "TCAConnector.h"
#include <memory>

class R3BNeulandNeutronReconstruction : public FairTask
{
  public:
    R3BNeulandNeutronReconstruction(Neuland::ReconstructionEngine* engine,
                                    const TString input = "NeulandClusters",
                                    const TString output = "NeulandNeutrons");
    ~R3BNeulandNeutronReconstruction() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandNeutronReconstruction(const R3BNeulandNeutronReconstruction&) = delete;            // copy constructor
    R3BNeulandNeutronReconstruction(R3BNeulandNeutronReconstruction&&) = delete;                 // move constructor
    R3BNeulandNeutronReconstruction& operator=(const R3BNeulandNeutronReconstruction&) = delete; // copy assignment
    R3BNeulandNeutronReconstruction& operator=(R3BNeulandNeutronReconstruction&&) = delete;      // move assignment

  protected:
    InitStatus Init() override;

  public:
    void Exec(Option_t*) override;

  private:
    std::unique_ptr<Neuland::ReconstructionEngine> fEngine;
    TCAInputConnector<R3BNeulandCluster> fClusters;
    TCAOutputConnector<R3BNeulandNeutron> fNeutrons;

  public:
    ClassDefOverride(R3BNeulandNeutronReconstruction, 0);
};

#endif // R3B_NEULAND_NEUTRON_RECONSTRUCTION_H
