#ifndef R3BROOT_R3BNEULANDONLINESPECTRA_H
#define R3BROOT_R3BNEULANDONLINESPECTRA_H

#include "FairTask.h"
#include "R3BLosCalData.h"
#include "R3BNeulandCalData.h"
#include "R3BNeulandHit.h"
#include "R3BPaddleTamexMappedData.h"
#include "TCAConnector.h"
#include <array>

class TCanvas;
class TH1D;
class TH2D;

class R3BNeulandOnlineSpectra : public FairTask
{

  public:
    R3BNeulandOnlineSpectra();

    InitStatus Init() override;
    void Exec(Option_t*) override;
    void FinishTask() override;

    void ResetHistos();

  private:
    static const unsigned int fNPlanes = 16;
    static const unsigned int fNBars = fNPlanes * 50;

    TCAInputConnector<R3BPaddleTamexMappedData> fNeulandMappedData;
    TCAInputConnector<R3BNeulandCalData> fNeulandCalData;
    TCAInputConnector<R3BNeulandHit> fNeulandHits;
    TCAInputConnector<R3BLosCalData> fLosCalData;

    TH1D* hTstart;
    TH1D* hNstart;

    std::array<TH1D*, 4> ahMappedBar1;
    std::array<TH1D*, 4> ahMappedBar2;

    std::array<TH2D*, 2> ahCalTvsBar;
    std::array<TH2D*, 2> ahCalEvsBar;

    TH2D* hHitEvsBar;

    TH2D* hHitEvsBarCosmics;

    TH2D* hTdiffvsBar;
    TH2D* hToFvsBar;
    TH2D* hTofvsEhit;

    TH2D* hTdiffvsBarCosmics;
    TH2D* hDT375;
    TH2D* hDT425;

    std::array<TH2D*, fNPlanes> ahXYperPlane;

  private:
    bool IsBeam() const;
    double GetTstart() const;

    ClassDefOverride(R3BNeulandOnlineSpectra, 0)
};

#endif // R3BROOT_R3BNEULANDONLINESPECTRA_H
