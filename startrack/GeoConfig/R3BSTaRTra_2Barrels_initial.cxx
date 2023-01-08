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
// -----                        R3BSTaRTra source file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------
#include "R3BSTaRTra.h"

#include "R3BGeoSTaRTra.h"
#include "R3BGeoSTaRTraPar.h"
#include "R3BMCStack.h"
#include "R3BSTaRTraPoint.h"

#include "FairGeoInterface.h"
#include "FairGeoLoader.h"
#include "FairGeoNode.h"
#include "FairGeoRootBuilder.h"
#include "FairRootManager.h"
#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "FairVolume.h"

#include "TClonesArray.h"
#include "TGeoMCGeometry.h"
#include "TObjArray.h"
#include "TParticle.h"
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
#include "TGeoPara.h"
#include "TGeoPgon.h"
#include "TGeoShapeAssembly.h"
#include "TGeoSphere.h"
#include "TGeoTube.h"
#include "TParticle.h"
#include "TVirtualMC.h"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
R3BSTaRTra::R3BSTaRTra()
    : R3BDetector("R3BSTaRTra", kTRUE, kTRA)
{
    ResetParameters();
    fTraCollection = new TClonesArray("R3BSTaRTraPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
R3BSTaRTra::R3BSTaRTra(const char* name, Bool_t active)
    : R3BDetector(name, active, kTRA)
{
    ResetParameters();
    fTraCollection = new TClonesArray("R3BSTaRTraPoint");
    fPosIndex = 0;
    kGeoSaved = kFALSE;
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fVerboseLevel = 1;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BSTaRTra::~R3BSTaRTra()
{

    if (flGeoPar)
        delete flGeoPar;
    if (fTraCollection)
    {
        fTraCollection->Delete();
        delete fTraCollection;
    }
}
// -------------------------------------------------------------------------

// ----   Initialize   -----------------------------------------------------

void R3BSTaRTra::Initialize()
{
    FairDetector::Initialize();

    cout << endl;
    cout << "-I- R3BSTaRTra: initialisation" << endl;
    cout << "-I- R3BSTaRTra: Sens. Vol. (McId) " << gMC->VolId("STaRTraLog") << endl;
}

// -----   Public method ProcessHits  --------------------------------------
Bool_t R3BSTaRTra::ProcessHits(FairVolume* vol)
{

    //      cout << " -I process hit called for:" <<  vol->GetName() << endl;
    // Set parameters at entrance of volume. Reset ELoss.

    //    if ( vol ) {
    //        cout << " Name Id:copy "
    //            << vol->getName() << " : " << vol->getMCid() << " : " << vol->getCopyNo() << endl;
    //        Int_t copyNo=0;
    //        cout << " Geant: " << gMC->CurrentVolID(copyNo) << ":" << copyNo << endl;
    //    }

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        // cout << "X,Y,X tracker=" << fPosIn(0) << " " << fPosIn(1) << " " << fPosIn(2) << endl;
        // cout << "track length=" << fLength << endl;
    }

    // Sum energy loss for all steps in the active volume
    fELoss += gMC->Edep();
    // cout << "Tracker Eloss=" << fELoss << "  " << gMC->Edep() << endl;

    // Set additional parameters at exit of active volume. Create R3BSTaRTraPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fVolumeID = vol->getCopyNo();  // getMCid();
        fDetCopyID = vol->getCopyNo(); // added by Marc
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

            //       cout << "1st direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2] <<
            //       endl;

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
                cerr << "2nd direction: " << olddirection[0] << "," << olddirection[1] << "," << olddirection[2]
                     << " with safety = " << safety << endl;
                cerr << "oldpos = " << oldpos[0] << "," << oldpos[1] << "," << oldpos[2] << endl;
                cerr << "newpos = " << newpos[0] << "," << newpos[1] << "," << newpos[2] << endl;
            }

            fPosOut.SetX(newpos[0]);
            fPosOut.SetY(newpos[1]);
            fPosOut.SetZ(newpos[2]);
        }

        AddHit(fTrackID,
               fVolumeID,
               fDetCopyID, // fdetCopyID, added by Marc
               TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
               TVector3(fPosOut.X(), fPosOut.Y(), fPosOut.Z()),
               TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
               TVector3(fMomOut.Px(), fMomOut.Py(), fMomOut.Pz()),
               fTime,
               fLength,
               fELoss);

        // Increment number of STaRTraPoints for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kTRA);

        ResetParameters();
    }

    return kTRUE;
}
// ----------------------------------------------------------------------------
// void R3BSTaRTra::SaveGeoParams(){
//
//  cout << " -I Save STS geo params " << endl;
//
//  TFolder *mf = (TFolder*) gDirectory->FindObjectAny("cbmroot");
//  cout << " mf: " << mf << endl;
//  TFolder *stsf = NULL;
//  if (mf ) stsf = (TFolder*) mf->FindObjectAny(GetName());
//  cout << " stsf: " << stsf << endl;
//  if (stsf) stsf->Add( flGeoPar0 ) ;
//  FairRootManager::Instance()->WriteFolder();
//  mf->Write("cbmroot",TObject::kWriteDelete);
//}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BSTaRTra::BeginEvent()
{

    //  cout << "-I- begin tracker event called ##########" << endl;

    if (gGeoManager)
    {
        TGeoVolume* vol = gGeoManager->FindVolumeFast("STaRTraLog");

        if (vol)
        {
            //    cout << "id tracker serial number : " << vol->GetNumber() << endl;
        }
    }

    //  if (! kGeoSaved ) {
    //      SaveGeoParams();
    //  cout << "-I STS geometry parameters saved " << endl;
    //  kGeoSaved = kTRUE;
    //  }
}
// -----   Public method EndOfEvent   -----------------------------------------
void R3BSTaRTra::EndOfEvent()
{

    if (fVerboseLevel)
        Print();
    fTraCollection->Clear();

    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method Register   -------------------------------------------
void R3BSTaRTra::Register() { FairRootManager::Instance()->Register("STaRTraPoint", GetName(), fTraCollection, kTRUE); }
// ----------------------------------------------------------------------------

// -----   Public method GetCollection   --------------------------------------
TClonesArray* R3BSTaRTra::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
        return fTraCollection;
    else
        return NULL;
}
// ----------------------------------------------------------------------------

// -----   Public method Print   ----------------------------------------------
void R3BSTaRTra::Print() const
{
    Int_t nHits = fTraCollection->GetEntriesFast();
    cout << "-I- R3BTra: " << nHits << " points registered in this event." << endl;
}
// ----------------------------------------------------------------------------

// -----   Public method Reset   ----------------------------------------------
void R3BSTaRTra::Reset()
{
    fTraCollection->Clear();
    ResetParameters();
}
// ----------------------------------------------------------------------------

// -----   Public method CopyClones   -----------------------------------------
void R3BSTaRTra::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    cout << "-I- R3BTra: " << nEntries << " entries to add." << endl;
    TClonesArray& clref = *cl2;
    R3BSTaRTraPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BSTaRTraPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BSTaRTraPoint(*oldpoint);
        fPosIndex++;
    }
    cout << " -I- R3BTra: " << cl2->GetEntriesFast() << " merged entries." << endl;
}

// -----   Private method AddHit   --------------------------------------------
R3BSTaRTraPoint* R3BSTaRTra::AddHit(Int_t trackID,
                                    Int_t detID,
                                    Int_t detCopyID,
                                    TVector3 posIn, // Int_t detCopyID added by Marc
                                    TVector3 posOut,
                                    TVector3 momIn,
                                    TVector3 momOut,
                                    Double_t time,
                                    Double_t length,
                                    Double_t eLoss)
{
    TClonesArray& clref = *fTraCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
        cout << "-I- R3BSTaRTra: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
             << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV"
             << endl;
    return new (clref[size]) R3BSTaRTraPoint(trackID,
                                             detID,
                                             detCopyID,
                                             posIn,
                                             posOut, // detCopy added by Marc
                                             momIn,
                                             momOut,
                                             time,
                                             length,
                                             eLoss);
}

// -----   Public method ConstructGeometry   ----------------------------------
void R3BSTaRTra::ConstructGeometry()
{

    // out-of-file geometry definition
    Double_t dx, dy, dz;
    // Double_t par[20];
    Double_t rmin, rmax;
    Double_t a;
    Double_t thx, phx, thy, phy, thz, phz;
    Double_t phi1, phi2;
    Double_t z, density, radl, absl, w;
    Double_t tx, ty, tz;
    Int_t nel, numed;

    /****************************************************************************/
    // Material definition

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

    // Material: Silicon
    TGeoMedium* pMed22 = NULL;
    if (gGeoManager->GetMedium("Silicon"))
    {
        pMed22 = gGeoManager->GetMedium("Silicon");
    }
    else
    {
        a = 28.090000;
        z = 14.000000;
        density = 2.330000;
        radl = 9.351106;
        absl = 456.628489;
        TGeoMaterial* pMat22 = new TGeoMaterial("Silicon", a, z, density);

        pMat22->SetIndex(21);
        // Medium: Silicon
        numed = 21; // medium number
        Double_t par[8];
        par[0] = 0.000000; // isvol
        par[1] = 0.000000; // ifield
        par[2] = 0.000000; // fieldm
        par[3] = 0.000000; // tmaxfd
        par[4] = 0.000000; // stemax
        par[5] = 0.000000; // deemax
        par[6] = 0.000100; // epsil
        par[7] = 0.000000; // stmin
        pMed22 = new TGeoMedium("Silicon", numed, pMat22, par);
    }

    // Material: Copper
    TGeoMedium* pMed25 = NULL;
    if (gGeoManager->GetMedium("Copper"))
    {
        pMed25 = gGeoManager->GetMedium("Copper");
    }
    else
    {
        a = 63.540000;
        z = 29.000000;
        density = 8.960000;
        radl = 1.435029;
        absl = 155.874854;
        TGeoMaterial* pMat25 = new TGeoMaterial("Copper", a, z, density, radl, absl);
        pMat25->SetIndex(24);
        // Medium: Copper
        numed = 24; // medium number
        Double_t par[8];
        par[0] = 0.000000; // isvol
        par[1] = 0.000000; // ifield
        par[2] = 0.000000; // fieldm
        par[3] = 0.000000; // tmaxfd
        par[4] = 0.000000; // stemax
        par[5] = 0.000000; // deemax
        par[6] = 0.000100; // epsil
        par[7] = 0.000000; // stmin
        pMed25 = new TGeoMedium("Copper", numed, pMat25, par);
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
        TGeoMaterial* pMat21 = new TGeoMaterial("Aluminum", a, z, density, radl, absl);
        pMat21->SetIndex(20);
        // Medium: Aluminum
        numed = 20; // medium number
        Double_t par[8];
        par[0] = 0.000000; // isvol
        par[1] = 0.000000; // ifield
        par[2] = 0.000000; // fieldm
        par[3] = 0.000000; // tmaxfd
        par[4] = 0.000000; // stemax
        par[5] = 0.000000; // deemax
        par[6] = 0.000100; // epsil
        par[7] = 0.000000; // stmin
        pMed21 = new TGeoMedium("Aluminum", numed, pMat21, par);
    }

    // TRANSFORMATION MATRICES

    // Detectors

    // Combi transformation:
    dx = -0.000000;
    dy = -2.5000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 90.000000;
    TGeoRotation* pMatrix3 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix2 = new TGeoCombiTrans("", dx, dy, dz, pMatrix3);

    // Combi transformation:
    dx = -1.7670000;
    dy = -1.767000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 315.000000;
    thz = 90.000000;
    phz = 45.000000;
    TGeoRotation* pMatrix5 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix4 = new TGeoCombiTrans("", dx, dy, dz, pMatrix5);

    // Combi transformation:
    dx = -2.50000;
    dy = 0.00000;
    dz = 15.00000;
    // Rotation:
    thx = 180.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix7 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix6 = new TGeoCombiTrans("", dx, dy, dz, pMatrix7);

    // Combi transformation:
    dx = -1.7670000;
    dy = 1.767000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 45.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix37 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix36 = new TGeoCombiTrans("", dx, dy, dz, pMatrix37);

    // Combi transformation:
    dx = 0.000000;
    dy = 2.5000;
    dz = 15.00000;
    // Rotation:
    thx = 180.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 90.000000;
    TGeoRotation* pMatrix11 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix10 = new TGeoCombiTrans("", dx, dy, dz, pMatrix11);

    // Combi transformation:
    dx = 1.7670000;
    dy = 1.767000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 315.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix35 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix34 = new TGeoCombiTrans("", dx, dy, dz, pMatrix35);

    // Combi transformation:
    dx = 2.50000;
    dy = 0.00000;
    dz = 15.00000;
    // Rotation:
    thx = 180.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix13 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix12 = new TGeoCombiTrans("", dx, dy, dz, pMatrix13);

    // Combi transformation:
    dx = 1.767000;
    dy = -1.767000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 45.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix39 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix38 = new TGeoCombiTrans("", dx, dy, dz, pMatrix39);

    // Combi transformation:
    dx = 0.000000;
    dy = -10.0000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 90.000000;
    TGeoRotation* pMatrix41 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix40 = new TGeoCombiTrans("", dx, dy, dz, pMatrix41);

    // Combi transformation:
    dx = -7.069000;
    dy = -7.06900;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 315.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix43 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix42 = new TGeoCombiTrans("", dx, dy, dz, pMatrix43);

    // Combi transformation:
    dx = -10.0000;
    dy = 0.00000;
    dz = 15.00000;
    // Rotation:
    thx = 180.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix45 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix44 = new TGeoCombiTrans("", dx, dy, dz, pMatrix45);

    // Combi transformation:
    dx = 0.000000;
    dy = 10.000;
    dz = 15.00000;
    // Rotation:
    thx = 180.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 90.000000;
    TGeoRotation* pMatrix47 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix46 = new TGeoCombiTrans("", dx, dy, dz, pMatrix47);

    // Combi transformation:
    dx = 10.0000;
    dy = 0.00000;
    dz = 15.00000;
    // Rotation:
    thx = 180.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix49 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix48 = new TGeoCombiTrans("", dx, dy, dz, pMatrix49);

    // Combi transformation:
    dx = -7.069000;
    dy = 7.069000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 45.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix51 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix50 = new TGeoCombiTrans("", dx, dy, dz, pMatrix51);

    // Combi transformation:
    dx = 7.0690000;
    dy = -7.069000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 45.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix53 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix52 = new TGeoCombiTrans("", dx, dy, dz, pMatrix53);

    // Combi transformation:
    dx = 7.0690000;
    dy = 7.069000;
    dz = 15.00000;
    // Rotation:
    thx = 0.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = -45.000000;
    thz = 90.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix55 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix54 = new TGeoCombiTrans("", dx, dy, dz, pMatrix55);

    // Mounting platform

    // Combi transformation:
    dx = 0.000000;
    dy = -14.090000;
    dz = 0.000000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 0.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 270.000000;
    TGeoRotation* pMatrix15 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix14 = new TGeoCombiTrans("", dx, dy, dz, pMatrix15);

    // Combi transformation:
    dx = 0.000000;
    dy = -17.000000;
    dz = 0.000000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 0.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 270.000000;
    TGeoRotation* pMatrix17 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix16 = new TGeoCombiTrans("", dx, dy, dz, pMatrix17);

    // Electronics modules

    // Combi transformation:
    dx = 0.000000;
    dy = -8.000000;
    dz = 10.820000;
    // Rotation:
    thx = 90.000000;
    phx = 270.000000;
    thy = 90.000000;
    phy = 0.000000;
    thz = 0.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix19 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix18 = new TGeoCombiTrans("", dx, dy, dz, pMatrix19);
    // Combi transformation:
    dx = 0.000000;
    dy = 8.000000;
    dz = 13.500000;
    // Rotation:
    thx = 90.000000;
    phx = 270.000000;
    thy = 90.000000;
    phy = 0.000000;
    thz = 0.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix33 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix32 = new TGeoCombiTrans("", dx, dy, dz, pMatrix33);
    // Combi transformation:
    dx = -2.100000;
    dy = -8.000000;
    dz = 4.470000;
    // Rotation:
    thx = 90.000000;
    phx = 90.000000;
    thy = 0.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 360.000000;
    TGeoRotation* pMatrix21 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix20 = new TGeoCombiTrans("", dx, dy, dz, pMatrix21);
    // Combi transformation:
    dx = 2.100000;
    dy = 8.000000;
    dz = 4.470000;
    // Rotation:
    thx = 90.000000;
    phx = 90.000000;
    thy = 180.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 180.000000;
    TGeoRotation* pMatrix23 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix22 = new TGeoCombiTrans("", dx, dy, dz, pMatrix23);
    // Combi transformation:
    dx = 8.000000;
    dy = -2.100000;
    dz = 4.470000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 180.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 90.000000;
    TGeoRotation* pMatrix25 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix24 = new TGeoCombiTrans("", dx, dy, dz, pMatrix25);
    // Combi transformation:
    dx = -8.000000;
    dy = 2.100000;
    dz = 4.470000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 180.000000;
    phy = 0.000000;
    thz = 90.000000;
    phz = 90.000000;
    TGeoRotation* pMatrix27 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix26 = new TGeoCombiTrans("", dx, dy, dz, pMatrix27);
    // Combi transformation:
    dx = -6.240000;
    dy = -3.900000;
    dz = 0.000000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 0.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix29 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix28 = new TGeoCombiTrans("", dx, dy, dz, pMatrix29);

    // Target

    // Combi transformation:
    dx = -6.240000;
    dy = -3.900000;
    dz = 0.000000;
    // Rotation:
    thx = 90.000000;
    phx = 0.000000;
    thy = 90.000000;
    phy = 90.000000;
    thz = 0.000000;
    phz = 0.000000;
    TGeoRotation* pMatrix31 = new TGeoRotation("", thx, phx, thy, phy, thz, phz);
    TGeoCombiTrans* pMatrix30 = new TGeoCombiTrans("", dx, dy, dz, pMatrix31);

    // Shape: World type: TGeoBBox
    TGeoVolume* pWorld = gGeoManager->GetTopVolume();
    pWorld->SetVisLeaves(kTRUE);

    /*

    // Create a global Mother Volume
    dx = 100.000000;
    dy = 100.000000;
    dz = 100.000000;
    TGeoShape *pBoxWorld = new TGeoBBox("TraBoxWorld", dx,dy,dz);
    TGeoVolume*
    pWorld  = new TGeoVolume("TraBoxLogWorld",pBoxWorld, pMed2);
    pWorld->SetVisLeaves(kTRUE);
    TGeoCombiTrans *pGlobalc = GetGlobalPosition();

    // add the sphere as Mother Volume
    pAWorld->AddNode(pWorld, 0, pGlobalc);

    */

    // SHAPES, VOLUMES AND GEOMETRICAL HIERARCHY
    // Shape: SiVacuumSphereWorld type: TGeoSphere

    // Si Shape & volume: TraBox type: TGeoBBox
    dx = 10.00000;
    dy = 1.035000;
    dz = 0.005000;
    // Volume: STaRTraLog
    TGeoVolume* STaRTraLog2 = gGeoManager->MakeBox("STaRTraLog2", pMed22, dx, dy, dz);

    // Si Shape & volume: TraBox type: TGeoBBox
    dx = 10.00000;
    dy = 4.141000;
    dz = 0.015000;
    // Volume: STaRTraLog
    TGeoVolume* STaRTraLog = gGeoManager->MakeBox("STaRTraLog", pMed22, dx, dy, dz);

    // Shape: MontagePlatform type: TGeoTubeSeg
    rmin = 2.750000;
    rmax = 18.000000;
    dz = 0.150000;
    phi1 = 0.000000;
    phi2 = 360.000000;
    TGeoShape* pMontagePlatform = new TGeoTubeSeg("MontagePlatform", rmin, rmax, dz, phi1, phi2);
    // Volume: MontagePlatformLog
    TGeoVolume* pMontagePlatformLog = new TGeoVolume("MontagePlatformLog", pMontagePlatform, pMed25);

    // Shape: MontageRing type: TGeoTubeSeg
    rmin = 12.000000;
    rmax = 15.000000;
    dz = 0.250000;
    phi1 = 0.000000;
    phi2 = 360.000000;
    TGeoShape* pMontageRing = new TGeoTubeSeg("MontageRing", rmin, rmax, dz, phi1, phi2);
    // Volume: MontageRingLog
    TGeoVolume* pMontageRingLog = new TGeoVolume("MontageRingLog", pMontageRing, pMed25);

    // Shape: targetWheel type: TGeoTubeSeg
    rmin = 0.410000;
    rmax = 5.300000;
    dz = 0.100000;
    phi1 = 0.000000;
    phi2 = 360.000000;
    TGeoShape* ptargetWheel = new TGeoTubeSeg("targetWheel", rmin, rmax, dz, phi1, phi2);
    // Volume: targetWheelLog
    TGeoVolume* ptargetWheelLog = new TGeoVolume("targetWheelLog", ptargetWheel, pMed21);

    // Shape: targetWheel2 type: TGeoTubeSeg
    rmin = 9.500000;
    rmax = 10.000000;
    dz = 0.100000;
    phi1 = 0.000000;
    phi2 = 360.000000;
    TGeoShape* ptargetWheel2 = new TGeoTubeSeg("targetWheel2", rmin, rmax, dz, phi1, phi2);
    // Volume: targetWheel2Log
    TGeoVolume* ptargetWheel2Log = new TGeoVolume("targetWheel2Log", ptargetWheel2, pMed21);

    // Shape: innerElectronicBox type: TGeoBBox
    dx = 3.800000;
    dy = 3.300000;
    dz = 0.800000;
    TGeoShape* pinnerElectronicBox = new TGeoBBox("innerElectronicBox", dx, dy, dz);
    // Volume: innerElectronicsLog
    TGeoVolume* pinnerElectronicsLog = new TGeoVolume("innerElectronicsLog", pinnerElectronicBox, pMed2);

    //
    // Make elementary assembly of the whole structure.
    //

    TGeoVolume* aSTaRTra = new TGeoVolumeAssembly("ASTARTRA");

    aSTaRTra->AddNode(ptargetWheel2Log, 1, pMatrix30);

    // aSTaRTra->AddNode(pinnerElectronicsLog ,1, pMatrix18);
    // aSTaRTra->AddNode(pinnerElectronicsLog ,2, pMatrix20);
    // aSTaRTra->AddNode(pinnerElectronicsLog ,3, pMatrix22);
    // aSTaRTra->AddNode(pinnerElectronicsLog ,4, pMatrix24);
    // aSTaRTra->AddNode(pinnerElectronicsLog ,5, pMatrix26);
    // aSTaRTra->AddNode(pinnerElectronicsLog ,6, pMatrix32);

    // aSTaRTra->AddNode(ptargetWheelLog ,1, pMatrix28);

    aSTaRTra->AddNode(pMontageRingLog, 1, pMatrix16);

    aSTaRTra->AddNode(pMontagePlatformLog, 1, pMatrix14);

    AddSensitiveVolume(STaRTraLog);
    AddSensitiveVolume(STaRTraLog2);
    fNbOfSensitiveVol += 1;

    // aSTaRTra->AddNode(STaRTraLog,3, pMatrix8);
    // aSTaRTra->AddNode(STaRTraLog,4, pMatrix10);
    // aSTaRTra->AddNode(STaRTraLog,5, pMatrix12);
    aSTaRTra->AddNode(STaRTraLog2, 1, pMatrix2);
    aSTaRTra->AddNode(STaRTraLog2, 2, pMatrix4);
    aSTaRTra->AddNode(STaRTraLog2, 3, pMatrix6);
    aSTaRTra->AddNode(STaRTraLog2, 4, pMatrix36);
    aSTaRTra->AddNode(STaRTraLog2, 5, pMatrix10);
    aSTaRTra->AddNode(STaRTraLog2, 6, pMatrix34);
    aSTaRTra->AddNode(STaRTraLog2, 7, pMatrix12);
    aSTaRTra->AddNode(STaRTraLog2, 8, pMatrix38);
    aSTaRTra->AddNode(STaRTraLog, 9, pMatrix40);
    aSTaRTra->AddNode(STaRTraLog, 10, pMatrix42);
    aSTaRTra->AddNode(STaRTraLog, 11, pMatrix44);
    aSTaRTra->AddNode(STaRTraLog, 12, pMatrix50);
    aSTaRTra->AddNode(STaRTraLog, 13, pMatrix46);
    aSTaRTra->AddNode(STaRTraLog, 14, pMatrix54);
    aSTaRTra->AddNode(STaRTraLog, 15, pMatrix48);
    aSTaRTra->AddNode(STaRTraLog, 16, pMatrix52);

    //

    TGeoRotation* rotg = new TGeoRotation();
    rotg->RotateX(0.);
    rotg->RotateY(0.);
    rotg->RotateZ(0.);
    dx = tx = 0.0;
    dy = ty = 0.0;
    dz = tz = 0.0;

    TGeoCombiTrans* t0 = new TGeoCombiTrans(tx, ty, tz, rotg);
    pWorld->AddNode(aSTaRTra, 1, GetGlobalPosition(t0));
}

/*
void R3BSTaRTra::ConstructGeometry() {

  FairGeoLoader*    geoLoad = FairGeoLoader::Instance();
  FairGeoInterface* geoFace = geoLoad->getGeoInterface();
  R3BGeoSTaRTra*       stsGeo  = new R3BGeoSTaRTra();
  stsGeo->setGeomFile(GetGeometryFileName());
  geoFace->addGeoModule(stsGeo);

  Bool_t rc = geoFace->readSet(stsGeo);
  if (rc) stsGeo->create(geoLoad->getGeoBuilder());
  TList* volList = stsGeo->getListOfVolumes();
  // store geo parameter
  FairRun *fRun = FairRun::Instance();
  FairRuntimeDb *rtdb= FairRun::Instance()->GetRuntimeDb();
  R3BGeoSTaRTraPar* par=(R3BGeoSTaRTraPar*)(rtdb->getContainer("R3BGeoSTaRTraPar"));
  TObjArray *fSensNodes = par->GetGeoSensitiveNodes();
  TObjArray *fPassNodes = par->GetGeoPassiveNodes();

  TListIter iter(volList);
  FairGeoNode* node   = NULL;
  FairGeoVolume *aVol=NULL;

  while( (node = (FairGeoNode*)iter.Next()) ) {
      aVol = dynamic_cast<FairGeoVolume*> ( node );
       if ( node->isSensitive()  ) {
           fSensNodes->AddLast( aVol );
       }else{
           fPassNodes->AddLast( aVol );
       }
  }
  par->setChanged();
  par->setInputVersion(fRun->GetRunId(),1);
  ProcessNodes( volList );

}
*/

ClassImp(R3BSTaRTra)
