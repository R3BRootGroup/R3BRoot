/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <TFile.h>
#include <TGeoManager.h>
#include <TGeoNavigator.h>
#include <TGeoVolume.h>
#include <TMath.h>
#include <TString.h>
#include <TSystem.h>
#include <TVector3.h>
#include <vector>

#include <FairLogger.h>

#include "R3BCalifaGeometry.h"

#include <iostream>
#include <stdlib.h>

using std::cerr;
using std::cout;
using std::endl;

R3BCalifaGeometry* R3BCalifaGeometry::inst = NULL;

R3BCalifaGeometry* R3BCalifaGeometry::Instance(Int_t version)
{
    LOG(DEBUG) << "R3BCalifaGeometry::Instance ";
    if (!inst)
        inst = new R3BCalifaGeometry(version);
    else if (inst->fGeometryVersion != version)
    {
        LOG(ERROR)
            << "R3BCalifaGeometry::Instance(): Existing instance with different geometry version than requested. "
            << "Undefined beheaviour possible!";

        inst = new R3BCalifaGeometry(version);
    }
    return inst;
}

R3BCalifaGeometry::R3BCalifaGeometry()
    : R3BCalifaGeometry(2020)
{
}

R3BCalifaGeometry::R3BCalifaGeometry(Int_t version)
    : fGeometryVersion(version)
{
    LOG(DEBUG) << "Creating new R3BCalifaGeometry for version " << version;

    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    geoPath += "/geometry/";

    switch (version)
    {
        case 2019:
            // BARREL: demonstrator from 2019
            geoPath += "califa_s444.geo.root";
            fNumCrystals = 10000;
            break;

        case 2020:
            // Full BARREL+iPhos version
            geoPath += "califa_2020.geo.root";
            fNumCrystals = 4864;
            break;

        default:
            LOG(ERROR) << "R3BCalifaGeometry: Unsupported geometry version: " << version;
            return;
    }

    LOG(INFO) << "R3BCalifaGeometry::Geometry file " << geoPath;

    if (gGeoManager && strcmp(gGeoManager->GetTopVolume()->GetName(), "cave") == 0)
    {
        // Already set up (MC mode)
        LOG(DEBUG) << "R3BCalifaGeometry: Using existing geometry";
        return;
    }

    // Stand alone mode
    LOG(DEBUG) << "R3BCalifaGeometry: Creating new geometry";
    TFile* f = new TFile(geoPath, "READ");
    TGeoVolume* v = dynamic_cast<TGeoVolume*>(f->Get("TOP"));
    if (!v)
    {
        LOG(ERROR) << "R3BCalifaGeometry: Could not open CALIFA geometry file: No TOP volume";
        return;
    }

    v->SetName("cave");
    if (!gGeoManager)
        gGeoManager = new TGeoManager();
    gGeoManager->SetTopVolume(v);
}

R3BCalifaGeometry::~R3BCalifaGeometry() {}

const TVector3& R3BCalifaGeometry::GetAngles(Int_t iD)
{
    static std::map<int, TVector3> cache;
    Double_t local[3] = { 0, 0, 0 };
    Double_t master[3];
    const static TVector3 invalid(NAN, NAN, NAN);
    const char* nameVolume;
    if (cache.count(iD))
        return cache[iD];

    // SOLUTION FOR DOUBLE READING CHANNELS
    if (iD > fNumCrystals / 2 && iD <= fNumCrystals)
        iD = iD - fNumCrystals / 2; // for double reading crystals (crystals from 1 to 2432)

    if (iD >= 1 && iD <= 2432)
    {
        nameVolume = GetCrystalVolumePath(iD);

        gGeoManager->CdTop();

        if (gGeoManager->CheckPath(nameVolume))
            gGeoManager->cd(nameVolume);
        else
        {
            LOG(ERROR) << "R3BCalifaGeometry: Invalid crystal path: " << nameVolume;
            return invalid;
        }
        gGeoManager->LocalToMaster(local, master);
    }
    else
    {
        LOG(ERROR) << "R3BCalifaGeometry: Invalid crystalId: " << iD;
        return invalid;
    }
    return cache[iD] = master;
}

void R3BCalifaGeometry::GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal, Double_t* rho)
{
    auto& masterV = this->GetAngles(iD);
    *polar = masterV.Theta();
    *azimuthal = masterV.Phi();
    *rho = masterV.Mag();
    if (std::isnan(*polar) || std::isnan(*azimuthal) || std::isnan(*rho))
        LOG(ERROR) << "R3BCalifaGeometry::GetAngles(" << iD << ",...) returns NaN";
}

const char* R3BCalifaGeometry::GetCrystalVolumePath(Int_t iD)
{
    Int_t alveolusCopy = -1;
    Int_t alvType = -1;
    Int_t cryType = -1;

    // SOLUTION FOR DOUBLE READING CHANNELS
    if (iD > fNumCrystals / 2 && iD <= fNumCrystals)
        iD = iD - fNumCrystals / 2; // for double reading crystals (crystals from 1 to 2432)

    static char nameVolume[200];

    if (iD >= 1 && iD <= 2432)
    {
        if (iD < 33)
        {                          // First ring in BARREL (single crystal per alveolus)
            alvType = 1;           // Alveolus type 1
            alveolusCopy = iD - 1; // copy from 0 to 31
            cryType = 1;           // Only one crystal type (1)
        }
        else if (iD < 2337)
        {                                                                // All 4-crystals alveoli in BARREL and iPhos
            alvType = (Int_t)((iD - 33) / 128) + 2;                      // Alveolus type (2, ..., 19)
            alveolusCopy = (Int_t)((iD - 33 - (alvType - 2) * 128) / 4); // copy from 0 to 31
            cryType = iD - 33 - (alvType - 2) * 128 - alveolusCopy * 4 + 1; // Four crystal types (1,2,3,4)
        }
        else
        {                                                                     // 3-crystals alveoli in iPhos
            alvType = (Int_t)((iD - 2337) / 24) + 20;                         // Alveolus type (20, 21, 22, 23)
            alveolusCopy = (Int_t)((iD - 2337 - (alvType - 20) * 24) / 3);    // copy from 0 to 7
            cryType = iD - 2337 - (alvType - 20) * 24 - alveolusCopy * 3 + 1; // Three crystal types (1,2,3)
        }
        char name_Alv[23][3] = { "01", "02", "03", "04", "05", "06", "07", "08", "09", "10", "11", "12",
                                 "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23" };
        sprintf(nameVolume,
                "/cave_1/CalifaWorld_0/Alveolus_%s_%i/InnerAlv_%s_0/WrapCry_%s_%i_0/Crystal_%s_%i_0",
                name_Alv[alvType - 1],
                alveolusCopy,
                name_Alv[alvType - 1],
                name_Alv[alvType - 1],
                cryType,
                name_Alv[alvType - 1],
                cryType);
    }
    else
    {
        LOG(ERROR) << "R3BCalifaGeometry: Invalid crystalId: " << iD;
    }

    return nameVolume;
}

double R3BCalifaGeometry::GetDistanceThroughCrystals(TVector3& startVertex,
                                                     TVector3& direction,
                                                     TVector3* hitPos,
                                                     Int_t* numCrystals,
                                                     Int_t* crystalIds)
{
    Int_t maxNumCrystals = 0;

    if (numCrystals != NULL && crystalIds != NULL)
    {
        maxNumCrystals = *numCrystals;
        *numCrystals = 0;
    }

    TGeoNode* n;

    gGeoManager->InitTrack(startVertex.X(),
                           startVertex.Y(),
                           startVertex.Z(),
                           direction.X() / direction.Mag(),
                           direction.Y() / direction.Mag(),
                           direction.Z() / direction.Mag());

    Double_t distance = 0.;
    const Double_t* pos;
    bool inCrystal = false, wasInCrystal = false;
    TString nodeName;

    // find the distance to the next boundary and then extrapolate the current point/direction
    // with this distance making sure that the boundary was crossed. Finally the goal would be to find the next state
    // after crossing the boundary
    while ((n = gGeoManager->FindNextBoundaryAndStep()))
    {
        nodeName = n->GetName();

        if (inCrystal)
            distance += gGeoManager->GetStep();

        // inCrystal = nodeName.BeginsWith("Crystal_");
        inCrystal = nodeName.BeginsWith("WrapCry_"); // can't enter to Crystal volume

        if (inCrystal && maxNumCrystals != 0)
        {
            Int_t cid = GetCrystalId(gGeoManager->GetPath());
            if (cid != -1 && (*numCrystals == 0 || cid != crystalIds[(*numCrystals) - 1]))
            {
                crystalIds[(*numCrystals)++] = cid;
                maxNumCrystals--;
            }
        }

        if (hitPos != NULL && inCrystal && !wasInCrystal)
        {
            pos = gGeoManager->GetCurrentPoint();
            hitPos->SetXYZ(pos[0], pos[1], pos[2]);
            wasInCrystal = true;
        }
    }

    return distance;
}

int R3BCalifaGeometry::GetCrystalId(const char* volumePath)
{
    Int_t crystalId;
    Int_t cryType = 0;
    Int_t alvType = atoi(volumePath + 31);      // converting to int the alveolus type
    Int_t alveolusCopy = atoi(volumePath + 34); // converting to int the alveolus copy
    if (alveolusCopy < 10)
        cryType = atoi(volumePath + 76); // converting to int the crystal type
    else
        cryType = atoi(volumePath + 77); // converting to int the crystal type

    if (cryType < 1 || cryType > 4 || alvType < 1 || alvType > 23)
    { // cryType runs from 1 to 4 while alvType runs from 1 to 23
        LOG(ERROR) << "R3BCalifaGeometry: Wrong crystal numbers (1)";
        cout << "---- cryType: " << cryType << "   alvType: " << alvType << endl;
        return 0;
    }

    if (alvType == 1)
        crystalId = 1 + alveolusCopy; // first alveoli ring, one crystal per alveolus
    else if (alvType < 20)
        crystalId = 33 + (alvType - 2) * 128 + alveolusCopy * 4 + (cryType - 1); // four crystal per alveolus
    else
        crystalId = 2337 + (alvType - 20) * 24 + alveolusCopy * 3 + (cryType - 1); // three crystal per alveolus

    if (crystalId < 1 || crystalId > 3432)
    { // crystalId runs from 1 to 3432
        LOG(ERROR) << "R3BCalifaGeometry: Wrong crystal numbers (2)";
        cout << "---- crystalId: " << crystalId << endl;
        return 0;
    }

    return crystalId;
}

ClassImp(R3BCalifaGeometry);
