/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------------------
// -----                  R3BXBallv1 source file                           -----
// -----            Created 26/03/09  by D.Bertini                     -----
// -----        new Version: 20/07/09 <D.Bertini@gsi.de>               -----
// -----        new Version: 08/04/10 <wranne@student.chalmers.se>     -----
// -----        new Version: 25/11/10 <marc.labiche@stfc.ac.uk>        -----
// -------------------------------------------------------------------------
#include "R3BXBallv1.h"

#include "R3BGeoXBall.h"
#include "R3BGeoXBallPar.h"
#include "R3BMCStack.h"
#include "R3BXBallPoint.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TRotation.h"
#include "TVirtualMC.h"

// includes for modeling
#include "TGeoArb8.h"
#include "TGeoBBox.h"
#include "TGeoBoolNode.h"
#include "TGeoCompositeShape.h"
#include "TGeoCone.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoPgon.h"
#include "TGeoTube.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"
// try the mathematical ROOT package
#include "Math/Point3D.h"
#include "Math/Rotation3D.h"

#define XB_CRYSTAL(no, type, theta, phi, psi, n1, n2, n3, n4, n5, n6) { no, type, theta, phi, psi },
#define XB_CRYSTAL_MOD(nr, mod, delta) { nr, mod, delta },

// -----   Default constructor   -------------------------------------------
R3BXBallv1::R3BXBallv1()
    : R3BDetector("R3BXBallv1", kTRUE, kCAL)
{
    ResetParameters();
    fXBallCollection = new TClonesArray("R3BXBallPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
    fGeoVersion = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BXBallv1::R3BXBallv1(const char* name, Bool_t active)
    : R3BDetector(name, active, kCAL)
{
    ResetParameters();
    fXBallCollection = new TClonesArray("R3BXBallPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
    fGeoVersion = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BXBallv1::~R3BXBallv1()
{

    if (flGeoPar)
        delete flGeoPar;
    if (fXBallCollection)
    {
        fXBallCollection->Delete();
        delete fXBallCollection;
    }
}
// -------------------------------------------------------------------------

void R3BXBallv1::Initialize()
{
    FairDetector::Initialize();

    LOG(info) << "R3BXBallv1 initialisation";
    LOG(debug) << "R3BXBallv1: Vol. (McId)";
    LOG(debug) << "R3BXBallv1: Crystal A : " << gMC->VolId("crystalLogNAJA");
    LOG(debug) << "R3BXBallv1: Crystal B : " << gMC->VolId("crystalLogNAJB");
    LOG(debug) << "R3BXBallv1: Crystal C : " << gMC->VolId("crystalLogNAJC");
    LOG(debug) << "R3BXBallv1: Crystal D : " << gMC->VolId("crystalLogNAJD");

    // Crystals type ID
    // type  ID
    // A    1
    // B    2
    // C    3
    // D    4

    fTypeA = gMC->VolId("crystalLogNAJA");
    fTypeB = gMC->VolId("crystalLogNAJB");
    fTypeC = gMC->VolId("crystalLogNAJC");
    fTypeD = gMC->VolId("crystalLogNAJD");

    TGeoVolume* vol = gGeoManager->GetVolume("CBLogWorld");
    vol->SetVisibility(kFALSE);
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BXBallv1::ProcessHits(FairVolume* vol)
{
    Int_t copyNo = 0;
    Int_t fVolID = gMC->CurrentVolID(copyNo);
    Int_t fCrystalType = GetCrystalType(fVolID);

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09; //[ns]
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();

    // Set additional parameters at exit of active volume. Create R3BXBallPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getMCid();
        gMC->TrackPosition(fPosOut);
        gMC->TrackMomentum(fMomOut);
        if (fELoss == 0.)
            return kFALSE;

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
            //  TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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
               fVolumeID,
               fCrystalType,
               copyNo,
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of XBallPoints for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kCAL);

        ResetParameters();
    }
    return kTRUE;
}
// ----------------------------------------------------------------------------

// -----   Public method EndOfEvent   -----------------------------------------
void R3BXBallv1::BeginEvent() {}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BXBallv1::EndOfEvent()
{
    if (fVerboseLevel)
        Print();
    fXBallCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BXBallv1::Register()
{
    FairRootManager::Instance()->Register("CrystalPoint", GetName(), fXBallCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BXBallv1::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fXBallCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BXBallv1::Print() const
{
    Int_t nHits = fXBallCollection->GetEntriesFast();
    LOG(info) << "R3BXBallv1: " << nHits << " points registered in this event";
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BXBallv1::Reset()
{
    fXBallCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BXBallv1::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(info) << "R3BXBallv1: " << nEntries << " entries to add";
    TClonesArray& clref = *cl2;
    R3BXBallPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BXBallPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BXBallPoint(*oldpoint);
        fPosIndex++;
    }
    LOG(info) << "R3BXBallv1: " << cl2->GetEntriesFast() << " merged entries";
}

// -----   Private method AddHit   --------------------------------------------
R3BXBallPoint* R3BXBallv1::AddHit(Int_t trackID,
                                  Int_t detID,
                                  Int_t type,
                                  Int_t cp,
                                  TVector3 posIn,
                                  TVector3 posOut,
                                  TVector3 momIn,
                                  TVector3 momOut,
                                  Double_t time,
                                  Double_t length,
                                  Double_t eLoss)
{
    TClonesArray& clref = *fXBallCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
        LOG(info) << "R3BXBallv1: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
                  << ") cm, detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV";
    return new (clref[size]) R3BXBallPoint(trackID, detID, type, cp, posIn, posOut, momIn, momOut, time, length, eLoss);
}

void R3BXBallv1::ConstructGeometry()
{

    LOG(info) << "##################################################################"
              << "*     -I- R3BXBallv1 R3BXBallv1::ConstructGeometry()                     *"
              << "*        Darmstadt-Heidelberg Crystal Ball - CAVE C Setup        *"
              << "##################################################################";

    // out-of-file geometry definition
    Double_t dx, dy, dz;
    Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
    Double_t a;
    Double_t theta1, theta2, phi1, phi2, dphi;
    Double_t z, density, radl, absl, w;
    Int_t nel, numed;
    double crystalPosRad = 35; // Radius of the center of the crystals [cm]

    // Definitions for the crystal types from "xb_crystal_loc.hh"
    int XB_TYPE_A = 0, XB_TYPE_B = 1, XB_TYPE_C = 2, XB_TYPE_D = 3, DISABLE = 0, MOVE_R = 1;

    // Get crystal position information
    xb_crystal_v1 xb_crystals[] = {
#include "xb_crystal_loc.hh"
    };

    int nrCrystals = sizeof(xb_crystals) / sizeof(xb_crystals[0]);
    for (int i = 0; i < nrCrystals; i++)
    {
        xb_crystals[i].active = true;
    }

    // Get corner information for the crystals
    double xb_crystal_parts[][16] = {
#include "xb_crystal_part.hh"
    };

    // Get modification information
    xb_crystal_mod xb_crystals_mod[] = {
#include "xb_crystal_mod.hh"
    };

    // --- Local Material definition

    // Mixture: Air
    TGeoMedium* pMed2 = NULL;
    if (gGeoManager->GetMedium("Air"))
        pMed2 = gGeoManager->GetMedium("Air");
    else
    {
        nel = 2;
        density = 0.001290;
        TGeoMixture* pMat2 = new TGeoMixture("Air", nel, density);
        a = 14.006740;
        z = 7.000000;
        w = 0.700000; // N
        pMat2->DefineElement(0, a, z, w);
        a = 15.999400;
        z = 8.000000;
        w = 0.300000; // O
        pMat2->DefineElement(1, a, z, w);
        pMat2->SetIndex(1);
        // Medium: Air
        numed = 1; // medium number
        Double_t par[8];
        par[0] = 0.000000; // isvol
        par[1] = 0.000000; // ifield
        par[2] = 0.000000; // fieldm
        par[3] = 0.000000; // tmaxfd
        par[4] = 0.000000; // stemax
        par[5] = 0.000000; // deemax
        par[6] = 0.000100; // epsil
        par[7] = 0.000000; // stmin
        pMed2 = new TGeoMedium("Air", numed, pMat2, par);
    }

    // Mixture: NaI
    TGeoMedium* pMed10 = NULL;
    if (gGeoManager->GetMedium("NaIn"))
    {
        pMed10 = gGeoManager->GetMedium("NaIn");
    }
    else
    {
        nel = 2;
        density = 3.670000;
        TGeoMixture* pMat10 = new TGeoMixture("NaIn", nel, density);
        a = 22.989770;
        z = 11.000000;
        w = 0.15337; // Na
        pMat10->DefineElement(0, a, z, w);
        a = 126.904470;
        z = 53.000000;
        w = 0.84663; // I
        pMat10->DefineElement(1, a, z, w);
        pMat10->SetIndex(611);
        Double_t par[8];
        par[0] = 0.000000; // isvol
        par[1] = 0.000000; // ifield
        par[2] = 0.000000; // fieldm
        par[3] = 0.000000; // tmaxfd
        par[4] = 0.000000; // stemax
        par[5] = 0.000000; // deemax
        par[6] = 0.000100; // epsil
        par[7] = 0.000000; // stmin
        pMed10 = new TGeoMedium("NaIn", 2, pMat10, par);
    }

    // Mixture: CsI
    TGeoMedium* pMed9 = NULL;
    if (gGeoManager->GetMedium("CsIn"))
        pMed9 = gGeoManager->GetMedium("CsIn");
    else
    {
        nel = 2;
        density = 4.510000;
        TGeoMixture* pMat9 = new TGeoMixture("CsIn", nel, density);
        a = 132.905450;
        z = 55.000000;
        w = 0.511549; // CS
        pMat9->DefineElement(0, a, z, w);
        a = 126.904470;
        z = 53.000000;
        w = 0.488451; // I
        pMat9->DefineElement(1, a, z, w);
        pMat9->SetIndex(601);
        Double_t par[8];
        par[0] = 0.000000; // isvol
        par[1] = 0.000000; // ifield
        par[2] = 0.000000; // fieldm
        par[3] = 0.000000; // tmaxfd
        par[4] = 0.000000; // stemax
        par[5] = 0.000000; // deemax
        par[6] = 0.000100; // epsil
        par[7] = 0.000000; // stmin
        pMed9 = new TGeoMedium("CsIn", 2, pMat9, par);
    }

    // Material: Aluminum
    TGeoMedium* pMed21 = NULL;
    if (gGeoManager->GetMedium("Aluminum"))
        pMed21 = gGeoManager->GetMedium("Aluminum");
    else
    {
        a = 26.980000;
        z = 13.000000;
        density = 2.700000;
        radl = 8.875105;
        absl = 388.793113;
        TGeoMaterial* matAl = new TGeoMaterial("Aluminum", a, z, density, radl, absl);
        Double_t par[8];
        par[0] = 0.000000; // isvol
        par[1] = 0.000000; // ifield
        par[2] = 0.000000; // fieldm
        par[3] = 0.000000; // tmaxfd
        par[4] = 0.000000; // stemax
        par[5] = 0.000000; // deemax
        par[6] = 0.000100; // epsil
        par[7] = 0.000000; // stmin
        pMed21 = new TGeoMedium("Aluminum", 3, matAl, par);
    }

    // Create volume: crystalLogNAJA
    TGeoShape* pTRA = createCrystal(xb_crystal_parts[0]);
    pTRA->SetTitle("TRA1+TRA2");
    TGeoVolume* pcrystalLogNAJA = new TGeoVolume("crystalLogNAJA", pTRA, pMed10);

    // Create volume: crystalLogNAJB
    TGeoShape* pTRB = createCrystal(xb_crystal_parts[1]);
    pTRB->SetTitle("TRB1+TRB2");
    TGeoVolume* pcrystalLogNAJB = new TGeoVolume("crystalLogNAJB", pTRB, pMed10);

    // Create volume: crystalLogNAJC
    TGeoShape* pTRC = createCrystal(xb_crystal_parts[2]);
    pTRC->SetTitle("TRC1+TRC2");
    TGeoVolume* pcrystalLogNAJC = new TGeoVolume("crystalLogNAJC", pTRC, pMed10);

    // Create volume: crystalLogNAJC
    TGeoShape* pTRD = createCrystal(xb_crystal_parts[3]);
    pTRD->SetTitle("TRD1+TRD2");
    TGeoVolume* pcrystalLogNAJD = new TGeoVolume("crystalLogNAJD", pTRD, pMed10);

    TGeoVolume* crystalVolumes[] = { pcrystalLogNAJA, pcrystalLogNAJB, pcrystalLogNAJC, pcrystalLogNAJD };

    // ---- Geometry Definition
    // Get the top Volume
    TGeoVolume* top = gGeoManager->GetTopVolume();

    // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
    // Shape: CBSphereWorld type: TGeoSphere
    rmin = 24.250000;
    rmax = 70.000000;
    theta1 = 0.000000;
    theta2 = 180.000000;
    phi1 = 0.000000;
    phi2 = 360.000000;
    TGeoShape* pCBSphereWorld = new TGeoSphere("CBSphereWorld", rmin, rmax, theta1, theta2, phi1, phi2);
    // Volume: CBLogWorld
    TGeoVolume* pCBLogWorld = new TGeoVolume("CBLogWorld", pCBSphereWorld, pMed2);
    pCBLogWorld->SetVisLeaves(kTRUE);

    TGeoRotation* zeroRot = new TGeoRotation;
    TGeoTranslation* zeroTrans = new TGeoTranslation;
    TGeoCombiTrans* zeroRotTrans = new TGeoCombiTrans(*zeroTrans, *zeroRot);

    // add the sphere as Mother Volume
    top->AddNode(pCBLogWorld, 0, zeroRotTrans);

    // --- Definition of the Reaction Chamber
    // Combi transformation:
    dx = 0.000000;
    dy = 0.000000;
    dz = 39.100000;
    // Rotation:
    TGeoCombiTrans* pCombTrans1 = new TGeoCombiTrans("", dx, dy, dz, zeroRot);
    // Combi transformation:
    dx = 0.000000;
    dy = 0.000000;
    dz = -39.100000;
    // Rotation:
    TGeoRotation* pRot1 = new TGeoRotation("", 0, 180, 0);
    TGeoCombiTrans* pCombTrans2 = new TGeoCombiTrans("", dx, dy, dz, pRot1);
    // Combi transformation:
    dx = 0.000000;
    dy = -39.100000;
    dz = 0.000000;
    // Rotation:
    TGeoRotation* pRot2 = new TGeoRotation("", 0, 90, 0);
    TGeoCombiTrans* pCombTrans3 = new TGeoCombiTrans("", dx, dy, dz, pRot2);

    // Shape: CBReactionChamber type: TGeoSphere
    rmin = 24.250000;
    rmax = 24.500000;
    theta1 = 6.423161;
    theta2 = 173.576839;
    phi1 = 0.000000;
    phi2 = 360.000000;
    TGeoShape* pCBReactionChamber = new TGeoSphere("CBReactionChamber", rmin, rmax, theta1, theta2, phi1, phi2);
    // Volume: CBChamberLog
    TGeoVolume* pCBChamberLog = new TGeoVolume("CBChamberLog", pCBReactionChamber, pMed21);
    pCBChamberLog->SetVisLeaves(kTRUE);
    pCBLogWorld->AddNode(pCBChamberLog, 0, zeroRotTrans);

    // Shape: CBConicalTube type: TGeoConeSeg
    dz = 14.600000;
    rmin1 = 2.730000;
    rmax1 = 2.855000;
    rmin2 = 5.125000;
    rmax2 = 5.250000;
    phi1 = 0.000000;
    phi2 = 360.000000;
    TGeoShape* pCBConicalTube = new TGeoConeSeg("CBConicalTube", dz, rmin1, rmax1, rmin2, rmax2, phi1, phi2);
    // Volume: CBConicalTubeLog
    TGeoVolume* pCBConicalTubeLog = new TGeoVolume("CBConicalTubeLog", pCBConicalTube, pMed21);
    pCBConicalTubeLog->SetVisLeaves(kTRUE);
    pCBLogWorld->AddNode(pCBConicalTubeLog, 0, pCombTrans1);
    pCBLogWorld->AddNode(pCBConicalTubeLog, 1, pCombTrans2);
    pCBLogWorld->AddNode(pCBConicalTubeLog, 2, pCombTrans3);

    // Add modified crystals to the crystal ball, defined in "xb_crystal_mod.hh"
    int nrModifiedCrystals = sizeof(xb_crystals_mod) / sizeof(xb_crystals_mod[0]);
    for (int i = 0; i < nrModifiedCrystals; i++)
    {
        xb_crystals[xb_crystals_mod[i].no - 1].active = false;
        if (xb_crystals_mod[i].mod == MOVE_R)
            insertCrystal(&xb_crystals[xb_crystals_mod[i].no - 1],
                          crystalVolumes,
                          pCBLogWorld,
                          crystalPosRad + xb_crystals_mod[i].delta);
    }

    // Add all enabled crystals to the crystal ball
    for (int i = 0; i < nrCrystals; i++)
    {
        if (xb_crystals[i].active)
            insertCrystal(&xb_crystals[i], crystalVolumes, pCBLogWorld, crystalPosRad);
    }

    // End of XBall Crystal definition
    // Define Sensitive Modules
    AddSensitiveVolume(pcrystalLogNAJA);
    AddSensitiveVolume(pcrystalLogNAJB);
    AddSensitiveVolume(pcrystalLogNAJC);
    AddSensitiveVolume(pcrystalLogNAJD);
}

// -----   Private method insertCrystal   ----------------------------------
// Add a crystal in the right location using radious and information from "xb_crysta_loc.hh"
void R3BXBallv1::insertCrystal(xb_crystal_v1* crystal, TGeoVolume** crystalVolumes, TGeoVolume* worldVolume, double r)
{
    double conv = TMath::DegToRad();
    double dx = r * sin(crystal->theta * conv) * cos(crystal->phi * conv);
    double dy = r * sin(crystal->theta * conv) * sin(crystal->phi * conv);
    double dz = r * cos(crystal->theta * conv);

    TGeoTranslation* trans = new TGeoTranslation(dx, dy, dz);
    TGeoRotation* pRot = new TGeoRotation;
    pRot->RotateZ(crystal->psi);
    pRot->RotateY(crystal->theta);
    pRot->RotateZ(crystal->phi);

    TGeoCombiTrans* combi = new TGeoCombiTrans(*trans, *pRot);

    worldVolume->AddNode(crystalVolumes[crystal->type], crystal->no, combi);
}

// -----   Private method createCrystal   ----------------------------------
// Create TGeoShape from two TGeoArb8 using the information in "xb_crysta_part_loc.hh"
TGeoShape* R3BXBallv1::createCrystal(double* arbCrystals)
{
    double innerR = 25.; //[cm]
    double dZ = 10.;     //[cm]

    // frameR is the radious of the frame which was the radious used during cornar definitions
    double frameR = 49.86932985; // [cm]
    double scaleOuter = (innerR + (2 * dZ)) / frameR;
    double scaleInner = innerR / frameR;

    double crystal1[][2] = { arbCrystals[0] * scaleInner, arbCrystals[1] * scaleInner, arbCrystals[2] * scaleInner,
                             arbCrystals[3] * scaleInner, arbCrystals[4] * scaleInner, arbCrystals[5] * scaleInner,
                             arbCrystals[6] * scaleInner, arbCrystals[7] * scaleInner, arbCrystals[0] * scaleOuter,
                             arbCrystals[1] * scaleOuter, arbCrystals[2] * scaleOuter, arbCrystals[3] * scaleOuter,
                             arbCrystals[4] * scaleOuter, arbCrystals[5] * scaleOuter, arbCrystals[6] * scaleOuter,
                             arbCrystals[7] * scaleOuter };

    double crystal2[][2] = { arbCrystals[8] * scaleInner,  arbCrystals[9] * scaleInner,  arbCrystals[10] * scaleInner,
                             arbCrystals[11] * scaleInner, arbCrystals[12] * scaleInner, arbCrystals[13] * scaleInner,
                             arbCrystals[14] * scaleInner, arbCrystals[15] * scaleInner, arbCrystals[8] * scaleOuter,
                             arbCrystals[9] * scaleOuter,  arbCrystals[10] * scaleOuter, arbCrystals[11] * scaleOuter,
                             arbCrystals[12] * scaleOuter, arbCrystals[13] * scaleOuter, arbCrystals[14] * scaleOuter,
                             arbCrystals[15] * scaleOuter };

    TGeoArb8* pTRA1 = new TGeoArb8("PARB1", dZ, *crystal1);
    TGeoArb8* pTRA2 = new TGeoArb8("PARB2", dZ, *crystal2);
    TGeoBoolNode* pBoolNode = new TGeoUnion(pTRA1, pTRA2, 0, 0);
    TGeoShape* pTRA = new TGeoCompositeShape("", pBoolNode);
    return pTRA;
}
ClassImp(R3BXBallv1)
