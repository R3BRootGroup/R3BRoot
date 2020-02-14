// Inclusion of the own header-file:
#include "R3BCADNEBULAVETO.h"

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

R3BCADNEBULAVETO::R3BCADNEBULAVETO()
    : R3BCADNEBULAVETO("")
{
}

R3BCADNEBULAVETO::R3BCADNEBULAVETO(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BCADNEBULAVETO(geoFile, { trans, rot })
{
}

R3BCADNEBULAVETO::R3BCADNEBULAVETO(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BCADDetector("R3BCADNEBULAVETO", kCADNEBULAVETO, geoFile, combi)
    , fNEBULAVETOPoints(new TClonesArray("R3BCADNEBULAVETOPoint"))
{
}

R3BCADNEBULAVETO::~R3BCADNEBULAVETO()
{
    if (fNEBULAVETOPoints)
    {
        fNEBULAVETOPoints->Delete();
        delete fNEBULAVETOPoints;
    }
}

void R3BCADNEBULAVETO::Initialize()
{
    LOG(INFO) << "R3BCADNEBULAVETO initialization ..." << FairLogger::endl;

    FairDetector::Initialize();

    WriteParameterFile();
    ResetValues();
}

Bool_t R3BCADNEBULAVETO::ProcessHits(FairVolume*)
{
    // New hit in detector
    if (gMC->IsTrackEntering())
    {
        if (!fLastHitDone)
        {
            LOG(WARNING) << "R3BCADNEBULAVETO: Incomplete hit discarded" << FairLogger::endl;
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

    // Set additional parameters at exit of active volume. Create R3BCADNEBULAVETOPoint.
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
        LOG(DEBUG) << "R3BCADNEBULAVETO: Adding Point at (" << fPosIn.X() << ", " << fPosIn.Y() << ", " << fPosIn.Z()
                   << ") cm,  paddle " << fPaddleID << ", track " << fTrackID << ", energy loss " << fELoss << " GeV "
                   << gMC->GetStack()->GetCurrentParentTrackNumber() << FairLogger::endl;

        Int_t size = fNEBULAVETOPoints->GetEntriesFast();
        new ((*fNEBULAVETOPoints)[size]) R3BCADNEBULAVETOPoint(fTrackID,
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
        stack->AddPoint(kCADNEBULAVETO);
        ResetValues();
    }

    return kTRUE;
}

void R3BCADNEBULAVETO::PostTrack()
{
    // WE do not need to do anything here.
}

Bool_t R3BCADNEBULAVETO::CheckIfSensitive(std::string name) { return name == "NEBULAVETOvolBC408"; }

void R3BCADNEBULAVETO::EndOfEvent()
{
    if (fVerboseLevel)
    {
        Print();
    }
    Reset();
}

TClonesArray* R3BCADNEBULAVETO::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
    {
        return fNEBULAVETOPoints;
    }
    return nullptr;
}

void R3BCADNEBULAVETO::Register()
{
    FairRootManager::Instance()->Register("NEBULAVETOPoints", GetName(), fNEBULAVETOPoints, kTRUE);
}

void R3BCADNEBULAVETO::Print(Option_t*) const
{
    LOG(INFO) << "R3BCADNEBULAVETO: " << fNEBULAVETOPoints->GetEntries() << " NEBULAVETO Points registered in this event"
              << FairLogger::endl;
}

void R3BCADNEBULAVETO::Reset()
{
    fNEBULAVETOPoints->Clear();
    ResetValues();
}

void R3BCADNEBULAVETO::ResetValues()
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

void R3BCADNEBULAVETO::WriteParameterFile()
{

    FairRuntimeDb* rtdb = FairRun::Instance()->GetRuntimeDb();
    fNEBULAVETOGeoPar = (R3BCADNEBULAVETOGeoPar*)rtdb->getContainer("R3BCADNEBULAVETOGeoPar");

    // Really bad way to find the NEBULAVETO *node* (not the volume!)
    TGeoNode* geoNodeNEBULAVETO = nullptr;
    for (Int_t i = 0; i < gGeoManager->GetTopNode()->GetNdaughters(); i++)
    {
        if (TString(gGeoManager->GetTopNode()->GetDaughter(i)->GetVolume()->GetName()) == "volNEBULAVETO")
        {
            geoNodeNEBULAVETO = gGeoManager->GetTopNode()->GetDaughter(i);
            break;
        }
    }

    if (!geoNodeNEBULAVETO)
    {
        LOG(FATAL) << "volNEBULAVETO not found" << FairLogger::endl;
    }

    fNEBULAVETOGeoPar->SetNEBULAVETOGeoNode(geoNodeNEBULAVETO);
    fNEBULAVETOGeoPar->setChanged();
}

ClassImp(R3BCADNEBULAVETO);
