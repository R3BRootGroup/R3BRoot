/** Neuland Monte Carlo Monitor
 * @author Jan Mayer
 * @since  07.07.2015
 *
 *  Input:  Monte Carlo Tracks "MCTrack".
 *  Output: Several histogramms:
 *          - Energy of primary neutrons
 *          - Number of particles by PDG code created by the first neutron interaction
 *          - Energy of non-neutron tracks in kLAND created by primary neutron interaction(s), by PID
 *          - Energy of neutron tracks created by primary neutron interaction
 *          - Total energy of non-neutron tracks in kLAND created by primary neutron interaction(s)
 *          - Total energy of non-neutron tracks in kLAND created by primary neutron interaction(s), by PID
 *          - IDs of tracks with a primary mother
 *          - Distribution of track mother IDs
 */

#ifndef R3BNEULANDMCMON_H
#define R3BNEULANDMCMON_H 1

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "TClonesArray.h"
#include <map>

class TH1D;
class TH3D;

class R3BNeulandMCMon : public FairTask
{
public:
  R3BNeulandMCMon();
  ~R3BNeulandMCMon();

  InitStatus Init();
  void Exec(Option_t* option);
  void Finish();

private:
  TClonesArray* mc_tracks_;
  TH1D* h_PDG_;
  TH1D* h_E_primary_neutrons_;
  TH1D* h_E_tot_;
  TH1D* h_E_secondary_neutrons_;
  TH1D* h_motherIDs_;
  TH1D* h_primary_daughter_IDs_;
  std::map<int, TH1D*> hm_E_PDG_;
  std::map<int, TH1D*> hm_E_tot_PDG_;
  TH3D* h3_;

  // TODO: Thats not the business of this class, should be in R3BMCTrack
  // Note: Reference to the pointer to R3BMCTrack so it can be changed within the function
  inline bool GetMotherTrack(const unsigned int i, R3BMCTrack* &mother_track)
  {
    if (i < mc_tracks_->GetEntries() && i >= 0) {
      mother_track = (R3BMCTrack*)mc_tracks_->At(i);
      return true;
    }
    return false;
  }

  inline bool isPrimaryNeutron(const R3BMCTrack* mc_track)
  {
    if (mc_track->GetPdgCode() == 2112 && mc_track->GetMotherId() == -1) {
      return true;
    }
    return false;
  }

  inline bool isMotherPrimaryNeutron(const R3BMCTrack* mc_track)
  {
    R3BMCTrack* mother_track;
    if (GetMotherTrack(mc_track->GetMotherId(), mother_track) && isPrimaryNeutron(mother_track)) {
      return true;
    }
    return false;
  }

  inline double GetKineticEnergy(const R3BMCTrack* mc_track){
    return (mc_track->GetEnergy() - mc_track->GetMass()) * 1000.;
  }


  ClassDef(R3BNeulandMCMon, 0);
};


#endif //R3BNEULANDMCMON_H