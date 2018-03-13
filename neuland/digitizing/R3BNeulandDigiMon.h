#ifndef R3BNEULANDDIGIMON_H
#define R3BNEULANDDIGIMON_H

/** Neuland Digi Monitor
 * @author Jan Mayer
 * @since  10.06.2015
 *
 *  Input:  Digis. Should work both on Digis from Monte Carlo simulations
 *          as well as experimental data on digi level.
 *  Output: Vairous diagrams, 3D scatterplot for each event if option 3DTRACK is given.
 */

#include "FairTask.h"

class TClonesArray;
class TH1D;
class TH2D;
class TH3D;

class R3BNeulandDigiMon : public FairTask
{
  public:
    R3BNeulandDigiMon(const TString input = "NeulandDigis",
                      const TString output = "NeulandDigiMon",
                      const Option_t* option = "");
    ~R3BNeulandDigiMon();

    InitStatus Init();
    void Exec(Option_t* option);
    void Finish();

  private:
    TString fInput;
    TString fOutput;

    TClonesArray* fDigis;

    Bool_t fIs3DTrackEnabled;
    TH3D* fh3;

    TH1D* hTime;
    TH1D* hDepth;
    TH1D* hForemostEnergy;
    TH1D* hSternmostEnergy;
    TH2D* hDepthVSForemostEnergy;
    TH2D* hDepthVSSternmostEnergy;
    TH1D* hEtot;
    TH1D* hE;
    TH2D* hDepthVSEtot;
    TH2D* hPosVSEnergy;
    TH2D* hdeltaEE;
    TH1D* hBeta;

    ClassDef(R3BNeulandDigiMon, 0);
};

#endif // R3BNEULANDDIGIMON_H
