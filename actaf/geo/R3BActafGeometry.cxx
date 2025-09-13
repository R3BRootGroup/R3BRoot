/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
#include <TRotation.h>
#include <TString.h>
#include <TSystem.h>
#include <TVector3.h>
#include <iostream>

#include "R3BActafGeometry.h"
#include "R3BLogger.h"

#include <boost/regex.hpp>

R3BActafGeometry* R3BActafGeometry::Instance()
{
    // Returns singleton instance
    static thread_local R3BActafGeometry instance;
    return &instance;
}

bool R3BActafGeometry::Init(int version)
{
    if (!IsInitialize)
        IsInitialize = kTRUE;
    else
        return kTRUE;

    R3BLOG(info, "");
    TString geoPath = gSystem->Getenv("VMCWORKDIR");
    geoPath += "/geometry/";

    switch (version)
    {
        case 2025:
            geoPath += "actaf_v25.geo.root";
            fNbPads = 128;
            fGeometryVersion = version;
            break;

        default:
            R3BLOG(error, "Unsupported geometry version: " << version);
            return kFALSE;
    }

    if (gGeoManager && strcmp(gGeoManager->GetTopVolume()->GetName(), "cave") == 0)
    {
        // Already set up (MC mode)
        R3BLOG(info, "Using existing geometry");
        fIsSimulation = kTRUE;
        return kTRUE;
    }

    // Stand alone mode
    R3BLOG(info, "Open geometry file " << geoPath << " for analysis.");
    f = new TFile(geoPath, "READ");
    TGeoVolume* v = dynamic_cast<TGeoVolume*>(f->Get("TOP"));
    if (!v)
    {
        R3BLOG(error, "Could not open geometry file, No TOP volume");
        return kFALSE;
    }

    v->SetName("cave");
    if (!gGeoManager)
        gGeoManager = new TGeoManager();
    gGeoManager->SetTopVolume(v);
    fIsSimulation = kFALSE;
    return kTRUE;
}

R3BActafGeometry::~R3BActafGeometry()
{
    R3BLOG(debug, "");
    if (gGeoManager)
        delete gGeoManager;
    if (f)
    {
        f->Close();
    }
}

std::string R3BActafGeometry::GetPadVolumePath(const int iD)
{
    int padtype = iD;
    int ringtype = 0;
    std::ostringstream oss;

    R3BLOG(debug, "PadId: " << iD);

    if (iD >= 1 && iD <= fNbPads)
    {
        ringtype = this->GetRingId(iD);
        oss << "/cave_1/ActafWorld_0/Section_1/Ring" << ringtype << "_Pad" << padtype << "_0";
    }
    else
    {
        R3BLOG(error, "Invalid padId: " << iD);
    }
    return oss.str();
}

const TVector3& R3BActafGeometry::GetPosition(int iD)
{
    static std::map<int, TVector3> cache;
    Double_t local[3] = { 0, 0, 0 };
    Double_t master[3];
    const static TVector3 invalid(NAN, NAN, NAN);
    if (cache.count(iD))
    {
        return cache[iD];
    }

    if (iD >= 1 && iD <= fNbPads)
    {
        std::string nameVolume = GetPadVolumePath(iD);
        gGeoManager->CdTop();

        if (gGeoManager->CheckPath(nameVolume.c_str()))
            gGeoManager->cd(nameVolume.c_str());
        else
        {
            R3BLOG(error, "Invalid pad path: " << nameVolume);
            return invalid;
        }
        gGeoManager->LocalToMaster(local, master);
    }
    else if (iD == 129)
    {
        R3BLOG(debug, "Pad for Syn-signal: " << iD);
    }
    else
    {
        R3BLOG(error, "Invalid padId: " << iD);
        return invalid;
    }

    cache[iD] = master;

    return cache[iD];
}

int R3BActafGeometry::GetRingId(int iD)
{
    R3BLOG(debug, "PadId: " << iD);
    int ringtype = 0;

    if (iD >= 1 && iD <= fNbPads)
    {
        if (fGeometryVersion == 2025)
        {
            if (iD > 64)
                iD = iD - 64;

            if (iD <= 13)
            {
                ringtype = 1;
            }
            else if (iD <= 21)
            {
                ringtype = 2;
            }
            else if (iD <= 29)
            {
                ringtype = 3;
            }
            else if (iD <= 37)
            {
                ringtype = 4;
            }
            else if (iD <= 45)
            {
                ringtype = 5;
            }
            else if (iD <= 51)
            {
                ringtype = 6;
            }
            else if (iD <= 58)
            {
                ringtype = 7;
            }
            else if (iD <= 64)
            {
                ringtype = 8;
            }
        }
    }
    else
    {
        R3BLOG(error, "Invalid padId: " << iD);
    }
    return ringtype;
}

int R3BActafGeometry::GetRingId(const std::string volumePath)
{
    int padtype = 0;
    int ringtype = 0;

    static const std::string restr = "Ring([0-9]+)_Pad([0-9]+)_([0-9]+)";
    static const boost::regex re(restr, boost::regex::extended);
    // NOLINTBEGIN
    boost::smatch m;
    if (!boost::regex_search(volumePath, m, re))
    {
        R3BLOG(error,
               "\"" << volumePath
                    << "\"\n"
                       "does not match RE \""
                    << restr << "\".\n");
        return 0;
    }
    // NOLINTEND

    ringtype = std::stoi(m[1].str());
    padtype = std::stoi(m[2].str());

    R3BLOG(debug, "Ring ID: " << ringtype << ", padID: " << padtype);

    return ringtype;
}

int R3BActafGeometry::GetPadId(const std::string volumePath)
{
    int padtype = 0;
    int ringtype = 0;

    static const std::string restr = "Ring([0-9]+)_Pad([0-9]+)_([0-9]+)";
    static const boost::regex re(restr, boost::regex::extended);
    // NOLINTBEGIN
    boost::smatch m;
    if (!boost::regex_search(volumePath, m, re))
    {
        R3BLOG(error,
               "\"" << volumePath
                    << "\"\n"
                       "does not match RE \""
                    << restr << "\".\n");
        return 0;
    }
    // NOLINTEND

    ringtype = std::stoi(m[1].str());
    padtype = std::stoi(m[2].str());

    R3BLOG(debug, "Ring ID: " << ringtype << ", padID: " << padtype);

    return padtype;
}

ClassImp(R3BActafGeometry)
