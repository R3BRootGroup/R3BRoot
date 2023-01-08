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

// ------------------------------------------------------------------------
// -----            R3BFootStripCal2Hit source file                   -----
// -----       Created 05/11/21 by J.L. Rodriguez-Sanchez             -----
// ------------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TSpectrum.h"
#include <iomanip>
#include <vector>

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// FOOT headers
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootMappingPar.h"
#include "R3BFootStripCal2Hit.h"
#include "R3BLogger.h"

// R3BFootStripCal2Hit::Default Constructor -------------------------------------
R3BFootStripCal2Hit::R3BFootStripCal2Hit()
    : R3BFootStripCal2Hit("R3BFootStripCal2Hit", 1)
{
}

// R3BFootStripCal2HitPar::Standard Constructor ---------------------------------
R3BFootStripCal2Hit::R3BFootStripCal2Hit(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fPitch(157.7)
    , fMiddle(50.)
    , fThSum(20.)
    , fMaxNumDet(16)
    , fMaxNumClusters(3) // Max number of clusters per foot detector set to 3
    , fFootCalData(NULL)
    , fFootHitData(NULL)
    , fMap_Par(NULL)
    , fOnline(kFALSE)
    , fPolPar(5)
    , fEnevsPosCorrPar{ { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. },
                        { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. }, { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. } }
    , fDistTarget{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
    , fAngleTheta{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
    , fAnglePhi{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
    , fOffsetX{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
    , fOffsetY{ 0., 0., 0., 0., 0., 0., 0., 0., 0., 0, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0 }
{
}

// Virtual R3BFootStripCal2Hit::Destructor --------------------------------------
R3BFootStripCal2Hit::~R3BFootStripCal2Hit()
{
    R3BLOG(debug1, "");
    if (fFootHitData)
        delete fFootHitData;
}

void R3BFootStripCal2Hit::SetParContainers()
{
    // Parameter Container
    // Reading footMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(error, !rtdb, "FairRuntimeDb not found");

    fMap_Par = (R3BFootMappingPar*)rtdb->getContainer("footMappingPar");
    if (!fMap_Par)
    {
        R3BLOG(error, "Couldn't get handle on footMappingPar container");
    }
    else
    {
        R3BLOG(info, "footMappingPar found");
    }
}

void R3BFootStripCal2Hit::SetParameter()
{
    if (!fMap_Par)
    {
        R3BLOG(warn, "Container footMappingPar not found");
        return;
    }
    //--- Parameter Container ---
    fMaxNumDet = fMap_Par->GetNumDets(); // Number of foot detectors
    fPolPar = fMap_Par->GetPolPar();     // Number of parameters for Energy correction of foot detectors
    LOG(info) << "R3BFootStripCal2Hit::NumDet from mapping " << fMaxNumDet;
    LOG(info) << "R3BFootStripCal2Hit::PolPar from mapping " << fPolPar;
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        fDistTarget[i] = fMap_Par->GetDist2target(i + 1);
        fAngleTheta[i] = fMap_Par->GetAngleTheta(i + 1);
        fAnglePhi[i] = fMap_Par->GetAnglePhi(i + 1);
        fOffsetX[i] = fMap_Par->GetOffsetX(i + 1);
        fOffsetY[i] = fMap_Par->GetOffsetY(i + 1);
        for (Int_t j = 0; j < fPolPar; j++)
        {
            fEnevsPosCorrPar[i][j] = fMap_Par->GetEnevsPosCorr(i * fPolPar + j + 1);
        }
    }
    fMap_Par->printParams();
}

// -----   Public method Init   -------------------------------------------------
InitStatus R3BFootStripCal2Hit::Init()
{
    R3BLOG(info, "");

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fFootCalData = (TClonesArray*)rootManager->GetObject("FootCalData");
    if (!fFootCalData)
    {
        R3BLOG(fatal, "FootCalData not found");
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    // Output data
    fFootHitData = new TClonesArray("R3BFootHitData");
    rootManager->Register("FootHitData", "FOOT Hit", fFootHitData, !fOnline);
    fFootHitData->Clear();

    char Name[255];
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        sprintf(Name, "hssd_%d", i + 1);
        hssd[i] = new TH1F(Name, "", 640, -0.5, 640.5);
    }

    return kSUCCESS;
}

// -----   Public method ReInit   -----------------------------------------------
InitStatus R3BFootStripCal2Hit::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----   Public method Execution   --------------------------------------------
void R3BFootStripCal2Hit::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input Cal Data
    Int_t nHits = fFootCalData->GetEntriesFast();
    if (nHits == 0)
        return;

    // Data from cal level
    R3BFootCalData** calData = new R3BFootCalData*[nHits];
    Int_t detId;
    Int_t stripId;
    Double_t energy;
    Double_t x = 0., y = 0., z = 0.;
    std::vector<Int_t> StripI[fMaxNumDet];
    std::vector<Double_t> StripE[fMaxNumDet];

    // Clustering algorithm - A. Revel
    // Init
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        StripI[i].clear();
        StripE[i].clear();
        ClusterMult[i] = 0;
        for (Int_t j = 0; j < 100; j++)
        {
            ClusterPos[i][j] = 0.;
            ClusterESum[i][j] = 0.;
            ClusterNStrip[i][j] = 0;
            Nu[i][j] = 0.;
            for (Int_t k = 0; k < 200; k++)
            {
                ClusterI[i][j][k] = 0.;
                ClusterE[i][j][k] = 0.;
            }
        }
    }

    // Filling vectors
    for (Int_t i = 0; i < nHits; i++)
    {
        calData[i] = (R3BFootCalData*)(fFootCalData->At(i));
        detId = calData[i]->GetDetId() - 1;
        stripId = calData[i]->GetStripId() - 1;
        energy = calData[i]->GetEnergy();

        StripI[detId].push_back(stripId);
        StripE[detId].push_back(energy);
    }

    // Sort (should be good by default but just in case)
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        if (StripI[i].size() > 1)
        {
            for (Int_t j = 0; j < StripI[i].size() - 1; j++)
            {
                for (Int_t k = j + 1; k < StripI[i].size(); k++)
                {
                    if (StripI[i].at(j) > StripI[i].at(k))
                    {
                        Int_t tempI = StripI[i].at(j);
                        StripI[i].at(j) = StripI[i].at(k);
                        StripI[i].at(k) = tempI;
                        Int_t tempD = StripE[i].at(j);
                        StripE[i].at(j) = StripE[i].at(k);
                        StripE[i].at(k) = tempD;
                    }
                }
            }
        }
    }

    // std::cout << "Event" << std::endl;
    // for (Int_t i = 0; i < fMaxNumDet; i++){
    //   for (Int_t j = 0; j < StripI[i].size() ; j++){
    //     std::cout << i+1 << "   " << j << "   " << StripI[i].at(j) << "   " << StripE[i].at(j) << std::endl;
    //   }
    // }

    // Clustering
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {

        Int_t ClusterCount = 0;
        Int_t StripCount = 0;
        Int_t TempI = 0;

        Int_t j = 0;

        while (j < StripI[i].size())
        {

            if (j == 0)
            {
                TempI = StripI[i].at(j);
            }

            ClusterNStrip[i][ClusterCount]++;
            ClusterI[i][ClusterCount][StripCount] = StripI[i].at(j);
            ClusterE[i][ClusterCount][StripCount] = StripE[i].at(j);

            Int_t k = j + 1;

            while (k < StripI[i].size() && (StripI[i].at(k) - TempI) == 1)
            {
                StripCount++;
                ClusterNStrip[i][ClusterCount]++;
                ClusterI[i][ClusterCount][StripCount] = StripI[i].at(k);
                ClusterE[i][ClusterCount][StripCount] = StripE[i].at(k);
                TempI = StripI[i].at(k);
                StripI[i].erase(StripI[i].begin() + k);
                StripE[i].erase(StripE[i].begin() + k);
            }

            StripI[i].erase(StripI[i].begin() + j);
            StripE[i].erase(StripE[i].begin() + j);

            ClusterCount++;
            StripCount = 0;
        }
        ClusterMult[i] = ClusterCount;
    }

    // std::cout << "Clustering Result : " << std::endl;
    // for(Int_t i = 0; i < fMaxNumDet; i++){
    //   if(ClusterMult[i]>0){
    //     std::cout << "FOOT " << i << std::endl;
    //     std::cout << ClusterMult[i] << std::endl;
    //     for(Int_t j = 0; j < ClusterMult[i] ; j++){
    //       for(Int_t k = 0; k < ClusterNStrip[i][j] ; k++){
    //         std::cout << j << "   " << ClusterNStrip[i][j] << "   " << ClusterI[i][j][k]<< "   " << ClusterE[i][j][k]
    //         << std::endl;
    //       }
    //     }
    //   }
    // }

    // Compute Sum Energy, Position and Eta
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        for (Int_t j = 0; j < ClusterMult[i]; j++)
        {
            for (Int_t k = 0; k < ClusterNStrip[i][j]; k++)
            {
                ClusterESum[i][j] += ClusterE[i][j][k];
                ClusterPos[i][j] += ClusterE[i][j][k] * ClusterI[i][j][k];
            }
            ClusterPos[i][j] = ClusterPos[i][j] / ClusterESum[i][j];
            if (ClusterNStrip[i][j] == 1)
            { // Clusters with only one strip fired
                ClusterPos[i][j] = gRandom->Gaus(ClusterPos[i][j], 0.001);
            }
            Nu[i][j] = ClusterPos[i][j] - (Int_t)ClusterPos[i][j];
        }
    }

    // Sort Cluster from Higher to Lower Energy
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        for (Int_t j = 0; j < ClusterMult[i] - 1; j++)
        {
            for (Int_t k = j + 1; k < ClusterMult[i]; k++)
            {
                if (ClusterESum[i][j] < ClusterESum[i][k])
                {

                    Int_t tempI = ClusterNStrip[i][j];
                    ClusterNStrip[i][j] = ClusterNStrip[i][k];
                    ClusterNStrip[i][k] = tempI;

                    Double_t tempD = ClusterESum[i][j];
                    ClusterESum[i][j] = ClusterESum[i][k];
                    ClusterESum[i][k] = tempD;

                    tempD = ClusterPos[i][j];
                    ClusterPos[i][j] = ClusterPos[i][k];
                    ClusterPos[i][k] = tempD;

                    tempD = Nu[i][j];
                    Nu[i][j] = Nu[i][k];
                    Nu[i][k] = tempD;
                }
            }
        }
    }

    // Filling HitData
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        for (Int_t j = 0; j < ClusterMult[i]; j++)
        {

            Double_t pos = 100. * ClusterPos[i][j] / 640. - 50.;

            if (fAnglePhi[i] == 0.)
            { // X-Foot (StripId numbered from left to right)
                x = pos * TMath::Cos(fAngleTheta[i] * TMath::DegToRad()) + fOffsetX[i];
                y = fOffsetY[i];
                z = pos * TMath::Sin(fAngleTheta[i] * TMath::DegToRad()) + fDistTarget[i];
            }
            else if (fAnglePhi[i] == 90.)
            { // Y-Foot (StripId numbered from bottom to top)
                x = fOffsetX[i];
                y = pos + fOffsetY[i];
                z = fDistTarget[i];
            }
            else if (fAnglePhi[i] == 180.)
            { // X-Foot (StripId numbered from right to left)
                x = -pos * TMath::Cos(fAngleTheta[i] * TMath::DegToRad()) + fOffsetX[i];
                y = fOffsetY[i];
                z = -pos * TMath::Sin(fAngleTheta[i] * TMath::DegToRad()) + fDistTarget[i];
            }
            else if (fAnglePhi[i] == 270.)
            { // Y-Foot (StripId numbered from top to bottom)
                x = fOffsetX[i];
                y = -pos + fOffsetY[i];
                z = fDistTarget[i];
            }
            else
            {
                LOG(info) << "R3BFootStripCal2Hit::AnglePhi is Wrong !";
            }

            TVector3 master(x, y, z);

            Double_t energy_corr = 0.;

            for (Int_t k = 1; k < fPolPar; k++)
            {
                energy_corr += fEnevsPosCorrPar[i][k] * TMath::Power(Nu[i][j], k);
            }

            energy = ClusterESum[i][j] - energy_corr;

            if (ClusterESum[i][j] > fThSum && j < fMaxNumClusters)
            {
                AddHitData(i + 1, ClusterNStrip[i][j], x, master, energy, ClusterMult[i]);
            }
        }
    }

    for (Int_t i = 0; i < fMaxNumDet; i++)
        hssd[i]->Reset();
    if (calData)
        delete[] calData;
    // delete ss;
    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BFootStripCal2Hit::Reset()
{
    R3BLOG(debug, "Clearing HitData Structure");
    if (fFootHitData)
        fFootHitData->Clear();
}

// -----   Private method AddHitData  -------------------------------------------
R3BFootHitData* R3BFootStripCal2Hit::AddHitData(Int_t detid,
                                                Int_t numhit,
                                                Double_t s,
                                                TVector3 master,
                                                Double_t energy_s,
                                                Int_t mulS)
{
    // It fills the R3BFootHitData
    TClonesArray& clref = *fFootHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFootHitData(detid, numhit, s, master, energy_s, mulS);
}

ClassImp(R3BFootStripCal2Hit);
