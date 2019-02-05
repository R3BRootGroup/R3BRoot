#ifndef R3BROOT_R3BNEULANDONLINESPECTRA_H
#define R3BROOT_R3BNEULANDONLINESPECTRA_H

#include "FairTask.h"
#include "R3BLosCalData.h"
#include "R3BNeulandCalData.h"
#include "R3BNeulandHit.h"
#include "TCAConnector.h"

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
    static const unsigned int fNPlanes = 8;
    static const unsigned int fNBars = fNPlanes * 100;

    TCAInputConnector<R3BNeulandCalData> fNeulandCalData;
    TCAInputConnector<R3BNeulandHit> fNeulandHits;
    TCAInputConnector<R3BLosCalData> fLosCalData;

    TH1D* hTstart;
    TH1D* hNstart;

    std::array<TH2D*, 2> ahCalTvsBar;
    std::array<TH2D*, 2> ahCalEvsBar;

    std::array<TH2D*, 3> ahHitTvsBar;
    std::array<TH2D*, 3> ahHitEvsBar;

    TH2D* hTdiffvsBar;
    TH2D* hToFvsBar;
    TH2D* hTofvsEhit;

    std::array<TH2D*, fNPlanes> ahXYperPlane;

  private:
    bool IsBeam() const;
    double GetTstart() const;

    ClassDefOverride(R3BNeulandOnlineSpectra, 0)
};

#endif // R3BROOT_R3BNEULANDONLINESPECTRA_H
