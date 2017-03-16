#ifndef R3BNEULANDNEUTRONRECONSTRUCTION_H
#define R3BNEULANDNEUTRONRECONSTRUCTION_H

/** Neuland Neutron Reconstruction
 * @author Jan Mayer
 *
 * Work in progress
 */

#include "FairTask.h"
#include "ReconstructionEngine.h"

class TClonesArray;

class R3BNeulandNeutronReconstruction : public FairTask
{
  public:
    R3BNeulandNeutronReconstruction(const TString input = "NeulandClusters", const TString output = "NeulandNeutrons");
    // NOTE: Task consumes engine (sink). TODO: #ROOT6: Replace with uniqe_ptr at some point (#goodsink)
    R3BNeulandNeutronReconstruction(Neuland::ReconstructionEngine* engine,
                                    const TString input = "NeulandClusters",
                                    const TString output = "NeulandNeutrons");
    ~R3BNeulandNeutronReconstruction();

  private:
    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandNeutronReconstruction(const R3BNeulandNeutronReconstruction&);            // copy constructor
    R3BNeulandNeutronReconstruction(R3BNeulandNeutronReconstruction&&);                 // move constructor
    R3BNeulandNeutronReconstruction& operator=(const R3BNeulandNeutronReconstruction&); // copy assignment
    R3BNeulandNeutronReconstruction& operator=(R3BNeulandNeutronReconstruction&&);      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    Neuland::ReconstructionEngine* fEngine;

    TString fInput;
    TString fOutput;

    TClonesArray* fClusters;
    TClonesArray* fNeutrons;

  public:
    ClassDefOverride(R3BNeulandNeutronReconstruction, 0);
};

#endif // R3BNEULANDNEUTRONRECONSTRUCTION_H
