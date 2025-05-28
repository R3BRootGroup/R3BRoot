/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------
// -----               R3BAlpideCal2Hit                    -----
// -----     Created 22/03/22 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>
#include <TRandom.h>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// ALPIDE headers
#include "R3BAlpideCal2Hit.h"
#include "R3BAlpideCalData.h"
#include "R3BAlpideCluster.h"
#include "R3BAlpideGeometry.h"
#include "R3BAlpideHitData.h"
#include "R3BAlpideMappingPar.h"
#include "R3BLogger.h"
#include "R3BTGeoPar.h"

// R3BAlpideCal2Hit::Default Constructor --------------------------
R3BAlpideCal2Hit::R3BAlpideCal2Hit()
    : R3BAlpideCal2Hit("R3BAlpideCal2Hit", 1)
{
}

// R3BAlpideCal2Hit::Standard Constructor --------------------------
R3BAlpideCal2Hit::R3BAlpideCal2Hit(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
    , fAlpideCluster(new TClonesArray("R3BAlpideCluster"))
{
    fTargetPos.SetXYZ(0., 0., 0.);
    fAlpidePos.SetXYZ(0., 0., 0.);
    fAlpidetoTargetPos.SetXYZ(0., 0., 0.);
}

// Virtual R3BAlpideCal2Hit::Destructor
R3BAlpideCal2Hit::~R3BAlpideCal2Hit()
{
    R3BLOG(debug1, "");
    if (fAlpideCluster)
    {
        delete fAlpideCluster;
    }

    if (fAlpideHitData)
    {
        delete fAlpideHitData;
    }
}

void R3BAlpideCal2Hit::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BAlpideMappingPar*>(rtdb->getContainer("alpideMappingPar"));
    R3BLOG_IF(fatal, !fMap_Par, "Container alpideMappingPar not found");

    fAlpideGeoPar = dynamic_cast<R3BTGeoPar*>(rtdb->getContainer("AlpideGeoPar"));
    fTargetGeoPar = dynamic_cast<R3BTGeoPar*>(rtdb->getContainer("TargetGeoPar"));
    if (!fAlpideGeoPar || !fTargetGeoPar)
    {
        R3BLOG_IF(warning, !fAlpideGeoPar, "Could not get access to AlpideGeoPar container.");
        R3BLOG_IF(warning, !fTargetGeoPar, "Could not get access to TargetGeoPar container.");
        return;
    }
    R3BLOG(info, "Container AlpideGeoPar found.");
    R3BLOG(info, "Container TargetGeoPar found.");

    return;
}

void R3BAlpideCal2Hit::SetParameter()
{
    //--- Parameter Container ---
    fGeoversion = fMap_Par->GetGeoVersion();
    R3BLOG(info, "Geometry version: " << fGeoversion);
    fNbSensors = fMap_Par->GetNbSensors();
    R3BLOG(info, "Nb of sensors: " << fNbSensors);

    if (fAlpideGeoPar && fTargetGeoPar)
    {
        fTargetPos.SetXYZ(fTargetGeoPar->GetPosX(), fTargetGeoPar->GetPosY(), fTargetGeoPar->GetPosZ());
        fAlpidePos.SetXYZ(fAlpideGeoPar->GetPosX(), fAlpideGeoPar->GetPosY(), fAlpideGeoPar->GetPosZ());
    }
    return;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAlpideCal2Hit::Init()
{
    R3BLOG(info, "");
    auto* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    // INPUT DATA
    fAlpideCalData = dynamic_cast<TClonesArray*>(mgr->GetObject("AlpideCalData"));
    if (!fAlpideCalData)
    {
        R3BLOG(fatal, "AlpideCalData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    fAlpideHitData = new TClonesArray("R3BAlpideHitData");
    mgr->Register("AlpideHitData", "ALPIDE_Hit", fAlpideHitData, !fOnline);
    Reset();

    SetParameter();

    if (fGeoversion == 202402)
    {
        fAlpideGeo = R3BAlpideGeometry::Instance();
        R3BLOG_IF(warn, !fAlpideGeo->Init(fGeoversion), "Alpide geometry " << fGeoversion << " not found");
    }
    fAlpidetoTargetPos = fAlpidePos - fTargetPos;

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BAlpideCal2Hit::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BAlpideCal2Hit::Exec(Option_t*)
{
    R3BLOG(debug, "New event");
    // Reset entries in the output arrays
    Reset();

    UInt_t nbcluster = 0;
    bool shouldround = false;

    // Reading the Input -- Cal Data --
    Int_t nHits = fAlpideCalData->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    auto fAlpidePixel = dynamic_cast<TClonesArray*>(fAlpideCalData->Clone());

nextcluster:
    nbcluster++;
    bool first = true;

nextround:
    shouldround = false;

    for (Int_t i = 0; i < fAlpidePixel->GetEntriesFast(); i++)
    {
        auto calData = dynamic_cast<R3BAlpideCalData*>(fAlpidePixel->At(i));
        auto sen = calData->GetSensorId();
        auto col = calData->GetCol();
        auto row = calData->GetRow();

        if (i == 0 && first)
        {
            new ((*fAlpideCluster)[fAlpideCluster->GetEntriesFast()]) R3BAlpideCluster(sen, nbcluster, col, row);
            fAlpidePixel->RemoveAt(i);
            fAlpidePixel->Compress();
            i--;
            first = false;
        }
        else
        {
            auto cHits = fAlpideCluster->GetEntriesFast();
            auto cluster = new R3BAlpideCluster*[cHits];
            for (Int_t j = 0; j < cHits; j++)
            {
                cluster[j] = dynamic_cast<R3BAlpideCluster*>(fAlpideCluster->At(j));
                if (((std::abs(cluster[j]->GetCol() - col) == 1 && std::abs(cluster[j]->GetRow() - row) == 0) ||
                     (std::abs(cluster[j]->GetRow() - row) == 1 && std::abs(cluster[j]->GetCol() - col) == 0) ||
                     (std::abs(cluster[j]->GetRow() - row) == 1 && std::abs(cluster[j]->GetCol() - col) == 1)) &&
                    sen == cluster[j]->GetSensorId())
                {
                    new ((*fAlpideCluster)[fAlpideCluster->GetEntriesFast()])
                        R3BAlpideCluster(sen, nbcluster, col, row);

                    shouldround = true;
                    fAlpidePixel->RemoveAt(i);
                    fAlpidePixel->Compress();
                    i--;
                    break;
                }
            }
            if (cluster)
            {
                delete[] cluster;
            }
        }
    }

    if (shouldround)
    {
        R3BLOG(debug, "Should be next round");
        goto nextround;
    }
    if (fAlpideCluster->GetEntriesFast() < nHits)
    {
        R3BLOG(debug, "Going for the next cluster");
        goto nextcluster;
    }

    FindClusters();

    delete fAlpidePixel;
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BAlpideCal2Hit::Reset()
{
    R3BLOG(debug, "Clearing Data Structure");
    if (fAlpideHitData)
    {
        fAlpideHitData->Clear();
    }
    if (fAlpideCluster)
    {
        fAlpideCluster->Clear();
    }
}

// -----   Private method FindClusters   -----------------------------------------
void R3BAlpideCal2Hit::FindClusters()
{
    R3BLOG(debug, "");
    UInt_t nbcluster = 0;

    auto nHits = fAlpideCluster->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    uint16_t mult[fNbSensors][nHits];  // NOLINT
    double meancol[fNbSensors][nHits]; // NOLINT
    double meanrow[fNbSensors][nHits]; // NOLINT
    for (size_t s = 0; s < fNbSensors; s++)
        for (size_t i = 0; i < nHits; i++)
        {
            mult[s][i] = 0;
            meancol[s][i] = 0.;
            meanrow[s][i] = 0.;
        }

    for (size_t i = 0; i < nHits; i++)
    {
        auto cluster = dynamic_cast<R3BAlpideCluster*>(fAlpideCluster->At(i));
        auto clid = cluster->GetClusterId() - 1;
        auto senid = cluster->GetSensorId() - 1;
        mult[senid][clid]++;
        meancol[senid][clid] += (double)cluster->GetCol();
        meanrow[senid][clid] += (double)cluster->GetRow();
    }

    if (fGeoversion == 202402)
    {
        for (size_t s = 0; s < fNbSensors; s++)
            for (size_t i = 0; i < nHits; i++)
                if (mult[s][i] > 0)
                {
                    nbcluster++;

                    fRot = fAlpideGeo->GetRotation(s + 1);
                    fTrans = fAlpideGeo->GetTranslation(s + 1);

                    TVector3 localpos;
                    localpos.SetXYZ(-meancol[s][i] / double(mult[s][i]) * fPixelSize_ls + 30. / 2.0,
                                    meanrow[s][i] / double(mult[s][i]) * fPixelSize_ss - 13.8 / 2.0,
                                    0.0);
                    // Lab frame
                    TVector3 labpos = fRot * localpos + fTrans * 10.; // 10 because fTrans is in mm
                    AddHitData(s + 1, mult[s][i], labpos.X(), labpos.Y(), labpos.Z(), localpos.X(), localpos.Y());
                }
    }
    else if (fGeoversion == 202505)
    {
        for (size_t s = 0; s < fNbSensors; s++)
            for (size_t i = 0; i < nHits; i++)
                if (mult[s][i] > 0)
                {
                    nbcluster++;

                    TVector3 localpos;
                    localpos.SetXYZ(meancol[s][i] / double(mult[s][i]) * fPixelSize_ls,
                                    meanrow[s][i] / double(mult[s][i]) * fPixelSize_ss,
                                    0.0);

                    TVector3 labpos;
                    if (s < 3)
                        labpos.SetXYZ(45. - localpos.X() - 30. * s, -45. - localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (s < 6)
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (s - 3), -45. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (s < 9)
                        labpos.SetXYZ(45. - localpos.X() - 30. * (s - 6), -15. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (s < 12)
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (s - 9), -15. - localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (s < 15)
                        labpos.SetXYZ(45. - localpos.X() - 30. * (s - 12), 15. - localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (s < 18)
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (s - 15), 15. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (s < 21)
                        labpos.SetXYZ(45. - localpos.X() - 30. * (s - 18), 45. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (s - 21), 45. - localpos.Y(), fAlpidetoTargetPos.Z());

                    AddHitData(s + 1, mult[s][i], labpos.X(), labpos.Y(), labpos.Z(), localpos.X(), localpos.Y());
                }
    }
    else if (fGeoversion == 202506)
    {
        for (size_t s = 0; s < fNbSensors; s++)
            for (size_t i = 0; i < nHits; i++)
                if (mult[s][i] > 0)
                {
                    nbcluster++;

                    TVector3 localpos;
                    localpos.SetXYZ(meancol[s][i] / double(mult[s][i]) * fPixelSize_ls,
                                    meanrow[s][i] / double(mult[s][i]) * fPixelSize_ss,
                                    0.0);

                    TVector3 labpos;
                    if (s < 3)
                        labpos.SetXYZ(45. - localpos.X() - 30. * s, -localpos.Y(), 0.0);
                    else if (s < 6)
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (s - 3), localpos.Y(), 0.0);
                    else if (s < 9)
                        labpos.SetXYZ(45. - localpos.X() - 30. * (s - 6), -localpos.Y(), 73.);
                    else
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (s - 9), localpos.Y(), 73.);

                    AddHitData(s + 1, mult[s][i], labpos.X(), labpos.Y(), labpos.Z(), localpos.X(), localpos.Y());
                }
    }

    R3BLOG(debug, "Number of clusters: " << nbcluster);
    return;
}

// -----   Private method AddHitData  --------------------------------------------
R3BAlpideHitData* R3BAlpideCal2Hit::AddHitData(uint16_t senId,
                                               uint16_t clustersize,
                                               double xpos,
                                               double ypos,
                                               double zpos,
                                               double locxpos,
                                               double locypos)
{
    // It fills the R3BAlpideHitData
    TClonesArray& clref = *fAlpideHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BAlpideHitData(senId, clustersize, xpos, ypos, zpos, locxpos, locypos);
}

ClassImp(R3BAlpideCal2Hit)
