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
#include <iostream>
#include <stdlib.h>
#include <vector>

#include <FairLogger.h>

#include "R3BCalifaGeometry.h"
#include "R3BLogger.h"

#include <boost/regex.hpp>

R3BCalifaGeometry* R3BCalifaGeometry::Instance()
{
    // Returns singleton instance
    static thread_local R3BCalifaGeometry instance;
    return &instance;
}

R3BCalifaGeometry::R3BCalifaGeometry()
    : TObject()
    , IsInitialize(kFALSE)
    , fNumCrystals(4864)
{
}

bool R3BCalifaGeometry::Init(Int_t version)
{
    if (!IsInitialize)
        IsInitialize = kTRUE;
    else
        return kTRUE;

    R3BLOG(INFO, "");
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

        case 2021:
            // s455 Experiment: Half Barrel + Full IPHOS
            geoPath += "califa_2021_s455.geo.root";
            fNumCrystals = 4864;
            break;

        default:
            R3BLOG(ERROR, "Unsupported geometry version: " << version);
            return kFALSE;
    }

    if (gGeoManager && strcmp(gGeoManager->GetTopVolume()->GetName(), "cave") == 0)
    {
        // Already set up (MC mode)
        R3BLOG(INFO, "Using existing geometry");
        // LOG(INFO) << "R3BCalifaGeometry::Opened geometry file " << geoPath;
        fIsSimulation = kTRUE;
        return kTRUE;
    }

    // Stand alone mode
    R3BLOG(INFO, "Open geometry file " << geoPath << " for analysis.");
    f = new TFile(geoPath, "READ");
    TGeoVolume* v = dynamic_cast<TGeoVolume*>(f->Get("TOP"));
    if (!v)
    {
        R3BLOG(ERROR, "Could not open geometry file, No TOP volume");
        return kFALSE;
    }

    v->SetName("cave");
    if (!gGeoManager)
        gGeoManager = new TGeoManager();
    gGeoManager->SetTopVolume(v);
    fIsSimulation = kFALSE;
    return kTRUE;
}

R3BCalifaGeometry::~R3BCalifaGeometry()
{
    R3BLOG(DEBUG1, "");
    if (gGeoManager)
        delete gGeoManager;
    if (f)
        f->Close();
}

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

    if (iD >= 1 && iD <= fNumCrystals / 2)
    {
        nameVolume = GetCrystalVolumePath(iD);

        gGeoManager->CdTop();

        if (gGeoManager->CheckPath(nameVolume))
            gGeoManager->cd(nameVolume);
        else
        {
            R3BLOG(ERROR, "Invalid crystal path: " << nameVolume);
            return invalid;
        }
        gGeoManager->LocalToMaster(local, master);
    }
    else
    {
        R3BLOG(ERROR, "Invalid crystalId: " << iD);
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
    {
        R3BLOG(ERROR, " returns NaN");
    }
}

const char* R3BCalifaGeometry::GetCrystalVolumePath(Int_t iD)
{
    Int_t alveolusCopy = -1;
    Int_t alvType = -1;
    Int_t cryType = -1;

    // SOLUTION FOR DOUBLE READING CHANNELS
    if (iD > fNumCrystals / 2 && iD <= fNumCrystals)
        iD = iD - fNumCrystals / 2; // for double reading crystals (crystals from 1 to 2432)

    static char nameVolume[400];

    if (iD >= 1 && iD <= fNumCrystals / 2)
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
        R3BLOG(ERROR, "Invalid crystalId: " << iD);
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

// ROOT (or pyroot) is unable to automatically load dependencies of so's for whatever reason.
// so we try to force loading the lib during var initialisation.
// yes, this is ugly.
// static bool _dummy_load_libboost_regex = []() {
//    char buf[100];
//    snprintf(buf, 100, "%s/lib/libboost_regex.so", getenv("SIMPATH"));
//    return gSystem->Load(buf);
//}();

int R3BCalifaGeometry::GetCrystalId(const char* volumePath)
{ /*
   * Yes, regex matching in the stepping action is probably slow.
   * But if we cared about speed, we would not use string processing here
   * in the first place.
   * At least, this way we have well defined behaviour if something fails,
   * instead of possible out of bounds access consequences (nasal demons etc).
   * Also, it hopefully works with the 2019/s444 geo file too, now.
   */
    static auto restr = "Alveolus_([0-9]+)_([0-9]+).*Crystal_[^_]+_([0-9]+)_";
    static auto re = boost::regex(restr, boost::regex::extended);
    boost::cmatch m;
    if (!boost::regex_search(volumePath, m, re))
    {
        R3BLOG(ERROR,
               "\"" << volumePath
                    << "\"\n"
                       "does not match RE \""
                    << restr << "\".\n");
        return 0;
    }

    Int_t crystalId;
    Int_t alvType = std::stoi(m[1].str());      // converting to int the alveolus type
    Int_t alveolusCopy = std::stoi(m[2].str()); // converting to int the alveolus copy
    Int_t cryType = std::stoi(m[3].str());      // converting to int the crystal type

    if (cryType < 1 || cryType > 4 || alvType < 1 || alvType > 23)
    { // cryType runs from 1 to 4 while alvType runs from 1 to 23
        R3BLOG(ERROR, "Wrong crystal numbers (1)");
        LOG(INFO) << "---- cryType: " << cryType << "   alvType: " << alvType;
        LOG(INFO) << "path=" << volumePath;
        return 0;
    }

    if (alvType == 1)
        crystalId = 1 + alveolusCopy; // first alveoli ring, one crystal per alveolus
    else if (alvType < 20)
        crystalId = 33 + (alvType - 2) * 128 + alveolusCopy * 4 + (cryType - 1); // four crystal per alveolus
    else
        crystalId = 2337 + (alvType - 20) * 24 + alveolusCopy * 3 + (cryType - 1); // three crystal per alveolus

    if (crystalId < 1 || crystalId > 2432)
    { // crystalId runs from 1 to 2432
        R3BLOG(ERROR, "Wrong crystal numbers (2)");
        LOG(INFO) << "---- crystalId: " << crystalId;
        return 0;
    }

    return crystalId;
}

ClassImp(R3BCalifaGeometry);
