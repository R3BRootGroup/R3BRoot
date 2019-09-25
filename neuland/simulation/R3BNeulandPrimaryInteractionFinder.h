#ifndef R3BROOT_R3BNEULANDPRIMARYINTERACTIONFINDER_H
#define R3BROOT_R3BNEULANDPRIMARYINTERACTIONFINDER_H

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandPoint.h"
#include "TCAConnector.h"
#include "TH1D.h"
#include "TH2D.h"

class R3BNeulandPrimaryInteractionFinder : public FairTask
{
  public:
    explicit R3BNeulandPrimaryInteractionFinder(TString pointsIn = "NeulandPoints",
                                                TString hitsIn = "NeulandHits",
                                                TString pointsOut = "NeulandPrimaryPoints",
                                                TString hitsOut = "NeulandPrimaryHits");

    ~R3BNeulandPrimaryInteractionFinder() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandPrimaryInteractionFinder(const R3BNeulandPrimaryInteractionFinder&) = delete;
    R3BNeulandPrimaryInteractionFinder(R3BNeulandPrimaryInteractionFinder&&) = delete;
    R3BNeulandPrimaryInteractionFinder& operator=(const R3BNeulandPrimaryInteractionFinder&) = delete;
    R3BNeulandPrimaryInteractionFinder& operator=(R3BNeulandPrimaryInteractionFinder&&) = delete;

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

  private:
    TCAInputConnector<R3BMCTrack> fTracks;
    TCAInputConnector<R3BNeulandPoint> fPointsIn;
    TCAInputConnector<R3BNeulandHit> fHitsIn;

    TCAOutputConnector<R3BNeulandPoint> fPointsOut;
    TCAOutputConnector<R3BNeulandHit> fHitsOut;

    TH1D* fhDistance;
    TH2D* fhPointsVsHits;
    TH2D* fhPointVsHitPaddle;

    ClassDefOverride(R3BNeulandPrimaryInteractionFinder, 0);
};

#endif // R3BROOT_R3BNEULANDPRIMARYINTERACTIONFINDER_H
