#include "R3BFi4.h"
#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"
#include "R3BFi4Point.h"
//#include "R3BGeoFi4.h"
#include "R3BMCStack.h"
#include "TClonesArray.h"
#include "TGeoArb8.h"
#include "TGeoBBox.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoMCGeometry.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include <stdlib.h>

R3BFi4::R3BFi4()
    : R3BFi4("")
{
}

R3BFi4::R3BFi4(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BFi4(geoFile, { trans, rot })
{
}

R3BFi4::R3BFi4(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("R3BFi4", kFI4, geoFile, combi)
    , fFi4Collection(new TClonesArray("R3BFi4Point"))
    , fPosIndex(0)
    , kGeoSaved(kFALSE)
    , flGeoPar(new TList())
{
    flGeoPar->SetName(GetName());
    ResetParameters();
}

R3BFi4::~R3BFi4()
{
    if (flGeoPar)
    {
        delete flGeoPar;
    }
    if (fFi4Collection)
    {
        fFi4Collection->Delete();
        delete fFi4Collection;
    }
}

void R3BFi4::Initialize()
{
    FairDetector::Initialize();

    LOG(INFO) << "R3BFi4: initialisation" << FairLogger::endl;
    LOG(DEBUG) << "R3BFi4: Vol. (McId) " << gMC->VolId("FI41Log") << FairLogger::endl;
}

void R3BFi4::SetSpecialPhysicsCuts()
{
    LOG(INFO) << "-I- R3BFi4: Adding customized Physics cut ... " << FairLogger::endl;

    if (gGeoManager)
    {
        TGeoMedium* pSi = gGeoManager->GetMedium("silicon");
        if (pSi)
        {
            // Setting processes for Si only
            gMC->Gstpar(pSi->GetId(), "LOSS", 3);
            gMC->Gstpar(pSi->GetId(), "STRA", 1.0);
            gMC->Gstpar(pSi->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pSi->GetId(), "COMP", 1.0);
            gMC->Gstpar(pSi->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pSi->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pSi->GetId(), "BREM", 1.0);
            gMC->Gstpar(pSi->GetId(), "HADR", 1.0);
            gMC->Gstpar(pSi->GetId(), "DRAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "MULS", 1.0);
            gMC->Gstpar(pSi->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Si Only
            Double_t cutE = fCutE; // GeV-> 1 keV

            LOG(INFO) << "-I- R3BFi4: silicon Medium Id " << pSi->GetId() << " Energy Cut-Off : " << cutE << " GeV"
                      << FairLogger::endl;

            // Si
            gMC->Gstpar(pSi->GetId(), "CUTGAM", cutE); /** gammas (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTELE", cutE); /** electrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTNEU", cutE); /** neutral hadrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTHAD", cutE); /** charged hadrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "CUTMUO", cutE); /** muons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "BCUTE", cutE);  /** electron bremsstrahlung (GeV)*/
            gMC->Gstpar(pSi->GetId(), "BCUTM", cutE);  /** muon and hadron bremsstrahlung(GeV)*/
            gMC->Gstpar(pSi->GetId(), "DCUTE", cutE);  /** delta-rays by electrons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "DCUTM", cutE);  /** delta-rays by muons (GeV)*/
            gMC->Gstpar(pSi->GetId(), "PPCUTM", -1.);  /** direct pair production by muons (GeV)*/
        }
    } //! gGeoManager
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BFi4::ProcessHits(FairVolume* vol)
{
    // 2 Simple Det PLane
    // get Info from DCH planes
    Int_t copyNo = -1;
    Int_t planeNr = -1;
    // Get the Geo info from MC Point
    gMC->CurrentVolID(copyNo);
    gMC->CurrentVolOffID(1, planeNr);

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        // fTime   = gMC->TrackTime() * 1.0e09;
        // fLength = gMC->TrackLength();
        fTime_in = gMC->TrackTime() * 1.0e09;
        fLength_in = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create R3BFi4Point.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol-> getMotherCopyNo();	
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
        if (fELoss == 0.)
            return kFALSE;

        fTime_out = gMC->TrackTime() * 1.0e09; // also in case particle is stopped in detector, or decays...
        fLength_out = gMC->TrackLength();
        fTime = (fTime_out + fTime_in) / 2.;
        fLength = (fLength_out + fLength_in) / 2.;

        if (gMC->IsTrackExiting())
        {
            const Double_t* oldpos;
            const Double_t* olddirection;
            Double_t newpos[3];
            Double_t newdirection[3];
            Double_t safety;

            gGeoManager->FindNode(fPosOut.X(), fPosOut.Y(), fPosOut.Z());
            oldpos = gGeoManager->GetCurrentPoint();
            olddirection = gGeoManager->GetCurrentDirection();

            for (Int_t i = 0; i < 3; i++)
            {
                newdirection[i] = -1 * olddirection[i];
            }

            gGeoManager->SetCurrentDirection(newdirection);
            // TGeoNode *bla = gGeoManager->FindNextBoundary(2);
            safety = gGeoManager->GetSafeDistance();

            gGeoManager->SetCurrentDirection(-newdirection[0], -newdirection[1], -newdirection[2]);

            for (Int_t i = 0; i < 3; i++)
            {
                newpos[i] = oldpos[i] - (3 * safety * olddirection[i]);
            }

            fPosOut.SetX(newpos[0]);
            fPosOut.SetY(newpos[1]);
            fPosOut.SetZ(newpos[2]);
        }

        AddHit(fTrackID,
               /*fVolumeID*//*copyNo*/planeNr,
               planeNr,
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of Fi4Points for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kFI4);

        ResetParameters();
    }

    return kTRUE;
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BFi4::BeginEvent() {}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BFi4::EndOfEvent()
{
    if (fVerboseLevel)
        Print();
    fFi4Collection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BFi4::Register() { FairRootManager::Instance()->Register("FI4Point", GetName(), fFi4Collection, kTRUE); }
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BFi4::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fFi4Collection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BFi4::Print(Option_t* option) const
{
    Int_t nHits = fFi4Collection->GetEntriesFast();
    LOG(INFO) << "R3BFi4: " << nHits << " points registered in this event" << FairLogger::endl;
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BFi4::Reset()
{
    fFi4Collection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BFi4::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(INFO) << "R3BFi4: " << nEntries << " entries to add" << FairLogger::endl;
    TClonesArray& clref = *cl2;
    R3BFi4Point* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BFi4Point*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BFi4Point(*oldpoint);
        fPosIndex++;
    }
    LOG(INFO) << "R3BFi4: " << cl2->GetEntriesFast() << " merged entries" << FairLogger::endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BFi4Point* R3BFi4::AddHit(Int_t trackID,
                            Int_t detID,
                            Int_t plane,
                            TVector3 posIn,
                            TVector3 posOut,
                            TVector3 momIn,
                            TVector3 momOut,
                            Double_t time,
                            Double_t length,
                            Double_t eLoss)
{
    TClonesArray& clref = *fFi4Collection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
    {
        LOG(INFO) << "R3BFi4: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
                  << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV"
                  << FairLogger::endl;
    }
    return new (clref[size]) R3BFi4Point(trackID, detID, plane, posIn, posOut, momIn, momOut, time, length, eLoss);
}

Bool_t R3BFi4::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("FI41Log") )
    {
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BFi4)
