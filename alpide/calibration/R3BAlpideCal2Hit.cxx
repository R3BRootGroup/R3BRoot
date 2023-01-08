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

// -------------------------------------------------------------
// -----               R3BAlpideCal2Hit                    -----
// -----     Created 22/03/22 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TMath.h"

// FAIR headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

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
R3BAlpideCal2Hit::R3BAlpideCal2Hit(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fAlpideCalData(NULL)
    , fAlpideHitData(NULL)
    // , fAlpidePixel(NULL)
    , fMap_Par(NULL)
    , fNbSensors(1)
    , fPixelSize(0.0292968) // TODO: put the right ones!
    , fAlpideGeo(NULL)
    , fGeoversion(2024)
    , fAlpideCluster(new TClonesArray("R3BAlpideCluster"))
    , fOnline(kFALSE)
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
    /* if (fAlpidePixel)
     {
         delete fAlpidePixel;
     }*/
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

    fMap_Par = (R3BAlpideMappingPar*)rtdb->getContainer("alpideMappingPar");
    R3BLOG_IF(fatal, !fMap_Par, "Container alpideMappingPar not found");

    fAlpideGeoPar = (R3BTGeoPar*)rtdb->getContainer("AlpideGeoPar");
    fTargetGeoPar = (R3BTGeoPar*)rtdb->getContainer("TargetGeoPar");
    if (!fAlpideGeoPar || !fTargetGeoPar)
    {
        R3BLOG_IF(warning, !fAlpideGeoPar, "Could not get access to AlpideGeoPar container.");
        R3BLOG_IF(warning, !fTargetGeoPar, "Could not get access to TargetGeoPar container.");
        return;
    }
    R3BLOG(info, "Container AlpideGeoPar found.");
    R3BLOG(info, "Container TargetGeoPar found.");

    fTargetPos.SetXYZ(fTargetGeoPar->GetPosX(), fTargetGeoPar->GetPosY(), fTargetGeoPar->GetPosZ());
    fAlpidePos.SetXYZ(fAlpideGeoPar->GetPosX(), fAlpideGeoPar->GetPosY(), fAlpideGeoPar->GetPosZ());
    return;
}

void R3BAlpideCal2Hit::SetParameter()
{
    //--- Parameter Container ---
    fGeoversion = fMap_Par->GetGeoVersion();
    R3BLOG(info, "Geometry version: " << fGeoversion);
    fNbSensors = fMap_Par->GetNbSensors();
    R3BLOG(info, "Nb of sensors: " << fNbSensors);
    return;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAlpideCal2Hit::Init()
{
    R3BLOG(info, "");
    FairRootManager* mgr = FairRootManager::Instance();
    if (!mgr)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    // INPUT DATA
    fAlpideCalData = (TClonesArray*)mgr->GetObject("AlpideCalData");
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

    fAlpideGeo = R3BAlpideGeometry::Instance();
    R3BLOG_IF(error, !fAlpideGeo->Init(fGeoversion), "Alpide geometry " << fGeoversion << " not found");

    //  for(int s=0;s<363;s++){
    //              TVector3 vref = this->GetAnglesVector(s+1);
    //            auto m = this->GetTransformation(s+1);
    // std::cout <<"sensorid: "<<s + 1 << " , "<<vref.Mag() <<" , "<<vref.Theta()*TMath::RadToDeg() <<" , "
    //<<vref.Phi()*TMath::RadToDeg()<<std::endl;
    //  m.Print();
    //     }

    fAlpidetoTargetPos = fTargetPos - fAlpidePos;

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
void R3BAlpideCal2Hit::Exec(Option_t* option)
{
    R3BLOG(debug, "New event");
    // Reset entries in the output arrays
    Reset();

    UInt_t nbcluster = 0;
    bool shouldround = false;

    // Reading the Input -- Cal Data --
    auto fAlpidePixel = (TClonesArray*)fAlpideCalData->Clone();
    Int_t nHits = fAlpidePixel->GetEntriesFast();
    if (nHits == 0)
    {
        delete fAlpidePixel;
        return;
    }

nextcluster:
    nbcluster++;
    bool first = true;

nextround:
    shouldround = false;

    auto calData = new R3BAlpideCalData*[fAlpidePixel->GetEntriesFast()];
    for (Int_t i = 0; i < fAlpidePixel->GetEntriesFast(); i++)
    {
        calData[i] = (R3BAlpideCalData*)(fAlpidePixel->At(i));
        auto sen = calData[i]->GetSensorId();
        auto col = calData[i]->GetCol();
        auto row = calData[i]->GetRow();

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
                cluster[j] = (R3BAlpideCluster*)fAlpideCluster->At(j);
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

    if (calData)
    {
        delete[] calData;
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
    /*  if (fAlpidePixel)
      {
          fAlpidePixel->Clear();
      }*/
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

    auto cluster = new R3BAlpideCluster*[nHits];
    Int_t mult[fNbSensors][nHits];
    Double_t meancol[fNbSensors][nHits];
    Double_t meanrow[fNbSensors][nHits];
    for (Int_t s = 0; s < fNbSensors; s++)
        for (Int_t i = 0; i < nHits; i++)
        {
            mult[s][i] = 0;
            meancol[s][i] = 0.;
            meanrow[s][i] = 0.;
        }

    for (Int_t i = 0; i < nHits; i++)
    {
        cluster[i] = (R3BAlpideCluster*)fAlpideCluster->At(i);
        auto clid = cluster[i]->GetClusterId() - 1;
        auto senid = cluster[i]->GetSensorId() - 1;
        mult[senid][clid]++;
        meancol[senid][clid] += (double)cluster[i]->GetCol();
        meanrow[senid][clid] += (double)cluster[i]->GetRow();
    }
    if (cluster)
        delete[] cluster;

    for (Int_t s = 0; s < fNbSensors; s++)
        for (Int_t i = 0; i < nHits; i++)
            if (mult[s][i] > 0)
            {
                nbcluster++;
                AddHitData(s + 1,
                           mult[s][i],
                           meancol[s][i] / double(mult[s][i]) * fPixelSize - 30. / 2.0,
                           meanrow[s][i] / double(mult[s][i]) * fPixelSize - 15. / 2.0);
            }

    R3BLOG(debug, "Number of clusters: " << nbcluster);
    return;
}

// -----   Private method AddHitData  --------------------------------------------
R3BAlpideHitData* R3BAlpideCal2Hit::AddHitData(UShort_t senId, UInt_t clustersize, Double_t x, Double_t y)
{
    // It fills the R3BAlpideHitData
    TClonesArray& clref = *fAlpideHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BAlpideHitData(senId, clustersize, x, y);
}

ClassImp(R3BAlpideCal2Hit);
