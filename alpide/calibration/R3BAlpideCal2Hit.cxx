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

// C++ headers
#include <set>

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

    // Call clustering algorithm
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

// -----   Dispatcher for clustering algorithms   -------------------------------
void R3BAlpideCal2Hit::FindClusters()
{
    if (fAlgorithm == "Default")
    {
        FindClustersDefault();
    }
    else if (fAlgorithm == "FloodFill")
    {
        FindClustersFloodFill();
    }
    else
    {
        R3BLOG(warning, "Unknown clustering algorithm '" << fAlgorithm << "'. Falling back to Default.");
        FindClustersDefault();
    }
}

// -----   Default clustering implementation (existing code) -------------------
void R3BAlpideCal2Hit::FindClustersDefault()
{
    R3BLOG(debug, "Running Default clustering algorithm");
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
    TVector3 nullMajor; // Null for old algorithm
    std::vector<double> nullHu;
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
                    AddHitData(s + 1,
                               mult[s][i],
                               0,
                               0,
                               nullMajor,
                               0,
                               0,
                               nullHu,
                               labpos.X(),
                               labpos.Y(),
                               labpos.Z(),
                               localpos.X(),
                               localpos.Y());
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

                    AddHitData(s + 1,
                               mult[s][i],
                               0,
                               0,
                               nullMajor,
                               0,
                               0,
                               nullHu,
                               labpos.X(),
                               labpos.Y(),
                               labpos.Z(),
                               localpos.X(),
                               localpos.Y());
                }
    }
    else if (fGeoversion == 202506)
    {
        const double Z_alpide1 = -1732.88;
        const double Z_alpide2 = -1659.88;

        const double z1_off = -0.837843;
        const double z2_off = -2.69759;

        const double rx = 0.0225419;
        const double ry = 0.0137131;
        const double rz[12] = { 0.00595207, 0.00815014, 0.0102531, 0.00857714,  0.00909736, 0.0015223,
                                0.0133541,  0.00969408, 0.0134827, 0.000125173, 0.00982907, 0.021181 };

        array<TVector3, 12> pos_offset = {
            TVector3(0.231386, 0.18477, z1_off),      TVector3(0.03624, 0.0145723, z1_off),
            TVector3(-0.162212, -0.217339, z1_off),   TVector3(-0.157992, -0.31182, z1_off),
            TVector3(0.0596373, -0.07224198, z1_off), TVector3(0.2458, 0.131979, z1_off),
            TVector3(0.713162, 0.203712, z2_off),     TVector3(0.5620141, -0.18166828, z2_off),
            TVector3(0.316365, -0.445912, z2_off),    TVector3(0.323927, -0.298097, z2_off),
            TVector3(0.5504176, -0.2855743, z2_off),  TVector3(0.781553, -0.0369948, z2_off)
        };

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

                    int sensorId = s;

                    TRotation Rloc;
                    Rloc.RotateZ(rz[sensorId]);
                    TVector3 local_rot = Rloc * localpos;

                    if (s < 3)
                        labpos.SetXYZ(45. - local_rot.X() - 30. * s, -local_rot.Y(), Z_alpide1);
                    else if (s < 6)
                        labpos.SetXYZ(-45. + local_rot.X() + 30. * (s - 3), local_rot.Y(), Z_alpide1);
                    else if (s < 9)
                        labpos.SetXYZ(45. - local_rot.X() - 30. * (s - 6), -local_rot.Y(), Z_alpide2);
                    else
                        labpos.SetXYZ(-45. + local_rot.X() + 30. * (s - 9), local_rot.Y(), Z_alpide2);

                    TVector3 center;
                    if (sensorId < 6)
                        center.SetXYZ(0., 0., Z_alpide1);
                    else
                        center.SetXYZ(0., 0., Z_alpide2);

                    TRotation R;
                    R.RotateX(rx);
                    R.RotateY(ry);

                    labpos = R * (labpos - center) + center;
                    labpos += pos_offset[sensorId];

                    AddHitData(s + 1,
                               mult[s][i],
                               0,
                               0,
                               nullMajor,
                               0,
                               0,
                               nullHu,
                               labpos.X(),
                               labpos.Y(),
                               labpos.Z(),
                               localpos.X(),
                               localpos.Y());
                }
    }

    R3BLOG(debug, "Number of clusters: " << nbcluster);
    return;
}

// -----   Alternatvie methods: Flood fill  ---------------------------
void R3BAlpideCal2Hit::FindClustersFloodFill()
{
    Int_t nHits = fAlpideCluster->GetEntriesFast();
    if (nHits == 0)
        return;

    std::map<Int_t, std::vector<std::pair<Int_t, Int_t>>> sensorPixels;
    for (Int_t i = 0; i < nHits; i++)
    {
        auto cluster = dynamic_cast<R3BAlpideCluster*>(fAlpideCluster->At(i));
        Int_t sen = cluster->GetSensorId() - 1;
        sensorPixels[sen].emplace_back(cluster->GetCol(), cluster->GetRow());
    }

    for (auto& sp : sensorPixels)
    {
        Int_t senId = sp.first;
        auto& pixels = sp.second;

        // Determine bounds with padding
        Int_t minCol = 522, maxCol = -1, minRow = 1034, maxRow = -1;
        for (auto& p : pixels)
        {
            minCol = std::min(minCol, p.first);
            maxCol = std::max(maxCol, p.first);
            minRow = std::min(minRow, p.second);
            maxRow = std::max(maxRow, p.second);
        }
        Int_t nCols = maxCol - minCol + 3;
        Int_t nRows = maxRow - minRow + 3;

        std::vector<std::vector<bool>> map(nCols, std::vector<bool>(nRows, false));
        std::vector<std::vector<bool>> visited(nCols, std::vector<bool>(nRows, false));
        for (auto& p : pixels)
            map[p.first - minCol + 1][p.second - minRow + 1] = true;

        // Flood-fill function
        std::function<void(Int_t, Int_t, std::vector<std::pair<Int_t, Int_t>>&)> fill;
        fill = [&](Int_t x, Int_t y, std::vector<std::pair<Int_t, Int_t>>& clusterPixels)
        {
            if (x < 0 || x >= nCols || y < 0 || y >= nRows)
                return;
            if (!map[x][y] || visited[x][y])
                return;
            visited[x][y] = true;
            clusterPixels.emplace_back(x + minCol - 1, y + minRow - 1);
            for (Int_t dx = -1; dx <= 1; dx++)
                for (Int_t dy = -1; dy <= 1; dy++)
                    if (dx != 0 || dy != 0)
                        fill(x + dx, y + dy, clusterPixels);
        };

        // Process all pixels
        for (Int_t x = 0; x < nCols; x++)
        {
            for (Int_t y = 0; y < nRows; y++)
            {
                if (!map[x][y] || visited[x][y])
                    continue;

                std::vector<std::pair<Int_t, Int_t>> clusterPixels;
                fill(x, y, clusterPixels);

                // ================== Hole Detection ==================
                // Set maximum size to consider->
                Int_t cmin = 9999, cmax = -1, rmin = 9999, rmax = -1;
                for (auto& pix : clusterPixels)
                {
                    cmin = std::min(cmin, pix.first);
                    cmax = std::max(cmax, pix.first);
                    rmin = std::min(rmin, pix.second);
                    rmax = std::max(rmax, pix.second);
                }
                Int_t nC = cmax - cmin + 3;
                Int_t nR = rmax - rmin + 3;
                std::vector<std::vector<int>> localMap(nC, std::vector<int>(nR, 0));
                for (auto& pix : clusterPixels)
                    localMap[pix.first - cmin + 1][pix.second - rmin + 1] = 1;

                std::vector<std::vector<bool>> visitedBkg(nC, std::vector<bool>(nR, false));
                std::function<void(int, int)> floodBkg = [&](int cx, int cy)
                {
                    if (cx < 0 || cx >= nC || cy < 0 || cy >= nR)
                        return;
                    if (visitedBkg[cx][cy] || localMap[cx][cy] == 1)
                        return;
                    visitedBkg[cx][cy] = true;
                    for (int dx = -1; dx <= 1; dx++)
                        for (int dy = -1; dy <= 1; dy++)
                            if (!(dx == 0 && dy == 0))
                                floodBkg(cx + dx, cy + dy);
                };
                for (int cx = 0; cx < nC; cx++)
                {
                    floodBkg(cx, 0);
                    floodBkg(cx, nR - 1);
                }
                for (int cy = 0; cy < nR; cy++)
                {
                    floodBkg(0, cy);
                    floodBkg(nC - 1, cy);
                }

                int holeCount = 0, holePixels = 0;
                std::vector<std::pair<int, int>> holePixelsVec;
                for (int cx = 0; cx < nC; cx++)
                {
                    for (int cy = 0; cy < nR; cy++)
                    {
                        if (localMap[cx][cy] == 0 && !visitedBkg[cx][cy])
                        {
                            holeCount++;
                            int holeSize = 0;
                            std::function<void(int, int)> fillHole = [&](int hx, int hy)
                            {
                                if (hx < 0 || hx >= nC || hy < 0 || hy >= nR)
                                    return;
                                if (visitedBkg[hx][hy] || localMap[hx][hy] == 1)
                                    return;
                                visitedBkg[hx][hy] = true;
                                holeSize++;
                                holePixelsVec.emplace_back(hx + cmin - 1, hy + rmin - 1);
                                for (int dx = -1; dx <= 1; dx++)
                                    for (int dy = -1; dy <= 1; dy++)
                                        if (!(dx == 0 && dy == 0))
                                            fillHole(hx + dx, hy + dy);
                            };
                            fillHole(cx, cy);
                            holePixels += holeSize;
                        }
                    }
                }
                // Consider hole
                int effectiveClusterSize = clusterPixels.size() - holePixels;
                // bool hasHole = (holeCount>0);

                // ================== Centroid, Moments, Shape Analysis ==================
                auto isHole = [&](const std::pair<int, int>& p)
                { return std::find(holePixelsVec.begin(), holePixelsVec.end(), p) != holePixelsVec.end(); };

                // ================== First pass centroid ==============
                double sumCol = 0.0, sumRow = 0.0;
                size_t pixelsInCentroid = 0;
                for (const auto& pix : clusterPixels)
                {
                    if (isHole(pix))
                        continue;
                    sumCol += pix.first;
                    sumRow += pix.second;
                    ++pixelsInCentroid;
                }
                if (pixelsInCentroid == 0)
                {
                    throw std::runtime_error("Cluster has no effective pixels (all holes?)");
                }
                double meanCol = sumCol / double(pixelsInCentroid);
                double meanRow = sumRow / double(pixelsInCentroid);

                //============Second pass for beyond 1st moments============
                double mu20 = 0, mu02 = 0, mu11 = 0;
                double mu30 = 0, mu03 = 0, mu21 = 0, mu12 = 0;
                for (const auto& pix : clusterPixels)
                {
                    if (isHole(pix))
                        continue;
                    double dx = double(pix.first) - meanCol;
                    double dy = double(pix.second) - meanRow;

                    double dx2 = dx * dx, dy2 = dy * dy;

                    // 2nd order
                    mu20 += dx2;
                    mu02 += dy2;
                    mu11 += dx * dy;

                    // 3rd order
                    mu30 += dx * dx2; // dx^3
                    mu03 += dy * dy2; // dy^3
                    mu21 += dx2 * dy; // dx^2*dy
                    mu12 += dx * dy2; // dx*dy^2
                }

                //===========Covariance matrix elements for second moment==============
                double Cxx = mu20 / double(pixelsInCentroid);
                double Cyy = mu02 / double(pixelsInCentroid);
                double Cxy = mu11 / double(pixelsInCentroid);

                //========== 1st moments ===========
                // double sigmaX = std::sqrt(std::max(0.0, Cxx));
                // double sigmaY = std::sqrt(std::max(0.0, Cyy));

                //==========Eigen vectors of Second moment ===========
                double trace = Cxx + Cyy;
                double det = Cxx * Cyy - Cxy * Cxy;
                double disc = std::max(0.0, trace * trace - 4.0 * det);

                double l1 = 0.5 * (trace + std::sqrt(disc)); // >=
                double l2 = 0.5 * (trace - std::sqrt(disc)); // <=

                double majorSigma = std::sqrt(std::max(0.0, l1));
                double minorSigma = std::sqrt(std::max(0.0, l2));
                double elongation =
                    (minorSigma > 0.0) ? (majorSigma / minorSigma) : std::numeric_limits<double>::infinity();

                //========== Major axis lengths ==========
                // double MajorL = 2.0 * majorSigma;
                // double MinorL = 2.0 * minorSigma;

                //========= Major axis angle ==========
                double majorAngle = 0.5 * std::atan2(2.0 * Cxy, Cxx - Cyy);

                //==========Eccentricity=========
                // double eccentricity =
                //    (MinorL > 0.0) ? std::sqrt(1.0 - (MajorL / MinorL)) : std::numeric_limits<double>::infinity();
                //=========Normalize the moments ==========
                double mu00 = double(pixelsInCentroid);

                auto eta = [&](double mu_pq, int p, int q)
                {
                    double gamma = 1.0 + 0.5 * (p + q);
                    return mu_pq / std::pow(mu00, gamma);
                };
                double eta20 = eta(mu20, 2, 0);
                double eta02 = eta(mu02, 0, 2);
                double eta11 = eta(mu11, 1, 1);
                double eta30 = eta(mu30, 3, 0);
                double eta03 = eta(mu03, 0, 3);
                double eta21 = eta(mu21, 2, 1);
                double eta12 = eta(mu12, 1, 2);

                //=========== Hu invariants ============
                double Hu1 = eta20 + eta02;
                double Hu2 = (eta20 - eta02) * (eta20 - eta02) + 4.0 * eta11 * eta11;
                double Hu3 = (eta30 - 3 * eta12) * (eta30 - 3 * eta12) + (3 * eta21 - eta03) * (3 * eta21 - eta03);
                double Hu4 = (eta30 + eta12) * (eta30 + eta12) + (eta21 + eta03) * (eta21 + eta03);

                double Hu5 = (eta30 - 3 * eta12) * (eta30 + eta12) *
                                 ((eta30 + eta12) * (eta30 + eta12) - 3 * (eta21 + eta03) * (eta21 + eta03)) +
                             (3 * eta21 - eta03) * (eta21 + eta03) *
                                 (3 * (eta30 + eta12) * (eta30 + eta12) - (eta21 + eta03) * (eta21 + eta03));

                double Hu6 = (eta20 - eta02) * ((eta30 + eta12) * (eta30 + eta12) - (eta21 + eta03) * (eta21 + eta03)) +
                             4 * eta11 * (eta30 + eta12) * (eta21 + eta03);

                double Hu7 = (3 * eta21 - eta03) * (eta30 + eta12) *
                                 ((eta30 + eta12) * (eta30 + eta12) - 3 * (eta21 + eta03) * (eta21 + eta03)) -
                             (eta30 - 3 * eta12) * (eta21 + eta03) *
                                 (3 * (eta30 + eta12) * (eta30 + eta12) - (eta21 + eta03) * (eta21 + eta03));
                std::vector<double> HuMoments = { Hu1, Hu2, Hu3, Hu4, Hu5, Hu6, Hu7 };

                // if (hasHole)
                {
                    R3BLOG(info,
                           "Cluster with hole(s): Sensor "
                               << senId + 1 << " Original size=" << clusterPixels.size()
                               << " Adjusted size=" << effectiveClusterSize << " Holes=" << holeCount
                               << " sTrace=" << trace << " sDet=" << det << " angle=" << majorAngle
                               << " Elong=" << elongation << " Hu1=" << Hu1 << " Hu2=" << Hu2 << " Hu3=" << Hu3
                               << " Hu4=" << Hu4 << " Hu5=" << Hu5 << " Hu6=" << Hu6 << " Hu7=" << Hu7);
                }

                // ================== Major-axis unit vector (local & lab) ==================
                // Unit vector in local pixel coordinates (col,row)
                double ucol = std::cos(majorAngle);
                double urow = std::sin(majorAngle);

                // convert to physical local vector (mm) using pixel sizes
                TVector3 localUnitRaw(ucol * fPixelSize_ls, urow * fPixelSize_ss, 0.0);

                // normalize to true unit vector in local mm frame
                double locLen = localUnitRaw.Mag();
                TVector3 localUnit(0.0, 0.0, 0.0);
                if (locLen > 0.0)
                    localUnit = localUnitRaw * (1.0 / locLen);
                else
                    localUnit.SetXYZ(0.0, 0.0, 0.0); // fallback

                // Transform this unit vector to lab frame depending on geometry version
                TVector3 labUnit(0.0, 0.0, 0.0);

                if (fGeoversion == 202402)
                {
                    // rotation matrix applies to vectors as well (no translation)
                    fRot = fAlpideGeo->GetRotation(senId + 1);
                    labUnit = fRot * localUnit;
                }
                else if (fGeoversion == 202505)
                {
                    // mapping signs derived from labpos mapping earlier
                    if (senId < 3)
                        labUnit.SetXYZ(-localUnit.X(), -localUnit.Y(), 0.0);
                    else if (senId < 6)
                        labUnit.SetXYZ(+localUnit.X(), +localUnit.Y(), 0.0);
                    else if (senId < 9)
                        labUnit.SetXYZ(-localUnit.X(), +localUnit.Y(), 0.0);
                    else if (senId < 12)
                        labUnit.SetXYZ(+localUnit.X(), -localUnit.Y(), 0.0);
                    else if (senId < 15)
                        labUnit.SetXYZ(-localUnit.X(), -localUnit.Y(), 0.0);
                    else if (senId < 18)
                        labUnit.SetXYZ(+localUnit.X(), +localUnit.Y(), 0.0);
                    else if (senId < 21)
                        labUnit.SetXYZ(-localUnit.X(), +localUnit.Y(), 0.0);
                    else
                        labUnit.SetXYZ(+localUnit.X(), -localUnit.Y(), 0.0);
                }
                else if (fGeoversion == 202506)
                {
                    if (senId < 3)
                        labUnit.SetXYZ(-localUnit.X(), -localUnit.Y(), 0.0);
                    else if (senId < 6)
                        labUnit.SetXYZ(+localUnit.X(), +localUnit.Y(), 0.0);
                    else if (senId < 9)
                        labUnit.SetXYZ(-localUnit.X(), -localUnit.Y(), 0.0);
                    else
                        labUnit.SetXYZ(+localUnit.X(), +localUnit.Y(), 0.0);
                }

                // Protect from NaN && Inf ->Set to non physical units where possible for later analysis.
                if (!std::isfinite(localUnit.X()))
                    localUnit.SetX(0.0);
                if (!std::isfinite(localUnit.Y()))
                    localUnit.SetY(0.0);
                if (!std::isfinite(labUnit.X()))
                    labUnit.SetX(0.0);
                if (!std::isfinite(labUnit.Y()))
                    labUnit.SetY(0.0);

                // ================== 4 connectivity edge-count Perimeter (alt simple method)  ==================
                double outerPerimeter = 0.0;
                double holePerimeter = 0.0;

                for (int cx = 0; cx < nC; ++cx)
                {
                    for (int cy = 0; cy < nR; ++cy)
                    {
                        if (localMap[cx][cy] != 1)
                            continue; // only filled pixels

                        const int nx[4] = { cx - 1, cx + 1, cx, cx };
                        const int ny[4] = { cy, cy, cy - 1, cy + 1 };

                        for (int in = 0; in < 4; ++in)
                        {
                            int mx = nx[in];
                            int my = ny[in];
                            bool neighborFilled = false;
                            bool neighborExternal = false;

                            if (mx < 0 || mx >= nC || my < 0 || my >= nR)
                            {
                                neighborFilled = false;
                                neighborExternal = true;
                            }
                            else
                            {
                                if (localMap[mx][my] == 1)
                                    neighborFilled = true;
                                else
                                {
                                    neighborFilled = false;
                                    neighborExternal = visitedBkg[mx][my];
                                }
                            }

                            if (!neighborFilled)
                            {
                                double edgeLen = 0.0;
                                if (nx[in] != cx) // left/right neighbor
                                    edgeLen = fPixelSize_ss;
                                else // up/down neighbor
                                    edgeLen = fPixelSize_ls;

                                if (neighborExternal)
                                    outerPerimeter += edgeLen;
                                else
                                    holePerimeter += edgeLen;
                            }
                        }
                    }
                }

                double totalPerimeter = outerPerimeter + holePerimeter;

                // ==================  Marching Squares Perimeter Estimator (more complicated)  ==================
                double msOuterPerimeter = 0.0;
                double msHolePerimeter = 0.0;

                double pixelDiag = std::sqrt(fPixelSize_ls * fPixelSize_ls + fPixelSize_ss * fPixelSize_ss);
                double avgPixSize = 0.5 * (fPixelSize_ls + fPixelSize_ss);

                for (int cx = 0; cx < nC - 1; ++cx)
                {
                    for (int cy = 0; cy < nR - 1; ++cy)
                    {
                        int v00 = localMap[cx][cy];         // top-left
                        int v10 = localMap[cx + 1][cy];     // top-right
                        int v11 = localMap[cx + 1][cy + 1]; // bottom-right
                        int v01 = localMap[cx][cy + 1];     // bottom-left

                        int caseIdx = (v00 ? 1 : 0) | (v10 ? 2 : 0) | (v11 ? 4 : 0) | (v01 ? 8 : 0);

                        if (caseIdx == 0 || caseIdx == 15)
                            continue;

                        double contrib = 0.0;
                        if (caseIdx == 1 || caseIdx == 2 || caseIdx == 4 || caseIdx == 8 || caseIdx == 11 ||
                            caseIdx == 7 || caseIdx == 13 || caseIdx == 14)
                            contrib = 1.0;
                        else if (caseIdx == 3 || caseIdx == 6 || caseIdx == 9 || caseIdx == 12)
                            contrib = std::sqrt(2.0);
                        else
                            contrib = 1.0; // ambiguous

                        double edgeLen = (contrib == 1.0) ? avgPixSize : pixelDiag;

                        // look at empty corners
                        bool touchesExternal = false;
                        bool touchesHole = false;
                        int lx[4] = { cx, cx + 1, cx + 1, cx };
                        int ly[4] = { cy, cy, cy + 1, cy + 1 };
                        int val[4] = { v00, v10, v11, v01 };

                        for (int k = 0; k < 4; k++)
                        {
                            if (val[k] == 0)
                            {
                                if (visitedBkg[lx[k]][ly[k]])
                                    touchesExternal = true;
                                else
                                    touchesHole = true;
                            }
                        }

                        if (touchesExternal)
                            msOuterPerimeter += edgeLen;
                        if (touchesHole)
                            msHolePerimeter += edgeLen;
                    }
                }

                double msTotalPerimeter = msOuterPerimeter + msHolePerimeter;

                // ================== Compare Results ==================
                R3BLOG(info,
                       Form("Sensor %d: EdgeCount [outer=%.4f mm, hole=%.4f mm, total=%.4f mm] | "
                            "MarchSq [outer=%.4f mm, hole=%.4f mm, total=%.4f mm]",
                            senId + 1,
                            outerPerimeter,
                            holePerimeter,
                            totalPerimeter,
                            msOuterPerimeter,
                            msHolePerimeter,
                            msTotalPerimeter));

                // ================== Boost to real coords ==================
                TVector3 localpos(meanCol * fPixelSize_ls, meanRow * fPixelSize_ss, 0.);
                TVector3 labpos;

                if (fGeoversion == 202402)
                {
                    fRot = fAlpideGeo->GetRotation(senId + 1);
                    fTrans = fAlpideGeo->GetTranslation(senId + 1);
                    labpos = fRot * localpos + fTrans * 10.;
                }
                else if (fGeoversion == 202505)
                {
                    if (senId < 3)
                        labpos.SetXYZ(45. - localpos.X() - 30. * senId, -45. - localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (senId < 6)
                        labpos.SetXYZ(
                            -45. + localpos.X() + 30. * (senId - 3), -45. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (senId < 9)
                        labpos.SetXYZ(
                            45. - localpos.X() - 30. * (senId - 6), -15. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (senId < 12)
                        labpos.SetXYZ(
                            -45. + localpos.X() + 30. * (senId - 9), -15. - localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (senId < 15)
                        labpos.SetXYZ(
                            45. - localpos.X() - 30. * (senId - 12), 15. - localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (senId < 18)
                        labpos.SetXYZ(
                            -45. + localpos.X() + 30. * (senId - 15), 15. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else if (senId < 21)
                        labpos.SetXYZ(
                            45. - localpos.X() - 30. * (senId - 18), 45. + localpos.Y(), fAlpidetoTargetPos.Z());
                    else
                        labpos.SetXYZ(
                            -45. + localpos.X() + 30. * (senId - 21), 45. - localpos.Y(), fAlpidetoTargetPos.Z());
                }
                else if (fGeoversion == 202506)
                {
                    if (senId < 3)
                        labpos.SetXYZ(45. - localpos.X() - 30. * senId, -localpos.Y(), 0.);
                    else if (senId < 6)
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (senId - 3), localpos.Y(), 0.);
                    else if (senId < 9)
                        labpos.SetXYZ(45. - localpos.X() - 30. * (senId - 6), -localpos.Y(), 73.);
                    else
                        labpos.SetXYZ(-45. + localpos.X() + 30. * (senId - 9), localpos.Y(), 73.);
                }

                if (std::isnan(elongation) || std::isinf(elongation))
                    elongation = -1.0;
                if (std::isnan(holeCount) || std::isinf(holeCount))
                    holeCount = -1.0;

                //  Need to now put in data.
                R3BLOG(info,
                       Form("Sensor %d: major_angle=%.5f rad, local_unit=(%.5f,%.5f,%.5f), lab_unit=(%.5f,%.5f,%.5f), "
                            "outerPerim=%.4f mm, holePerim=%.4f mm, totalPerim=%.4f mm",
                            senId + 1,
                            majorAngle,
                            localUnit.X(),
                            localUnit.Y(),
                            localUnit.Z(),
                            labUnit.X(),
                            labUnit.Y(),
                            labUnit.Z(),
                            outerPerimeter,
                            holePerimeter,
                            totalPerimeter));
                TVector3 majorUnit;
                majorUnit.SetXYZ(labUnit.X(), labUnit.Y(), labUnit.Z());
                AddHitData(senId + 1,
                           effectiveClusterSize,
                           (int)holeCount,
                           elongation,
                           majorUnit,
                           holePerimeter,
                           totalPerimeter,
                           HuMoments,
                           labpos.X(),
                           labpos.Y(),
                           labpos.Z(),
                           localpos.X(),
                           localpos.Y());
            }
        }
    }
}

// -----   Private method AddHitData  ------------------------------------------
R3BAlpideHitData* R3BAlpideCal2Hit::AddHitData(uint16_t senId,
                                               uint16_t clustersize,
                                               uint16_t genus,
                                               double elong,
                                               TVector3 majorUnit,
                                               double holePerimeter,
                                               double totalPerimeter,
                                               std::vector<double> hu,
                                               double xpos,
                                               double ypos,
                                               double zpos,
                                               double locxpos,
                                               double locypos)
{
    // It fills the R3BAlpideHitData
    TClonesArray& clref = *fAlpideHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BAlpideHitData(senId,
                                              clustersize,
                                              genus,
                                              elong,
                                              majorUnit,
                                              holePerimeter,
                                              totalPerimeter,
                                              hu,
                                              xpos,
                                              ypos,
                                              zpos,
                                              locxpos,
                                              locypos);
}

ClassImp(R3BAlpideCal2Hit)
