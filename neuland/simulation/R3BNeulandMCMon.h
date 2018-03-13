/** Neuland Monte Carlo Monitor
 * @author Jan Mayer
 * @since  07.07.2015
 *
 *  Input:  Monte Carlo Tracks "MCTrack".
 *  Output: Several histograms:
 *          - Energy of primary neutrons
 *          - Number of particles by PDG code created by the first neutron interaction
 *          - Energy of non-neutron tracks in kLAND created by primary neutron interaction(s), by PID
 *          - Energy of neutron tracks created by primary neutron interaction
 *          - Total energy of non-neutron tracks in kLAND created by primary neutron interaction(s)
 *          - Total energy of non-neutron tracks in kLAND created by primary neutron interaction(s), by PID
 *          - IDs of tracks with a primary mother
 *          - Distribution of track mother IDs
 *          - ...
 *          - Option FULLSIMANA: Create histograms for reactions products of primary neutron interactions
 *            Grouped by Pdg, Reaction and Reaction&Pdg (several 1000 histograms)
 *          - Option 3DTRACK: Prepare 3D Histogram for each event.
 *  Usage:  run->AddTask(new R3BNeulandMCMon("FULLSIMANA"));
 *          Requires MCTracks, NeulandPoints and NPNIPS from R3BNeuland simulation.
 */

#ifndef R3BNEULANDMCMON_H
#define R3BNEULANDMCMON_H 1

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandPoint.h"
#include "TClonesArray.h"
#include <map>

class TH1D;
class TH2D;
class TH3D;

class R3BNeulandMCMon : public FairTask
{
  public:
    R3BNeulandMCMon(const Option_t* option = "");

    InitStatus Init() override;
    void Exec(Option_t* option) override;
    void Finish() override;

  private:
    Bool_t fIs3DTrackEnabled;
    Bool_t fIsFullSimAnaEnabled;

    TClonesArray* fMCTracks;
    TClonesArray* fNeulandPoints;
    TClonesArray* fNPNIPs;
    TH1D* fhPDG;
    TH1D* fhEPrimarys;
    TH1D* fhEPrimaryNeutrons;
    TH1D* fhEtot;
    TH1D* fhEtotPrim;
    TH1D* fhESecondaryNeutrons;
    TH1D* fhMotherIDs;
    TH1D* fhPrimaryDaughterIDs;
    TH1D* fhMCToF;
    TH1D* fhnNPNIPs;
    TH2D* fhNPNIPsEToFVSTime;
    TH2D* fhNPNIPSrvsz;
    TH2D* fhNPNIPSxy;
    TH2D* fhThetaLight;
    std::map<Int_t, TH1D*> fhmEPdg;
    std::map<Int_t, TH1D*> fhmEtotPdg;
    std::map<Int_t, TH1D*> fhmEtotPdgRel;

    std::map<Int_t, TH1D*> fhmEnergyByProductPdg;
    std::map<TString, TH1D*> fhmEnergyByReaction;
    std::map<TString, std::map<Int_t, TH1D*>> fhmmEnergyByReactionByProductPdg;
    TH1D* fhnProducts;
    TH1D* fhnProductsCharged;
    TH1D* fhSumProductEnergy;
    TH1D* fhnSecondaryProtons;
    TH1D* fhnSecondaryNeutrons;

    TH3D* fh3;

    // TODO: Thats not the business of this class, should be in R3BMCTrack
    // Note: Reference to the pointer to R3BMCTrack so it can be changed within the function
    inline Bool_t GetMotherTrack(const Int_t i, R3BMCTrack*& motherTrack)
    {
        if (i < fMCTracks->GetEntries() && i >= 0)
        {
            motherTrack = (R3BMCTrack*)fMCTracks->At(i);
            return true;
        }
        return false;
    }

    inline Bool_t IsPrimaryNeutron(const R3BMCTrack* mcTrack)
    {
        if (mcTrack->GetPdgCode() == 2112 && mcTrack->GetMotherId() == -1)
        {
            return true;
        }
        return false;
    }

    inline Bool_t IsMotherPrimaryNeutron(const R3BMCTrack* mcTrack)
    {
        R3BMCTrack* motherTrack;
        if (GetMotherTrack(mcTrack->GetMotherId(), motherTrack) && IsPrimaryNeutron(motherTrack))
        {
            return true;
        }
        return false;
    }

    inline Double_t GetKineticEnergy(const R3BMCTrack* mcTrack)
    {
        return (mcTrack->GetEnergy() - mcTrack->GetMass()) * 1000.;
    }

    inline Double_t GetTheta(const R3BMCTrack* mcTrack) { return std::acos(mcTrack->GetPz() / mcTrack->GetP()); }

    ClassDefOverride(R3BNeulandMCMon, 0)
};

#endif // R3BNEULANDMCMON_H
