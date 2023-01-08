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
// -----                        R3BXBallv0 source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -----                  new Version: 20/07/09 <D.Bertini@gsi.de>     -----
// -------------------------------------------------------------------------
#include "R3BXBallv0.h"

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
#include "TGeoSphere.h"
#include "TGeoTube.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TVirtualMC.h"
// try the mathematical ROOT package
#include "Math/Point3D.h"
#include "Math/Rotation3D.h"

// -----   Default constructor   -------------------------------------------
R3BXBallv0::R3BXBallv0()
    : R3BDetector("R3BXBallv0", kTRUE, kCAL)
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
R3BXBallv0::R3BXBallv0(const char* name, Bool_t active)
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
R3BXBallv0::~R3BXBallv0()
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

void R3BXBallv0::Initialize()
{
    FairDetector::Initialize();

    LOG(info) << "R3BXBallv0 initialisation";
    LOG(debug) << "R3BXBallv0: Vol. (McId)";
    LOG(debug) << "R3BXBallv0: Crystal A   : " << gMC->VolId("crystalLogNAJA");
    LOG(debug) << "R3BXBallv0: Crystal B   : " << gMC->VolId("crystalLogNAJB");
    LOG(debug) << "R3BXBallv0: Crystal C   : " << gMC->VolId("crystalLogNAJC");
    LOG(debug) << "R3BXBallv0: Crystal D   : " << gMC->VolId("crystalLogNAJD");

    // Crystals type ID
    //  type   ID
    //   A     1
    //   B     2
    //   C     3
    //   D     4

    fTypeA = gMC->VolId("crystalLogNAJA");
    fTypeB = gMC->VolId("crystalLogNAJB");
    fTypeC = gMC->VolId("crystalLogNAJC");
    fTypeD = gMC->VolId("crystalLogNAJD");

    TGeoVolume* vol = gGeoManager->GetVolume("CBLogWorld");
    vol->SetVisibility(kFALSE);
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BXBallv0::ProcessHits(FairVolume* vol)
{

    Int_t copyNo = 0;
    Int_t fVolID = gMC->CurrentVolID(copyNo);
    Int_t fCrystalType = GetCrystalType(fVolID);

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
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
            //   TGeoNode *bla = gGeoManager->FindNextBoundary(2);
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

// -----   Public method EndOfEvent   -----------------------------------------
void R3BXBallv0::BeginEvent() {}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BXBallv0::EndOfEvent()
{

    if (fVerboseLevel)
        Print();
    fXBallCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BXBallv0::Register()
{
    FairRootManager::Instance()->Register("CrystalPoint", GetName(), fXBallCollection, kTRUE);
}
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BXBallv0::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fXBallCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BXBallv0::Print() const
{
    Int_t nHits = fXBallCollection->GetEntriesFast();
    LOG(info) << "R3BXBallv0: " << nHits << " points registered in this event";
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BXBallv0::Reset()
{
    fXBallCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BXBallv0::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(info) << "R3BXBallv0: " << nEntries << " entries to add";
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
    LOG(info) << "R3BXBallv0: " << cl2->GetEntriesFast() << " merged entries";
}

// -----   Private method AddHit   --------------------------------------------
R3BXBallPoint* R3BXBallv0::AddHit(Int_t trackID,
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
        LOG(info) << "R3BXBallv0: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
                  << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV";
    return new (clref[size]) R3BXBallPoint(trackID, detID, type, cp, posIn, posOut, momIn, momOut, time, length, eLoss);
}

void R3BXBallv0::ConstructGeometry() { return ConstructGeometry2(); }

void R3BXBallv0::ConstructGeometry2()
{

    LOG(info) << "##################################################################"

              << "*     -I- R3BXBallv0 R3BXBallv0::ConstructGeometry()                     *"

              << "*        Darmstadt-Heidelberg Crystal Ball - CAVE C Setup        *"

              << "##################################################################";

    // out-of-file geometry definition
    Double_t dx, dy, dz;
    Double_t theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2;
    Double_t rmin, rmax, rmin1, rmax1, rmin2, rmax2;
    Double_t a;
    Double_t thx, phx, thy, phy, thz, phz;
    Double_t theta1, theta2, phi1, phi2, dphi;
    Double_t z, density, radl, absl, w;
    Int_t nel, numed, nz, nedges;
    TGeoBoolNode* pBoolNode = 0;

    // --- Local Material definition

    // Mixture: Air
    TGeoMedium* pMed2 = NULL;
    if (gGeoManager->GetMedium("Air"))
    {
        pMed2 = gGeoManager->GetMedium("Air");
    }
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

    // Mixture: CsI
    TGeoMedium* pMed9 = NULL;
    if (gGeoManager->GetMedium("CsIn"))
    {
        pMed9 = gGeoManager->GetMedium("CsIn");
    }
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
    {
        pMed21 = gGeoManager->GetMedium("Aluminum");
    }
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

    TGeoRotation* pGlobalRot = createMatrix(TMath::Pi() * TMath::RadToDeg() / 2., 0.0, 0.0);
    TGeoTranslation* pGlobalt = new TGeoTranslation(0.0, 0.0, 0.0);
    TGeoCombiTrans* pGlobalc = new TGeoCombiTrans(*pGlobalt, *pGlobalRot);

    //  TGeoCombiTrans *pG = GetGlobalPosition();
    //  TGeoHMatrix pTotal = (*pG) * (*pGlobalc);

    // add the sphere as Mother Volume
    top->AddNode(pCBLogWorld, 0, GetGlobalPosition(pGlobalc));

    // Shape: PGON type: TGeoPgon
    phi1 = 0.000000;
    dphi = 360.000000;
    nedges = 5;
    nz = 2;
    Double_t zPlane[2] = { -10.0, 10.0 };
    Double_t rInner[2] = { 0., 0. };
    Double_t rOuter[2] = { 3.688, 6.638 };
    TGeoPgon* pgon = new TGeoPgon("PGON", phi1, dphi, nedges, nz);
    pgon->DefineSection(0, zPlane[0], rInner[0], rOuter[0]);
    pgon->DefineSection(1, zPlane[1], rInner[1], rOuter[1]);
    TGeoShape* pPGON = pgon;

    Double_t NAJA_X[12] = { -184.0195, 184.0195, 0, 0, -297.7205, -297.7205,
                            297.7205,  297.7205, 0, 0, -184.0195, 184.0195 };
    Double_t NAJA_Y[12] = { 297.7205, 297.7205, 184.0195,  184.0195,  0,         0,
                            0,        0,        -184.0195, -184.0195, -297.7205, -297.7205 };
    Double_t NAJA_Z[12] = { 0,         0,        297.7205, -297.7205, -184.0195, 184.0195,
                            -184.0195, 184.0195, 297.7205, -297.7205, 0,         0 };

    TVector3 NAJA_Rot_col_X[12];
    NAJA_Rot_col_X[0].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Cos(211.720000 * TMath::Pi() / 180),
                             TMath::Sin(72.000000 * TMath::Pi() / 180) * TMath::Cos(31.720000 * TMath::Pi() / 180),
                             TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Cos(121.720000 * TMath::Pi() / 180));

    NAJA_Rot_col_X[1].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Cos(328.280000 * TMath::Pi() / 180),
                             TMath::Sin(108.000000 * TMath::Pi() / 180) * TMath::Cos(328.280000 * TMath::Pi() / 180),
                             TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Cos(58.280000 * TMath::Pi() / 180));

    NAJA_Rot_col_X[2].SetXYZ(TMath::Sin(99.350000 * TMath::Pi() / 180) * TMath::Cos(15.450000 * TMath::Pi() / 180),
                             TMath::Sin(120.000000 * TMath::Pi() / 180) * TMath::Cos(110.905000 * TMath::Pi() / 180),
                             TMath::Sin(31.718000 * TMath::Pi() / 180) * TMath::Cos(90.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_X[3].SetXYZ(TMath::Sin(80.650000 * TMath::Pi() / 180) * TMath::Cos(15.450000 * TMath::Pi() / 180),
                             TMath::Sin(120.000000 * TMath::Pi() / 180) * TMath::Cos(290.905000 * TMath::Pi() / 180),
                             TMath::Sin(148.282000 * TMath::Pi() / 180) * TMath::Cos(90.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_X[4].SetXYZ(TMath::Sin(105.24000 * TMath::Pi() / 180) * TMath::Cos(80.310000 * TMath::Pi() / 180),
                             TMath::Sin(144.00000 * TMath::Pi() / 180) * TMath::Cos(328.28620 * TMath::Pi() / 180),
                             TMath::Sin(121.717186 * TMath::Pi() / 180) * TMath::Cos(180.0035095 * TMath::Pi() / 180));

    NAJA_Rot_col_X[5].SetXYZ(TMath::Sin(74.760000 * TMath::Pi() / 180) * TMath::Cos(80.310000 * TMath::Pi() / 180),
                             TMath::Sin(144.000000 * TMath::Pi() / 180) * TMath::Cos(148.286600 * TMath::Pi() / 180),
                             TMath::Sin(58.2822647 * TMath::Pi() / 180) * TMath::Cos(180.0037079 * TMath::Pi() / 180));

    NAJA_Rot_col_X[6].SetXYZ(TMath::Sin(105.240000 * TMath::Pi() / 180) * TMath::Cos(99.690000 * TMath::Pi() / 180),
                             TMath::Sin(36.000000 * TMath::Pi() / 180) * TMath::Cos(31.713800 * TMath::Pi() / 180),
                             TMath::Sin(121.717000 * TMath::Pi() / 180) * TMath::Cos(359.9964905 * TMath::Pi() / 180));

    NAJA_Rot_col_X[7].SetXYZ(TMath::Sin(74.760000 * TMath::Pi() / 180) * TMath::Cos(99.690000 * TMath::Pi() / 180),
                             TMath::Sin(36.000000 * TMath::Pi() / 180) * TMath::Cos(211.713560 * TMath::Pi() / 180),
                             TMath::Sin(58.282450 * TMath::Pi() / 180) * TMath::Cos(359.996300 * TMath::Pi() / 180));

    NAJA_Rot_col_X[8].SetXYZ(TMath::Sin(99.350000 * TMath::Pi() / 180) * TMath::Cos(344.550000 * TMath::Pi() / 180),
                             TMath::Sin(60.000000 * TMath::Pi() / 180) * TMath::Cos(69.095000 * TMath::Pi() / 180),
                             TMath::Sin(31.717600 * TMath::Pi() / 180) * TMath::Cos(270.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_X[9].SetXYZ(TMath::Sin(80.650000 * TMath::Pi() / 180) * TMath::Cos(344.550000 * TMath::Pi() / 180),
                             TMath::Sin(60.000000 * TMath::Pi() / 180) * TMath::Cos(249.095000 * TMath::Pi() / 180),
                             TMath::Sin(148.282400 * TMath::Pi() / 180) * TMath::Cos(270.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_X[10].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Cos(148.280000 * TMath::Pi() / 180),
                              TMath::Sin(108.000000 * TMath::Pi() / 180) * TMath::Cos(148.280000 * TMath::Pi() / 180),
                              TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Cos(238.280000 * TMath::Pi() / 180));

    NAJA_Rot_col_X[11].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Cos(31.720000 * TMath::Pi() / 180),
                              TMath::Sin(72.000000 * TMath::Pi() / 180) * TMath::Cos(211.720000 * TMath::Pi() / 180),
                              TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Cos(301.720000 * TMath::Pi() / 180));

    TVector3 NAJA_Rot_col_Y[12];
    NAJA_Rot_col_Y[0].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Sin(211.720000 * TMath::Pi() / 180),
                             TMath::Sin(72.000000 * TMath::Pi() / 180) * TMath::Sin(31.720000 * TMath::Pi() / 180),
                             TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Sin(121.720000 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[1].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Sin(328.280000 * TMath::Pi() / 180),
                             TMath::Sin(108.000000 * TMath::Pi() / 180) * TMath::Sin(328.280000 * TMath::Pi() / 180),
                             TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Sin(58.280000 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[2].SetXYZ(TMath::Sin(99.350000 * TMath::Pi() / 180) * TMath::Sin(15.450000 * TMath::Pi() / 180),
                             TMath::Sin(120.000000 * TMath::Pi() / 180) * TMath::Sin(110.905000 * TMath::Pi() / 180),
                             TMath::Sin(31.718000 * TMath::Pi() / 180) * TMath::Sin(90.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[3].SetXYZ(TMath::Sin(80.650000 * TMath::Pi() / 180) * TMath::Sin(15.450000 * TMath::Pi() / 180),
                             TMath::Sin(120.000000 * TMath::Pi() / 180) * TMath::Sin(290.905000 * TMath::Pi() / 180),
                             TMath::Sin(148.282000 * TMath::Pi() / 180) * TMath::Sin(90.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[4].SetXYZ(TMath::Sin(105.24000 * TMath::Pi() / 180) * TMath::Sin(80.310000 * TMath::Pi() / 180),
                             TMath::Sin(144.00000 * TMath::Pi() / 180) * TMath::Sin(328.28620 * TMath::Pi() / 180),
                             TMath::Sin(121.717186 * TMath::Pi() / 180) * TMath::Sin(180.0035095 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[5].SetXYZ(TMath::Sin(74.760000 * TMath::Pi() / 180) * TMath::Sin(80.310000 * TMath::Pi() / 180),
                             TMath::Sin(144.000000 * TMath::Pi() / 180) * TMath::Sin(148.286600 * TMath::Pi() / 180),
                             TMath::Sin(58.2822647 * TMath::Pi() / 180) * TMath::Sin(180.0037079 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[6].SetXYZ(TMath::Sin(105.240000 * TMath::Pi() / 180) * TMath::Sin(99.690000 * TMath::Pi() / 180),
                             TMath::Sin(36.000000 * TMath::Pi() / 180) * TMath::Sin(31.713800 * TMath::Pi() / 180),
                             TMath::Sin(121.717000 * TMath::Pi() / 180) * TMath::Sin(359.9964905 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[7].SetXYZ(TMath::Sin(74.760000 * TMath::Pi() / 180) * TMath::Sin(99.690000 * TMath::Pi() / 180),
                             TMath::Sin(36.000000 * TMath::Pi() / 180) * TMath::Sin(211.713560 * TMath::Pi() / 180),
                             TMath::Sin(58.282450 * TMath::Pi() / 180) * TMath::Sin(359.996300 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[8].SetXYZ(TMath::Sin(99.350000 * TMath::Pi() / 180) * TMath::Sin(344.550000 * TMath::Pi() / 180),
                             TMath::Sin(60.000000 * TMath::Pi() / 180) * TMath::Sin(69.095000 * TMath::Pi() / 180),
                             TMath::Sin(31.717600 * TMath::Pi() / 180) * TMath::Sin(270.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[9].SetXYZ(TMath::Sin(80.650000 * TMath::Pi() / 180) * TMath::Sin(344.550000 * TMath::Pi() / 180),
                             TMath::Sin(60.000000 * TMath::Pi() / 180) * TMath::Sin(249.095000 * TMath::Pi() / 180),
                             TMath::Sin(148.282400 * TMath::Pi() / 180) * TMath::Sin(270.000000 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[10].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Sin(148.280000 * TMath::Pi() / 180),
                              TMath::Sin(108.000000 * TMath::Pi() / 180) * TMath::Sin(148.280000 * TMath::Pi() / 180),
                              TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Sin(238.280000 * TMath::Pi() / 180));

    NAJA_Rot_col_Y[11].SetXYZ(TMath::Sin(18.000000 * TMath::Pi() / 180) * TMath::Sin(31.720000 * TMath::Pi() / 180),
                              TMath::Sin(72.000000 * TMath::Pi() / 180) * TMath::Sin(211.720000 * TMath::Pi() / 180),
                              TMath::Sin(90.000000 * TMath::Pi() / 180) * TMath::Sin(301.720000 * TMath::Pi() / 180));

    TVector3 NAJA_Rot_col_Z[12];
    NAJA_Rot_col_Z[0].SetXYZ(TMath::Cos(18.000000 * TMath::Pi() / 180),
                             TMath::Cos(72.000000 * TMath::Pi() / 180),
                             TMath::Cos(90.000000 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[1].SetXYZ(TMath::Cos(18.000000 * TMath::Pi() / 180),
                             TMath::Cos(108.000000 * TMath::Pi() / 180),
                             TMath::Cos(90.000000 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[2].SetXYZ(TMath::Cos(99.350000 * TMath::Pi() / 180),
                             TMath::Cos(120.000000 * TMath::Pi() / 180),
                             TMath::Cos(31.718000 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[3].SetXYZ(TMath::Cos(80.650000 * TMath::Pi() / 180),
                             TMath::Cos(120.000000 * TMath::Pi() / 180),
                             TMath::Cos(148.282000 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[4].SetXYZ(TMath::Cos(105.24000 * TMath::Pi() / 180),
                             TMath::Cos(144.00000 * TMath::Pi() / 180),
                             TMath::Cos(121.717186 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[5].SetXYZ(TMath::Cos(74.760000 * TMath::Pi() / 180),
                             TMath::Cos(144.000000 * TMath::Pi() / 180),
                             TMath::Cos(58.2822647 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[6].SetXYZ(TMath::Cos(105.240000 * TMath::Pi() / 180),
                             TMath::Cos(36.000000 * TMath::Pi() / 180),
                             TMath::Cos(121.717000 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[7].SetXYZ(TMath::Cos(74.760000 * TMath::Pi() / 180),
                             TMath::Cos(36.000000 * TMath::Pi() / 180),
                             TMath::Cos(58.282450 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[8].SetXYZ(TMath::Cos(99.350000 * TMath::Pi() / 180),
                             TMath::Cos(60.000000 * TMath::Pi() / 180),
                             TMath::Cos(31.717600 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[9].SetXYZ(TMath::Cos(80.650000 * TMath::Pi() / 180),
                             TMath::Cos(60.000000 * TMath::Pi() / 180),
                             TMath::Cos(148.282400 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[10].SetXYZ(TMath::Cos(18.000000 * TMath::Pi() / 180),
                              TMath::Cos(108.000000 * TMath::Pi() / 180),
                              TMath::Cos(90.000000 * TMath::Pi() / 180));
    NAJA_Rot_col_Z[11].SetXYZ(TMath::Cos(18.000000 * TMath::Pi() / 180),
                              TMath::Cos(72.000000 * TMath::Pi() / 180),
                              TMath::Cos(90.000000 * TMath::Pi() / 180));

    // Volume: crystalLogNAJA
    TGeoVolume* pcrystalLogNAJA = new TGeoVolume("crystalLogNAJA", pPGON, pMed9);
    AddSensitiveVolume(pcrystalLogNAJA);
    pcrystalLogNAJA->SetVisLeaves(kTRUE);

    // -- First part defined
    TGeoRotation* pRot = NULL;
    for (Int_t iter1 = 0; iter1 < 12; iter1++)
    {

        Double_t fRotation[9] = {
            NAJA_Rot_col_X[iter1].X(), NAJA_Rot_col_X[iter1].Y(), NAJA_Rot_col_X[iter1].Z(),
            NAJA_Rot_col_Y[iter1].X(), NAJA_Rot_col_Y[iter1].Y(), NAJA_Rot_col_Y[iter1].Z(),
            NAJA_Rot_col_Z[iter1].X(), NAJA_Rot_col_Z[iter1].Y(), NAJA_Rot_col_Z[iter1].Z(),
        };

        pRot = new TGeoRotation();
        pRot->SetMatrix(fRotation);

        dx = NAJA_X[iter1] / 10.;
        dy = NAJA_Y[iter1] / 10.;
        dz = NAJA_Z[iter1] / 10.;

        TGeoTranslation* trans = new TGeoTranslation(dx, dy, dz);
        TGeoCombiTrans* combi = new TGeoCombiTrans(*trans, *pRot);

        pCBLogWorld->AddNode(pcrystalLogNAJA, iter1, combi);

    } //! for iter1

    //-- Adding single crystal

    // Shape: TRB1 type: TGeoTrap
    dz = 10.000000;
    theta = 0.000000;
    phi = 0.000000;
    h1 = 1.542000;
    bl1 = 4.848000;
    tl1 = 2.658000;
    alpha1 = 0.000000;
    h2 = 2.778000;
    bl2 = 8.726000;
    tl2 = 4.780000;
    alpha2 = 0.000000;
    TGeoShape* pTRB1 = new TGeoTrap("TRB1", dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
    // Shape: TRB2 type: TGeoTrap
    dz = 10.000000;
    theta = 0.000000;
    phi = 0.000000;
    h1 = 1.863000;
    bl1 = 4.850000;
    tl1 = 2.162000;
    alpha1 = 0.000000;
    h2 = 3.353000;
    bl2 = 8.730000;
    tl2 = 3.892000;
    alpha2 = 0.000000;
    TGeoShape* pTRB2 = new TGeoTrap("TRB2", dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
    // Combi transformation: TRB
    dx = 0.000000;
    dy = -4.748000;
    dz = -0.292480;
    // Rotation:
    thx = 90.000000;
    phx = 180.000000;
    thy = 97.775000;
    phy = 270.000000;
    thz = 7.775000;
    phz = 270.000000;
    TGeoRotation* pMatrix3 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("TRB", dx, dy, dz, pMatrix3);
    // Union for TRB1 and TRB2
    pBoolNode = new TGeoUnion(pTRB1, pTRB2, 0, pMatrix2);
    // Shape: TRB type: TGeoCompositeShape
    TGeoShape* pTRB = new TGeoCompositeShape("TRB", pBoolNode);
    pTRB->SetTitle("TRB1+TRB2:TRB");
    // Volume: crystalLogNAJB
    TGeoVolume* pcrystalLogNAJB = new TGeoVolume("crystalLogNAJB", pTRB, pMed9);

    Double_t NAJB_X[60] = { -93.1805,  93.1805,   -152.88,   -152.88,   152.88,    152.88,    -241.4999, -241.4999,
                            -54.7505,  54.7505,   241.4999,  241.4999,  54.7505,   -54.7505,  -88.6375,  88.6375,
                            88.6375,   -88.6375,  -247.3589, -302.1269, -302.1269, -247.3589, 0,         247.3589,
                            302.1269,  302.1269,  247.3589,  0,         -337.3685, -337.3685, 337.3685,  337.3685,
                            0,         -247.3589, -302.1269, -302.1269, -247.3589, 0,         247.3589,  302.1269,
                            302.1269,  247.3589,  88.6375,   -88.6375,  -88.6375,  88.6375,   54.7505,   -54.7505,
                            -241.4999, -241.4999, -54.7505,  54.7505,   241.4999,  241.4999,  -152.88,   -152.88,
                            152.88,    152.88,    -93.1805,  93.1805 };
    Double_t NAJB_Y[60] = { 337.3685,  337.3685,  302.1129,  302.1129,  302.1129,  302.1129,  247.3274,  247.3274,
                            241.4789,  241.4789,  247.3274,  247.3274,  241.4789,  241.4789,  152.873,   152.873,
                            152.873,   152.873,   54.775,    88.6375,   88.6375,   54.775,    98.056,    54.775,
                            88.6375,   88.6375,   54.775,    98.056,    0,         0,         0,         0,
                            -98.056,   -54.775,   -88.6375,  -88.6375,  -54.775,   -98.056,   -54.775,   -88.6375,
                            -88.6375,  -54.775,   -152.873,  -152.873,  -152.873,  -152.873,  -241.4789, -241.4789,
                            -247.3274, -247.3274, -241.4789, -241.4789, -247.3274, -247.3274, -302.1129, -302.1129,
                            -302.1129, -302.1129, -337.3685, -337.3685 };
    Double_t NAJB_Z[60] = { 0,         0,         -88.6305,  88.6305,   88.6305,   -88.6305,  -54.831,   54.831,
                            247.3659,  247.3659,  54.831,    -54.831,   -247.3659, -247.3659, 302.1129,  302.1129,
                            -302.1129, -302.1129, -241.4824, -152.8485, 152.8485,  241.4824,  335.9824,  241.4824,
                            152.8485,  -152.8485, -241.4824, -335.9824, -93.1805,  93.1805,   -93.1805,  93.1805,
                            335.9824,  241.4824,  152.8485,  -152.8485, -241.4824, -335.9824, -241.4824, -152.8485,
                            152.8485,  241.4824,  302.1129,  302.1129,  -302.1129, -302.1129, 247.3659,  247.3659,
                            54.831,    -54.831,   -247.3659, -247.3659, -54.831,   54.831,    88.6305,   -88.6305,
                            -88.6305,  88.6305,   0,         0 };

    TVector3 NAJB_Rot_col_X[60];
    NAJB_Rot_col_X[0].SetXYZ(TMath::Sin(179.999939 * TMath::Pi() / 180) * TMath::Cos(-121.714996 * TMath::Pi() / 180),
                             TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(-163.721970 * TMath::Pi() / 180),
                             TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Cos(106.278061 * TMath::Pi() / 180));

    NAJB_Rot_col_X[1].SetXYZ(TMath::Sin(0.000003 * TMath::Pi() / 180) * TMath::Cos(-121.720016 * TMath::Pi() / 180),
                             TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Cos(-16.278015 * TMath::Pi() / 180),
                             TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(73.721939 * TMath::Pi() / 180));

    NAJB_Rot_col_X[2].SetXYZ(TMath::Sin(108.000031 * TMath::Pi() / 180) * TMath::Cos(-148.279999 * TMath::Pi() / 180),
                             TMath::Sin(23.548264 * TMath::Pi() / 180) * TMath::Cos(169.926025 * TMath::Pi() / 180),
                             TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Cos(116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_X[3].SetXYZ(TMath::Sin(108.000092 * TMath::Pi() / 180) * TMath::Cos(31.719986 * TMath::Pi() / 180),
                             TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Cos(169.926025 * TMath::Pi() / 180),
                             TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Cos(116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_X[4].SetXYZ(TMath::Sin(71.999969 * TMath::Pi() / 180) * TMath::Cos(-31.720016 * TMath::Pi() / 180),
                             TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Cos(10.073961 * TMath::Pi() / 180),
                             TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Cos(63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_X[5].SetXYZ(TMath::Sin(71.999908 * TMath::Pi() / 180) * TMath::Cos(148.279953 * TMath::Pi() / 180),
                             TMath::Sin(23.548248 * TMath::Pi() / 180) * TMath::Cos(10.073951 * TMath::Pi() / 180),
                             TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Cos(63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_X[6].SetXYZ(TMath::Sin(35.999985 * TMath::Pi() / 180) * TMath::Cos(-148.279999 * TMath::Pi() / 180),
                             TMath::Sin(55.488815 * TMath::Pi() / 180) * TMath::Cos(50.572174 * TMath::Pi() / 180),
                             TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Cos(134.317352 * TMath::Pi() / 180));

    NAJB_Rot_col_X[7].SetXYZ(TMath::Sin(35.999985 * TMath::Pi() / 180) * TMath::Cos(31.719986 * TMath::Pi() / 180),
                             TMath::Sin(124.511108 * TMath::Pi() / 180) * TMath::Cos(50.572174 * TMath::Pi() / 180),
                             TMath::Sin(80.995865 * TMath::Pi() / 180) * TMath::Cos(134.317398 * TMath::Pi() / 180));

    NAJB_Rot_col_X[8].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Cos(-148.283234 * TMath::Pi() / 180),
                             TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Cos(-42.728638 * TMath::Pi() / 180),
                             TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Cos(102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_X[9].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Cos(148.283234 * TMath::Pi() / 180),
                             TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Cos(-137.271378 * TMath::Pi() / 180),
                             TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Cos(77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_X[10].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Cos(-31.720016 * TMath::Pi() / 180),
                              TMath::Sin(124.511169 * TMath::Pi() / 180) * TMath::Cos(129.427750 * TMath::Pi() / 180),
                              TMath::Sin(80.995819 * TMath::Pi() / 180) * TMath::Cos(45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_X[11].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Cos(148.279953 * TMath::Pi() / 180),
                              TMath::Sin(55.488846 * TMath::Pi() / 180) * TMath::Cos(129.427811 * TMath::Pi() / 180),
                              TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Cos(45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_X[12].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Cos(-31.716766 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Cos(-137.271317 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Cos(77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_X[13].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Cos(31.716766 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Cos(-42.728607 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Cos(102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_X[14].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Cos(-69.094238 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Cos(23.223938 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Cos(120.102737 * TMath::Pi() / 180));

    NAJB_Rot_col_X[15].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Cos(69.094284 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Cos(156.776047 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Cos(59.897247 * TMath::Pi() / 180));

    NAJB_Rot_col_X[16].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Cos(-110.905762 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Cos(156.776047 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Cos(59.897308 * TMath::Pi() / 180));

    NAJB_Rot_col_X[17].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Cos(110.905701 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Cos(23.223938 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Cos(120.102692 * TMath::Pi() / 180));

    NAJB_Rot_col_X[18].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Cos(-69.093414 * TMath::Pi() / 180),
                              TMath::Sin(58.443619 * TMath::Pi() / 180) * TMath::Cos(-138.326401 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Cos(167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_X[19].SetXYZ(TMath::Sin(143.997803 * TMath::Pi() / 180) * TMath::Cos(31.715622 * TMath::Pi() / 180),
                              TMath::Sin(113.163864 * TMath::Pi() / 180) * TMath::Cos(-94.359375 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Cos(163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_X[20].SetXYZ(TMath::Sin(143.997803 * TMath::Pi() / 180) * TMath::Cos(-148.284317 * TMath::Pi() / 180),
                              TMath::Sin(66.836121 * TMath::Pi() / 180) * TMath::Cos(-94.359436 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Cos(163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_X[21].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Cos(110.906570 * TMath::Pi() / 180),
                              TMath::Sin(121.556427 * TMath::Pi() / 180) * TMath::Cos(-138.326401 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Cos(167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_X[22].SetXYZ(TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Cos(-0.000001 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Cos(90.000076 * TMath::Pi() / 180),
                              TMath::Sin(16.277985 * TMath::Pi() / 180) * TMath::Cos(89.999969 * TMath::Pi() / 180));

    NAJB_Rot_col_X[23].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Cos(-110.906570 * TMath::Pi() / 180),
                              TMath::Sin(121.556366 * TMath::Pi() / 180) * TMath::Cos(-41.673569 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Cos(12.487767 * TMath::Pi() / 180));

    NAJB_Rot_col_X[24].SetXYZ(TMath::Sin(36.002151 * TMath::Pi() / 180) * TMath::Cos(148.284378 * TMath::Pi() / 180),
                              TMath::Sin(66.836121 * TMath::Pi() / 180) * TMath::Cos(-85.640564 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Cos(16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_X[25].SetXYZ(TMath::Sin(36.002136 * TMath::Pi() / 180) * TMath::Cos(-31.715652 * TMath::Pi() / 180),
                              TMath::Sin(113.163864 * TMath::Pi() / 180) * TMath::Cos(-85.640564 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Cos(16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_X[26].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Cos(69.093414 * TMath::Pi() / 180),
                              TMath::Sin(58.443573 * TMath::Pi() / 180) * TMath::Cos(-41.673553 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Cos(12.487760 * TMath::Pi() / 180));

    NAJB_Rot_col_X[27].SetXYZ(TMath::Sin(89.999908 * TMath::Pi() / 180) * TMath::Cos(180.000000 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Cos(89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Cos(90.000076 * TMath::Pi() / 180));

    NAJB_Rot_col_X[28].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Cos(-0.000009 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Cos(180.000000 * TMath::Pi() / 180));

    NAJB_Rot_col_X[29].SetXYZ(TMath::Sin(89.999908 * TMath::Pi() / 180) * TMath::Cos(-89.999908 * TMath::Pi() / 180),
                              TMath::Sin(16.278000 * TMath::Pi() / 180) * TMath::Cos(0.000002 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Cos(-180.000000 * TMath::Pi() / 180));

    NAJB_Rot_col_X[30].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(-89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Cos(180.000000 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Cos(-0.000001 * TMath::Pi() / 180));

    NAJB_Rot_col_X[31].SetXYZ(TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Cos(90.000076 * TMath::Pi() / 180),
                              TMath::Sin(16.278015 * TMath::Pi() / 180) * TMath::Cos(-179.999939 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Cos(0.000000 * TMath::Pi() / 180));

    NAJB_Rot_col_X[32].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(-180.000000 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Cos(-89.999969 * TMath::Pi() / 180),
                              TMath::Sin(16.277985 * TMath::Pi() / 180) * TMath::Cos(-89.999969 * TMath::Pi() / 180));

    NAJB_Rot_col_X[33].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Cos(69.093414 * TMath::Pi() / 180),
                              TMath::Sin(121.556366 * TMath::Pi() / 180) * TMath::Cos(138.326401 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Cos(-167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_X[34].SetXYZ(TMath::Sin(36.002136 * TMath::Pi() / 180) * TMath::Cos(-31.715622 * TMath::Pi() / 180),
                              TMath::Sin(66.836060 * TMath::Pi() / 180) * TMath::Cos(94.359436 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Cos(-163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_X[35].SetXYZ(TMath::Sin(36.002136 * TMath::Pi() / 180) * TMath::Cos(148.284317 * TMath::Pi() / 180),
                              TMath::Sin(113.163864 * TMath::Pi() / 180) * TMath::Cos(94.359436 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Cos(-163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_X[36].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Cos(-110.906570 * TMath::Pi() / 180),
                              TMath::Sin(58.443573 * TMath::Pi() / 180) * TMath::Cos(138.326401 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Cos(-167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_X[37].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(0.000001 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Cos(-89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Cos(-89.999969 * TMath::Pi() / 180));

    NAJB_Rot_col_X[38].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Cos(110.906570 * TMath::Pi() / 180),
                              TMath::Sin(58.443573 * TMath::Pi() / 180) * TMath::Cos(41.673584 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Cos(-12.487763 * TMath::Pi() / 180));

    NAJB_Rot_col_X[39].SetXYZ(TMath::Sin(143.997864 * TMath::Pi() / 180) * TMath::Cos(-148.284317 * TMath::Pi() / 180),
                              TMath::Sin(113.163925 * TMath::Pi() / 180) * TMath::Cos(85.640564 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Cos(-16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_X[40].SetXYZ(TMath::Sin(143.997803 * TMath::Pi() / 180) * TMath::Cos(31.715637 * TMath::Pi() / 180),
                              TMath::Sin(66.836121 * TMath::Pi() / 180) * TMath::Cos(85.640610 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Cos(-16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_X[41].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Cos(-69.093414 * TMath::Pi() / 180),
                              TMath::Sin(121.556427 * TMath::Pi() / 180) * TMath::Cos(41.673553 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Cos(-12.487760 * TMath::Pi() / 180));

    NAJB_Rot_col_X[42].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Cos(110.905762 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Cos(-156.776047 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Cos(-59.897247 * TMath::Pi() / 180));

    NAJB_Rot_col_X[43].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Cos(-110.905701 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Cos(-23.223938 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Cos(-120.102737 * TMath::Pi() / 180));

    NAJB_Rot_col_X[44].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Cos(69.094238 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Cos(-23.223938 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Cos(-120.102692 * TMath::Pi() / 180));

    NAJB_Rot_col_X[45].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Cos(-69.094284 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Cos(-156.776047 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Cos(-59.897308 * TMath::Pi() / 180));

    NAJB_Rot_col_X[46].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Cos(31.716766 * TMath::Pi() / 180),
                              TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Cos(137.271317 * TMath::Pi() / 180),
                              TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Cos(-77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_X[47].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Cos(-31.716766 * TMath::Pi() / 180),
                              TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Cos(42.728607 * TMath::Pi() / 180),
                              TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Cos(-102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_X[48].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Cos(148.279953 * TMath::Pi() / 180),
                              TMath::Sin(124.511169 * TMath::Pi() / 180) * TMath::Cos(-50.572174 * TMath::Pi() / 180),
                              TMath::Sin(80.995819 * TMath::Pi() / 180) * TMath::Cos(-134.317398 * TMath::Pi() / 180));

    NAJB_Rot_col_X[49].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Cos(-31.719986 * TMath::Pi() / 180),
                              TMath::Sin(55.488846 * TMath::Pi() / 180) * TMath::Cos(-50.572174 * TMath::Pi() / 180),
                              TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Cos(-134.317398 * TMath::Pi() / 180));

    NAJB_Rot_col_X[50].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Cos(148.283234 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Cos(42.728638 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Cos(-102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_X[51].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Cos(-148.283234 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Cos(137.271378 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Cos(-77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_X[52].SetXYZ(TMath::Sin(35.999969 * TMath::Pi() / 180) * TMath::Cos(31.720001 * TMath::Pi() / 180),
                              TMath::Sin(55.488815 * TMath::Pi() / 180) * TMath::Cos(-129.427811 * TMath::Pi() / 180),
                              TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Cos(-45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_X[53].SetXYZ(TMath::Sin(35.999985 * TMath::Pi() / 180) * TMath::Cos(-148.279953 * TMath::Pi() / 180),
                              TMath::Sin(124.511108 * TMath::Pi() / 180) * TMath::Cos(-129.427811 * TMath::Pi() / 180),
                              TMath::Sin(80.995865 * TMath::Pi() / 180) * TMath::Cos(-45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_X[54].SetXYZ(TMath::Sin(71.999969 * TMath::Pi() / 180) * TMath::Cos(148.279953 * TMath::Pi() / 180),
                              TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Cos(-169.926025 * TMath::Pi() / 180),
                              TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Cos(-116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_X[55].SetXYZ(TMath::Sin(71.999908 * TMath::Pi() / 180) * TMath::Cos(-31.719986 * TMath::Pi() / 180),
                              TMath::Sin(23.548248 * TMath::Pi() / 180) * TMath::Cos(-169.926025 * TMath::Pi() / 180),
                              TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Cos(-116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_X[56].SetXYZ(TMath::Sin(108.000031 * TMath::Pi() / 180) * TMath::Cos(31.720001 * TMath::Pi() / 180),
                              TMath::Sin(23.548264 * TMath::Pi() / 180) * TMath::Cos(-10.073964 * TMath::Pi() / 180),
                              TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Cos(-63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_X[57].SetXYZ(TMath::Sin(108.000092 * TMath::Pi() / 180) * TMath::Cos(-148.279999 * TMath::Pi() / 180),
                              TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Cos(-10.073964 * TMath::Pi() / 180),
                              TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Cos(-63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_X[58].SetXYZ(TMath::Sin(0.000003 * TMath::Pi() / 180) * TMath::Cos(58.279968 * TMath::Pi() / 180),
                              TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Cos(163.721970 * TMath::Pi() / 180),
                              TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(-106.278061 * TMath::Pi() / 180));

    NAJB_Rot_col_X[59].SetXYZ(TMath::Sin(179.999939 * TMath::Pi() / 180) * TMath::Cos(58.285004 * TMath::Pi() / 180),
                              TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Cos(16.278000 * TMath::Pi() / 180),
                              TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Cos(-73.721939 * TMath::Pi() / 180));

    TVector3 NAJB_Rot_col_Y[60];
    NAJB_Rot_col_Y[0].SetXYZ(TMath::Sin(179.999939 * TMath::Pi() / 180) * TMath::Sin(-121.714996 * TMath::Pi() / 180),
                             TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(-163.721970 * TMath::Pi() / 180),
                             TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Sin(106.278061 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[1].SetXYZ(TMath::Sin(0.000003 * TMath::Pi() / 180) * TMath::Sin(-121.720016 * TMath::Pi() / 180),
                             TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Sin(-16.278015 * TMath::Pi() / 180),
                             TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(73.721939 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[2].SetXYZ(TMath::Sin(108.000031 * TMath::Pi() / 180) * TMath::Sin(-148.279999 * TMath::Pi() / 180),
                             TMath::Sin(23.548264 * TMath::Pi() / 180) * TMath::Sin(169.926025 * TMath::Pi() / 180),
                             TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Sin(116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[3].SetXYZ(TMath::Sin(108.000092 * TMath::Pi() / 180) * TMath::Sin(31.719986 * TMath::Pi() / 180),
                             TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Sin(169.926025 * TMath::Pi() / 180),
                             TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Sin(116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[4].SetXYZ(TMath::Sin(71.999969 * TMath::Pi() / 180) * TMath::Sin(-31.720016 * TMath::Pi() / 180),
                             TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Sin(10.073961 * TMath::Pi() / 180),
                             TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Sin(63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[5].SetXYZ(TMath::Sin(71.999908 * TMath::Pi() / 180) * TMath::Sin(148.279953 * TMath::Pi() / 180),
                             TMath::Sin(23.548248 * TMath::Pi() / 180) * TMath::Sin(10.073951 * TMath::Pi() / 180),
                             TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Sin(63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[6].SetXYZ(TMath::Sin(35.999985 * TMath::Pi() / 180) * TMath::Sin(-148.279999 * TMath::Pi() / 180),
                             TMath::Sin(55.488815 * TMath::Pi() / 180) * TMath::Sin(50.572174 * TMath::Pi() / 180),
                             TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Sin(134.317352 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[7].SetXYZ(TMath::Sin(35.999985 * TMath::Pi() / 180) * TMath::Sin(31.719986 * TMath::Pi() / 180),
                             TMath::Sin(124.511108 * TMath::Pi() / 180) * TMath::Sin(50.572174 * TMath::Pi() / 180),
                             TMath::Sin(80.995865 * TMath::Pi() / 180) * TMath::Sin(134.317398 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[8].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Sin(-148.283234 * TMath::Pi() / 180),
                             TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Sin(-42.728638 * TMath::Pi() / 180),
                             TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Sin(102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[9].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Sin(148.283234 * TMath::Pi() / 180),
                             TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Sin(-137.271378 * TMath::Pi() / 180),
                             TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Sin(77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[10].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Sin(-31.720016 * TMath::Pi() / 180),
                              TMath::Sin(124.511169 * TMath::Pi() / 180) * TMath::Sin(129.427750 * TMath::Pi() / 180),
                              TMath::Sin(80.995819 * TMath::Pi() / 180) * TMath::Sin(45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[11].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Sin(148.279953 * TMath::Pi() / 180),
                              TMath::Sin(55.488846 * TMath::Pi() / 180) * TMath::Sin(129.427811 * TMath::Pi() / 180),
                              TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Sin(45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[12].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Sin(-31.716766 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Sin(-137.271317 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Sin(77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[13].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Sin(31.716766 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Sin(-42.728607 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Sin(102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[14].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Sin(-69.094238 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Sin(23.223938 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Sin(120.102737 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[15].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Sin(69.094284 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Sin(156.776047 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Sin(59.897247 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[16].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Sin(-110.905762 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Sin(156.776047 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Sin(59.897308 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[17].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Sin(110.905701 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Sin(23.223938 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Sin(120.102692 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[18].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Sin(-69.093414 * TMath::Pi() / 180),
                              TMath::Sin(58.443619 * TMath::Pi() / 180) * TMath::Sin(-138.326401 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Sin(167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[19].SetXYZ(TMath::Sin(143.997803 * TMath::Pi() / 180) * TMath::Sin(31.715622 * TMath::Pi() / 180),
                              TMath::Sin(113.163864 * TMath::Pi() / 180) * TMath::Sin(-94.359375 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Sin(163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[20].SetXYZ(TMath::Sin(143.997803 * TMath::Pi() / 180) * TMath::Sin(-148.284317 * TMath::Pi() / 180),
                              TMath::Sin(66.836121 * TMath::Pi() / 180) * TMath::Sin(-94.359436 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Sin(163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[21].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Sin(110.906570 * TMath::Pi() / 180),
                              TMath::Sin(121.556427 * TMath::Pi() / 180) * TMath::Sin(-138.326401 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Sin(167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[22].SetXYZ(TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Sin(-0.000001 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Sin(90.000076 * TMath::Pi() / 180),
                              TMath::Sin(16.277985 * TMath::Pi() / 180) * TMath::Sin(89.999969 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[23].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Sin(-110.906570 * TMath::Pi() / 180),
                              TMath::Sin(121.556366 * TMath::Pi() / 180) * TMath::Sin(-41.673569 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Sin(12.487767 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[24].SetXYZ(TMath::Sin(36.002151 * TMath::Pi() / 180) * TMath::Sin(148.284378 * TMath::Pi() / 180),
                              TMath::Sin(66.836121 * TMath::Pi() / 180) * TMath::Sin(-85.640564 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Sin(16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[25].SetXYZ(TMath::Sin(36.002136 * TMath::Pi() / 180) * TMath::Sin(-31.715652 * TMath::Pi() / 180),
                              TMath::Sin(113.163864 * TMath::Pi() / 180) * TMath::Sin(-85.640564 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Sin(16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[26].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Sin(69.093414 * TMath::Pi() / 180),
                              TMath::Sin(58.443573 * TMath::Pi() / 180) * TMath::Sin(-41.673553 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Sin(12.487760 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[27].SetXYZ(TMath::Sin(89.999908 * TMath::Pi() / 180) * TMath::Sin(180.000000 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Sin(89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Sin(90.000076 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[28].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Sin(-0.000009 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Sin(180.000000 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[29].SetXYZ(TMath::Sin(89.999908 * TMath::Pi() / 180) * TMath::Sin(-89.999908 * TMath::Pi() / 180),
                              TMath::Sin(16.278000 * TMath::Pi() / 180) * TMath::Sin(0.000002 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Sin(-180.000000 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[30].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(-89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Sin(180.000000 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Sin(-0.000001 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[31].SetXYZ(TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Sin(90.000076 * TMath::Pi() / 180),
                              TMath::Sin(16.278015 * TMath::Pi() / 180) * TMath::Sin(-179.999939 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Sin(0.000000 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[32].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(-180.000000 * TMath::Pi() / 180),
                              TMath::Sin(106.278061 * TMath::Pi() / 180) * TMath::Sin(-89.999969 * TMath::Pi() / 180),
                              TMath::Sin(16.277985 * TMath::Pi() / 180) * TMath::Sin(-89.999969 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[33].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Sin(69.093414 * TMath::Pi() / 180),
                              TMath::Sin(121.556366 * TMath::Pi() / 180) * TMath::Sin(138.326401 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Sin(-167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[34].SetXYZ(TMath::Sin(36.002136 * TMath::Pi() / 180) * TMath::Sin(-31.715622 * TMath::Pi() / 180),
                              TMath::Sin(66.836060 * TMath::Pi() / 180) * TMath::Sin(94.359436 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Sin(-163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[35].SetXYZ(TMath::Sin(36.002136 * TMath::Pi() / 180) * TMath::Sin(148.284317 * TMath::Pi() / 180),
                              TMath::Sin(113.163864 * TMath::Pi() / 180) * TMath::Sin(94.359436 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Sin(-163.649902 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[36].SetXYZ(TMath::Sin(60.000854 * TMath::Pi() / 180) * TMath::Sin(-110.906570 * TMath::Pi() / 180),
                              TMath::Sin(58.443573 * TMath::Pi() / 180) * TMath::Sin(138.326401 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Sin(-167.512238 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[37].SetXYZ(TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(0.000001 * TMath::Pi() / 180),
                              TMath::Sin(73.721939 * TMath::Pi() / 180) * TMath::Sin(-89.999969 * TMath::Pi() / 180),
                              TMath::Sin(163.721970 * TMath::Pi() / 180) * TMath::Sin(-89.999969 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[38].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Sin(110.906570 * TMath::Pi() / 180),
                              TMath::Sin(58.443573 * TMath::Pi() / 180) * TMath::Sin(41.673584 * TMath::Pi() / 180),
                              TMath::Sin(133.631973 * TMath::Pi() / 180) * TMath::Sin(-12.487763 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[39].SetXYZ(TMath::Sin(143.997864 * TMath::Pi() / 180) * TMath::Sin(-148.284317 * TMath::Pi() / 180),
                              TMath::Sin(113.163925 * TMath::Pi() / 180) * TMath::Sin(85.640564 * TMath::Pi() / 180),
                              TMath::Sin(115.899887 * TMath::Pi() / 180) * TMath::Sin(-16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[40].SetXYZ(TMath::Sin(143.997803 * TMath::Pi() / 180) * TMath::Sin(31.715637 * TMath::Pi() / 180),
                              TMath::Sin(66.836121 * TMath::Pi() / 180) * TMath::Sin(85.640610 * TMath::Pi() / 180),
                              TMath::Sin(64.100113 * TMath::Pi() / 180) * TMath::Sin(-16.350067 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[41].SetXYZ(TMath::Sin(119.999146 * TMath::Pi() / 180) * TMath::Sin(-69.093414 * TMath::Pi() / 180),
                              TMath::Sin(121.556427 * TMath::Pi() / 180) * TMath::Sin(41.673553 * TMath::Pi() / 180),
                              TMath::Sin(46.368011 * TMath::Pi() / 180) * TMath::Sin(-12.487760 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[42].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Sin(110.905762 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Sin(-156.776047 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Sin(-59.897247 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[43].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Sin(-110.905701 * TMath::Pi() / 180),
                              TMath::Sin(85.992615 * TMath::Pi() / 180) * TMath::Sin(-23.223938 * TMath::Pi() / 180),
                              TMath::Sin(30.324402 * TMath::Pi() / 180) * TMath::Sin(-120.102737 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[44].SetXYZ(TMath::Sin(120.002426 * TMath::Pi() / 180) * TMath::Sin(69.094238 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Sin(-23.223938 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Sin(-120.102692 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[45].SetXYZ(TMath::Sin(59.997574 * TMath::Pi() / 180) * TMath::Sin(-69.094284 * TMath::Pi() / 180),
                              TMath::Sin(94.007385 * TMath::Pi() / 180) * TMath::Sin(-156.776047 * TMath::Pi() / 180),
                              TMath::Sin(149.675552 * TMath::Pi() / 180) * TMath::Sin(-59.897308 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[46].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Sin(31.716766 * TMath::Pi() / 180),
                              TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Sin(137.271317 * TMath::Pi() / 180),
                              TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Sin(-77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[47].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Sin(-31.716766 * TMath::Pi() / 180),
                              TMath::Sin(50.469315 * TMath::Pi() / 180) * TMath::Sin(42.728607 * TMath::Pi() / 180),
                              TMath::Sin(45.035828 * TMath::Pi() / 180) * TMath::Sin(-102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[48].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Sin(148.279953 * TMath::Pi() / 180),
                              TMath::Sin(124.511169 * TMath::Pi() / 180) * TMath::Sin(-50.572174 * TMath::Pi() / 180),
                              TMath::Sin(80.995819 * TMath::Pi() / 180) * TMath::Sin(-134.317398 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[49].SetXYZ(TMath::Sin(143.999985 * TMath::Pi() / 180) * TMath::Sin(-31.719986 * TMath::Pi() / 180),
                              TMath::Sin(55.488846 * TMath::Pi() / 180) * TMath::Sin(-50.572174 * TMath::Pi() / 180),
                              TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Sin(-134.317398 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[50].SetXYZ(TMath::Sin(108.001343 * TMath::Pi() / 180) * TMath::Sin(148.283234 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Sin(42.728638 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Sin(-102.779205 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[51].SetXYZ(TMath::Sin(71.998657 * TMath::Pi() / 180) * TMath::Sin(-148.283234 * TMath::Pi() / 180),
                              TMath::Sin(129.530640 * TMath::Pi() / 180) * TMath::Sin(137.271378 * TMath::Pi() / 180),
                              TMath::Sin(134.964142 * TMath::Pi() / 180) * TMath::Sin(-77.220795 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[52].SetXYZ(TMath::Sin(35.999969 * TMath::Pi() / 180) * TMath::Sin(31.720001 * TMath::Pi() / 180),
                              TMath::Sin(55.488815 * TMath::Pi() / 180) * TMath::Sin(-129.427811 * TMath::Pi() / 180),
                              TMath::Sin(99.004181 * TMath::Pi() / 180) * TMath::Sin(-45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[53].SetXYZ(TMath::Sin(35.999985 * TMath::Pi() / 180) * TMath::Sin(-148.279953 * TMath::Pi() / 180),
                              TMath::Sin(124.511108 * TMath::Pi() / 180) * TMath::Sin(-129.427811 * TMath::Pi() / 180),
                              TMath::Sin(80.995865 * TMath::Pi() / 180) * TMath::Sin(-45.682571 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[54].SetXYZ(TMath::Sin(71.999969 * TMath::Pi() / 180) * TMath::Sin(148.279953 * TMath::Pi() / 180),
                              TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Sin(-169.926025 * TMath::Pi() / 180),
                              TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Sin(-116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[55].SetXYZ(TMath::Sin(71.999908 * TMath::Pi() / 180) * TMath::Sin(-31.719986 * TMath::Pi() / 180),
                              TMath::Sin(23.548248 * TMath::Pi() / 180) * TMath::Sin(-169.926025 * TMath::Pi() / 180),
                              TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Sin(-116.841034 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[56].SetXYZ(TMath::Sin(108.000031 * TMath::Pi() / 180) * TMath::Sin(31.720001 * TMath::Pi() / 180),
                              TMath::Sin(23.548264 * TMath::Pi() / 180) * TMath::Sin(-10.073964 * TMath::Pi() / 180),
                              TMath::Sin(104.668808 * TMath::Pi() / 180) * TMath::Sin(-63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[57].SetXYZ(TMath::Sin(108.000092 * TMath::Pi() / 180) * TMath::Sin(-148.279999 * TMath::Pi() / 180),
                              TMath::Sin(156.451691 * TMath::Pi() / 180) * TMath::Sin(-10.073964 * TMath::Pi() / 180),
                              TMath::Sin(75.331177 * TMath::Pi() / 180) * TMath::Sin(-63.158966 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[58].SetXYZ(TMath::Sin(0.000003 * TMath::Pi() / 180) * TMath::Sin(58.279968 * TMath::Pi() / 180),
                              TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Sin(163.721970 * TMath::Pi() / 180),
                              TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(-106.278061 * TMath::Pi() / 180));

    NAJB_Rot_col_Y[59].SetXYZ(TMath::Sin(179.999939 * TMath::Pi() / 180) * TMath::Sin(58.285004 * TMath::Pi() / 180),
                              TMath::Sin(89.999969 * TMath::Pi() / 180) * TMath::Sin(16.278000 * TMath::Pi() / 180),
                              TMath::Sin(90.000076 * TMath::Pi() / 180) * TMath::Sin(-73.721939 * TMath::Pi() / 180));

    TVector3 NAJB_Rot_col_Z[60];
    NAJB_Rot_col_Z[0].SetXYZ(TMath::Cos(179.999939 * TMath::Pi() / 180),
                             TMath::Cos(89.999969 * TMath::Pi() / 180),
                             TMath::Cos(90.000076 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[1].SetXYZ(TMath::Cos(0.000003 * TMath::Pi() / 180),
                             TMath::Cos(90.000076 * TMath::Pi() / 180),
                             TMath::Cos(89.999969 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[2].SetXYZ(TMath::Cos(108.000031 * TMath::Pi() / 180),
                             TMath::Cos(23.548264 * TMath::Pi() / 180),
                             TMath::Cos(104.668808 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[3].SetXYZ(TMath::Cos(108.000092 * TMath::Pi() / 180),
                             TMath::Cos(156.451691 * TMath::Pi() / 180),
                             TMath::Cos(75.331177 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[4].SetXYZ(TMath::Cos(71.999969 * TMath::Pi() / 180),
                             TMath::Cos(156.451691 * TMath::Pi() / 180),
                             TMath::Cos(75.331177 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[5].SetXYZ(TMath::Cos(71.999908 * TMath::Pi() / 180),
                             TMath::Cos(23.548248 * TMath::Pi() / 180),
                             TMath::Cos(104.668808 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[6].SetXYZ(TMath::Cos(35.999985 * TMath::Pi() / 180),
                             TMath::Cos(55.488815 * TMath::Pi() / 180),
                             TMath::Cos(99.004181 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[7].SetXYZ(TMath::Cos(35.999985 * TMath::Pi() / 180),
                             TMath::Cos(124.511108 * TMath::Pi() / 180),
                             TMath::Cos(80.995865 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[8].SetXYZ(TMath::Cos(71.998657 * TMath::Pi() / 180),
                             TMath::Cos(50.469315 * TMath::Pi() / 180),
                             TMath::Cos(45.035828 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[9].SetXYZ(TMath::Cos(108.001343 * TMath::Pi() / 180),
                             TMath::Cos(50.469315 * TMath::Pi() / 180),
                             TMath::Cos(45.035828 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[10].SetXYZ(TMath::Cos(143.999985 * TMath::Pi() / 180),
                              TMath::Cos(124.511169 * TMath::Pi() / 180),
                              TMath::Cos(80.995819 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[11].SetXYZ(TMath::Cos(143.999985 * TMath::Pi() / 180),
                              TMath::Cos(55.488846 * TMath::Pi() / 180),
                              TMath::Cos(99.004181 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[12].SetXYZ(TMath::Cos(108.001343 * TMath::Pi() / 180),
                              TMath::Cos(129.530640 * TMath::Pi() / 180),
                              TMath::Cos(134.964142 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[13].SetXYZ(TMath::Cos(71.998657 * TMath::Pi() / 180),
                              TMath::Cos(129.530640 * TMath::Pi() / 180),
                              TMath::Cos(134.964142 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[14].SetXYZ(TMath::Cos(59.997574 * TMath::Pi() / 180),
                              TMath::Cos(85.992615 * TMath::Pi() / 180),
                              TMath::Cos(30.324402 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[15].SetXYZ(TMath::Cos(120.002426 * TMath::Pi() / 180),
                              TMath::Cos(85.992615 * TMath::Pi() / 180),
                              TMath::Cos(30.324402 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[16].SetXYZ(TMath::Cos(120.002426 * TMath::Pi() / 180),
                              TMath::Cos(94.007385 * TMath::Pi() / 180),
                              TMath::Cos(149.675552 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[17].SetXYZ(TMath::Cos(59.997574 * TMath::Pi() / 180),
                              TMath::Cos(94.007385 * TMath::Pi() / 180),
                              TMath::Cos(149.675552 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[18].SetXYZ(TMath::Cos(119.999146 * TMath::Pi() / 180),
                              TMath::Cos(58.443619 * TMath::Pi() / 180),
                              TMath::Cos(133.631973 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[19].SetXYZ(TMath::Cos(143.997803 * TMath::Pi() / 180),
                              TMath::Cos(113.163864 * TMath::Pi() / 180),
                              TMath::Cos(115.899887 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[20].SetXYZ(TMath::Cos(143.997803 * TMath::Pi() / 180),
                              TMath::Cos(66.836121 * TMath::Pi() / 180),
                              TMath::Cos(64.100113 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[21].SetXYZ(TMath::Cos(119.999146 * TMath::Pi() / 180),
                              TMath::Cos(121.556427 * TMath::Pi() / 180),
                              TMath::Cos(46.368011 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[22].SetXYZ(TMath::Cos(90.000076 * TMath::Pi() / 180),
                              TMath::Cos(106.278061 * TMath::Pi() / 180),
                              TMath::Cos(16.277985 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[23].SetXYZ(TMath::Cos(60.000854 * TMath::Pi() / 180),
                              TMath::Cos(121.556366 * TMath::Pi() / 180),
                              TMath::Cos(46.368011 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[24].SetXYZ(TMath::Cos(36.002151 * TMath::Pi() / 180),
                              TMath::Cos(66.836121 * TMath::Pi() / 180),
                              TMath::Cos(64.100113 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[25].SetXYZ(TMath::Cos(36.002136 * TMath::Pi() / 180),
                              TMath::Cos(113.163864 * TMath::Pi() / 180),
                              TMath::Cos(115.899887 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[26].SetXYZ(TMath::Cos(60.000854 * TMath::Pi() / 180),
                              TMath::Cos(58.443573 * TMath::Pi() / 180),
                              TMath::Cos(133.631973 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[27].SetXYZ(TMath::Cos(89.999908 * TMath::Pi() / 180),
                              TMath::Cos(73.721939 * TMath::Pi() / 180),
                              TMath::Cos(163.721970 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[28].SetXYZ(TMath::Cos(89.999969 * TMath::Pi() / 180),
                              TMath::Cos(163.721970 * TMath::Pi() / 180),
                              TMath::Cos(106.278061 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[29].SetXYZ(TMath::Cos(89.999908 * TMath::Pi() / 180),
                              TMath::Cos(16.278000 * TMath::Pi() / 180),
                              TMath::Cos(73.721939 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[30].SetXYZ(TMath::Cos(89.999969 * TMath::Pi() / 180),
                              TMath::Cos(163.721970 * TMath::Pi() / 180),
                              TMath::Cos(106.278061 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[31].SetXYZ(TMath::Cos(90.000076 * TMath::Pi() / 180),
                              TMath::Cos(16.278015 * TMath::Pi() / 180),
                              TMath::Cos(73.721939 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[32].SetXYZ(TMath::Cos(89.999969 * TMath::Pi() / 180),
                              TMath::Cos(106.278061 * TMath::Pi() / 180),
                              TMath::Cos(16.277985 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[33].SetXYZ(TMath::Cos(60.000854 * TMath::Pi() / 180),
                              TMath::Cos(121.556366 * TMath::Pi() / 180),
                              TMath::Cos(46.368011 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[34].SetXYZ(TMath::Cos(36.002136 * TMath::Pi() / 180),
                              TMath::Cos(66.836060 * TMath::Pi() / 180),
                              TMath::Cos(64.100113 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[35].SetXYZ(TMath::Cos(36.002136 * TMath::Pi() / 180),
                              TMath::Cos(113.163864 * TMath::Pi() / 180),
                              TMath::Cos(115.899887 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[36].SetXYZ(TMath::Cos(60.000854 * TMath::Pi() / 180),
                              TMath::Cos(58.443573 * TMath::Pi() / 180),
                              TMath::Cos(133.631973 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[37].SetXYZ(TMath::Cos(89.999969 * TMath::Pi() / 180),
                              TMath::Cos(73.721939 * TMath::Pi() / 180),
                              TMath::Cos(163.721970 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[38].SetXYZ(TMath::Cos(119.999146 * TMath::Pi() / 180),
                              TMath::Cos(58.443573 * TMath::Pi() / 180),
                              TMath::Cos(133.631973 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[39].SetXYZ(TMath::Cos(143.997864 * TMath::Pi() / 180),
                              TMath::Cos(113.163925 * TMath::Pi() / 180),
                              TMath::Cos(115.899887 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[40].SetXYZ(TMath::Cos(143.997803 * TMath::Pi() / 180),
                              TMath::Cos(66.836121 * TMath::Pi() / 180),
                              TMath::Cos(64.100113 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[41].SetXYZ(TMath::Cos(119.999146 * TMath::Pi() / 180),
                              TMath::Cos(121.556427 * TMath::Pi() / 180),
                              TMath::Cos(46.368011 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[42].SetXYZ(TMath::Cos(59.997574 * TMath::Pi() / 180),
                              TMath::Cos(85.992615 * TMath::Pi() / 180),
                              TMath::Cos(30.324402 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[43].SetXYZ(TMath::Cos(120.002426 * TMath::Pi() / 180),
                              TMath::Cos(85.992615 * TMath::Pi() / 180),
                              TMath::Cos(30.324402 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[44].SetXYZ(TMath::Cos(120.002426 * TMath::Pi() / 180),
                              TMath::Cos(94.007385 * TMath::Pi() / 180),
                              TMath::Cos(149.675552 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[45].SetXYZ(TMath::Cos(59.997574 * TMath::Pi() / 180),
                              TMath::Cos(94.007385 * TMath::Pi() / 180),
                              TMath::Cos(149.675552 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[46].SetXYZ(TMath::Cos(71.998657 * TMath::Pi() / 180),
                              TMath::Cos(50.469315 * TMath::Pi() / 180),
                              TMath::Cos(45.035828 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[47].SetXYZ(TMath::Cos(108.001343 * TMath::Pi() / 180),
                              TMath::Cos(50.469315 * TMath::Pi() / 180),
                              TMath::Cos(45.035828 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[48].SetXYZ(TMath::Cos(143.999985 * TMath::Pi() / 180),
                              TMath::Cos(124.511169 * TMath::Pi() / 180),
                              TMath::Cos(80.995819 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[49].SetXYZ(TMath::Cos(143.999985 * TMath::Pi() / 180),
                              TMath::Cos(55.488846 * TMath::Pi() / 180),
                              TMath::Cos(99.004181 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[50].SetXYZ(TMath::Cos(108.001343 * TMath::Pi() / 180),
                              TMath::Cos(129.530640 * TMath::Pi() / 180),
                              TMath::Cos(134.964142 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[51].SetXYZ(TMath::Cos(71.998657 * TMath::Pi() / 180),
                              TMath::Cos(129.530640 * TMath::Pi() / 180),
                              TMath::Cos(134.964142 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[52].SetXYZ(TMath::Cos(35.999969 * TMath::Pi() / 180),
                              TMath::Cos(55.488815 * TMath::Pi() / 180),
                              TMath::Cos(99.004181 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[53].SetXYZ(TMath::Cos(35.999985 * TMath::Pi() / 180),
                              TMath::Cos(124.511108 * TMath::Pi() / 180),
                              TMath::Cos(80.995865 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[54].SetXYZ(TMath::Cos(71.999969 * TMath::Pi() / 180),
                              TMath::Cos(156.451691 * TMath::Pi() / 180),
                              TMath::Cos(75.331177 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[55].SetXYZ(TMath::Cos(71.999908 * TMath::Pi() / 180),
                              TMath::Cos(23.548248 * TMath::Pi() / 180),
                              TMath::Cos(104.668808 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[56].SetXYZ(TMath::Cos(108.000031 * TMath::Pi() / 180),
                              TMath::Cos(23.548264 * TMath::Pi() / 180),
                              TMath::Cos(104.668808 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[57].SetXYZ(TMath::Cos(108.000092 * TMath::Pi() / 180),
                              TMath::Cos(156.451691 * TMath::Pi() / 180),
                              TMath::Cos(75.331177 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[58].SetXYZ(TMath::Cos(0.000003 * TMath::Pi() / 180),
                              TMath::Cos(90.000076 * TMath::Pi() / 180),
                              TMath::Cos(89.999969 * TMath::Pi() / 180));
    NAJB_Rot_col_Z[59].SetXYZ(TMath::Cos(179.999939 * TMath::Pi() / 180),
                              TMath::Cos(89.999969 * TMath::Pi() / 180),
                              TMath::Cos(90.000076 * TMath::Pi() / 180));

    // m1_TRB1/2
    TVector3 col_X_m1_TRB1(TMath::Sin(90. * TMath::Pi() / 180) * TMath::Cos(0. * TMath::Pi() / 180),
                           TMath::Sin(93.525 * TMath::Pi() / 180) * TMath::Cos(90. * TMath::Pi() / 180),
                           TMath::Sin(3.525 * TMath::Pi() / 180) * TMath::Cos(90. * TMath::Pi() / 180));
    TVector3 col_Y_m1_TRB1(TMath::Sin(90. * TMath::Pi() / 180) * TMath::Sin(0. * TMath::Pi() / 180),
                           TMath::Sin(93.525 * TMath::Pi() / 180) * TMath::Sin(90. * TMath::Pi() / 180),
                           TMath::Sin(3.525 * TMath::Pi() / 180) * TMath::Sin(90. * TMath::Pi() / 180));
    TVector3 col_Z_m1_TRB1(TMath::Cos(90. * TMath::Pi() / 180),
                           TMath::Cos(93.525 * TMath::Pi() / 180),
                           TMath::Cos(3.525 * TMath::Pi() / 180));
    TVector3 col_X_m2_TRB2(TMath::Sin(90. * TMath::Pi() / 180) * TMath::Cos(180. * TMath::Pi() / 180),
                           TMath::Sin(94.25 * TMath::Pi() / 180) * TMath::Cos(270. * TMath::Pi() / 180),
                           TMath::Sin(4.25 * TMath::Pi() / 180) * TMath::Cos(270. * TMath::Pi() / 180));
    TVector3 col_Y_m2_TRB2(TMath::Sin(90. * TMath::Pi() / 180) * TMath::Sin(180. * TMath::Pi() / 180),
                           TMath::Sin(94.25 * TMath::Pi() / 180) * TMath::Sin(270. * TMath::Pi() / 180),
                           TMath::Sin(4.25 * TMath::Pi() / 180) * TMath::Sin(270. * TMath::Pi() / 180));
    TVector3 col_Z_m2_TRB2(TMath::Cos(90. * TMath::Pi() / 180),
                           TMath::Cos(94.25 * TMath::Pi() / 180),
                           TMath::Cos(4.25 * TMath::Pi() / 180));

    // - Adapted from R3BSim Geant4 definition
    // - Using the new templated Math ROOT Library
    // - <D.Bertini@gsi.de>

    //-  TRB1
    //--- Fill info for global Rotation / Translation
    Double_t fRotable0[9] = {
        col_X_m1_TRB1.X(), col_X_m1_TRB1.Y(), col_X_m1_TRB1.Z(), col_Y_m1_TRB1.X(), col_Y_m1_TRB1.Y(),
        col_Y_m1_TRB1.Z(), col_Z_m1_TRB1.X(), col_Z_m1_TRB1.Y(), col_Z_m1_TRB1.Z(),
    };
    ROOT::Math::XYZPoint tra1_TRB1(0., 21.56 / 10., 0.);
    ROOT::Math::Rotation3D* pm1_TRB1 = new ROOT::Math::Rotation3D(fRotable0[0],
                                                                  fRotable0[3],
                                                                  fRotable0[6],
                                                                  fRotable0[1],
                                                                  fRotable0[4],
                                                                  fRotable0[7],
                                                                  fRotable0[2],
                                                                  fRotable0[5],
                                                                  fRotable0[8]);
    //--- Single Crystal definition
    TGeoRotation* pIndividualRot = NULL;
    TGeoRotation* pRingRot = NULL;

    for (Int_t iter2 = 0; iter2 < 60; iter2++)
    {
        Double_t fRotable1[9] = { NAJB_Rot_col_X[iter2].X(), NAJB_Rot_col_X[iter2].Y(), NAJB_Rot_col_X[iter2].Z(),
                                  NAJB_Rot_col_Y[iter2].X(), NAJB_Rot_col_Y[iter2].Y(), NAJB_Rot_col_Y[iter2].Z(),
                                  NAJB_Rot_col_Z[iter2].X(), NAJB_Rot_col_Z[iter2].Y(), NAJB_Rot_col_Z[iter2].Z() };

        pIndividualRot = new TGeoRotation();
        pIndividualRot->SetMatrix(fRotable1);

        ROOT::Math::Rotation3D* pIndRot = new ROOT::Math::Rotation3D(fRotable1[0],
                                                                     fRotable1[3],
                                                                     fRotable1[6],
                                                                     fRotable1[1],
                                                                     fRotable1[4],
                                                                     fRotable1[7],
                                                                     fRotable1[2],
                                                                     fRotable1[5],
                                                                     fRotable1[8]);

        ROOT::Math::Rotation3D pIndRotInv = pIndRot->Inverse();
        dx = NAJB_X[iter2] / 10.;
        dy = NAJB_Y[iter2] / 10.;
        dz = NAJB_Z[iter2] / 10.;
        ROOT::Math::XYZPoint tt(dx, dy, dz);
        ROOT::Math::XYZPoint ttt(0.0, 0.0, 0.0);
        ttt = pIndRotInv * tra1_TRB1;
        ttt.SetX(ttt.X() + tt.X());
        ttt.SetY(ttt.Y() + tt.Y());
        ttt.SetZ(ttt.Z() + tt.Z());

        ROOT::Math::Rotation3D pRingTmp;
        pRingTmp = (*pm1_TRB1) * (*pIndRot);
        Double_t fRotable2[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
        pRingTmp.GetComponents(fRotable2[0],
                               fRotable2[3],
                               fRotable2[6],
                               fRotable2[1],
                               fRotable2[4],
                               fRotable2[7],
                               fRotable2[2],
                               fRotable2[5],
                               fRotable2[8]);
        pRingRot = new TGeoRotation();
        pRingRot->SetMatrix(fRotable2);
        TGeoTranslation* trans = new TGeoTranslation(ttt.X(), ttt.Y(), ttt.Z());
        TGeoCombiTrans* combi = new TGeoCombiTrans(*trans, *pRingRot);

        // Add the Node in CBLogWorld
        pCBLogWorld->AddNode(pcrystalLogNAJB, iter2, combi);

    } //! for iter2

    //-- TRC  part
    // Shape: TRC1 type: TGeoTrap
    dz = 10.000000;
    theta = 0.000000;
    phi = 0.000000;
    h1 = 2.209000;
    bl1 = 3.614000;
    tl1 = 4.265000;
    alpha1 = 0.000000;
    h2 = 3.976000;
    bl2 = 6.505000;
    tl2 = 7.676000;
    alpha2 = 0.000000;
    TGeoShape* pTRC1 = new TGeoTrap("TRC1", dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
    // Shape: TRC2 type: TGeoTrap
    dz = 10.000000;
    theta = 0.000000;
    phi = 0.000000;
    h1 = 0.808000;
    bl1 = 4.302000;
    tl1 = 0.001000;
    alpha1 = 0.000000;
    h2 = 1.454000;
    bl2 = 7.743504;
    tl2 = 0.005324;
    alpha2 = 0.000000;
    TGeoShape* pTRC2 = new TGeoTrap("TRC2", dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
    // Combi transformation: TRC_21
    dx = 0.000000;
    dy = 4.229809;
    dz = 0.040235;
    // Rotation:
    thx = 90.000000;
    phx = 360.000000;
    thy = 96.883000;
    phy = 90.000000;
    thz = 6.883000;
    phz = 90.000000;
    TGeoRotation* pMatrix5 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix4 = new TGeoCombiTrans("TRC_21", dx, dy, dz, pMatrix5);
    pBoolNode = new TGeoUnion(pTRC1, pTRC2, 0, pMatrix4);
    // Shape: TRC_21 type: TGeoCompositeShape
    TGeoShape* pTRC_21 = new TGeoCompositeShape("TRC_21", pBoolNode);
    pTRC_21->SetTitle("TRC1+TRC2:TRC_21");
    // Shape: TRC3 type: TGeoTrap
    dz = 10.000000;
    theta = 0.000000;
    phi = 0.000000;
    h1 = 1.046000;
    bl1 = 3.618000;
    tl1 = 0.001000;
    alpha1 = 0.000000;
    h2 = 1.883000;
    bl2 = 6.515776;
    tl2 = 0.003459;
    alpha2 = 0.000000;
    TGeoShape* pTRC3 = new TGeoTrap("TRC3", dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
    // Combi transformation: TRC
    dx = 0.000000;
    dy = -4.557794;
    dz = -0.043355;
    // Rotation:
    thx = 90.000000;
    phx = 180.000000;
    thy = 97.428000;
    phy = 270.000000;
    thz = 7.428000;
    phz = 270.000000;
    TGeoRotation* pMatrix7 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix6 = new TGeoCombiTrans("TRC", dx, dy, dz, pMatrix7);
    pBoolNode = new TGeoUnion(pTRC_21, pTRC3, 0, pMatrix6);
    // Shape: TRC type: TGeoCompositeShape
    TGeoShape* pTRC_11 = new TGeoCompositeShape("TRC", pBoolNode);
    pTRC_11->SetTitle("TRC_21+TRC3:TRC");
    // Volume: crystalLogNAJC
    TGeoVolume* pcrystalLogNAJC = new TGeoVolume("crystalLogNAJC", pTRC_11, pMed9);

    Double_t NAJC_X[60] = { -49.014,   49.014,    49.014,    -49.014,   0,         0,         -205.674,  -205.674,
                            -156.8805, 156.8805,  205.674,   205.674,   156.8805,  -156.8805, -235.7959, -302.589,
                            -235.7809, 235.7809,  302.589,   235.7809,  -332.9059, -332.9059, -107.2383, 107.2383,
                            332.9059,  332.9059,  96.2395,   -96.2395,  -175.8994, -175.8994, 175.8994,  175.8994,
                            107.2383,  -107.2383, -332.9059, -332.9059, -96.2395,  96.2395,   332.9059,  332.9059,
                            -235.7809, -302.589,  -235.7809, 235.7809,  302.589,   235.7809,  156.8805,  -156.8805,
                            -205.674,  -205.674,  -156.8805, 156.8805,  205.674,   205.674,   0,         0,
                            49.014,    -49.014,   -49.014,   49.014 };
    Double_t NAJC_Y[60] = { 332.9094,  332.9094,  332.9094,  332.9094,  302.7429,  302.7429,  235.7669,  235.795,
                            205.6319,  205.6319,  235.795,   235.7669,  205.6319,  205.6319,  156.891,   175.8994,
                            156.891,   156.891,   175.8994,  156.891,   97.237,    97.237,    54.6468,   54.6468,
                            97.237,    97.237,    49.042,    49.042,    0,         0,         0,         0,
                            -54.6468,  -54.6468,  -97.237,   -97.237,   -49.042,   -49.042,   -97.237,   -97.237,
                            -156.891,  -175.8994, -156.891,  -156.891,  -175.8994, -156.9645, -205.6319, -205.6319,
                            -235.7669, -235.795,  -205.6319, -205.6319, -235.795,  -235.7669, -302.7429, -302.7429,
                            -332.9094, -332.9094, -332.9094, -332.9094 };
    Double_t NAJC_Z[60] = { 96.278,   96.278,    -96.278,   -96.278,   175.6334,  -175.6334, -156.8805, 156.8385,
                            235.8054, 235.8054,  156.8385,  -156.8805, -235.8054, -235.8054, -205.6529, 0,
                            205.6529, 205.6529,  0,         -205.6529, -47.089,   47.089,    370.9639,  370.9639,
                            47.089,   -47.089,   -332.9164, -332.9164, -302.589,  302.589,   -302.589,  302.589,
                            370.9639, 370.9639,  47.089,    -47.089,   -332.9164, -332.9164, -47.089,   47.089,
                            205.6529, 0,         -205.6529, -205.6529, 0,         205.5969,  235.8054,  235.8054,
                            156.8805, -156.8385, -235.8054, -235.8054, -156.8385, 156.8805,  175.6334,  -175.6334,
                            96.278,   96.278,    -96.278,   -96.278 };

    TVector3 NAJC_Rot_col_X[60];
    NAJC_Rot_col_X[0].SetXYZ(sin(144.001129 * TMath::Pi() / 180) * cos(31.714935 * TMath::Pi() / 180),
                             sin(121.226883 * TMath::Pi() / 180) * cos(178.276993 * TMath::Pi() / 180),
                             sin(73.921265 * TMath::Pi() / 180) * cos(98.340836 * TMath::Pi() / 180));

    NAJC_Rot_col_X[1].SetXYZ(sin(35.998871 * TMath::Pi() / 180) * cos(-31.714935 * TMath::Pi() / 180),
                             sin(121.226883 * TMath::Pi() / 180) * cos(1.722995 * TMath::Pi() / 180),
                             sin(73.921265 * TMath::Pi() / 180) * cos(81.659164 * TMath::Pi() / 180));

    NAJC_Rot_col_X[2].SetXYZ(sin(35.999969 * TMath::Pi() / 180) * cos(148.279953 * TMath::Pi() / 180),
                             sin(58.773056 * TMath::Pi() / 180) * cos(1.721368 * TMath::Pi() / 180),
                             sin(106.080643 * TMath::Pi() / 180) * cos(81.656204 * TMath::Pi() / 180));

    NAJC_Rot_col_X[3].SetXYZ(sin(144.000153 * TMath::Pi() / 180) * cos(-148.285461 * TMath::Pi() / 180),
                             sin(58.773270 * TMath::Pi() / 180) * cos(178.272949 * TMath::Pi() / 180),
                             sin(106.080688 * TMath::Pi() / 180) * cos(98.338043 * TMath::Pi() / 180));

    NAJC_Rot_col_X[4].SetXYZ(sin(89.999908 * TMath::Pi() / 180) * cos(-179.999939 * TMath::Pi() / 180),
                             sin(30.164978 * TMath::Pi() / 180) * cos(-89.999908 * TMath::Pi() / 180),
                             sin(59.834961 * TMath::Pi() / 180) * cos(89.999969 * TMath::Pi() / 180));

    NAJC_Rot_col_X[5].SetXYZ(sin(89.999802 * TMath::Pi() / 180) * cos(-0.002833 * TMath::Pi() / 180),
                             sin(149.833023 * TMath::Pi() / 180) * cos(-90.002701 * TMath::Pi() / 180),
                             sin(120.167007 * TMath::Pi() / 180) * cos(89.997025 * TMath::Pi() / 180));

    NAJC_Rot_col_X[6].SetXYZ(sin(72.001923 * TMath::Pi() / 180) * cos(-148.282684 * TMath::Pi() / 180),
                             sin(32.983261 * TMath::Pi() / 180) * cos(91.678024 * TMath::Pi() / 180),
                             sin(116.629028 * TMath::Pi() / 180) * cos(131.092255 * TMath::Pi() / 180));

    NAJC_Rot_col_X[7].SetXYZ(sin(72.001495 * TMath::Pi() / 180) * cos(31.717911 * TMath::Pi() / 180),
                             sin(147.019424 * TMath::Pi() / 180) * cos(91.674408 * TMath::Pi() / 180),
                             sin(63.374359 * TMath::Pi() / 180) * cos(131.091690 * TMath::Pi() / 180));

    NAJC_Rot_col_X[8].SetXYZ(sin(59.997574 * TMath::Pi() / 180) * cos(-110.905701 * TMath::Pi() / 180),
                             sin(57.033188 * TMath::Pi() / 180) * cos(1.087461 * TMath::Pi() / 180),
                             sin(47.647202 * TMath::Pi() / 180) * cos(127.333786 * TMath::Pi() / 180));

    NAJC_Rot_col_X[9].SetXYZ(sin(120.002365 * TMath::Pi() / 180) * cos(110.905762 * TMath::Pi() / 180),
                             sin(57.033188 * TMath::Pi() / 180) * cos(178.912521 * TMath::Pi() / 180),
                             sin(47.647202 * TMath::Pi() / 180) * cos(52.666214 * TMath::Pi() / 180));

    NAJC_Rot_col_X[10].SetXYZ(sin(107.998550 * TMath::Pi() / 180) * cos(-31.717896 * TMath::Pi() / 180),
                              sin(147.019424 * TMath::Pi() / 180) * cos(88.325638 * TMath::Pi() / 180),
                              sin(63.374359 * TMath::Pi() / 180) * cos(48.908279 * TMath::Pi() / 180));

    NAJC_Rot_col_X[11].SetXYZ(sin(108.000031 * TMath::Pi() / 180) * cos(148.279953 * TMath::Pi() / 180),
                              sin(32.982681 * TMath::Pi() / 180) * cos(88.323837 * TMath::Pi() / 180),
                              sin(116.626938 * TMath::Pi() / 180) * cos(48.904816 * TMath::Pi() / 180));

    NAJC_Rot_col_X[12].SetXYZ(sin(120.000229 * TMath::Pi() / 180) * cos(-69.097076 * TMath::Pi() / 180),
                              sin(122.967331 * TMath::Pi() / 180) * cos(178.911270 * TMath::Pi() / 180),
                              sin(132.354141 * TMath::Pi() / 180) * cos(52.662109 * TMath::Pi() / 180));

    NAJC_Rot_col_X[13].SetXYZ(sin(59.997574 * TMath::Pi() / 180) * cos(69.094299 * TMath::Pi() / 180),
                              sin(122.966812 * TMath::Pi() / 180) * cos(1.087461 * TMath::Pi() / 180),
                              sin(132.352798 * TMath::Pi() / 180) * cos(127.333786 * TMath::Pi() / 180));

    NAJC_Rot_col_X[14].SetXYZ(sin(143.997803 * TMath::Pi() / 180) * cos(-31.715652 * TMath::Pi() / 180),
                              sin(90.912369 * TMath::Pi() / 180) * cos(-122.971481 * TMath::Pi() / 180),
                              sin(125.986862 * TMath::Pi() / 180) * cos(146.365906 * TMath::Pi() / 180));

    NAJC_Rot_col_X[15].SetXYZ(sin(0.002820 * TMath::Pi() / 180) * cos(177.780685 * TMath::Pi() / 180),
                              sin(89.998596 * TMath::Pi() / 180) * cos(59.832993 * TMath::Pi() / 180),
                              sin(90.002533 * TMath::Pi() / 180) * cos(149.833023 * TMath::Pi() / 180));

    NAJC_Rot_col_X[16].SetXYZ(sin(144.001022 * TMath::Pi() / 180) * cos(148.285355 * TMath::Pi() / 180),
                              sin(89.086578 * TMath::Pi() / 180) * cos(-122.972015 * TMath::Pi() / 180),
                              sin(54.016281 * TMath::Pi() / 180) * cos(146.364716 * TMath::Pi() / 180));

    NAJC_Rot_col_X[17].SetXYZ(sin(35.998978 * TMath::Pi() / 180) * cos(-148.285355 * TMath::Pi() / 180),
                              sin(89.086578 * TMath::Pi() / 180) * cos(-57.027939 * TMath::Pi() / 180),
                              sin(54.016281 * TMath::Pi() / 180) * cos(33.635269 * TMath::Pi() / 180));

    NAJC_Rot_col_X[18].SetXYZ(sin(179.999939 * TMath::Pi() / 180) * cos(135.715851 * TMath::Pi() / 180),
                              sin(89.999908 * TMath::Pi() / 180) * cos(120.165039 * TMath::Pi() / 180),
                              sin(90.000076 * TMath::Pi() / 180) * cos(30.164993 * TMath::Pi() / 180));

    NAJC_Rot_col_X[19].SetXYZ(sin(35.999680 * TMath::Pi() / 180) * cos(31.714813 * TMath::Pi() / 180),
                              sin(90.911392 * TMath::Pi() / 180) * cos(-57.030579 * TMath::Pi() / 180),
                              sin(125.984406 * TMath::Pi() / 180) * cos(33.631210 * TMath::Pi() / 180));

    NAJC_Rot_col_X[20].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * cos(69.093414 * TMath::Pi() / 180),
                              sin(148.730469 * TMath::Pi() / 180) * cos(-92.836624 * TMath::Pi() / 180),
                              sin(98.015121 * TMath::Pi() / 180) * cos(163.756393 * TMath::Pi() / 180));

    NAJC_Rot_col_X[21].SetXYZ(sin(120.000946 * TMath::Pi() / 180) * cos(-110.903458 * TMath::Pi() / 180),
                              sin(31.269562 * TMath::Pi() / 180) * cos(-92.845917 * TMath::Pi() / 180),
                              sin(81.990341 * TMath::Pi() / 180) * cos(163.756561 * TMath::Pi() / 180));

    NAJC_Rot_col_X[22].SetXYZ(sin(71.998657 * TMath::Pi() / 180) * cos(-31.716751 * TMath::Pi() / 180),
                              sin(88.528030 * TMath::Pi() / 180) * cos(58.761581 * TMath::Pi() / 180),
                              sin(18.065536 * TMath::Pi() / 180) * cos(153.279816 * TMath::Pi() / 180));

    NAJC_Rot_col_X[23].SetXYZ(sin(107.998169 * TMath::Pi() / 180) * cos(31.717102 * TMath::Pi() / 180),
                              sin(88.526611 * TMath::Pi() / 180) * cos(121.238419 * TMath::Pi() / 180),
                              sin(18.062485 * TMath::Pi() / 180) * cos(26.715042 * TMath::Pi() / 180));

    NAJC_Rot_col_X[24].SetXYZ(sin(59.999924 * TMath::Pi() / 180) * cos(110.903030 * TMath::Pi() / 180),
                              sin(31.269562 * TMath::Pi() / 180) * cos(-87.160690 * TMath::Pi() / 180),
                              sin(81.987610 * TMath::Pi() / 180) * cos(16.241531 * TMath::Pi() / 180));

    NAJC_Rot_col_X[25].SetXYZ(sin(59.999969 * TMath::Pi() / 180) * cos(-69.096970 * TMath::Pi() / 180),
                              sin(148.730408 * TMath::Pi() / 180) * cos(-87.160736 * TMath::Pi() / 180),
                              sin(98.012375 * TMath::Pi() / 180) * cos(16.241531 * TMath::Pi() / 180));

    NAJC_Rot_col_X[26].SetXYZ(sin(107.998398 * TMath::Pi() / 180) * cos(-148.281860 * TMath::Pi() / 180),
                              sin(91.476761 * TMath::Pi() / 180) * cos(121.238251 * TMath::Pi() / 180),
                              sin(161.936996 * TMath::Pi() / 180) * cos(26.704483 * TMath::Pi() / 180));

    NAJC_Rot_col_X[27].SetXYZ(sin(72.001816 * TMath::Pi() / 180) * cos(148.282852 * TMath::Pi() / 180),
                              sin(91.473373 * TMath::Pi() / 180) * cos(58.761581 * TMath::Pi() / 180),
                              sin(161.937485 * TMath::Pi() / 180) * cos(153.284943 * TMath::Pi() / 180));

    NAJC_Rot_col_X[28].SetXYZ(sin(89.999908 * TMath::Pi() / 180) * cos(-89.999969 * TMath::Pi() / 180),
                              sin(59.834961 * TMath::Pi() / 180) * cos(179.999939 * TMath::Pi() / 180),
                              sin(149.834991 * TMath::Pi() / 180) * cos(-180.0000 * TMath::Pi() / 180));

    NAJC_Rot_col_X[29].SetXYZ(sin(90.002869 * TMath::Pi() / 180) * cos(89.999802 * TMath::Pi() / 180),
                              sin(120.167007 * TMath::Pi() / 180) * cos(-179.998413 * TMath::Pi() / 180),
                              sin(30.166962 * TMath::Pi() / 180) * cos(179.994980 * TMath::Pi() / 180));

    NAJC_Rot_col_X[30].SetXYZ(sin(90.002869 * TMath::Pi() / 180) * cos(89.999863 * TMath::Pi() / 180),
                              sin(59.832993 * TMath::Pi() / 180) * cos(0.001541 * TMath::Pi() / 180),
                              sin(149.833023 * TMath::Pi() / 180) * cos(-0.004976 * TMath::Pi() / 180));

    NAJC_Rot_col_X[31].SetXYZ(sin(89.999969 * TMath::Pi() / 180) * cos(-89.999908 * TMath::Pi() / 180),
                              sin(120.165039 * TMath::Pi() / 180) * cos(0.000006 * TMath::Pi() / 180),
                              sin(30.164993 * TMath::Pi() / 180) * cos(-0.000001 * TMath::Pi() / 180));

    NAJC_Rot_col_X[32].SetXYZ(sin(72.001816 * TMath::Pi() / 180) * cos(148.282898 * TMath::Pi() / 180),
                              sin(88.526611 * TMath::Pi() / 180) * cos(-121.238419 * TMath::Pi() / 180),
                              sin(18.062500 * TMath::Pi() / 180) * cos(-26.715073 * TMath::Pi() / 180));

    NAJC_Rot_col_X[33].SetXYZ(sin(107.998398 * TMath::Pi() / 180) * cos(-148.281860 * TMath::Pi() / 180),
                              sin(88.523224 * TMath::Pi() / 180) * cos(-58.761688 * TMath::Pi() / 180),
                              sin(18.063004 * TMath::Pi() / 180) * cos(-153.295486 * TMath::Pi() / 180));

    NAJC_Rot_col_X[34].SetXYZ(sin(59.999924 * TMath::Pi() / 180) * cos(-69.096970 * TMath::Pi() / 180),
                              sin(31.269547 * TMath::Pi() / 180) * cos(92.839310 * TMath::Pi() / 180),
                              sin(81.987610 * TMath::Pi() / 180) * cos(-163.758408 * TMath::Pi() / 180));

    NAJC_Rot_col_X[35].SetXYZ(sin(59.999969 * TMath::Pi() / 180) * cos(110.903030 * TMath::Pi() / 180),
                              sin(148.730408 * TMath::Pi() / 180) * cos(92.839249 * TMath::Pi() / 180),
                              sin(98.012375 * TMath::Pi() / 180) * cos(-163.758408 * TMath::Pi() / 180));

    NAJC_Rot_col_X[36].SetXYZ(sin(107.998169 * TMath::Pi() / 180) * cos(31.717087 * TMath::Pi() / 180),
                              sin(91.473328 * TMath::Pi() / 180) * cos(-58.761581 * TMath::Pi() / 180),
                              sin(161.937485 * TMath::Pi() / 180) * cos(-153.284897 * TMath::Pi() / 180));

    NAJC_Rot_col_X[37].SetXYZ(sin(71.998657 * TMath::Pi() / 180) * cos(-31.716766 * TMath::Pi() / 180),
                              sin(91.471954 * TMath::Pi() / 180) * cos(-121.238419 * TMath::Pi() / 180),
                              sin(161.934418 * TMath::Pi() / 180) * cos(-26.720200 * TMath::Pi() / 180));

    NAJC_Rot_col_X[38].SetXYZ(sin(120.000946 * TMath::Pi() / 180) * cos(-110.903458 * TMath::Pi() / 180),
                              sin(148.730408 * TMath::Pi() / 180) * cos(87.154083 * TMath::Pi() / 180),
                              sin(98.009598 * TMath::Pi() / 180) * cos(-16.243423 * TMath::Pi() / 180));

    NAJC_Rot_col_X[39].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * cos(69.093414 * TMath::Pi() / 180),
                              sin(31.269516 * TMath::Pi() / 180) * cos(87.163361 * TMath::Pi() / 180),
                              sin(81.984879 * TMath::Pi() / 180) * cos(-16.243607 * TMath::Pi() / 180));

    NAJC_Rot_col_X[40].SetXYZ(sin(35.999680 * TMath::Pi() / 180) * cos(31.714874 * TMath::Pi() / 180),
                              sin(89.088608 * TMath::Pi() / 180) * cos(122.969406 * TMath::Pi() / 180),
                              sin(54.015533 * TMath::Pi() / 180) * cos(-146.368759 * TMath::Pi() / 180));

    NAJC_Rot_col_X[41].SetXYZ(sin(179.999939 * TMath::Pi() / 180) * cos(-44.284103 * TMath::Pi() / 180),
                              sin(89.999908 * TMath::Pi() / 180) * cos(-59.834961 * TMath::Pi() / 180),
                              sin(90.000076 * TMath::Pi() / 180) * cos(-149.834991 * TMath::Pi() / 180));

    NAJC_Rot_col_X[42].SetXYZ(sin(35.998962 * TMath::Pi() / 180) * cos(-148.285355 * TMath::Pi() / 180),
                              sin(90.913406 * TMath::Pi() / 180) * cos(122.972015 * TMath::Pi() / 180),
                              sin(125.983643 * TMath::Pi() / 180) * cos(-146.364716 * TMath::Pi() / 180));

    NAJC_Rot_col_X[43].SetXYZ(sin(144.000977 * TMath::Pi() / 180) * cos(148.285355 * TMath::Pi() / 180),
                              sin(90.913406 * TMath::Pi() / 180) * cos(57.027939 * TMath::Pi() / 180),
                              sin(125.983643 * TMath::Pi() / 180) * cos(-33.635284 * TMath::Pi() / 180));

    NAJC_Rot_col_X[44].SetXYZ(sin(0.002835 * TMath::Pi() / 180) * cos(177.685333 * TMath::Pi() / 180),
                              sin(90.001389 * TMath::Pi() / 180) * cos(-120.167007 * TMath::Pi() / 180),
                              sin(89.997452 * TMath::Pi() / 180) * cos(-30.166962 * TMath::Pi() / 180));

    NAJC_Rot_col_X[45].SetXYZ(sin(143.997803 * TMath::Pi() / 180) * cos(-31.715607 * TMath::Pi() / 180),
                              sin(89.087616 * TMath::Pi() / 180) * cos(57.028473 * TMath::Pi() / 180),
                              sin(54.013092 * TMath::Pi() / 180) * cos(-33.634064 * TMath::Pi() / 180));

    NAJC_Rot_col_X[46].SetXYZ(sin(59.999268 * TMath::Pi() / 180) * cos(69.096420 * TMath::Pi() / 180),
                              sin(57.035294 * TMath::Pi() / 180) * cos(-178.913727 * TMath::Pi() / 180),
                              sin(47.643875 * TMath::Pi() / 180) * cos(-52.665833 * TMath::Pi() / 180));

    NAJC_Rot_col_X[47].SetXYZ(sin(120.000229 * TMath::Pi() / 180) * cos(-69.097031 * TMath::Pi() / 180),
                              sin(57.032654 * TMath::Pi() / 180) * cos(-1.088669 * TMath::Pi() / 180),
                              sin(47.645844 * TMath::Pi() / 180) * cos(-127.337875 * TMath::Pi() / 180));

    NAJC_Rot_col_X[48].SetXYZ(sin(108.000092 * TMath::Pi() / 180) * cos(148.279999 * TMath::Pi() / 180),
                              sin(147.017288 * TMath::Pi() / 180) * cos(-91.676147 * TMath::Pi() / 180),
                              sin(63.373047 * TMath::Pi() / 180) * cos(-131.095139 * TMath::Pi() / 180));

    NAJC_Rot_col_X[49].SetXYZ(sin(107.998505 * TMath::Pi() / 180) * cos(-31.717911 * TMath::Pi() / 180),
                              sin(32.980560 * TMath::Pi() / 180) * cos(-91.674408 * TMath::Pi() / 180),
                              sin(116.625626 * TMath::Pi() / 180) * cos(-131.091690 * TMath::Pi() / 180));

    NAJC_Rot_col_X[50].SetXYZ(sin(120.002426 * TMath::Pi() / 180) * cos(110.905701 * TMath::Pi() / 180),
                              sin(122.966782 * TMath::Pi() / 180) * cos(-1.087466 * TMath::Pi() / 180),
                              sin(132.352707 * TMath::Pi() / 180) * cos(-127.333725 * TMath::Pi() / 180));

    NAJC_Rot_col_X[51].SetXYZ(sin(59.997635 * TMath::Pi() / 180) * cos(-110.905762 * TMath::Pi() / 180),
                              sin(122.966782 * TMath::Pi() / 180) * cos(-178.912521 * TMath::Pi() / 180),
                              sin(132.352707 * TMath::Pi() / 180) * cos(-52.666229 * TMath::Pi() / 180));

    NAJC_Rot_col_X[52].SetXYZ(sin(72.001434 * TMath::Pi() / 180) * cos(31.717896 * TMath::Pi() / 180),
                              sin(32.980560 * TMath::Pi() / 180) * cos(-88.325638 * TMath::Pi() / 180),
                              sin(116.625626 * TMath::Pi() / 180) * cos(-48.908279 * TMath::Pi() / 180));

    NAJC_Rot_col_X[53].SetXYZ(sin(72.001923 * TMath::Pi() / 180) * cos(-148.282684 * TMath::Pi() / 180),
                              sin(147.016739 * TMath::Pi() / 180) * cos(-88.321976 * TMath::Pi() / 180),
                              sin(63.370972 * TMath::Pi() / 180) * cos(-48.907745 * TMath::Pi() / 180));

    NAJC_Rot_col_X[54].SetXYZ(sin(89.999802 * TMath::Pi() / 180) * cos(-0.002818 * TMath::Pi() / 180),
                              sin(30.166946 * TMath::Pi() / 180) * cos(89.997299 * TMath::Pi() / 180),
                              sin(59.832993 * TMath::Pi() / 180) * cos(-90.002914 * TMath::Pi() / 180));

    NAJC_Rot_col_X[55].SetXYZ(sin(89.999969 * TMath::Pi() / 180) * cos(179.999939 * TMath::Pi() / 180),
                              sin(149.834991 * TMath::Pi() / 180) * cos(89.999908 * TMath::Pi() / 180),
                              sin(120.165039 * TMath::Pi() / 180) * cos(-89.999969 * TMath::Pi() / 180));

    NAJC_Rot_col_X[56].SetXYZ(sin(144.000214 * TMath::Pi() / 180) * cos(-148.285461 * TMath::Pi() / 180),
                              sin(121.226715 * TMath::Pi() / 180) * cos(-1.727005 * TMath::Pi() / 180),
                              sin(73.919296 * TMath::Pi() / 180) * cos(-81.661942 * TMath::Pi() / 180));

    NAJC_Rot_col_X[57].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * cos(148.279999 * TMath::Pi() / 180),
                              sin(121.226944 * TMath::Pi() / 180) * cos(-178.278625 * TMath::Pi() / 180),
                              sin(73.919357 * TMath::Pi() / 180) * cos(-98.343781 * TMath::Pi() / 180));

    NAJC_Rot_col_X[58].SetXYZ(sin(35.998840 * TMath::Pi() / 180) * cos(-31.714935 * TMath::Pi() / 180),
                              sin(58.773117 * TMath::Pi() / 180) * cos(-178.276993 * TMath::Pi() / 180),
                              sin(106.078720 * TMath::Pi() / 180) * cos(-98.340836 * TMath::Pi() / 180));

    NAJC_Rot_col_X[59].SetXYZ(sin(144.001083 * TMath::Pi() / 180) * cos(31.714920 * TMath::Pi() / 180),
                              sin(58.773117 * TMath::Pi() / 180) * cos(-1.722990 * TMath::Pi() / 180),
                              sin(106.078720 * TMath::Pi() / 180) * cos(-81.659164 * TMath::Pi() / 180));

    TVector3 NAJC_Rot_col_Y[60];
    NAJC_Rot_col_Y[0].SetXYZ(sin(144.001129 * TMath::Pi() / 180) * sin(31.714935 * TMath::Pi() / 180),
                             sin(121.226883 * TMath::Pi() / 180) * sin(178.276993 * TMath::Pi() / 180),
                             sin(73.921265 * TMath::Pi() / 180) * sin(98.340836 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[1].SetXYZ(sin(35.998871 * TMath::Pi() / 180) * sin(-31.714935 * TMath::Pi() / 180),
                             sin(121.226883 * TMath::Pi() / 180) * sin(1.722995 * TMath::Pi() / 180),
                             sin(73.921265 * TMath::Pi() / 180) * sin(81.659164 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[2].SetXYZ(sin(35.999969 * TMath::Pi() / 180) * sin(148.279953 * TMath::Pi() / 180),
                             sin(58.773056 * TMath::Pi() / 180) * sin(1.721368 * TMath::Pi() / 180),
                             sin(106.080643 * TMath::Pi() / 180) * sin(81.656204 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[3].SetXYZ(sin(144.000153 * TMath::Pi() / 180) * sin(-148.285461 * TMath::Pi() / 180),
                             sin(58.773270 * TMath::Pi() / 180) * sin(178.272949 * TMath::Pi() / 180),
                             sin(106.080688 * TMath::Pi() / 180) * sin(98.338043 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[4].SetXYZ(sin(89.999908 * TMath::Pi() / 180) * sin(-179.999939 * TMath::Pi() / 180),
                             sin(30.164978 * TMath::Pi() / 180) * sin(-89.999908 * TMath::Pi() / 180),
                             sin(59.834961 * TMath::Pi() / 180) * sin(89.999969 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[5].SetXYZ(sin(89.999802 * TMath::Pi() / 180) * sin(-0.002833 * TMath::Pi() / 180),
                             sin(149.833023 * TMath::Pi() / 180) * sin(-90.002701 * TMath::Pi() / 180),
                             sin(120.167007 * TMath::Pi() / 180) * sin(89.997025 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[6].SetXYZ(sin(72.001923 * TMath::Pi() / 180) * sin(-148.282684 * TMath::Pi() / 180),
                             sin(32.983261 * TMath::Pi() / 180) * sin(91.678024 * TMath::Pi() / 180),
                             sin(116.629028 * TMath::Pi() / 180) * sin(131.092255 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[7].SetXYZ(sin(72.001495 * TMath::Pi() / 180) * sin(31.717911 * TMath::Pi() / 180),
                             sin(147.019424 * TMath::Pi() / 180) * sin(91.674408 * TMath::Pi() / 180),
                             sin(63.374359 * TMath::Pi() / 180) * sin(131.091690 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[8].SetXYZ(sin(59.997574 * TMath::Pi() / 180) * sin(-110.905701 * TMath::Pi() / 180),
                             sin(57.033188 * TMath::Pi() / 180) * sin(1.087461 * TMath::Pi() / 180),
                             sin(47.647202 * TMath::Pi() / 180) * sin(127.333786 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[9].SetXYZ(sin(120.002365 * TMath::Pi() / 180) * sin(110.905762 * TMath::Pi() / 180),
                             sin(57.033188 * TMath::Pi() / 180) * sin(178.912521 * TMath::Pi() / 180),
                             sin(47.647202 * TMath::Pi() / 180) * sin(52.666214 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[10].SetXYZ(sin(107.998550 * TMath::Pi() / 180) * sin(-31.717896 * TMath::Pi() / 180),
                              sin(147.019424 * TMath::Pi() / 180) * sin(88.325638 * TMath::Pi() / 180),
                              sin(63.374359 * TMath::Pi() / 180) * sin(48.908279 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[11].SetXYZ(sin(108.000031 * TMath::Pi() / 180) * sin(148.279953 * TMath::Pi() / 180),
                              sin(32.982681 * TMath::Pi() / 180) * sin(88.323837 * TMath::Pi() / 180),
                              sin(116.626938 * TMath::Pi() / 180) * sin(48.904816 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[12].SetXYZ(sin(120.000229 * TMath::Pi() / 180) * sin(-69.097076 * TMath::Pi() / 180),
                              sin(122.967331 * TMath::Pi() / 180) * sin(178.911270 * TMath::Pi() / 180),
                              sin(132.354141 * TMath::Pi() / 180) * sin(52.662109 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[13].SetXYZ(sin(59.997574 * TMath::Pi() / 180) * sin(69.094299 * TMath::Pi() / 180),
                              sin(122.966812 * TMath::Pi() / 180) * sin(1.087461 * TMath::Pi() / 180),
                              sin(132.352798 * TMath::Pi() / 180) * sin(127.333786 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[14].SetXYZ(sin(143.997803 * TMath::Pi() / 180) * sin(-31.715652 * TMath::Pi() / 180),
                              sin(90.912369 * TMath::Pi() / 180) * sin(-122.971481 * TMath::Pi() / 180),
                              sin(125.986862 * TMath::Pi() / 180) * sin(146.365906 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[15].SetXYZ(sin(0.002820 * TMath::Pi() / 180) * sin(177.780685 * TMath::Pi() / 180),
                              sin(89.998596 * TMath::Pi() / 180) * sin(59.832993 * TMath::Pi() / 180),
                              sin(90.002533 * TMath::Pi() / 180) * sin(149.833023 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[16].SetXYZ(sin(144.001022 * TMath::Pi() / 180) * sin(148.285355 * TMath::Pi() / 180),
                              sin(89.086578 * TMath::Pi() / 180) * sin(-122.972015 * TMath::Pi() / 180),
                              sin(54.016281 * TMath::Pi() / 180) * sin(146.364716 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[17].SetXYZ(sin(35.998978 * TMath::Pi() / 180) * sin(-148.285355 * TMath::Pi() / 180),
                              sin(89.086578 * TMath::Pi() / 180) * sin(-57.027939 * TMath::Pi() / 180),
                              sin(54.016281 * TMath::Pi() / 180) * sin(33.635269 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[18].SetXYZ(sin(179.999939 * TMath::Pi() / 180) * sin(135.715851 * TMath::Pi() / 180),
                              sin(89.999908 * TMath::Pi() / 180) * sin(120.165039 * TMath::Pi() / 180),
                              sin(90.000076 * TMath::Pi() / 180) * sin(30.164993 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[19].SetXYZ(sin(35.999680 * TMath::Pi() / 180) * sin(31.714813 * TMath::Pi() / 180),
                              sin(90.911392 * TMath::Pi() / 180) * sin(-57.030579 * TMath::Pi() / 180),
                              sin(125.984406 * TMath::Pi() / 180) * sin(33.631210 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[20].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * sin(69.093414 * TMath::Pi() / 180),
                              sin(148.730469 * TMath::Pi() / 180) * sin(-92.836624 * TMath::Pi() / 180),
                              sin(98.015121 * TMath::Pi() / 180) * sin(163.756393 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[21].SetXYZ(sin(120.000946 * TMath::Pi() / 180) * sin(-110.903458 * TMath::Pi() / 180),
                              sin(31.269562 * TMath::Pi() / 180) * sin(-92.845917 * TMath::Pi() / 180),
                              sin(81.990341 * TMath::Pi() / 180) * sin(163.756561 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[22].SetXYZ(sin(71.998657 * TMath::Pi() / 180) * sin(-31.716751 * TMath::Pi() / 180),
                              sin(88.528030 * TMath::Pi() / 180) * sin(58.761581 * TMath::Pi() / 180),
                              sin(18.065536 * TMath::Pi() / 180) * sin(153.279816 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[23].SetXYZ(sin(107.998169 * TMath::Pi() / 180) * sin(31.717102 * TMath::Pi() / 180),
                              sin(88.526611 * TMath::Pi() / 180) * sin(121.238419 * TMath::Pi() / 180),
                              sin(18.062485 * TMath::Pi() / 180) * sin(26.715042 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[24].SetXYZ(sin(59.999924 * TMath::Pi() / 180) * sin(110.903030 * TMath::Pi() / 180),
                              sin(31.269562 * TMath::Pi() / 180) * sin(-87.160690 * TMath::Pi() / 180),
                              sin(81.987610 * TMath::Pi() / 180) * sin(16.241531 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[25].SetXYZ(sin(59.999969 * TMath::Pi() / 180) * sin(-69.096970 * TMath::Pi() / 180),
                              sin(148.730408 * TMath::Pi() / 180) * sin(-87.160736 * TMath::Pi() / 180),
                              sin(98.012375 * TMath::Pi() / 180) * sin(16.241531 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[26].SetXYZ(sin(107.998398 * TMath::Pi() / 180) * sin(-148.281860 * TMath::Pi() / 180),
                              sin(91.476761 * TMath::Pi() / 180) * sin(121.238251 * TMath::Pi() / 180),
                              sin(161.936996 * TMath::Pi() / 180) * sin(26.704483 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[27].SetXYZ(sin(72.001816 * TMath::Pi() / 180) * sin(148.282852 * TMath::Pi() / 180),
                              sin(91.473373 * TMath::Pi() / 180) * sin(58.761581 * TMath::Pi() / 180),
                              sin(161.937485 * TMath::Pi() / 180) * sin(153.284943 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[28].SetXYZ(sin(89.999908 * TMath::Pi() / 180) * sin(-89.999969 * TMath::Pi() / 180),
                              sin(59.834961 * TMath::Pi() / 180) * sin(179.999939 * TMath::Pi() / 180),
                              sin(149.834991 * TMath::Pi() / 180) * sin(-180.0000 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[29].SetXYZ(sin(90.002869 * TMath::Pi() / 180) * sin(89.999802 * TMath::Pi() / 180),
                              sin(120.167007 * TMath::Pi() / 180) * sin(-179.998413 * TMath::Pi() / 180),
                              sin(30.166962 * TMath::Pi() / 180) * sin(179.994980 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[30].SetXYZ(sin(90.002869 * TMath::Pi() / 180) * sin(89.999863 * TMath::Pi() / 180),
                              sin(59.832993 * TMath::Pi() / 180) * sin(0.001541 * TMath::Pi() / 180),
                              sin(149.833023 * TMath::Pi() / 180) * sin(-0.004976 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[31].SetXYZ(sin(89.999969 * TMath::Pi() / 180) * sin(-89.999908 * TMath::Pi() / 180),
                              sin(120.165039 * TMath::Pi() / 180) * sin(0.000006 * TMath::Pi() / 180),
                              sin(30.164993 * TMath::Pi() / 180) * sin(-0.000001 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[32].SetXYZ(sin(72.001816 * TMath::Pi() / 180) * sin(148.282898 * TMath::Pi() / 180),
                              sin(88.526611 * TMath::Pi() / 180) * sin(-121.238419 * TMath::Pi() / 180),
                              sin(18.062500 * TMath::Pi() / 180) * sin(-26.715073 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[33].SetXYZ(sin(107.998398 * TMath::Pi() / 180) * sin(-148.281860 * TMath::Pi() / 180),
                              sin(88.523224 * TMath::Pi() / 180) * sin(-58.761688 * TMath::Pi() / 180),
                              sin(18.063004 * TMath::Pi() / 180) * sin(-153.295486 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[34].SetXYZ(sin(59.999924 * TMath::Pi() / 180) * sin(-69.096970 * TMath::Pi() / 180),
                              sin(31.269547 * TMath::Pi() / 180) * sin(92.839310 * TMath::Pi() / 180),
                              sin(81.987610 * TMath::Pi() / 180) * sin(-163.758408 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[35].SetXYZ(sin(59.999969 * TMath::Pi() / 180) * sin(110.903030 * TMath::Pi() / 180),
                              sin(148.730408 * TMath::Pi() / 180) * sin(92.839249 * TMath::Pi() / 180),
                              sin(98.012375 * TMath::Pi() / 180) * sin(-163.758408 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[36].SetXYZ(sin(107.998169 * TMath::Pi() / 180) * sin(31.717087 * TMath::Pi() / 180),
                              sin(91.473328 * TMath::Pi() / 180) * sin(-58.761581 * TMath::Pi() / 180),
                              sin(161.937485 * TMath::Pi() / 180) * sin(-153.284897 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[37].SetXYZ(sin(71.998657 * TMath::Pi() / 180) * sin(-31.716766 * TMath::Pi() / 180),
                              sin(91.471954 * TMath::Pi() / 180) * sin(-121.238419 * TMath::Pi() / 180),
                              sin(161.934418 * TMath::Pi() / 180) * sin(-26.720200 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[38].SetXYZ(sin(120.000946 * TMath::Pi() / 180) * sin(-110.903458 * TMath::Pi() / 180),
                              sin(148.730408 * TMath::Pi() / 180) * sin(87.154083 * TMath::Pi() / 180),
                              sin(98.009598 * TMath::Pi() / 180) * sin(-16.243423 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[39].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * sin(69.093414 * TMath::Pi() / 180),
                              sin(31.269516 * TMath::Pi() / 180) * sin(87.163361 * TMath::Pi() / 180),
                              sin(81.984879 * TMath::Pi() / 180) * sin(-16.243607 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[40].SetXYZ(sin(35.999680 * TMath::Pi() / 180) * sin(31.714874 * TMath::Pi() / 180),
                              sin(89.088608 * TMath::Pi() / 180) * sin(122.969406 * TMath::Pi() / 180),
                              sin(54.015533 * TMath::Pi() / 180) * sin(-146.368759 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[41].SetXYZ(sin(179.999939 * TMath::Pi() / 180) * sin(-44.284103 * TMath::Pi() / 180),
                              sin(89.999908 * TMath::Pi() / 180) * sin(-59.834961 * TMath::Pi() / 180),
                              sin(90.000076 * TMath::Pi() / 180) * sin(-149.834991 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[42].SetXYZ(sin(35.998962 * TMath::Pi() / 180) * sin(-148.285355 * TMath::Pi() / 180),
                              sin(90.913406 * TMath::Pi() / 180) * sin(122.972015 * TMath::Pi() / 180),
                              sin(125.983643 * TMath::Pi() / 180) * sin(-146.364716 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[43].SetXYZ(sin(144.000977 * TMath::Pi() / 180) * sin(148.285355 * TMath::Pi() / 180),
                              sin(90.913406 * TMath::Pi() / 180) * sin(57.027939 * TMath::Pi() / 180),
                              sin(125.983643 * TMath::Pi() / 180) * sin(-33.635284 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[44].SetXYZ(sin(0.002835 * TMath::Pi() / 180) * sin(177.685333 * TMath::Pi() / 180),
                              sin(90.001389 * TMath::Pi() / 180) * sin(-120.167007 * TMath::Pi() / 180),
                              sin(89.997452 * TMath::Pi() / 180) * sin(-30.166962 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[45].SetXYZ(sin(143.997803 * TMath::Pi() / 180) * sin(-31.715607 * TMath::Pi() / 180),
                              sin(89.087616 * TMath::Pi() / 180) * sin(57.028473 * TMath::Pi() / 180),
                              sin(54.013092 * TMath::Pi() / 180) * sin(-33.634064 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[46].SetXYZ(sin(59.999268 * TMath::Pi() / 180) * sin(69.096420 * TMath::Pi() / 180),
                              sin(57.035294 * TMath::Pi() / 180) * sin(-178.913727 * TMath::Pi() / 180),
                              sin(47.643875 * TMath::Pi() / 180) * sin(-52.665833 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[47].SetXYZ(sin(120.000229 * TMath::Pi() / 180) * sin(-69.097031 * TMath::Pi() / 180),
                              sin(57.032654 * TMath::Pi() / 180) * sin(-1.088669 * TMath::Pi() / 180),
                              sin(47.645844 * TMath::Pi() / 180) * sin(-127.337875 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[48].SetXYZ(sin(108.000092 * TMath::Pi() / 180) * sin(148.279999 * TMath::Pi() / 180),
                              sin(147.017288 * TMath::Pi() / 180) * sin(-91.676147 * TMath::Pi() / 180),
                              sin(63.373047 * TMath::Pi() / 180) * sin(-131.095139 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[49].SetXYZ(sin(107.998505 * TMath::Pi() / 180) * sin(-31.717911 * TMath::Pi() / 180),
                              sin(32.980560 * TMath::Pi() / 180) * sin(-91.674408 * TMath::Pi() / 180),
                              sin(116.625626 * TMath::Pi() / 180) * sin(-131.091690 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[50].SetXYZ(sin(120.002426 * TMath::Pi() / 180) * sin(110.905701 * TMath::Pi() / 180),
                              sin(122.966782 * TMath::Pi() / 180) * sin(-1.087466 * TMath::Pi() / 180),
                              sin(132.352707 * TMath::Pi() / 180) * sin(-127.333725 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[51].SetXYZ(sin(59.997635 * TMath::Pi() / 180) * sin(-110.905762 * TMath::Pi() / 180),
                              sin(122.966782 * TMath::Pi() / 180) * sin(-178.912521 * TMath::Pi() / 180),
                              sin(132.352707 * TMath::Pi() / 180) * sin(-52.666229 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[52].SetXYZ(sin(72.001434 * TMath::Pi() / 180) * sin(31.717896 * TMath::Pi() / 180),
                              sin(32.980560 * TMath::Pi() / 180) * sin(-88.325638 * TMath::Pi() / 180),
                              sin(116.625626 * TMath::Pi() / 180) * sin(-48.908279 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[53].SetXYZ(sin(72.001923 * TMath::Pi() / 180) * sin(-148.282684 * TMath::Pi() / 180),
                              sin(147.016739 * TMath::Pi() / 180) * sin(-88.321976 * TMath::Pi() / 180),
                              sin(63.370972 * TMath::Pi() / 180) * sin(-48.907745 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[54].SetXYZ(sin(89.999802 * TMath::Pi() / 180) * sin(-0.002818 * TMath::Pi() / 180),
                              sin(30.166946 * TMath::Pi() / 180) * sin(89.997299 * TMath::Pi() / 180),
                              sin(59.832993 * TMath::Pi() / 180) * sin(-90.002914 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[55].SetXYZ(sin(89.999969 * TMath::Pi() / 180) * sin(179.999939 * TMath::Pi() / 180),
                              sin(149.834991 * TMath::Pi() / 180) * sin(89.999908 * TMath::Pi() / 180),
                              sin(120.165039 * TMath::Pi() / 180) * sin(-89.999969 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[56].SetXYZ(sin(144.000214 * TMath::Pi() / 180) * sin(-148.285461 * TMath::Pi() / 180),
                              sin(121.226715 * TMath::Pi() / 180) * sin(-1.727005 * TMath::Pi() / 180),
                              sin(73.919296 * TMath::Pi() / 180) * sin(-81.661942 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[57].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * sin(148.279999 * TMath::Pi() / 180),
                              sin(121.226944 * TMath::Pi() / 180) * sin(-178.278625 * TMath::Pi() / 180),
                              sin(73.919357 * TMath::Pi() / 180) * sin(-98.343781 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[58].SetXYZ(sin(35.998840 * TMath::Pi() / 180) * sin(-31.714935 * TMath::Pi() / 180),
                              sin(58.773117 * TMath::Pi() / 180) * sin(-178.276993 * TMath::Pi() / 180),
                              sin(106.078720 * TMath::Pi() / 180) * sin(-98.340836 * TMath::Pi() / 180));

    NAJC_Rot_col_Y[59].SetXYZ(sin(144.001083 * TMath::Pi() / 180) * sin(31.714920 * TMath::Pi() / 180),
                              sin(58.773117 * TMath::Pi() / 180) * sin(-1.722990 * TMath::Pi() / 180),
                              sin(106.078720 * TMath::Pi() / 180) * sin(-81.659164 * TMath::Pi() / 180));

    TVector3 NAJC_Rot_col_Z[60];
    NAJC_Rot_col_Z[0].SetXYZ(
        cos(144.001129 * TMath::Pi() / 180), cos(121.226883 * TMath::Pi() / 180), cos(73.921265 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[1].SetXYZ(
        cos(35.998871 * TMath::Pi() / 180), cos(121.226883 * TMath::Pi() / 180), cos(73.921265 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[2].SetXYZ(
        cos(35.999969 * TMath::Pi() / 180), cos(58.773056 * TMath::Pi() / 180), cos(106.080643 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[3].SetXYZ(
        cos(144.000153 * TMath::Pi() / 180), cos(58.773270 * TMath::Pi() / 180), cos(106.080688 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[4].SetXYZ(
        cos(89.999908 * TMath::Pi() / 180), cos(30.164978 * TMath::Pi() / 180), cos(59.834961 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[5].SetXYZ(
        cos(89.999802 * TMath::Pi() / 180), cos(149.833023 * TMath::Pi() / 180), cos(120.167007 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[6].SetXYZ(
        cos(72.001923 * TMath::Pi() / 180), cos(32.983261 * TMath::Pi() / 180), cos(116.629028 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[7].SetXYZ(
        cos(72.001495 * TMath::Pi() / 180), cos(147.019424 * TMath::Pi() / 180), cos(63.374359 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[8].SetXYZ(
        cos(59.997574 * TMath::Pi() / 180), cos(57.033188 * TMath::Pi() / 180), cos(47.647202 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[9].SetXYZ(
        cos(120.002365 * TMath::Pi() / 180), cos(57.033188 * TMath::Pi() / 180), cos(47.647202 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[10].SetXYZ(
        cos(107.998550 * TMath::Pi() / 180), cos(147.019424 * TMath::Pi() / 180), cos(63.374359 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[11].SetXYZ(
        cos(108.000031 * TMath::Pi() / 180), cos(32.982681 * TMath::Pi() / 180), cos(116.626938 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[12].SetXYZ(
        cos(120.000229 * TMath::Pi() / 180), cos(122.967331 * TMath::Pi() / 180), cos(132.354141 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[13].SetXYZ(
        cos(59.997574 * TMath::Pi() / 180), cos(122.966812 * TMath::Pi() / 180), cos(132.352798 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[14].SetXYZ(
        cos(143.997803 * TMath::Pi() / 180), cos(90.912369 * TMath::Pi() / 180), cos(125.986862 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[15].SetXYZ(
        cos(0.002820 * TMath::Pi() / 180), cos(89.998596 * TMath::Pi() / 180), cos(90.002533 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[16].SetXYZ(
        cos(144.001022 * TMath::Pi() / 180), cos(89.086578 * TMath::Pi() / 180), cos(54.016281 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[17].SetXYZ(
        cos(35.998978 * TMath::Pi() / 180), cos(89.086578 * TMath::Pi() / 180), cos(54.016281 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[18].SetXYZ(
        cos(179.999939 * TMath::Pi() / 180), cos(89.999908 * TMath::Pi() / 180), cos(90.000076 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[19].SetXYZ(
        cos(35.999680 * TMath::Pi() / 180), cos(90.911392 * TMath::Pi() / 180), cos(125.984406 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[20].SetXYZ(
        cos(119.999146 * TMath::Pi() / 180), cos(148.730469 * TMath::Pi() / 180), cos(98.015121 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[21].SetXYZ(
        cos(120.000946 * TMath::Pi() / 180), cos(31.269562 * TMath::Pi() / 180), cos(81.990341 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[22].SetXYZ(
        cos(71.998657 * TMath::Pi() / 180), cos(88.528030 * TMath::Pi() / 180), cos(18.065536 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[23].SetXYZ(
        cos(107.998169 * TMath::Pi() / 180), cos(88.526611 * TMath::Pi() / 180), cos(18.062485 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[24].SetXYZ(
        cos(59.999924 * TMath::Pi() / 180), cos(31.269562 * TMath::Pi() / 180), cos(81.987610 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[25].SetXYZ(
        cos(59.999969 * TMath::Pi() / 180), cos(148.730408 * TMath::Pi() / 180), cos(98.012375 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[26].SetXYZ(
        cos(107.998398 * TMath::Pi() / 180), cos(91.476761 * TMath::Pi() / 180), cos(161.936996 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[27].SetXYZ(
        cos(72.001816 * TMath::Pi() / 180), cos(91.473373 * TMath::Pi() / 180), cos(161.937485 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[28].SetXYZ(
        cos(89.999908 * TMath::Pi() / 180), cos(59.834961 * TMath::Pi() / 180), cos(149.834991 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[29].SetXYZ(
        cos(90.002869 * TMath::Pi() / 180), cos(120.167007 * TMath::Pi() / 180), cos(30.166962 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[30].SetXYZ(
        cos(90.002869 * TMath::Pi() / 180), cos(59.832993 * TMath::Pi() / 180), cos(149.833023 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[31].SetXYZ(
        cos(89.999969 * TMath::Pi() / 180), cos(120.165039 * TMath::Pi() / 180), cos(30.164993 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[32].SetXYZ(
        cos(72.001816 * TMath::Pi() / 180), cos(88.526611 * TMath::Pi() / 180), cos(18.062500 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[33].SetXYZ(
        cos(107.998398 * TMath::Pi() / 180), cos(88.523224 * TMath::Pi() / 180), cos(18.063004 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[34].SetXYZ(
        cos(59.999924 * TMath::Pi() / 180), cos(31.269547 * TMath::Pi() / 180), cos(81.987610 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[35].SetXYZ(
        cos(59.999969 * TMath::Pi() / 180), cos(148.730408 * TMath::Pi() / 180), cos(98.012375 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[36].SetXYZ(
        cos(107.998169 * TMath::Pi() / 180), cos(91.473328 * TMath::Pi() / 180), cos(161.937485 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[37].SetXYZ(
        cos(71.998657 * TMath::Pi() / 180), cos(91.471954 * TMath::Pi() / 180), cos(161.934418 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[38].SetXYZ(
        cos(120.000946 * TMath::Pi() / 180), cos(148.730408 * TMath::Pi() / 180), cos(98.009598 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[39].SetXYZ(
        cos(119.999146 * TMath::Pi() / 180), cos(31.269516 * TMath::Pi() / 180), cos(81.984879 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[40].SetXYZ(
        cos(35.999680 * TMath::Pi() / 180), cos(89.088608 * TMath::Pi() / 180), cos(54.015533 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[41].SetXYZ(
        cos(179.999939 * TMath::Pi() / 180), cos(89.999908 * TMath::Pi() / 180), cos(90.000076 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[42].SetXYZ(
        cos(35.998962 * TMath::Pi() / 180), cos(90.913406 * TMath::Pi() / 180), cos(125.983643 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[43].SetXYZ(
        cos(144.000977 * TMath::Pi() / 180), cos(90.913406 * TMath::Pi() / 180), cos(125.983643 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[44].SetXYZ(
        cos(0.002835 * TMath::Pi() / 180), cos(90.001389 * TMath::Pi() / 180), cos(89.997452 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[45].SetXYZ(
        cos(143.997803 * TMath::Pi() / 180), cos(89.087616 * TMath::Pi() / 180), cos(54.013092 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[46].SetXYZ(
        cos(59.999268 * TMath::Pi() / 180), cos(57.035294 * TMath::Pi() / 180), cos(47.643875 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[47].SetXYZ(
        cos(120.000229 * TMath::Pi() / 180), cos(57.032654 * TMath::Pi() / 180), cos(47.645844 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[48].SetXYZ(
        cos(108.000092 * TMath::Pi() / 180), cos(147.017288 * TMath::Pi() / 180), cos(63.373047 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[49].SetXYZ(
        cos(107.998505 * TMath::Pi() / 180), cos(32.980560 * TMath::Pi() / 180), cos(116.625626 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[50].SetXYZ(
        cos(120.002426 * TMath::Pi() / 180), cos(122.966782 * TMath::Pi() / 180), cos(132.352707 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[51].SetXYZ(
        cos(59.997635 * TMath::Pi() / 180), cos(122.966782 * TMath::Pi() / 180), cos(132.352707 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[52].SetXYZ(
        cos(72.001434 * TMath::Pi() / 180), cos(32.980560 * TMath::Pi() / 180), cos(116.625626 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[53].SetXYZ(
        cos(72.001923 * TMath::Pi() / 180), cos(147.016739 * TMath::Pi() / 180), cos(63.370972 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[54].SetXYZ(
        cos(89.999802 * TMath::Pi() / 180), cos(30.166946 * TMath::Pi() / 180), cos(59.832993 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[55].SetXYZ(
        cos(89.999969 * TMath::Pi() / 180), cos(149.834991 * TMath::Pi() / 180), cos(120.165039 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[56].SetXYZ(
        cos(144.000214 * TMath::Pi() / 180), cos(121.226715 * TMath::Pi() / 180), cos(73.919296 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[57].SetXYZ(
        cos(35.999985 * TMath::Pi() / 180), cos(121.226944 * TMath::Pi() / 180), cos(73.919357 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[58].SetXYZ(
        cos(35.998840 * TMath::Pi() / 180), cos(58.773117 * TMath::Pi() / 180), cos(106.078720 * TMath::Pi() / 180));
    NAJC_Rot_col_Z[59].SetXYZ(
        cos(144.001083 * TMath::Pi() / 180), cos(58.773117 * TMath::Pi() / 180), cos(106.078720 * TMath::Pi() / 180));

    /// here the loop now ....
    // m1_TRB1/2
    Double_t pi = TMath::Pi();
    TVector3 col_X_m1_TRC1(sin(90. * pi / 180) * cos(0. * pi / 180),
                           sin(90.545 * pi / 180) * cos(90. * pi / 180),
                           sin(0.545 * pi / 180) * cos(90. * pi / 180));
    TVector3 col_Y_m1_TRC1(sin(90. * pi / 180) * sin(0. * pi / 180),
                           sin(90.545 * pi / 180) * sin(90. * pi / 180),
                           sin(0.545 * pi / 180) * sin(90. * pi / 180));
    TVector3 col_Z_m1_TRC1(cos(90. * pi / 180), cos(90.545 * pi / 180), cos(0.545 * pi / 180));
    TVector3 col_X_m2_TRC2(sin(90. * pi / 180) * cos(0. * pi / 180),
                           sin(97.428 * pi / 180) * cos(90. * pi / 180),
                           sin(7.428 * pi / 180) * cos(90. * pi / 180));
    TVector3 col_Y_m2_TRC2(sin(90. * pi / 180) * sin(0. * pi / 180),
                           sin(97.428 * pi / 180) * sin(90. * pi / 180),
                           sin(7.428 * pi / 180) * sin(90. * pi / 180));
    TVector3 col_Z_m2_TRC2(cos(90. * pi / 180), cos(97.428 * pi / 180), cos(7.428 * pi / 180));
    TVector3 col_X_m3_TRC3(sin(90. * pi / 180) * cos(180. * pi / 180),
                           sin(96.883 * pi / 180) * cos(270. * pi / 180),
                           sin(6.883 * pi / 180) * cos(270. * pi / 180));
    TVector3 col_Y_m3_TRC3(sin(90. * pi / 180) * sin(180. * pi / 180),
                           sin(96.883 * pi / 180) * sin(270. * pi / 180),
                           sin(6.883 * pi / 180) * sin(270. * pi / 180));
    TVector3 col_Z_m3_TRC3(cos(90. * pi / 180), cos(96.883 * pi / 180), cos(6.883 * pi / 180));

    // - Adapted from R3BSim Geant4 definition
    // - Using the new templated Math ROOT Library
    // - <D.Bertini@gsi.de>

    //-  TRB1
    //--- Fill info for global Rotation / Translation
    Double_t fRotable0_0[9] = {
        col_X_m1_TRC1.X(), col_X_m1_TRC1.Y(), col_X_m1_TRC1.Z(), col_Y_m1_TRC1.X(), col_Y_m1_TRC1.Y(),
        col_Y_m1_TRC1.Z(), col_Z_m1_TRC1.X(), col_Z_m1_TRC1.Y(), col_Z_m1_TRC1.Z(),
    };
    ROOT::Math::XYZPoint tra1_TRC1(0., 3.33 / 10., 0.);
    ROOT::Math::Rotation3D* pm1_TRC1 = new ROOT::Math::Rotation3D(fRotable0_0[0],
                                                                  fRotable0_0[3],
                                                                  fRotable0_0[6],
                                                                  fRotable0_0[1],
                                                                  fRotable0_0[4],
                                                                  fRotable0_0[7],
                                                                  fRotable0_0[2],
                                                                  fRotable0_0[5],
                                                                  fRotable0_0[8]);
    //--- Single Crystal definition
    TGeoRotation* pIndividualRot_0 = NULL;
    TGeoRotation* pRingRot_0 = NULL;

    for (Int_t iter2 = 0; iter2 < 60; iter2++)
    {
        Double_t fRotable1_0[9] = { NAJC_Rot_col_X[iter2].X(), NAJC_Rot_col_X[iter2].Y(), NAJC_Rot_col_X[iter2].Z(),
                                    NAJC_Rot_col_Y[iter2].X(), NAJC_Rot_col_Y[iter2].Y(), NAJC_Rot_col_Y[iter2].Z(),
                                    NAJC_Rot_col_Z[iter2].X(), NAJC_Rot_col_Z[iter2].Y(), NAJC_Rot_col_Z[iter2].Z() };

        pIndividualRot_0 = new TGeoRotation();
        pIndividualRot_0->SetMatrix(fRotable1_0);

        ROOT::Math::Rotation3D* pIndRot_0 = new ROOT::Math::Rotation3D(fRotable1_0[0],
                                                                       fRotable1_0[3],
                                                                       fRotable1_0[6],
                                                                       fRotable1_0[1],
                                                                       fRotable1_0[4],
                                                                       fRotable1_0[7],
                                                                       fRotable1_0[2],
                                                                       fRotable1_0[5],
                                                                       fRotable1_0[8]);

        ROOT::Math::Rotation3D pIndRotInv_0 = pIndRot_0->Inverse();
        dx = NAJC_X[iter2] / 10.;
        dy = NAJC_Y[iter2] / 10.;
        dz = NAJC_Z[iter2] / 10.;
        ROOT::Math::XYZPoint tt_0(dx, dy, dz);
        ROOT::Math::XYZPoint ttt_0(0.0, 0.0, 0.0);
        ttt_0 = pIndRotInv_0 * tra1_TRC1;
        ttt_0.SetX(ttt_0.X() + tt_0.X());
        ttt_0.SetY(ttt_0.Y() + tt_0.Y());
        ttt_0.SetZ(ttt_0.Z() + tt_0.Z());

        ROOT::Math::Rotation3D pRingTmp_0;
        pRingTmp_0 = (*pm1_TRC1) * (*pIndRot_0);
        Double_t fRotable2_0[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
        pRingTmp_0.GetComponents(fRotable2_0[0],
                                 fRotable2_0[3],
                                 fRotable2_0[6],
                                 fRotable2_0[1],
                                 fRotable2_0[4],
                                 fRotable2_0[7],
                                 fRotable2_0[2],
                                 fRotable2_0[5],
                                 fRotable2_0[8]);
        pRingRot_0 = new TGeoRotation();
        pRingRot_0->SetMatrix(fRotable2_0);
        TGeoTranslation* trans_0 = new TGeoTranslation(ttt_0.X(), ttt_0.Y(), ttt_0.Z());
        TGeoCombiTrans* combi_0 = new TGeoCombiTrans(*trans_0, *pRingRot_0);

        // Add the Node in CBLogWorld
        pCBLogWorld->AddNode(pcrystalLogNAJC, iter2, combi_0);

    } //! for iter2

    // TRAP part
    // some other specs
    // Shape: TRAP1 type: TGeoTrap
    dz = 10.000000;
    theta = 0.000000;
    phi = 0.000000;
    h1 = 1.702000;
    bl1 = 5.068000;
    tl1 = 2.162000;
    alpha1 = 0.000000;
    h2 = 3.064000;
    bl2 = 9.122000;
    tl2 = 3.890000;
    alpha2 = 0.000000;
    TGeoShape* pTRAP1 = new TGeoTrap("TRAP1", dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
    // Shape: TRAP2 type: TGeoTrap
    dz = 10.000000;
    theta = 0.000000;
    phi = 0.000000;
    h1 = 1.702000;
    bl1 = 5.068000;
    tl1 = 2.162000;
    alpha1 = 0.000000;
    h2 = 3.064000;
    bl2 = 9.122000;
    tl2 = 3.890000;
    alpha2 = 0.000000;
    TGeoShape* pTRAP2 = new TGeoTrap("TRAP2", dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2);
    // Combi transformation: TRAP
    dx = 0.000000;
    dy = -4.745065;
    dz = -0.322321;
    // Rotation:
    thx = 90.000000;
    phx = 180.000000;
    thy = 97.772000;
    phy = 270.000000;
    thz = 7.772000;
    phz = 270.000000;
    TGeoRotation* pMatrix9 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix8 = new TGeoCombiTrans("TRAP", dx, dy, dz, pMatrix9);
    pBoolNode = new TGeoUnion(pTRAP1, pTRAP2, 0, pMatrix8);
    // Shape: TRAP type: TGeoCompositeShape
    TGeoShape* pTRAP = new TGeoCompositeShape("TRAP", pBoolNode);
    pTRAP->SetTitle("TRAP1+TRAP2:TRAP");
    // Volume: crystalLogNAJD
    TGeoVolume* pcrystalLogNAJD = new TGeoVolume("crystalLogNAJD", pTRAP, pMed9);

    Double_t NAJD_X[27] = { 0,        -108.157, 108.157,  108.157,  -108.157,  -283.1569, -283.1569,
                            283.1569, 283.1569, -175,     -175,     175,       175,       350,
                            -175,     -175,     175,      175,      -283.1569, -283.1569, 283.1569,
                            283.1569, 108.157,  -108.157, -108.157, 108.157,   0 };
    Double_t NAJD_Y[27] = { 350,       283.2094,  283.2094,  283.2094,  283.2094,  174.9965,  174.9965,
                            174.9965,  174.9965,  108.1815,  108.1815,  108.1815,  108.1815,  0,
                            -108.178,  -108.1815, -108.1815, -108.178,  -174.9965, -174.9965, -174.9965,
                            -174.9965, -283.2094, -283.2094, -283.2094, -283.2094, -350 };
    Double_t NAJD_Z[27] = { 0,        174.9124,  174.9124,  -174.9124, -174.9124, -108.164,  108.164,
                            108.164,  -108.164,  -283.1464, 283.1464,  283.1464,  -283.1464, 0,
                            283.1464, -283.1464, -283.1464, 283.1464,  108.164,   -108.164,  -108.164,
                            108.164,  174.9124,  174.9124,  -174.9124, -174.9124, 0 };

    TVector3 NAJD_Rot_col_X[27];
    NAJD_Rot_col_X[0].SetXYZ(sin(0.000003 * TMath::Pi() / 180) * cos(-121.720016 * TMath::Pi() / 180),
                             sin(90.000076 * TMath::Pi() / 180) * cos(-0.006011 * TMath::Pi() / 180),
                             sin(89.999969 * TMath::Pi() / 180) * cos(89.993958 * TMath::Pi() / 180));

    NAJD_Rot_col_X[1].SetXYZ(sin(108.000092 * TMath::Pi() / 180) * cos(31.719986 * TMath::Pi() / 180),
                             sin(144.002945 * TMath::Pi() / 180) * cos(148.288147 * TMath::Pi() / 180),
                             sin(60.003204 * TMath::Pi() / 180) * cos(110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_X[2].SetXYZ(sin(71.999969 * TMath::Pi() / 180) * cos(-31.720016 * TMath::Pi() / 180),
                             sin(144.002945 * TMath::Pi() / 180) * cos(31.711823 * TMath::Pi() / 180),
                             sin(60.003204 * TMath::Pi() / 180) * cos(69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_X[3].SetXYZ(sin(71.999908 * TMath::Pi() / 180) * cos(148.279953 * TMath::Pi() / 180),
                             sin(35.997025 * TMath::Pi() / 180) * cos(31.711823 * TMath::Pi() / 180),
                             sin(119.996796 * TMath::Pi() / 180) * cos(69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_X[4].SetXYZ(sin(108.000031 * TMath::Pi() / 180) * cos(-148.279999 * TMath::Pi() / 180),
                             sin(35.997040 * TMath::Pi() / 180) * cos(148.288147 * TMath::Pi() / 180),
                             sin(119.996796 * TMath::Pi() / 180) * cos(110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_X[5].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * cos(-148.279999 * TMath::Pi() / 180),
                             sin(59.998718 * TMath::Pi() / 180) * cos(69.093582 * TMath::Pi() / 180),
                             sin(107.998230 * TMath::Pi() / 180) * cos(148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_X[6].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * cos(31.719986 * TMath::Pi() / 180),
                             sin(120.001266 * TMath::Pi() / 180) * cos(69.093582 * TMath::Pi() / 180),
                             sin(72.001770 * TMath::Pi() / 180) * cos(148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_X[7].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * cos(-31.720016 * TMath::Pi() / 180),
                             sin(120.001266 * TMath::Pi() / 180) * cos(110.906418 * TMath::Pi() / 180),
                             sin(72.001770 * TMath::Pi() / 180) * cos(31.718048 * TMath::Pi() / 180));

    NAJD_Rot_col_X[8].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * cos(148.279953 * TMath::Pi() / 180),
                             sin(59.998718 * TMath::Pi() / 180) * cos(110.906418 * TMath::Pi() / 180),
                             sin(107.998230 * TMath::Pi() / 180) * cos(31.718033 * TMath::Pi() / 180));

    NAJD_Rot_col_X[9].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * cos(-69.093414 * TMath::Pi() / 180),
                             sin(71.999146 * TMath::Pi() / 180) * cos(-148.280991 * TMath::Pi() / 180),
                             sin(144.000320 * TMath::Pi() / 180) * cos(148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_X[10].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * cos(110.906570 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * cos(-148.281036 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * cos(148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_X[11].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * cos(-110.906570 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * cos(-31.718948 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * cos(31.714279 * TMath::Pi() / 180));

    NAJD_Rot_col_X[12].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * cos(69.093414 * TMath::Pi() / 180),
                              sin(71.999146 * TMath::Pi() / 180) * cos(-31.718948 * TMath::Pi() / 180),
                              sin(144.000320 * TMath::Pi() / 180) * cos(31.714249 * TMath::Pi() / 180));

    NAJD_Rot_col_X[13].SetXYZ(sin(90.000076 * TMath::Pi() / 180) * cos(90.000076 * TMath::Pi() / 180),
                              sin(0.006011 * TMath::Pi() / 180) * cos(-179.979675 * TMath::Pi() / 180),
                              sin(89.993958 * TMath::Pi() / 180) * cos(0.000000 * TMath::Pi() / 180));

    NAJD_Rot_col_X[14].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * cos(69.093414 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * cos(148.280991 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * cos(-148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_X[15].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * cos(-110.906570 * TMath::Pi() / 180),
                              sin(71.999146 * TMath::Pi() / 180) * cos(148.281036 * TMath::Pi() / 180),
                              sin(144.000320 * TMath::Pi() / 180) * cos(-148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_X[16].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * cos(110.906570 * TMath::Pi() / 180),
                              sin(71.999146 * TMath::Pi() / 180) * cos(31.718964 * TMath::Pi() / 180),
                              sin(144.000320 * TMath::Pi() / 180) * cos(-31.714264 * TMath::Pi() / 180));

    NAJD_Rot_col_X[17].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * cos(-69.093414 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * cos(31.718948 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * cos(-31.714249 * TMath::Pi() / 180));

    NAJD_Rot_col_X[18].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * cos(148.279953 * TMath::Pi() / 180),
                              sin(120.001266 * TMath::Pi() / 180) * cos(-69.093582 * TMath::Pi() / 180),
                              sin(72.001770 * TMath::Pi() / 180) * cos(-148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_X[19].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * cos(-31.719986 * TMath::Pi() / 180),
                              sin(59.998718 * TMath::Pi() / 180) * cos(-69.093582 * TMath::Pi() / 180),
                              sin(107.998230 * TMath::Pi() / 180) * cos(-148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_X[20].SetXYZ(sin(35.999969 * TMath::Pi() / 180) * cos(31.720001 * TMath::Pi() / 180),
                              sin(59.998718 * TMath::Pi() / 180) * cos(-110.906418 * TMath::Pi() / 180),
                              sin(107.998230 * TMath::Pi() / 180) * cos(-31.718048 * TMath::Pi() / 180));

    NAJD_Rot_col_X[21].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * cos(-148.279953 * TMath::Pi() / 180),
                              sin(120.001266 * TMath::Pi() / 180) * cos(-110.906418 * TMath::Pi() / 180),
                              sin(72.001770 * TMath::Pi() / 180) * cos(-31.718033 * TMath::Pi() / 180));

    NAJD_Rot_col_X[22].SetXYZ(sin(108.000092 * TMath::Pi() / 180) * cos(-148.279999 * TMath::Pi() / 180),
                              sin(144.002945 * TMath::Pi() / 180) * cos(-31.711838 * TMath::Pi() / 180),
                              sin(60.003204 * TMath::Pi() / 180) * cos(-69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_X[23].SetXYZ(sin(71.999969 * TMath::Pi() / 180) * cos(148.279953 * TMath::Pi() / 180),
                              sin(144.002945 * TMath::Pi() / 180) * cos(-148.288147 * TMath::Pi() / 180),
                              sin(60.003204 * TMath::Pi() / 180) * cos(-110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_X[24].SetXYZ(sin(71.999908 * TMath::Pi() / 180) * cos(-31.719986 * TMath::Pi() / 180),
                              sin(35.997025 * TMath::Pi() / 180) * cos(-148.288147 * TMath::Pi() / 180),
                              sin(119.996796 * TMath::Pi() / 180) * cos(-110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_X[25].SetXYZ(sin(108.000031 * TMath::Pi() / 180) * cos(31.720001 * TMath::Pi() / 180),
                              sin(35.997040 * TMath::Pi() / 180) * cos(-31.711838 * TMath::Pi() / 180),
                              sin(119.996796 * TMath::Pi() / 180) * cos(-69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_X[26].SetXYZ(sin(0.000003 * TMath::Pi() / 180) * cos(58.279968 * TMath::Pi() / 180),
                              sin(90.000076 * TMath::Pi() / 180) * cos(179.993988 * TMath::Pi() / 180),
                              sin(89.999969 * TMath::Pi() / 180) * cos(-90.006027 * TMath::Pi() / 180));

    TVector3 NAJD_Rot_col_Y[27];
    NAJD_Rot_col_Y[0].SetXYZ(sin(0.000003 * TMath::Pi() / 180) * sin(-121.720016 * TMath::Pi() / 180),
                             sin(90.000076 * TMath::Pi() / 180) * sin(-0.006011 * TMath::Pi() / 180),
                             sin(89.999969 * TMath::Pi() / 180) * sin(89.993958 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[1].SetXYZ(sin(108.000092 * TMath::Pi() / 180) * sin(31.719986 * TMath::Pi() / 180),
                             sin(144.002945 * TMath::Pi() / 180) * sin(148.288147 * TMath::Pi() / 180),
                             sin(60.003204 * TMath::Pi() / 180) * sin(110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[2].SetXYZ(sin(71.999969 * TMath::Pi() / 180) * sin(-31.720016 * TMath::Pi() / 180),
                             sin(144.002945 * TMath::Pi() / 180) * sin(31.711823 * TMath::Pi() / 180),
                             sin(60.003204 * TMath::Pi() / 180) * sin(69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[3].SetXYZ(sin(71.999908 * TMath::Pi() / 180) * sin(148.279953 * TMath::Pi() / 180),
                             sin(35.997025 * TMath::Pi() / 180) * sin(31.711823 * TMath::Pi() / 180),
                             sin(119.996796 * TMath::Pi() / 180) * sin(69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[4].SetXYZ(sin(108.000031 * TMath::Pi() / 180) * sin(-148.279999 * TMath::Pi() / 180),
                             sin(35.997040 * TMath::Pi() / 180) * sin(148.288147 * TMath::Pi() / 180),
                             sin(119.996796 * TMath::Pi() / 180) * sin(110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[5].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * sin(-148.279999 * TMath::Pi() / 180),
                             sin(59.998718 * TMath::Pi() / 180) * sin(69.093582 * TMath::Pi() / 180),
                             sin(107.998230 * TMath::Pi() / 180) * sin(148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[6].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * sin(31.719986 * TMath::Pi() / 180),
                             sin(120.001266 * TMath::Pi() / 180) * sin(69.093582 * TMath::Pi() / 180),
                             sin(72.001770 * TMath::Pi() / 180) * sin(148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[7].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * sin(-31.720016 * TMath::Pi() / 180),
                             sin(120.001266 * TMath::Pi() / 180) * sin(110.906418 * TMath::Pi() / 180),
                             sin(72.001770 * TMath::Pi() / 180) * sin(31.718048 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[8].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * sin(148.279953 * TMath::Pi() / 180),
                             sin(59.998718 * TMath::Pi() / 180) * sin(110.906418 * TMath::Pi() / 180),
                             sin(107.998230 * TMath::Pi() / 180) * sin(31.718033 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[9].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * sin(-69.093414 * TMath::Pi() / 180),
                             sin(71.999146 * TMath::Pi() / 180) * sin(-148.280991 * TMath::Pi() / 180),
                             sin(144.000320 * TMath::Pi() / 180) * sin(148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[10].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * sin(110.906570 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * sin(-148.281036 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * sin(148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[11].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * sin(-110.906570 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * sin(-31.718948 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * sin(31.714279 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[12].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * sin(69.093414 * TMath::Pi() / 180),
                              sin(71.999146 * TMath::Pi() / 180) * sin(-31.718948 * TMath::Pi() / 180),
                              sin(144.000320 * TMath::Pi() / 180) * sin(31.714249 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[13].SetXYZ(sin(90.000076 * TMath::Pi() / 180) * sin(90.000076 * TMath::Pi() / 180),
                              sin(0.006011 * TMath::Pi() / 180) * sin(-179.979675 * TMath::Pi() / 180),
                              sin(89.993958 * TMath::Pi() / 180) * sin(0.000000 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[14].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * sin(69.093414 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * sin(148.280991 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * sin(-148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[15].SetXYZ(sin(60.000854 * TMath::Pi() / 180) * sin(-110.906570 * TMath::Pi() / 180),
                              sin(71.999146 * TMath::Pi() / 180) * sin(148.281036 * TMath::Pi() / 180),
                              sin(144.000320 * TMath::Pi() / 180) * sin(-148.285690 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[16].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * sin(110.906570 * TMath::Pi() / 180),
                              sin(71.999146 * TMath::Pi() / 180) * sin(31.718964 * TMath::Pi() / 180),
                              sin(144.000320 * TMath::Pi() / 180) * sin(-31.714264 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[17].SetXYZ(sin(119.999146 * TMath::Pi() / 180) * sin(-69.093414 * TMath::Pi() / 180),
                              sin(108.000854 * TMath::Pi() / 180) * sin(31.718948 * TMath::Pi() / 180),
                              sin(35.999664 * TMath::Pi() / 180) * sin(-31.714249 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[18].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * sin(148.279953 * TMath::Pi() / 180),
                              sin(120.001266 * TMath::Pi() / 180) * sin(-69.093582 * TMath::Pi() / 180),
                              sin(72.001770 * TMath::Pi() / 180) * sin(-148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[19].SetXYZ(sin(143.999985 * TMath::Pi() / 180) * sin(-31.719986 * TMath::Pi() / 180),
                              sin(59.998718 * TMath::Pi() / 180) * sin(-69.093582 * TMath::Pi() / 180),
                              sin(107.998230 * TMath::Pi() / 180) * sin(-148.281921 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[20].SetXYZ(sin(35.999969 * TMath::Pi() / 180) * sin(31.720001 * TMath::Pi() / 180),
                              sin(59.998718 * TMath::Pi() / 180) * sin(-110.906418 * TMath::Pi() / 180),
                              sin(107.998230 * TMath::Pi() / 180) * sin(-31.718048 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[21].SetXYZ(sin(35.999985 * TMath::Pi() / 180) * sin(-148.279953 * TMath::Pi() / 180),
                              sin(120.001266 * TMath::Pi() / 180) * sin(-110.906418 * TMath::Pi() / 180),
                              sin(72.001770 * TMath::Pi() / 180) * sin(-31.718033 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[22].SetXYZ(sin(108.000092 * TMath::Pi() / 180) * sin(-148.279999 * TMath::Pi() / 180),
                              sin(144.002945 * TMath::Pi() / 180) * sin(-31.711838 * TMath::Pi() / 180),
                              sin(60.003204 * TMath::Pi() / 180) * sin(-69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[23].SetXYZ(sin(71.999969 * TMath::Pi() / 180) * sin(148.279953 * TMath::Pi() / 180),
                              sin(144.002945 * TMath::Pi() / 180) * sin(-148.288147 * TMath::Pi() / 180),
                              sin(60.003204 * TMath::Pi() / 180) * sin(-110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[24].SetXYZ(sin(71.999908 * TMath::Pi() / 180) * sin(-31.719986 * TMath::Pi() / 180),
                              sin(35.997025 * TMath::Pi() / 180) * sin(-148.288147 * TMath::Pi() / 180),
                              sin(119.996796 * TMath::Pi() / 180) * sin(-110.909149 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[25].SetXYZ(sin(108.000031 * TMath::Pi() / 180) * sin(31.720001 * TMath::Pi() / 180),
                              sin(35.997040 * TMath::Pi() / 180) * sin(-31.711838 * TMath::Pi() / 180),
                              sin(119.996796 * TMath::Pi() / 180) * sin(-69.090851 * TMath::Pi() / 180));

    NAJD_Rot_col_Y[26].SetXYZ(sin(0.000003 * TMath::Pi() / 180) * sin(58.279968 * TMath::Pi() / 180),
                              sin(90.000076 * TMath::Pi() / 180) * sin(179.993988 * TMath::Pi() / 180),
                              sin(89.999969 * TMath::Pi() / 180) * sin(-90.006027 * TMath::Pi() / 180));

    TVector3 NAJD_Rot_col_Z[27];
    NAJD_Rot_col_Z[0].SetXYZ(
        cos(0.000003 * TMath::Pi() / 180), cos(90.000076 * TMath::Pi() / 180), cos(89.999969 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[1].SetXYZ(
        cos(108.000092 * TMath::Pi() / 180), cos(144.002945 * TMath::Pi() / 180), cos(60.003204 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[2].SetXYZ(
        cos(71.999969 * TMath::Pi() / 180), cos(144.002945 * TMath::Pi() / 180), cos(60.003204 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[3].SetXYZ(
        cos(71.999908 * TMath::Pi() / 180), cos(35.997025 * TMath::Pi() / 180), cos(119.996796 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[4].SetXYZ(
        cos(108.000031 * TMath::Pi() / 180), cos(35.997040 * TMath::Pi() / 180), cos(119.996796 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[5].SetXYZ(
        cos(35.999985 * TMath::Pi() / 180), cos(59.998718 * TMath::Pi() / 180), cos(107.998230 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[6].SetXYZ(
        cos(35.999985 * TMath::Pi() / 180), cos(120.001266 * TMath::Pi() / 180), cos(72.001770 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[7].SetXYZ(
        cos(143.999985 * TMath::Pi() / 180), cos(120.001266 * TMath::Pi() / 180), cos(72.001770 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[8].SetXYZ(
        cos(143.999985 * TMath::Pi() / 180), cos(59.998718 * TMath::Pi() / 180), cos(107.998230 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[9].SetXYZ(
        cos(119.999146 * TMath::Pi() / 180), cos(71.999146 * TMath::Pi() / 180), cos(144.000320 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[10].SetXYZ(
        cos(119.999146 * TMath::Pi() / 180), cos(108.000854 * TMath::Pi() / 180), cos(35.999664 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[11].SetXYZ(
        cos(60.000854 * TMath::Pi() / 180), cos(108.000854 * TMath::Pi() / 180), cos(35.999664 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[12].SetXYZ(
        cos(60.000854 * TMath::Pi() / 180), cos(71.999146 * TMath::Pi() / 180), cos(144.000320 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[13].SetXYZ(
        cos(90.000076 * TMath::Pi() / 180), cos(0.006011 * TMath::Pi() / 180), cos(89.993958 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[14].SetXYZ(
        cos(60.000854 * TMath::Pi() / 180), cos(108.000854 * TMath::Pi() / 180), cos(35.999664 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[15].SetXYZ(
        cos(60.000854 * TMath::Pi() / 180), cos(71.999146 * TMath::Pi() / 180), cos(144.000320 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[16].SetXYZ(
        cos(119.999146 * TMath::Pi() / 180), cos(71.999146 * TMath::Pi() / 180), cos(144.000320 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[17].SetXYZ(
        cos(119.999146 * TMath::Pi() / 180), cos(108.000854 * TMath::Pi() / 180), cos(35.999664 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[18].SetXYZ(
        cos(143.999985 * TMath::Pi() / 180), cos(120.001266 * TMath::Pi() / 180), cos(72.001770 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[19].SetXYZ(
        cos(143.999985 * TMath::Pi() / 180), cos(59.998718 * TMath::Pi() / 180), cos(107.998230 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[20].SetXYZ(
        cos(35.999969 * TMath::Pi() / 180), cos(59.998718 * TMath::Pi() / 180), cos(107.998230 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[21].SetXYZ(
        cos(35.999985 * TMath::Pi() / 180), cos(120.001266 * TMath::Pi() / 180), cos(72.001770 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[22].SetXYZ(
        cos(108.000092 * TMath::Pi() / 180), cos(144.002945 * TMath::Pi() / 180), cos(60.003204 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[23].SetXYZ(
        cos(71.999969 * TMath::Pi() / 180), cos(144.002945 * TMath::Pi() / 180), cos(60.003204 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[24].SetXYZ(
        cos(71.999908 * TMath::Pi() / 180), cos(35.997025 * TMath::Pi() / 180), cos(119.996796 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[25].SetXYZ(
        cos(108.000031 * TMath::Pi() / 180), cos(35.997040 * TMath::Pi() / 180), cos(119.996796 * TMath::Pi() / 180));
    NAJD_Rot_col_Z[26].SetXYZ(
        cos(0.000003 * TMath::Pi() / 180), cos(90.000076 * TMath::Pi() / 180), cos(89.999969 * TMath::Pi() / 180));

    TVector3 col_X_m1_TRAP1(sin(90. * TMath::Pi() / 180) * cos(0. * TMath::Pi() / 180),
                            sin(93.886 * TMath::Pi() / 180) * cos(90. * TMath::Pi() / 180),
                            sin(3.886 * TMath::Pi() / 180) * cos(90. * TMath::Pi() / 180));
    TVector3 col_X_m2_TRAP2(sin(90. * TMath::Pi() / 180) * cos(180. * TMath::Pi() / 180),
                            sin(93.886 * TMath::Pi() / 180) * cos(270. * TMath::Pi() / 180),
                            sin(3.886 * TMath::Pi() / 180) * cos(270. * TMath::Pi() / 180));
    TVector3 col_Y_m1_TRAP1(sin(90. * TMath::Pi() / 180) * sin(0. * TMath::Pi() / 180),
                            sin(93.886 * TMath::Pi() / 180) * sin(90. * TMath::Pi() / 180),
                            sin(3.886 * TMath::Pi() / 180) * sin(90. * TMath::Pi() / 180));
    TVector3 col_Y_m2_TRAP2(sin(90. * TMath::Pi() / 180) * sin(180. * TMath::Pi() / 180),
                            sin(93.886 * TMath::Pi() / 180) * sin(270. * TMath::Pi() / 180),
                            sin(3.886 * TMath::Pi() / 180) * sin(270. * TMath::Pi() / 180));
    TVector3 col_Z_m1_TRAP1(
        cos(90. * TMath::Pi() / 180), cos(93.886 * TMath::Pi() / 180), cos(3.886 * TMath::Pi() / 180));
    TVector3 col_Z_m2_TRAP2(
        cos(90. * TMath::Pi() / 180), cos(93.886 * TMath::Pi() / 180), cos(3.886 * TMath::Pi() / 180));

    // - Adapted from R3BSim Geant4 definition
    // - Using the new templated Math ROOT Library
    // - <D.Bertini@gsi.de>

    //-  TRB1
    //--- Fill info for global Rotation / Translation
    Double_t fRotable0_1[9] = {
        col_X_m1_TRAP1.X(), col_X_m1_TRAP1.Y(), col_X_m1_TRAP1.Z(), col_Y_m1_TRAP1.X(), col_Y_m1_TRAP1.Y(),
        col_Y_m1_TRAP1.Z(), col_Z_m1_TRAP1.X(), col_Z_m1_TRAP1.Y(), col_Z_m1_TRAP1.Z(),
    };
    ROOT::Math::XYZPoint tra1_TRAP1(0., 23.78 / 10., 0.);
    ROOT::Math::Rotation3D* pm1_TRAP1 = new ROOT::Math::Rotation3D(fRotable0_1[0],
                                                                   fRotable0_1[3],
                                                                   fRotable0_1[6],
                                                                   fRotable0_1[1],
                                                                   fRotable0_1[4],
                                                                   fRotable0_1[7],
                                                                   fRotable0_1[2],
                                                                   fRotable0_1[5],
                                                                   fRotable0_1[8]);
    //--- Single Crystal definition
    TGeoRotation* pIndividualRot_1 = NULL;
    TGeoRotation* pRingRot_1 = NULL;

    for (Int_t iter2 = 0; iter2 < 27; iter2++)
    {
        Double_t fRotable1_1[9] = { NAJD_Rot_col_X[iter2].X(), NAJD_Rot_col_X[iter2].Y(), NAJD_Rot_col_X[iter2].Z(),
                                    NAJD_Rot_col_Y[iter2].X(), NAJD_Rot_col_Y[iter2].Y(), NAJD_Rot_col_Y[iter2].Z(),
                                    NAJD_Rot_col_Z[iter2].X(), NAJD_Rot_col_Z[iter2].Y(), NAJD_Rot_col_Z[iter2].Z() };

        pIndividualRot_1 = new TGeoRotation();
        pIndividualRot_1->SetMatrix(fRotable1_1);

        ROOT::Math::Rotation3D* pIndRot_1 = new ROOT::Math::Rotation3D(fRotable1_1[0],
                                                                       fRotable1_1[3],
                                                                       fRotable1_1[6],
                                                                       fRotable1_1[1],
                                                                       fRotable1_1[4],
                                                                       fRotable1_1[7],
                                                                       fRotable1_1[2],
                                                                       fRotable1_1[5],
                                                                       fRotable1_1[8]);

        ROOT::Math::Rotation3D pIndRotInv_1 = pIndRot_1->Inverse();
        dx = NAJD_X[iter2] / 10.;
        dy = NAJD_Y[iter2] / 10.;
        dz = NAJD_Z[iter2] / 10.;
        ROOT::Math::XYZPoint tt_1(dx, dy, dz);
        ROOT::Math::XYZPoint ttt_1(0.0, 0.0, 0.0);
        ttt_1 = pIndRotInv_1 * tra1_TRAP1;
        ttt_1.SetX(ttt_1.X() + tt_1.X());
        ttt_1.SetY(ttt_1.Y() + tt_1.Y());
        ttt_1.SetZ(ttt_1.Z() + tt_1.Z());

        ROOT::Math::Rotation3D pRingTmp_1;
        pRingTmp_1 = (*pm1_TRAP1) * (*pIndRot_1);
        Double_t fRotable2_1[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
        pRingTmp_1.GetComponents(fRotable2_1[0],
                                 fRotable2_1[3],
                                 fRotable2_1[6],
                                 fRotable2_1[1],
                                 fRotable2_1[4],
                                 fRotable2_1[7],
                                 fRotable2_1[2],
                                 fRotable2_1[5],
                                 fRotable2_1[8]);
        pRingRot_1 = new TGeoRotation();
        pRingRot_1->SetMatrix(fRotable2_1);
        TGeoTranslation* trans_1 = new TGeoTranslation(ttt_1.X(), ttt_1.Y(), ttt_1.Z());
        TGeoCombiTrans* combi_1 = new TGeoCombiTrans(*trans_1, *pRingRot_1);

        // Add the Node in CBLogWorld
        pCBLogWorld->AddNode(pcrystalLogNAJD, iter2, combi_1);

    } //! for iter2

    // --- Definition of the Reaction Chamber

    // -- Combi transformation:
    dx = 0.000000;
    dy = 0.000000;
    dz = 0.000000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 0.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix_1 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix_0 = new TGeoCombiTrans("", dx, dy, dz, pMatrix_1);
    // Combi transformation:
    dx = 0.000000;
    dy = 0.000000;
    dz = 39.100000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 0.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix_3 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix_2 = new TGeoCombiTrans("", dx, dy, dz, pMatrix_3);
    // Combi transformation:
    dx = 0.000000;
    dy = 0.000000;
    dz = -39.100000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 270.000000;
    thz = 180.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix_5 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix_4 = new TGeoCombiTrans("", dx, dy, dz, pMatrix_5);
    // Combi transformation:
    dx = -39.100000;
    dy = 0.000000;
    dz = 0.000000;
    // Rotation:
    thx = 90.000000;
    phx = 90.000000;
    thy = 180.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 180.000000;
    TGeoRotation* pMatrix_7 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix_6 = new TGeoCombiTrans("", dx, dy, dz, pMatrix_7);

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
    pCBLogWorld->AddNode(pCBChamberLog, 0, pMatrix_0);
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
    pCBLogWorld->AddNode(pCBConicalTubeLog, 0, pMatrix_2);
    pCBLogWorld->AddNode(pCBConicalTubeLog, 1, pMatrix_4);
    pCBLogWorld->AddNode(pCBConicalTubeLog, 2, pMatrix_6);

    // End of XBall Crystal definition
    // Define Sensitive Modules
    AddSensitiveVolume(pcrystalLogNAJA);
    AddSensitiveVolume(pcrystalLogNAJB);
    AddSensitiveVolume(pcrystalLogNAJC);
    AddSensitiveVolume(pcrystalLogNAJD);
}

// -----   Public method ConstructGeometry   ----------------------------------
void R3BXBallv0::ConstructGeometry1()
{
    LOG(error) << "R3BXBallv0 Dummy function called !!! ";
    LOG(error) << "no Crystal geometry will be created ... ";
    return;
}

TGeoRotation* R3BXBallv0::createMatrix(Double_t phi, Double_t theta, Double_t psi)
{

    // Rotation
    TGeoRotation* matrix = new TGeoRotation("");

    Double_t rm[9];

    Double_t degrad = TMath::Pi() / 180.;

    // define trigonometry
    Double_t sinPhi = sin(degrad * phi);
    Double_t cosPhi = TMath::Cos(degrad * phi);
    Double_t sinTheta = sin(degrad * theta);
    Double_t cosTheta = TMath::Cos(degrad * theta);
    Double_t sinPsi = sin(degrad * psi);
    Double_t cosPsi = TMath::Cos(degrad * psi);

    // filling from Euler definition a la G4 !

    rm[0] = cosPsi * cosPhi - cosTheta * sinPhi * sinPsi;
    rm[1] = cosPsi * sinPhi + cosTheta * cosPhi * sinPsi;
    rm[2] = sinPsi * sinTheta;

    rm[3] = -sinPsi * cosPhi - cosTheta * sinPhi * cosPsi;
    rm[4] = -sinPsi * sinPhi + cosTheta * cosPhi * cosPsi;
    rm[5] = cosPsi * sinTheta;

    rm[6] = sinTheta * sinPhi;
    rm[7] = -sinTheta * cosPhi;
    rm[8] = cosTheta;

    matrix->SetMatrix((const Double_t*)&rm[0]);

    return matrix;
}

ClassImp(R3BXBallv0)
