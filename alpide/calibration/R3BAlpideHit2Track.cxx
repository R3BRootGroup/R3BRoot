#include "R3BAlpideHit2Track.h"

#include "R3BAlpideHitData.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BTrackingParticle.h"

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

#include <TClonesArray.h>
#include <TMath.h>
#include <TVector3.h>
#include <iostream>

R3BAlpideHit2Track::R3BAlpideHit2Track(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

R3BAlpideHit2Track::~R3BAlpideHit2Track()
{
    if (fAlpideTracks)
        delete fAlpideTracks;
}

InitStatus R3BAlpideHit2Track::Init()
{
    FairRootManager* rootManager = FairRootManager::Instance();
    R3BLOG_IF(fatal, rootManager == nullptr, "FairRootManager not found");

    fHeader = dynamic_cast<R3BEventHeader*>(rootManager->GetObject("EventHeader."));
    R3BLOG_IF(fatal, NULL == fHeader, "EventHeader. not found");

    fAlpideHitData = dynamic_cast<TClonesArray*>(rootManager->GetObject("AlpideHitData"));
    R3BLOG_IF(fatal, !fAlpideHitData, "AlpideHitData not found");

    fAlpideTracks = new TClonesArray("R3BTrackingParticle");
    rootManager->Register("AlpideTrack", "Alpide_tracks", fAlpideTracks, !fOnline);

    NEvents = 0;
    NEventsWithTrack = 0;

    Reset();

    R3BLOG(info, "R3BAlpideHit2Track is initialized");
    return kSUCCESS;
}

InitStatus R3BAlpideHit2Track::ReInit() { return kSUCCESS; }

void R3BAlpideHit2Track::Exec(Option_t* /*option*/)
{
    NEvents++;

    Reset();

    if (fHeader->GetTrigger() != 1)
        return;

    const auto nHits = fAlpideHitData->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    R3BTrackingParticle track;
    if (!BuildTrack(track))
    {
        return;
    }

    AddTrack(track);
    NEventsWithTrack++;
}

Bool_t R3BAlpideHit2Track::BuildTrack(R3BTrackingParticle& outTrack)
{
    AlpideBestHit alp1;
    AlpideBestHit alp2;

    const auto nHits = fAlpideHitData->GetEntriesFast();
    for (int i = 0; i < nHits; ++i)
    {
        auto hit = dynamic_cast<R3BAlpideHitData*>(fAlpideHitData->At(i));
        R3BLOG_IF(fatal, !hit, "ERROR in the alpide hit");

        const int sensorId = hit->GetSensorId();
        const int clusterSize = hit->GetClusterSize();

        if (sensorId <= 6)
        {
            if (clusterSize > alp1.clusterSize)
            {
                alp1.clusterSize = clusterSize;
                alp1.hit = hit;
                alp1.index = i;
            }
        }

        if (sensorId >= 7)
        {
            if (clusterSize > alp2.clusterSize)
            {
                alp2.clusterSize = clusterSize;
                alp2.hit = hit;
                alp2.index = i;
            }
        }
    }

    if (!alp1.hit || !alp2.hit)
    {
        return kFALSE;
    }

    TVector3 p1(alp1.hit->GetX(), alp1.hit->GetY(), alp1.hit->GetZ());
    TVector3 p2(alp2.hit->GetX(), alp2.hit->GetY(), alp2.hit->GetZ());

    TVector3 startPos = p2;

    TVector3 dir = (p2 - p1);
    if (dir.Mag() == 0.)
    {
        return kFALSE;
    }

    dir = dir.Unit();
    dir.SetMag(10.);

    outTrack.SetStartPosition(startPos);
    outTrack.SetStartMomentum(dir);

    outTrack.AddHit("alpide1", alp1.index);
    outTrack.AddHit("alpide2", alp2.index);

    return kTRUE;
}

R3BTrackingParticle* R3BAlpideHit2Track::AddTrack(const R3BTrackingParticle& track)
{
    TClonesArray& clref = *fAlpideTracks;
    int size = clref.GetEntriesFast();
    return new (clref.ConstructedAt(size)) R3BTrackingParticle(track);
}

void R3BAlpideHit2Track::Reset()
{
    R3BLOG(debug, "Clearing AlpideTracks");
    if (fAlpideTracks)
    {
        fAlpideTracks->Clear();
    }
}

void R3BAlpideHit2Track::FinishTask()
{
    std::cout << "\nN events processed = " << NEvents;
    std::cout << "\nN events with Alpide track = " << NEventsWithTrack << "\n";
}

ClassImp(R3BAlpideHit2Track)
