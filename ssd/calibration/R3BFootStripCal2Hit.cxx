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

// -------------------------------------------------------------------------
// -----           R3BFootStripCal2Hit source file                     -----
// -----       Created 05/11/21  by J.L. Rodriguez-Sanchez             -----
// -------------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"
#include "TSpectrum.h"
#include <iomanip>

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

// AMS headers
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootMappingPar.h"
#include "R3BFootStripCal2Hit.h"

// R3BFootStripCal2Hit: Default Constructor --------------------------
R3BFootStripCal2Hit::R3BFootStripCal2Hit()
    : R3BFootStripCal2Hit("R3BFootStripCal2Hit", 1)
{
}

// R3BFootStripCal2HitPar: Standard Constructor --------------------------
R3BFootStripCal2Hit::R3BFootStripCal2Hit(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fPitch(157.7)
    , fMiddle(50.)
    , fThSum(20.)
    , fMaxNumDet(10)
    , fMaxNumClusters(3) // Max number of clusters per ams detector set to 3
    , fFootCalData(NULL)
    , fFootHitData(NULL)
    , fMap_Par(NULL)
    , fOnline(kFALSE)
{
}

// Virtual R3BFootStripCal2Hit: Destructor
R3BFootStripCal2Hit::~R3BFootStripCal2Hit()
{
    LOG(DEBUG) << "R3BFootStripCal2Hit::Delete instance";
    if (fFootCalData)
        delete fFootCalData;
    if (fFootHitData)
        delete fFootHitData;
}

void R3BFootStripCal2Hit::SetParContainers()
{
    // Parameter Container
    // Reading amsMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "R3BFootStripCal2Hit::FairRuntimeDb not found";
    }

    fMap_Par = (R3BFootMappingPar*)rtdb->getContainer("footMappingPar");
    if (!fMap_Par)
    {
        LOG(ERROR) << "R3BFootStripCal2Hit::Init() Couldn't get handle on footMappingPar container";
    }
    else
    {
        LOG(INFO) << "R3BFootStripCal2Hit::footMappingPar found";
    }
}

void R3BFootStripCal2Hit::SetParameter()
{
    if (!fMap_Par)
    {
        LOG(WARNING) << "R3BFootStripCal2Hit::Container footMappingPar not found";
    }
    //--- Parameter Container ---
    fMaxNumDet = fMap_Par->GetNumDets(); // Number of ams detectors
    LOG(INFO) << "R3BFootStripCal2Hit::NumDet from mapping " << fMaxNumDet;
    fMap_Par->printParams();
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BFootStripCal2Hit::Init()
{
    LOG(INFO) << "R3BFootStripCal2Hit::Init()";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        LOG(FATAL) << "R3BFootStripCal2Hit::FairRootManager not found";
        return kFATAL;
    }

    fFootCalData = (TClonesArray*)rootManager->GetObject("FootCalData");
    if (!fFootCalData)
    {
        LOG(FATAL) << "R3BFootStripCal2Hit::FootCalData not found";
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    // Output data
    fFootHitData = new TClonesArray("R3BFootHitData", fMaxNumDet * fMaxNumClusters);
    rootManager->Register("FootHitData", "FOOT Hit", fFootHitData, !fOnline);

    char Name[255];
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        sprintf(Name, "hssd_%d", i + 1);
        hssd[i] = new TH1F(Name, "", 634, -0.5, 633.5);
    }

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
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
    Int_t nHits = fFootCalData->GetEntries();
    if (!nHits)
        return;

    // Data from cal level
    R3BFootCalData** calData;
    calData = new R3BFootCalData*[nHits];
    Int_t detId;
    Int_t stripId;
    Double_t energy;
    for (Int_t i = 0; i < nHits; i++)
    {
        calData[i] = (R3BFootCalData*)(fFootCalData->At(i));
        detId = calData[i]->GetDetId() - 1;
        stripId = calData[i]->GetStripId();
        energy = calData[i]->GetEnergy();
        hssd[detId]->SetBinContent(stripId, energy);
        // std::cout << stripId <<" "<< energy << std::endl;
    }

    Int_t nfound = 0;
    Double_t x = 0., y = 0., z = 0.;
    TSpectrum* ss = new TSpectrum(10000);
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        // Looking for hits in side S
        nfound = ss->Search(hssd[i], 1., "goff", 0.0001);
        fChannelPeaks = (Double_t*)ss->GetPositionX();
        Double_t clusterS[nfound][2];
        DefineClusters(&nfound, fPitch, fChannelPeaks, hssd[i], clusterS);

        // std::cout << nfound  << std::endl;

        // Add hits per detector from the maximum energy to the lower one, but limiting the number
        // of clusters per detector to fMaxNumClusters
        if (nfound > 0)
        {
            for (Int_t mul = 0; mul < std::min(nfound, fMaxNumClusters); mul++)
            {
                if (fMap_Par->GetGeometry() == 202111)
                {
                    // Julich test with 6 detectors
                    if (i >= 0)
                    {
                        x = 1.0 * clusterS[mul][1] - fMiddle;
                        y = 0.;
                        z = fMap_Par->GetDist2target(i + 1);
                    }
                }

                TVector3 master(x, y, z);
                AddHitData(i + 1, mul, clusterS[mul][1], master, clusterS[mul][0], nfound);
            }
        }
    }

    for (Int_t i = 0; i < fMaxNumDet; i++)
        hssd[i]->Reset();
    if (calData)
        delete calData;
    delete ss;
    return;
}

// -----   Protected method Finish   --------------------------------------------
void R3BFootStripCal2Hit::Finish() {}

// -----   Protected method to define clusters   --------------------------------
void R3BFootStripCal2Hit::DefineClusters(Int_t* nfoundhits,
                                         Double_t fPitchs,
                                         Double_t* fChannels,
                                         TH1F* hsst,
                                         Double_t cluster[][2])
{
    Int_t nfound = *nfoundhits;
    *nfoundhits = 0;

    Double_t SumEnergy[nfound], Position[nfound], energy = 0.;
    Int_t CountDet = 0;
    Double_t CoG[2];
    // Double_t cluster[nfound][2];
    for (Int_t i = 0; i < nfound; i++)
    {
        SumEnergy[i] = 0.;
        Position[i] = 0.;
        for (Int_t j = 0; j < 2; j++)
            cluster[i][j] = 0.;
    }

    for (Int_t i = 0; i < nfound; i++)
    {
        for (Int_t j = 0; j < 2; j++)
            CoG[j] = 0.;
        Int_t initstrip = fChannels[i];
        for (int k = 0; k < 10; k++)
            if (hsst->GetBinContent(fChannels[i] - k) > 0)
                initstrip--;
            else
                break;
        if (initstrip < 0)
            initstrip = 0;
        Int_t finalstrip = fChannels[i] + 1;
        for (Int_t strip = initstrip; strip < finalstrip; strip++)
        {
            energy = hsst->GetBinContent(strip);
            if (hsst->GetBinContent(strip + 1) > 0)
                finalstrip++;
            // std::cout<< strip <<" "<< energy <<std::endl;
            CoG[0] = CoG[0] + energy * strip;
            CoG[1] = CoG[1] + energy;
            SumEnergy[i] = SumEnergy[i] + energy;
            hsst->SetBinContent(strip, 0.);
        }
        Position[i] = CoG[0] / CoG[1] * fPitchs / 1000.;
    }
    // for(Int_t i = 0; i < nfound; i++)std::cout<< i <<" " << SumEnergy[i] <<" "<< Position[i] <<std::endl;
    Int_t v = 0;
    for (Int_t j = 0; j < nfound; j++)
    {
        if (SumEnergy[j] > fThSum)
        {
            cluster[v][0] = SumEnergy[j];
            cluster[v][1] = Position[j];
            v++;
        }
    }
    *nfoundhits = v;
    // for(Int_t i = 0; i < nfound; i++)std::cout<< i <<" " << cluster[i][0] <<" "<< cluster[i][1] <<std::endl;
}

// -----   Public method Reset   ------------------------------------------------
void R3BFootStripCal2Hit::Reset()
{
    LOG(DEBUG) << "Clearing HitData Structure";
    if (fFootHitData)
        fFootHitData->Clear();
}

// -----   Private method AddHitData  --------------------------------------------
R3BFootHitData* R3BFootStripCal2Hit::AddHitData(Int_t detid,
                                                Int_t numhit,
                                                Double_t s,
                                                TVector3 master,
                                                Double_t energy_s,
                                                Int_t mulS)
{
    // It fills the R3BAmsHitData
    TClonesArray& clref = *fFootHitData;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BFootHitData(detid, numhit, s, master, energy_s, mulS);
}

ClassImp(R3BFootStripCal2Hit);
