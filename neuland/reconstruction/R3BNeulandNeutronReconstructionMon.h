#ifndef R3BNEULANDNEUTRONRECONSTRUCTIONMON_H
#define R3BNEULANDNEUTRONRECONSTRUCTIONMON_H

/** Neuland Neutron Reconstruction Evaluation
 * @author Jan Mayer
 *
 * Work in progress
 */

#include "FairTask.h"

class TClonesArray;
class TH1D;
class TH2D;

class R3BNeulandNeutronReconstructionMon : public FairTask
{
  public:
    R3BNeulandNeutronReconstructionMon(const TString input = "NeulandNeutrons",
                                       const TString output = "NeulandNeutronReconstructionMon");
    ~R3BNeulandNeutronReconstructionMon();

  private:
    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandNeutronReconstructionMon(const R3BNeulandNeutronReconstructionMon&);            // copy constructor
    R3BNeulandNeutronReconstructionMon(R3BNeulandNeutronReconstructionMon&&);                 // move constructor
    R3BNeulandNeutronReconstructionMon& operator=(const R3BNeulandNeutronReconstructionMon&); // copy assignment
    R3BNeulandNeutronReconstructionMon& operator=(R3BNeulandNeutronReconstructionMon&&);      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    TString fInput;
    TString fOutput;

    TH1D* fhCountN;
    TH1D* fhCountNdiff;
    TH1D* fhScore;
    TH1D* fhEdiff;
    TH1D* fhErel;
    TH1D* fhErelMC;
    TH2D* fhErelVSnNreco;
    TH2D* fhErelVSnNrecoNPNIPs;
    TH2D* fhNreacNreco;

    TClonesArray* fPrimaryNeutronInteractionPoints;
    TClonesArray* fReconstructedNeutrons;
    TClonesArray* fMCTracks;

  public:
    ClassDefOverride(R3BNeulandNeutronReconstructionMon, 0);
};

#endif // R3BNEULANDNEUTRONRECONSTRUCTIONMON_H
