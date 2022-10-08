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

#include "R3BAlpideGeometry.h"
#include "R3BLogger.h"

#include <boost/regex.hpp>

R3BAlpideGeometry* R3BAlpideGeometry::Instance()
{
    // Returns singleton instance
    static thread_local R3BAlpideGeometry instance;
    return &instance;
}

R3BAlpideGeometry::R3BAlpideGeometry()
    : TObject()
    , IsInitialize(kFALSE)
    , fNbCyl(2)
    , fNbSensor(363)
{
}

bool R3BAlpideGeometry::Init(Int_t version)
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
        case 2022:
            // Two Barrels
            geoPath += "tracking_alpide_v22.geo.root";
            fNbSensor = 363;
            fNbCyl = 2;
            break;

        default:
            R3BLOG(ERROR, "Unsupported geometry version: " << version);
            return kFALSE;
    }

    if (gGeoManager && strcmp(gGeoManager->GetTopVolume()->GetName(), "cave") == 0)
    {
        // Already set up (MC mode)
        R3BLOG(INFO, "Using existing geometry");
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

R3BAlpideGeometry::~R3BAlpideGeometry()
{
    R3BLOG(DEBUG1, "");
    if (gGeoManager)
        delete gGeoManager;
    if (f)
        f->Close();
}

const TVector3& R3BAlpideGeometry::GetAngles(Int_t iD)
{
    static std::map<int, TVector3> cache;
    Double_t local[3] = { 0, 0, 0 };
    Double_t master[3];
    const static TVector3 invalid(NAN, NAN, NAN);
    const char* nameVolume;
    if (cache.count(iD))
        return cache[iD];

    if (iD >= 1 && iD <= fNbSensor)
    {
        nameVolume = GetSensorVolumePath(iD);

        gGeoManager->CdTop();

        if (gGeoManager->CheckPath(nameVolume))
            gGeoManager->cd(nameVolume);
        else
        {
            R3BLOG(ERROR, "Invalid sensor path: " << nameVolume);
            return invalid;
        }
        gGeoManager->LocalToMaster(local, master);
    }
    else
    {
        R3BLOG(ERROR, "Invalid sensorId: " << iD);
        return invalid;
    }

    return cache[iD] = master;
}

void R3BAlpideGeometry::GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal, Double_t* rho)
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

const char* R3BAlpideGeometry::GetSensorVolumePath(Int_t iD)
{
    static char nameVolume[400];
    Int_t sid = 0;
    Int_t bartype = 0;

    if (iD >= 1 && iD <= fNbSensor)
    {
        if (iD <= 153)
        {
            bartype = 1;
            sid = iD;
        }
        else
        {
            bartype = 2;
            sid = iD - 153;
        }

        sprintf(nameVolume, "/cave_1/VCWorld_0/Cylinder%i_1/Alpide%i_%i", bartype, bartype, sid);
    }
    else
    {
        R3BLOG(ERROR, "Invalid sensorId: " << iD);
    }

    return nameVolume;
}

int R3BAlpideGeometry::GetBarrelId(const char* volumePath)
{
    Int_t barID = 0;
    static auto restr = "Alpide([0-9]+)_([0-9]+)";
    static auto re = boost::regex(restr, boost::regex::extended);
    boost::cmatch m;
    if (!boost::regex_search(volumePath, m, re))
    {
        R3BLOG(ERROR,
               "\"" << volumePath
                    << "\"\n"
                       "does not match RE \""
                    << restr << "\".\n");
        return barID;
    }

    barID = std::stoi(m[1].str()); // converting to int the barrel type

    R3BLOG(DEBUG1, "Barrel ID: " << barID);

    return barID;
}

int R3BAlpideGeometry::GetSensorId(const char* volumePath)
{
    Int_t sensorId = 0;
    static auto restr = "Alpide([0-9]+)_([0-9]+)";
    static auto re = boost::regex(restr, boost::regex::extended);
    boost::cmatch m;
    if (!boost::regex_search(volumePath, m, re))
    {
        R3BLOG(ERROR,
               "\"" << volumePath
                    << "\"\n"
                       "does not match RE \""
                    << restr << "\".\n");
        return sensorId;
    }

    Int_t bartype = std::stoi(m[1].str()); // converting to int the barrel type
    Int_t sid = std::stoi(m[2].str());     // converting to int the sensor ID

    if (bartype == 2)
    {
        sensorId = sid;
        sensorId += 153;
    }
    else
    {
        sensorId = sid;
    }

    R3BLOG(DEBUG1, "Barrel: " << bartype << ", sensorID in barrel: " << sid << ", sensorID: " << sensorId);

    return sensorId;
}

ClassImp(R3BAlpideGeometry);
