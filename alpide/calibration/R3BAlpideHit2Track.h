#pragma once

#include "R3BAlpideHitData.h"
#include "R3BEventHeader.h"
#include "R3BTrackingParticle.h"

#include <FairTask.h>

#include <Rtypes.h>

class TClonesArray;

class R3BAlpideHit2Track : public FairTask
{
  public:
    R3BAlpideHit2Track(const TString& name = "R3BAlpideHit2Track", int iVerbose = 1);

    ~R3BAlpideHit2Track();

    void Exec(Option_t* /*option*/) override;

    virtual void Reset();

    InitStatus Init() override;

    InitStatus ReInit() override;

    void FinishTask() override;

    inline void SetOnline(Bool_t option) { fOnline = option; }

  private:
    struct AlpideBestHit
    {
        R3BAlpideHitData* hit = nullptr;
        int clusterSize = 0;
        int index = -1;
    };

    Bool_t BuildTrack(R3BTrackingParticle& outTrack);
    R3BTrackingParticle* AddTrack(const R3BTrackingParticle& track);

  private:
    TClonesArray* fAlpideHitData = nullptr;
    R3BEventHeader* fHeader = nullptr;

    TClonesArray* fAlpideTracks = nullptr;

    Bool_t fOnline = kFALSE;

    int NEvents = 0;
    int NEventsWithTrack = 0;

  public:
    ClassDefOverride(R3BAlpideHit2Track, 1);
};
