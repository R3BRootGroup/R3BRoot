// Inclusion of the own header-file:
#include "R3BCADNEBULA.h"

// Initialize variables from Birk' s Law
static constexpr Double_t BirkdP = 1.032;
static constexpr Double_t BirkC1 = 0.013 / BirkdP;
static constexpr Double_t BirkC2 = 9.6e-6 / (BirkdP * BirkdP);

inline Double_t GetLightYield(const Int_t charge, const Double_t length, const Double_t edep)
{
    // Apply Birk's law ( Adapted from G3BIRK/Geant3)
    if (charge != 0 && length > 0)
    {
        Double_t birkC1Mod = BirkC1;

        // Apply correction for higher charge states
        if (TMath::Abs(charge) >= 2)
        {
            birkC1Mod *= 7.2 / 12.6;
        }

        Double_t dedxcm = 1000. * edep / length;
        Double_t lightYield = edep / (1. + birkC1Mod * dedxcm + BirkC2 * dedxcm * dedxcm);
        return lightYield;
    }
    return 0.;
}

R3BCADNEBULA::R3BCADNEBULA()
    : R3BCADNEBULA("")
{
}

R3BCADNEBULA::R3BCADNEBULA(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BCADNEBULA(geoFile, { trans, rot })
{
}

R3BCADNEBULA::R3BCADNEBULA(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BCADDetector("R3BCADNEBULA", kCADNEBULA, geoFile, combi)
    , fNEBULAPoints(new TClonesArray("R3BCADNEBULAPoint"))
{
}

R3BCADNEBULA::~R3BCADNEBULA()
{
    if (fNEBULAPoints)
    {
        fNEBULAPoints->Delete();
        delete fNEBULAPoints;
    }
}

void R3BCADNEBULA::Initialize()
{
    LOG(INFO) << "R3BCADNEBULA initialization ..." << FairLogger::endl;

    FairDetector::Initialize();

    WriteParameterFile();
    ResetValues();
}

Bool_t R3BCADNEBULA::ProcessHits(FairVolume*)
{
    // New hit in detector
    if (gMC->IsTrackEntering())
    {
        if (!fLastHitDone)
        {
            LOG(WARNING) << "R3BCADNEBULA: Incomplete hit discarded" << FairLogger::endl;
            ResetValues();
        }

        fLastHitDone = kFALSE;
        fELoss = 0.;
        fLightYield = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        gMC->CurrentVolOffID(1, fPaddleID);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
    fLightYield += GetLightYield(gMC->TrackCharge(), gMC->TrackStep(), gMC->Edep());

    // Set additional parameters at exit of active volume. Create R3BCADNEBULAPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {

        // Do not save a hit if no energy deposited
        if (fELoss == 0.)
        {
            ResetValues();
            return kFALSE;
        }

        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);

        // Add Point
        LOG(DEBUG) << "R3BCADNEBULA: Adding Point at (" << fPosIn.X() << ", " << fPosIn.Y() << ", " << fPosIn.Z()
                   << ") cm,  paddle " << fPaddleID << ", track " << fTrackID << ", energy loss " << fELoss << " GeV "
                   << gMC->GetStack()->GetCurrentParentTrackNumber() << FairLogger::endl;

        Int_t size = fNEBULAPoints->GetEntriesFast();
        new ((*fNEBULAPoints)[size]) R3BCADNEBULAPoint(fTrackID,
                                                      fPaddleID,
                                                      fPosIn.Vect(),
                                                      fMomIn.Vect(),
                                                      fTime,
                                                      fLength,
                                                      fELoss,
                                                      gMC->CurrentEvent(),
                                                      fLightYield);

        // Increment number of LandPoints for this track
        auto stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kCADNEBULA);
        ResetValues();
    }

    return kTRUE;
}

void R3BCADNEBULA::PostTrack()
{
    // WE do not need to do anything here.
}

Bool_t R3BCADNEBULA::CheckIfSensitive(std::string name) { return name == "NEBULAvolBC408"; }

void R3BCADNEBULA::EndOfEvent()
{
    if (fVerboseLevel)
    {
        Print();
    }
    Reset();
}

TClonesArray* R3BCADNEBULA::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
    {
        return fNEBULAPoints;
    }
    return nullptr;
}

void R3BCADNEBULA::Register()
{
    FairRootManager::Instance()->Register("NEBULAPoints", GetName(), fNEBULAPoints, kTRUE);
}

void R3BCADNEBULA::Print(Option_t*) const
{
    LOG(INFO) << "R3BCADNEBULA: " << fNEBULAPoints->GetEntries() << " NEBULA Points registered in this event"
              << FairLogger::endl;
}

void R3BCADNEBULA::Reset()
{
    fNEBULAPoints->Clear();
    ResetValues();
}

void R3BCADNEBULA::ResetValues()
{
    fLastHitDone = kTRUE;
    fTrackID = 0;
    fPaddleID = -1;
    fPosIn.Clear();
    fPosOut.Clear();
    fMomIn.Clear();
    fMomOut.Clear();
    fTime = fLength = fELoss = fLightYield = 0;
}

void R3BCADNEBULA::WriteParameterFile()
{

    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    fNEBULAGeoPar = (R3BCADNEBULAGeoPar*)rtdb->getContainer("R3BCADNEBULAGeoPar");

    // Really bad way to find the NEBULA *node* (not the volume!)
    TGeoNode* geoNodeNEBULA = nullptr;
    for (Int_t i = 0; i < gGeoManager->GetTopNode()->GetNdaughters(); i++)
    {
        if (TString(gGeoManager->GetTopNode()->GetDaughter(i)->GetVolume()->GetName()) == "volNEBULA")
        {
            geoNodeNEBULA = gGeoManager->GetTopNode()->GetDaughter(i);
            break;
        }
    }

    if (!geoNodeNEBULA)
    {
        LOG(FATAL) << "volNEBULA not found" << FairLogger::endl;
    }

    fNEBULAGeoPar->SetNEBULAGeoNode(geoNodeNEBULA);
    fNEBULAGeoPar->setChanged();
}

ClassImp(R3BCADNEBULA);
