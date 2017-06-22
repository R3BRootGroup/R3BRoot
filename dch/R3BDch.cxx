// -------------------------------------------------------------------------
// -----                        R3BDch source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BDch.h"

#include "R3BGeoDch.h"
#include "R3BDchPoint.h"
#include "R3BDchFullPoint.h"
#include "R3BGeoDchPar.h"
#include "R3BMCStack.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TObjArray.h"

// includes for modeling
#include "TGeoManager.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include "TGeoMatrix.h"
#include "TGeoMaterial.h"
#include "TGeoMedium.h"
#include "TGeoBBox.h"
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoSphere.h"
#include "TGeoArb8.h"
#include "TGeoCone.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TRandom.h"

#include <stdlib.h>
#include <iomanip>

using namespace std;

// -----   Default constructor   -------------------------------------------
R3BDch::R3BDch()
    : R3BDetector("R3BDch", kTRUE, kDCH)
{
    ResetParameters();
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
    kHelium = kFALSE;
    fDynamicStepSize = kFALSE;
    fVerbose = kFALSE;
    refMatrix = NULL;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BDch::R3BDch(const char* name, Bool_t active)
    : R3BDetector(name, active, kDCH)
    , fPos1(-123.219446, 3.597104, 444.126271)
    , fPos2(-167.015888, 1.016917, 535.093884)
    , fRot1(new TGeoRotation())
    , fRot2(new TGeoRotation())
{
    ResetParameters();
    fRot1->RotateZ(-8.88);
    fRot1->RotateY(-31.);
    fRot2->RotateZ(9.35);
    fRot2->RotateY(-31.);
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
    kHelium = kFALSE;
    fDynamicStepSize = kFALSE;
    fVerbose = kFALSE;
    refMatrix = NULL;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BDch::R3BDch(const char* name,
               TString geoFile,
               Bool_t active,
               Double_t x1,
               Double_t y1,
               Double_t z1,
               Double_t rot_y1,
               Double_t rot_z1,
               Double_t x2,
               Double_t y2,
               Double_t z2,
               Double_t rot_y2,
               Double_t rot_z2)
    : R3BDetector(name, active, kDCH)
    , fPos1(x1, y1, z1)
    , fPos2(x2, y2, z2)
    , fRot1(new TGeoRotation())
    , fRot2(new TGeoRotation())
{
    ResetParameters();
    SetGeometryFileName(geoFile);
    fRot1->RotateZ(rot_z1);
    fRot1->RotateY(rot_y1);
    fRot2->RotateZ(rot_z2);
    fRot2->RotateY(rot_y2);
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
    kHelium = kFALSE;
    fDynamicStepSize = kFALSE;
    fVerbose = kFALSE;
    refMatrix = NULL;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BDch::~R3BDch()
{

    if (flGeoPar)
        delete flGeoPar;
    if (fDchCollection)
    {
        fDchCollection->Delete();
        delete fDchCollection;
    }
}
// -------------------------------------------------------------------------

void R3BDch::Initialize()
{
    if (fDynamicStepSize)
        fDchCollection = new TClonesArray("R3BDchFullPoint");
    else
        fDchCollection = new TClonesArray("R3BDchPoint");

    FairDetector::Initialize();

    LOG(INFO) << "R3BDch: initialisation" << FairLogger::endl;
    LOG(DEBUG) << "R3BDch: Gas. Vol: (McId) " << gMC->VolId("ActGASBoxLog") << FairLogger::endl;

    // get the transformation matrixes for
    // the sensitive nodes.
    TGeoVolume* vol = gGeoManager->GetVolume("ALBoxLog");
    vol->SetVisibility(kTRUE);
    vol->SetVisContainers();
    gGeoManager->GetTopVolume()->SetVisContainers();
}

void R3BDch::SetSpecialPhysicsCuts()
{
    LOG(INFO) << "-I- R3BDch: Adding customized Physics cut: " << FairLogger::endl;

    if (gGeoManager)
    {
        TGeoMedium* pSi = gGeoManager->GetMedium("DCHgas");
        if (pSi)
        {
            // Setting processes for Gas mixture only
            // ELoss + deltas + reduced fluctuation
            gMC->Gstpar(pSi->GetId(), "LOSS", 3);
            // collision sampling on PAI Model ( thin layer )
            gMC->Gstpar(pSi->GetId(), "STRA", 1.0);
            // Deltas on
            gMC->Gstpar(pSi->GetId(), "DRAY", 1.0);
            // Default processes
            gMC->Gstpar(pSi->GetId(), "PAIR", 1.0);
            gMC->Gstpar(pSi->GetId(), "COMP", 1.0);
            gMC->Gstpar(pSi->GetId(), "PHOT", 1.0);
            gMC->Gstpar(pSi->GetId(), "ANNI", 1.0);
            gMC->Gstpar(pSi->GetId(), "BREM", 1.0);
            gMC->Gstpar(pSi->GetId(), "HADR", 1.0);
            gMC->Gstpar(pSi->GetId(), "DCAY", 1.0);
            gMC->Gstpar(pSi->GetId(), "MULS", 1.0);
            gMC->Gstpar(pSi->GetId(), "RAYL", 1.0);

            // Setting Energy-CutOff for Drift chamber Gas Only
            Double_t cutE = fCutE; // GeV-> 1 keV

            LOG(INFO) << "-I- R3BDch: DCHgas Medium Id " << pSi->GetId() << " Energy Cut-Off : " << cutE << " GeV" << FairLogger::endl;

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

    } //!gGeoManager
}

void R3BDch::FindNodePath(TObjArray* arr)
{
    TString sVol = "ActGASBoxLog";
    TString topName = gGeoManager->GetTopNode()->GetName();
    TString path = "/" + topName;

    for (Int_t i = 0; i < arr->GetEntries(); i++)
    {
        TGeoNode* aNode = (TGeoNode*)arr->At(i);
        TString nodName = aNode->GetName();
        if (nodName.Contains("DCH1") || nodName.Contains(sVol.Data()))
        {
            path += "/" + nodName;
            LOG(INFO) << "intermediate path: " << path << FairLogger::endl;
        }
        FindNodePath(aNode->GetNodes());
    }

    LOG(INFO) << "found path: " << path << FairLogger::endl;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BDch::ProcessHits(FairVolume* vol)
{
    if (fVerbose)
        PrintInfo();

    if (fDynamicStepSize)
        RecordFullMcHit();
    else
        RecordPartialMcHit();

    return kTRUE;
}

void R3BDch::RecordFullMcHit()
{
    // Record the full McHit history using
    // a dynamical update of the step size
    // at each Geant steps
    // Better estimation for Gaseous thin
    // layer

    // DCH Geometry Hiearchy
    // Int_t medId = gMC->CurrentMedium();
    Int_t copyNo = -1;
    Int_t ModuleNr0 = -1;
    Int_t ModuleNr1 = -1;
    Int_t ModuleNr2 = -1;
    Int_t ModuleNr3 = -1;
    // Get the Geometry info from MC Point
    gMC->CurrentVolID(copyNo);
    gMC->CurrentVolOffID(0, ModuleNr0);
    gMC->CurrentVolOffID(1, ModuleNr1);
    gMC->CurrentVolOffID(2, ModuleNr2); //! Module No
    gMC->CurrentVolOffID(3, ModuleNr3); //!! changed !!!

    // Detector plane
    Int_t mod = ModuleNr3; // formerly 2
    // MC Track Info
    Int_t trackId = gMC->GetStack()->GetCurrentTrackNumber();
    Double_t time = gMC->TrackTime() * 1.0e09;
    Double_t length = gMC->TrackStep();
    TLorentzVector pos1;
    gMC->TrackPosition(pos1);
    TLorentzVector mom1;
    gMC->TrackMomentum(mom1);
    TVector3 pos(pos1.X(), pos1.Y(), pos1.Z());
    TVector3 mom(mom1.X(), mom1.Y(), mom1.Z());
    // Mc Eloss
    Double_t eLoss = gMC->Edep();
    // Local Coordinates
    Double_t MasterPoint[] = { pos.X(), pos.Y(), pos.Z() };
    Double_t MasterMom[] = { mom.X(), mom.Y(), mom.Z() };
    Double_t localPoint[3] = { 0., 0., 0. };
    Double_t localMom[3] = { 0., 0., 0. };

    gGeoManager->FindNode(MasterPoint[0], MasterPoint[1], MasterPoint[2]);
    gGeoManager->GetCurrentNavigator()->MasterToLocal(MasterPoint, localPoint);
    gGeoManager->GetCurrentNavigator()->MasterToLocal(MasterMom, localMom);
    TVector3 lpos(localPoint[0], localPoint[1], localPoint[2]);
    TVector3 lmom(localMom[0], localMom[1], localMom[2]);
    // Layer & cell
    Int_t layer = -1;
    Int_t cell = -1; // not filled for now !
    layer = (localPoint[2] > 0.) ? 1 : 0;

    // Add hit if Energy deposited > 0
    if (eLoss > 0.)
    {

        AddFullHit(trackId, mod, layer, cell, pos, lpos, mom, lmom, time, length, eLoss);

        // Increment number of DCH Points for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kDCH);

        ResetParameters();

    } //! eLoss

    // Set maximum MC step size allowed
    SetStepToNextCollision();
}

void R3BDch::RecordPartialMcHit()
{

    // DCH Geometry Hiearchy
    // Int_t medId = gMC->CurrentMedium();
    Int_t copyNo = -1;
    Int_t ModuleNr0 = -1;
    Int_t ModuleNr1 = -1;
    Int_t ModuleNr2 = -1;
    Int_t ModuleNr3 = -1;
    // Get the Geometry info from MC Point
    gMC->CurrentVolID(copyNo);
    gMC->CurrentVolOffID(0, ModuleNr0);
    gMC->CurrentVolOffID(1, ModuleNr1);
    gMC->CurrentVolOffID(2, ModuleNr2); //! Module No
    gMC->CurrentVolOffID(3, ModuleNr3); //!!! changed

    // Detector plane
    Int_t mod = ModuleNr3; // formerly 2

    // Standard registration
    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        // fTime   = gMC->TrackTime() * 1.0e09;
        // fLength = gMC->TrackLength();
        fTime_in = gMC->TrackTime() * 1.0e09;
        fLength_in = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        refMatrix = gGeoManager->GetCurrentMatrix();
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create R3BmTofPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
//        if (fELoss == 0.)
//            return;

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
            //   TGeoNode *bla = gGeoManager->FindNextBoundary(2);
            safety = gGeoManager->GetSafeDistance();

            gGeoManager->SetCurrentDirection(-newdirection[0], -newdirection[1], -newdirection[2]);

            for (Int_t i = 0; i < 3; i++)
            {
                newpos[i] = oldpos[i] - (3 * safety * olddirection[i]);
            }

            if (fPosIn.Z() < 30. && newpos[2] > 30.02)
            {
                LOG(ERROR) << "2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] << " with safety = " << safety
                           << FairLogger::endl;
                LOG(ERROR) << "oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << FairLogger::endl;
                LOG(ERROR) << "newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << FairLogger::endl;
            }

            fPosOut.SetX(newpos[0]);
            fPosOut.SetY(newpos[1]);
            fPosOut.SetZ(newpos[2]);

        } //! track exiting

        // Local Coordinates In
        Double_t MasterPointIn[] = { fPosIn.X(), fPosIn.Y(), fPosIn.Z() };
        Double_t MasterMomIn[] = { fMomIn.X(), fMomIn.Y(), fMomIn.Z() };
        Double_t LocalPointIn[3] = { 0., 0., 0. };
        Double_t LocalMomIn[3] = { 0., 0., 0. };
        refMatrix->MasterToLocal(MasterPointIn, LocalPointIn);
        refMatrix->MasterToLocal(MasterMomIn, LocalMomIn);
        // Local Coordinates Out
        Double_t MasterPointOut[] = { fPosOut.X(), fPosOut.Y(), fPosOut.Z() };
        Double_t MasterMomOut[] = { fMomOut.X(), fMomOut.Y(), fMomOut.Z() };
        Double_t LocalPointOut[3] = { 0., 0., 0. };
        Double_t LocalMomOut[3] = { 0., 0., 0. };
        refMatrix->MasterToLocal(MasterPointOut, LocalPointOut);
        refMatrix->MasterToLocal(MasterMomOut, LocalMomOut);

        Int_t layer = -1;
        Int_t cell = -1; // not filled for now !
        // layer has no meaning here and will
        // be always set to 1
        layer = (LocalPointOut[2] > 0.) ? 1 : 0;

        AddHit(fTrackID,
               mod,
               layer,
               cell,
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               TVector3(LocalPointIn[0], LocalPointIn[1], LocalPointIn[2]),
               TVector3(LocalMomIn[0], LocalMomIn[1], LocalMomIn[2]),
               TVector3(LocalPointOut[0], LocalPointOut[1], LocalPointOut[2]),
               TVector3(LocalMomOut[0], LocalMomOut[1], LocalMomOut[2]),
               fTime,
               fLength,
               fELoss);

        // Increment number of DCH Points for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kDCH);

        ResetParameters();
    }
}

void R3BDch::PrintInfo()
{
    // dump of Hit Information
    LOG(INFO) << "X(cm)    "
              << "Y(cm)    "
              << "Z(cm)  "
              << "KinE(MeV)   "
              << "dE(MeV) "
              << "Step(cm) "
              << "TrackL(cm) "
              << "Volume  "
              << "Process " << FairLogger::endl;

    // Position
    Double_t x, y, z;
    gMC->TrackPosition(x, y, z);
    LOG(INFO) << setw(8) << setprecision(3) << x << " " << setw(8) << setprecision(3) << y << " " << setw(8) << setprecision(3) << z << "  ";

    // Kinetic energy
    Double_t px, py, pz, etot;
    gMC->TrackMomentum(px, py, pz, etot);
    Double_t ekin = etot - gMC->TrackMass();
    LOG(INFO) << setw(9) << setprecision(4) << ekin * 1e03 << " ";

    // Energy deposit
    LOG(INFO) << setw(9) << setprecision(4) << gMC->Edep() * 1e03 << " ";
    // Step length
    LOG(INFO) << setw(8) << setprecision(3) << gMC->TrackStep() << " ";

    // Track length
    LOG(INFO) << setw(8) << setprecision(3) << gMC->TrackLength() << "     ";

    // Volume
    if (gMC->CurrentVolName() != 0)
        LOG(INFO) << setw(4) << gMC->CurrentVolName() << "  ";
    else
        LOG(INFO) << setw(4) << "None"
                  << "  ";

    // Process involved
    TArrayI processes;
    Int_t nofProcesses = gMC->StepProcesses(processes);

    for (int ip = 0; ip < nofProcesses; ip++)
        LOG(INFO) << TMCProcessName[processes[ip]] << " / ";

    LOG(INFO) << FairLogger::endl;
}

void R3BDch::SetStepToNextCollision()
{
    //  <DB>
    //  Weighted average calculation of the
    //  number of primary collisions per 1 cm for MIPs
    // in case of LAND DCH gas mixture
    //
    //           80% Ar + 20% CO2

    const Float_t prim = 25.5;

    Double_t charge = gMC->TrackCharge();
    Float_t pp;

    TLorentzVector mom;
    gMC->TrackMomentum(mom);
    Float_t ptot = mom.Rho();
    if (ptot <= 1e-12)
        return;
    Float_t beta_gamma = ptot / gMC->TrackMass();

    // Select Particle types from Pids
    // electron(3), positron(2), photon(1)
    // to optimize the BBloch calculation

    if (gMC->IdFromPDG(gMC->TrackPid()) <= 3 && ptot > 0.02)
    {
        // electrons above 20 MeV/c reach the plateau!
        pp = prim * 1.58;
    }
    else
    {
        // assuming form of Bethe Bloch depends only on mean free path
        pp = prim * BetheBloch(beta_gamma);
        if (TMath::Abs(charge) > 1.)
            pp *= (charge * charge);
    }

    // Mean free path sampling
    TRandom* rGenerator = gMC->GetRandom();
    Double_t rnd = rGenerator->Rndm();

    // get random free mean path from poisson statistics and mean pp
    gMC->SetMaxStep(-TMath::Log(rnd) / pp);
}

Double_t R3BDch::BetheBloch(Double_t bg)
{
    // <DB>
    // Parametrisation of the
    // Bethe-Bloch energy loss formula
    // taken from  Lehrhaus NIM 217 (1983) 43.

    // This parameters have been adjusted
    // to averaged values from GEANT
    const Double_t kP1 = 7.17960e-02;
    const Double_t kP2 = 8.54196;
    const Double_t kP3 = 1.38065e-06;
    const Double_t kP4 = 5.30972;
    const Double_t kP5 = 2.83798;

    // Lower cutoff of the Bethe-Bloch-curve to limit step sizes
    const Double_t kBgMin = 0.8;
    const Double_t kBBMax = 6.83298;

    if (bg > kBgMin)
    {
        Double_t yy = bg / TMath::Sqrt(1. + bg * bg);
        Double_t aa = TMath::Power(yy, kP4);
        Double_t bb = TMath::Power((1. / bg), kP5);
        bb = TMath::Log(kP3 + bb);
        return ((kP2 - aa - bb) * kP1 / aa);
    }
    else
    {
        return kBBMax;
    }
}

// ----------------------------------------------------------------------------
// void R3BDch::SaveGeoParams(){
//}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BDch::BeginEvent()
{
}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BDch::EndOfEvent()
{

    if (fVerboseLevel)
        Print();
    fDchCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BDch::Register()
{
    if (fDynamicStepSize)
        FairRootManager::Instance()->Register("DCHFullPoint", GetName(), fDchCollection, kTRUE);
    else
        FairRootManager::Instance()->Register("DCHPoint", GetName(), fDchCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BDch::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fDchCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BDch::Print(Option_t* option) const
{
    Int_t nHits = fDchCollection->GetEntriesFast();
    LOG(INFO) << "R3BDch: " << nHits << " points registered in this event" << FairLogger::endl;
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BDch::Reset()
{
    fDchCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BDch::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(INFO) << "R3BDch: " << nEntries << " entries to add" << FairLogger::endl;
    TClonesArray& clref = *cl2;

    if (fDynamicStepSize)
    {
        R3BDchFullPoint* oldpoint = NULL;
        for (Int_t i = 0; i < nEntries; i++)
        {
            oldpoint = (R3BDchFullPoint*)cl1->At(i);
            Int_t index = oldpoint->GetTrackID() + offset;
            oldpoint->SetTrackID(index);
            new (clref[fPosIndex]) R3BDchFullPoint(*oldpoint);
            fPosIndex++;
        }
    }
    else
    {
        R3BDchPoint* oldpoint = NULL;
        for (Int_t i = 0; i < nEntries; i++)
        {
            oldpoint = (R3BDchPoint*)cl1->At(i);
            Int_t index = oldpoint->GetTrackID() + offset;
            oldpoint->SetTrackID(index);
            new (clref[fPosIndex]) R3BDchPoint(*oldpoint);
            fPosIndex++;
        }
    }
    LOG(INFO) << "R3BDch: " << cl2->GetEntriesFast() << " merged entries" << FairLogger::endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BDchFullPoint* R3BDch::AddFullHit(Int_t trackId,
                                    Int_t mod,
                                    Int_t layer,
                                    Int_t cell,
                                    TVector3 pos,
                                    TVector3 lpos,
                                    TVector3 mom,
                                    TVector3 lmom,
                                    Double_t time,
                                    Double_t length,
                                    Double_t eLoss)
{
    TClonesArray& clref = *fDchCollection;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BDchFullPoint(trackId, mod, layer, cell, pos, lpos, mom, lmom, time, length, eLoss);
}

R3BDchPoint* R3BDch::AddHit(Int_t trackId,
                            Int_t mod,
                            Int_t layer,
                            Int_t cell,
                            TVector3 posIn,
                            TVector3 posOut,
                            TVector3 momIn,
                            TVector3 momOut,
                            TVector3 lpos1,
                            TVector3 lmom1,
                            TVector3 lpos2,
                            TVector3 lmom2,
                            Double_t time,
                            Double_t length,
                            Double_t eLoss)
{
    TClonesArray& clref = *fDchCollection;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BDchPoint(trackId, mod, layer, cell, posIn, posOut, momIn, momOut, lpos1, lmom1, lpos2, lmom2, time, length, eLoss);
}

// -----   Public method ConstructGeometry   ----------------------------------
void R3BDch::ConstructGeometry()
{
    TString fileName = GetGeometryFileName();
    if (fileName.EndsWith(".root"))
    {
        LOG(INFO) << "Constructing DCH geometry from ROOT file " << fileName.Data() << FairLogger::endl;
        ConstructRootGeometry();
        
        TGeoNode* dch_node = gGeoManager->GetTopVolume()->GetNode("DCH_0");
        
        TGeoNode* node = dch_node->GetVolume()->GetNode("DCH1_0");
        TGeoCombiTrans* combtrans = (TGeoCombiTrans*)((TGeoNodeMatrix*)node)->GetMatrix();
        combtrans->SetDx(fPos1.X());
        combtrans->SetDy(fPos1.Y());
        combtrans->SetDz(fPos1.Z());
        combtrans->SetRotation(fRot1);
        
        node = dch_node->GetVolume()->GetNode("DCH1_1");
        combtrans = (TGeoCombiTrans*)((TGeoNodeMatrix*)node)->GetMatrix();
        combtrans->SetDx(fPos2.X());
        combtrans->SetDy(fPos2.Y());
        combtrans->SetDz(fPos2.Z());
        combtrans->SetRotation(fRot2);
        
        node = dch_node->GetVolume()->GetNode("HeParaLog_0");
        combtrans = (TGeoCombiTrans*)((TGeoNodeMatrix*)node)->GetMatrix();
        combtrans->SetDx((fPos1.X() + fPos2.X())/2.);
        combtrans->SetDy(fPos2.Y());
        combtrans->SetDz((fPos1.Z() + fPos2.Z())/2.);
        combtrans->SetRotation(fRot1);
    }
    else
    {
        LOG(FATAL) << "DCH geometry file name is not set" << FairLogger::endl;
        exit(1);
    }
}

Bool_t R3BDch::CheckIfSensitive(std::string name)
{
    if (0 == TString(name).CompareTo("ActGASBoxLog"))
    {
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BDch)
