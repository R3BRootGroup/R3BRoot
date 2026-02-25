/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -----------------------------------------------------------------
// -----            R3BActafHit2Cluster source file            -----
// -----      Created 12/04/26 by J.L. Rodriguez-Sanchez       -----
// -----------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>
#include <cmath>
#include <queue>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>

// ACTAF headers
#include "R3BActafClusterData.h"
#include "R3BActafHit2Cluster.h"
#include "R3BActafHitData.h"
#include "R3BLogger.h"

const std::vector<double> weights = { 1. / 24.,    1. / 4012.,  1. / 6463.,  1. / 8913.,
                                      1. / 11363., 1. / 18419., 1. / 18588., 1. / 14330. };

// R3BActafHit2Cluster::Default Constructor --------------------------
R3BActafHit2Cluster::R3BActafHit2Cluster()
    : R3BActafHit2Cluster("R3BActafHit2Cluster", 1)
{
}

// R3BActafHit2Cluster::Standard Constructor --------------------------
R3BActafHit2Cluster::R3BActafHit2Cluster(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
{
}

// Virtual R3BActafHit2Cluster::Destructor
R3BActafHit2Cluster::~R3BActafHit2Cluster()
{
    R3BLOG(debug1, "");
    if (fActafCluster)
    {
        delete fActafCluster;
    }
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BActafHit2Cluster::Init()
{
    R3BLOG(info, "");
    auto* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, !mgr, "FairRootManager not found");

    // INPUT DATA
    fActafHitData = dynamic_cast<TClonesArray*>(mgr->GetObject("ActafHitData"));
    R3BLOG_IF(fatal, !fActafHitData, "ActafHitData not found");

    // OUTPUT DATA
    fActafCluster = new TClonesArray("R3BActafClusterData");
    mgr->Register("ActafClusterData", "ACTAF_Cluster", fActafCluster, !fOnline);
    Reset();

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BActafHit2Cluster::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

double R3BActafHit2Cluster::Distance(const HitPoint& a, const HitPoint& b)
{
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

void R3BActafHit2Cluster::FitTrack(const std::vector<Point>& pts,
                                   double& x0,
                                   double& y0,
                                   double& z0,
                                   double& phi,
                                   double& theta,
                                   double& Etot,
                                   double& chi2polarfit)
{
    int N = pts.size();
    if (N < 2 || N > 16)
        return;

    // Total energy for the cluster
    Etot = 0.0;
    for (const auto& p : pts)
        Etot += p.energy;

    // Energy per ring
    std::map<int, double> energyPerRing;
    for (const auto& p : pts)
    {
        int ring = p.ring - 1;
        energyPerRing[ring] += p.energy;
    }

    if (energyPerRing.count(0) == 0 || energyPerRing[0] == 0 || energyPerRing.count(1) == 0 || energyPerRing[1] == 0)
        return;

    double sumW = 0.0;
    x0 = 0.0;
    y0 = 0.0;
    z0 = 0.0;

    for (int i = 0; i < N; i++)
    {
        int ring = pts[i].ring - 1;

        double Ering = energyPerRing[ring];
        double wi = 0.0;

        // auto radius = sqrt(pts[i].x * pts[i].x + pts[i].y * pts[i].y);

        if (Ering > 0)
            wi = weights[ring] * (pts[i].energy / Ering);

        // std::cout << "ring: " << pts[i].ring << " pad: " << pts[i].pad << " w_eff: " << wi
        //           << " energy: " << pts[i].energy << " posR: " << radius << " posZ: " << pts[i].z << std::endl;

        x0 += wi * pts[i].x;
        y0 += wi * pts[i].y;
        z0 += wi * pts[i].z;
        sumW += wi;
    }

    if (sumW == 0)
        return;

    x0 /= sumW;
    y0 /= sumW;
    z0 /= sumW;

    double Sxx = 0.0, Syy = 0.0, Sxy = 0.0;

    for (int i = 0; i < N; i++)
    {
        int ring = pts[i].ring - 1;

        double Ering = energyPerRing[ring];
        double wi = 0.0;

        if (Ering > 0)
            wi = weights[ring] * (pts[i].energy / Ering);

        double dx = pts[i].x - x0;
        double dy = pts[i].y - y0;

        Sxx += wi * dx * dx;
        Syy += wi * dy * dy;
        Sxy += wi * dx * dy;
    }

    phi = 0.5 * atan2(2 * Sxy, Sxx - Syy);

    // double dx = cos(phi);
    // double dy = sin(phi);

    auto rb = sqrt(pts.back().x * pts.back().x + pts.back().y * pts.back().y);
    auto rf = sqrt(pts.front().x * pts.front().x + pts.front().y * pts.front().y);

    double vx = 0.;
    double vy = 0.;

    if (rb > rf)
    {
        vx = pts.back().x - pts.front().x;
        vy = pts.back().y - pts.front().y;
    }
    else
    {
        vx = pts.front().x - pts.back().x;
        vy = pts.front().y - pts.back().y;
    }

    if (vx > 0 && vy < 0)
        phi += 2. * TMath::Pi();

    if (vx < 0 && vy < 0)
        phi += TMath::Pi();

    if (vx < 0 && vy > 0)
        phi += TMath::Pi();

    // Calculation of the polar angle theta
    double R0 = 0.0;
    for (int i = 0; i < pts.size(); i++)
    {
        int ring = pts[i].ring - 1;

        double Ering = energyPerRing[ring];
        double wi = 0.0;

        if (Ering > 0)
            wi = weights[ring] * (pts[i].energy / Ering);

        double R = sqrt(pts[i].x * pts[i].x + pts[i].y * pts[i].y);
        R0 += wi * R;
    }
    R0 /= sumW;

    double Szz = 0.0;
    double SRR = 0.0;
    double SzR = 0.0;

    for (int i = 0; i < N; i++)
    {
        int ring = pts[i].ring - 1;

        double Ering = energyPerRing[ring];
        double wi = 0.0;

        if (Ering > 0)
            wi = weights[ring] * (pts[i].energy / Ering);

        double R = sqrt(pts[i].x * pts[i].x + pts[i].y * pts[i].y);

        double dz = pts[i].z - z0;
        double dR = R - R0;

        Szz += wi * dz * dz;
        SRR += wi * dR * dR;
        SzR += wi * dz * dR;
    }

    theta = 0.5 * atan2(2 * SzR, Szz - SRR);

    double slope = tan(theta);

    std::vector<Point> pts_clean;

    for (int i = 0; i < N; i++)
    {
        // int ring = pts[i].ring - 1;

        // double Ering = energyPerRing[ring];
        // double wi = 0.0;

        // if (Ering > 0)
        //    wi = weights[ring] * (pts[i].energy / Ering);

        double R = sqrt(pts[i].x * pts[i].x + pts[i].y * pts[i].y);

        double dz = pts[i].z - z0;
        double dR = R - R0;

        double dist = fabs(dR - slope * dz) / sqrt(1.0 + slope * slope);

        if (dist < 5.0) // FIX this value
            pts_clean.push_back(pts[i]);
    }

    R0 = 0.0;
    sumW = 0.0;

    for (int i = 0; i < pts_clean.size(); i++)
    {
        int ring = pts_clean[i].ring - 1;

        double Ering = energyPerRing[ring];
        double wi = 0.0;

        if (Ering > 0)
            wi = weights[ring] * (pts_clean[i].energy / Ering);

        double R = sqrt(pts_clean[i].x * pts_clean[i].x + pts_clean[i].y * pts_clean[i].y);
        R0 += wi * R;
        sumW += wi;
    }

    R0 /= sumW;

    Szz = 0.0;
    SRR = 0.0;
    SzR = 0.0;

    for (int i = 0; i < pts_clean.size(); i++)
    {
        int ring = pts_clean[i].ring - 1;

        double Ering = energyPerRing[ring];
        double wi = 0.0;

        if (Ering > 0)
            wi = weights[ring] * (pts_clean[i].energy / Ering);

        double R = sqrt(pts_clean[i].x * pts_clean[i].x + pts_clean[i].y * pts_clean[i].y);
        double dz = pts_clean[i].z - z0;
        double dR = R - R0;

        Szz += wi * dz * dz;
        SRR += wi * dR * dR;
        SzR += wi * dz * dR;
    }

    theta = 0.5 * atan2(2 * SzR, Szz - SRR);

    if (theta < 0.)
        theta += TMath::Pi();

    double chi2 = 0.0;

    for (int i = 0; i < pts_clean.size(); i++)
    {
        int ring = pts_clean[i].ring - 1;

        double Ering = energyPerRing[ring];
        double wi = 0.0;

        if (Ering > 0)
            wi = weights[ring] * (pts_clean[i].energy / Ering);

        double R = sqrt(pts_clean[i].x * pts_clean[i].x + pts_clean[i].y * pts_clean[i].y);

        double dz = pts_clean[i].z - z0;
        double dR = R - R0;

        double dist = (dR - slope * dz) / sqrt(1.0 + slope * slope);

        chi2 += wi * dist * dist;
    }

    int ndf = pts_clean.size() - 2;

    chi2polarfit = (ndf > 0) ? chi2 / ndf : -1.0;
}

// -----   Public method Execution   --------------------------------------------
void R3BActafHit2Cluster::Exec(Option_t*)
{
    // Reset entries in the output arrays
    Reset();

    // Reading the Input -- Hit Data --
    auto nHits = fActafHitData->GetEntriesFast();
    if (nHits == 0)
    {
        return;
    }

    std::vector<Point> pts;
    std::vector<HitPoint> hits;

    for (size_t index = 0; index < nHits; ++index)
    {
        auto hitData = dynamic_cast<R3BActafHitData const*>(fActafHitData->At(index));
        int pad = hitData->GetPad();
        int side = hitData->GetSide();
        int ring = hitData->GetRing();
        auto xpos = hitData->GetXpos();
        auto ypos = hitData->GetYpos();
        auto zpos = hitData->GetZpos();
        auto energy = hitData->GetEnergy();
        auto track = hitData->GetTrack();

        hits.push_back({ xpos, ypos, zpos, track.Phi(), energy, side, ring, pad, false });
        pts.push_back({ xpos, ypos, zpos, side, ring, pad, energy });
    }

    std::vector<std::vector<HitPoint>> clusters;
    const double dist_thr = 14.0;

    for (size_t i = 0; i < hits.size(); i++)
    {
        if (hits[i].used)
            continue;

        std::vector<HitPoint> cluster;
        std::queue<size_t> q;
        q.push(i);

        while (!q.empty())
        {
            size_t idx = q.front();
            q.pop();

            if (hits[idx].used)
                continue;

            hits[idx].used = true;
            cluster.push_back(hits[idx]);

            for (size_t j = 0; j < hits.size(); j++)
            {
                if (hits[j].used)
                    continue;

                if (this->Distance(hits[idx], hits[j]) < dist_thr && std::fabs(hits[idx].phi - hits[j].phi) < 0.8 &&
                    std::abs(hits[idx].ring - hits[j].ring) <= 2 && hits[idx].side == hits[j].side)
                {
                    q.push(j);
                }
            }
        }

        if (cluster.size() > 0)
            clusters.push_back(cluster);
    }

    for (auto& cl : clusters)
    {
        std::vector<Point> clpts;

        std::vector<uint16_t> padlist;
        TVector3 track;
        double chi2polarfit = 0.;
        int side = 0;

        for (auto& h : cl)
        {
            clpts.push_back({ h.x, h.y, h.z, h.side, h.ring, h.pad, h.energy });
            side = h.side;
            padlist.push_back(h.pad);
        }

        double x0 = 0., y0 = 0., z0 = 0., phi = 0., theta = 0., energy = 0.;
        this->FitTrack(clpts, x0, y0, z0, phi, theta, energy, chi2polarfit);

        track.SetMagThetaPhi(energy, theta, phi);

        if (energy > 0 && theta > 0)
            AddClusterData(side, x0, y0, z0, energy, track, padlist, chi2polarfit);
    }

    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BActafHit2Cluster::Reset()
{
    R3BLOG(debug1, "Clearing ClusterData Structure");
    if (fActafCluster)
    {
        fActafCluster->Clear();
    }
}

// -----   Private method AddClusterData  ----------------------------------------
R3BActafClusterData* R3BActafHit2Cluster::AddClusterData(UInt_t side,
                                                         double xpos,
                                                         double ypos,
                                                         double zpos,
                                                         double energy,
                                                         TVector3 track,
                                                         std::vector<uint16_t> padlist,
                                                         double chi2polarfit)
{
    // It fills the R3BActafClusterData
    TClonesArray& clref = *fActafCluster;
    auto size = clref.GetEntriesFast();
    return new (clref[size]) R3BActafClusterData(side, xpos, ypos, zpos, energy, track, padlist, chi2polarfit);
}

ClassImp(R3BActafHit2Cluster)
