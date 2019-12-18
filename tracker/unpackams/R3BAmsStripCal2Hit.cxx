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
// -----         R3BAmsStripCal2Hit source file                        -----
// -----             Created 01/06/18  by J.L. Rodriguez-Sanchez       -----
// -------------------------------------------------------------------------

// ROOT headers
#include "TClonesArray.h"
#include "TF1.h"
#include "TH1F.h"
#include "TMath.h"
#include "TSpectrum.h"

// Fair headers
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"

#include <iomanip>

// Califa headers
#include "R3BAmsHitData.h"
#include "R3BAmsStripCal2Hit.h"
#include "R3BAmsStripCalData.h"

// R3BAmsStripCal2Hit: Default Constructor --------------------------
R3BAmsStripCal2Hit::R3BAmsStripCal2Hit()
    : FairTask("R3B Hit-AMS Calibrator", 1)
    , fPitchK(104.)
    , fPitchS(110.)
    , fMaxNumDet(6)
    , // Max number of AMS detectors set to 4, experiment s444
    fMaxNumClusters(3)
    , // Max number of clusters per ams detector set to 3, experiment s444
    fAmsStripCalDataCA(NULL)
    , fAmsHitDataCA(NULL)
    , fOnline(kFALSE)
{
}

// R3BAmsStripCal2HitPar: Standard Constructor --------------------------
R3BAmsStripCal2Hit::R3BAmsStripCal2Hit(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fPitchK(104.)
    , fPitchS(110.)
    , fMaxNumDet(6)
    , // Max number of AMS detectors set to 4, experiment s444
    fMaxNumClusters(3)
    , // Max number of clusters per ams detector set to 3, experiment s444
    fAmsStripCalDataCA(NULL)
    , fAmsHitDataCA(NULL)
    , fOnline(kFALSE)
{
}

// Virtual R3BAmsStripCal2Hit: Destructor
R3BAmsStripCal2Hit::~R3BAmsStripCal2Hit()
{
    LOG(INFO) << "R3BAmsStripCal2Hit: Delete instance";
    if (fAmsStripCalDataCA)
        delete fAmsStripCalDataCA;
    if (fAmsHitDataCA)
        delete fAmsHitDataCA;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BAmsStripCal2Hit::Init()
{
    LOG(INFO) << "R3BAmsStripCal2Hit: Init";

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        return kFATAL;
    }

    fAmsStripCalDataCA = (TClonesArray*)rootManager->GetObject("AmsStripCalData");
    if (!fAmsStripCalDataCA)
    {
        return kFATAL;
    }

    // OUTPUT DATA
    // Hit data
    fAmsHitDataCA = new TClonesArray("R3BAmsHitData", 10);

    if (!fOnline)
    {
        rootManager->Register("AmsHitData", "AMS Hit", fAmsHitDataCA, kTRUE);
    }
    else
    {
        rootManager->Register("AmsHitData", "AMS Hit", fAmsHitDataCA, kFALSE);
    }

    char Name[255];
    for (Int_t i = 0; i < fMaxNumDet * 2; i++)
    {
        sprintf(Name, "hams_%d", i + 1);
        hams[i] = new TH1F(Name, "", 1024, -0.5, 1023.5);
    }

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BAmsStripCal2Hit::ReInit() { return kSUCCESS; }

// -----   Public method Execution   --------------------------------------------
void R3BAmsStripCal2Hit::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Cal Data --
    Int_t nHits = fAmsStripCalDataCA->GetEntries();
    if (!nHits)
        return;

    // Data from cal level
    R3BAmsStripCalData** calData;
    calData = new R3BAmsStripCalData*[nHits];
    Int_t detId;
    Int_t sideId;
    Int_t stripId;
    Double_t energy;
    for (Int_t i = 0; i < nHits; i++)
    {
        calData[i] = (R3BAmsStripCalData*)(fAmsStripCalDataCA->At(i));
        detId = calData[i]->GetDetId();
        sideId = calData[i]->GetSideId();
        stripId = calData[i]->GetStripId();
        energy = calData[i]->GetEnergy();
        hams[detId * 2 + sideId]->SetBinContent(stripId + 1, energy);
        // std::cout << stripId <<" "<< energy << std::endl;
    }

    Int_t nfoundS = 0, nfoundK = 0;
    Double_t x = 0., y = 0., z = 0.;
    TSpectrum* ss = new TSpectrum(10000);
    for (Int_t i = 0; i < fMaxNumDet; i++)
    {
        // Looking for hits in side S
        nfoundS = ss->Search(hams[i * 2], 1., "goff", 0.0001);
        fChannelPeaks = (Double_t*)ss->GetPositionX();
        // for(int j=0;j<nfound;j++)std::cout << nfound <<" "<< fChannelPeaks[j] << std::endl;
        Double_t clusterS[nfoundS][2];
        DefineClusters(&nfoundS, fPitchS, fChannelPeaks, hams[i * 2], clusterS);

        // Looking for hits in side K
        nfoundK = ss->Search(hams[i * 2 + 1], 1., "goff", 0.0001);
        fChannelPeaks = (Double_t*)ss->GetPositionX();
        // for(int j=0;j<nfound;j++)std::cout << nfound <<" "<< fChannelPeaks[j] << std::endl;
        // std::cout << " antes:"<< nfoundK << std::endl;
        Double_t clusterK[nfoundK][2];
        DefineClusters(&nfoundK, fPitchK, fChannelPeaks, hams[i * 2 + 1], clusterK);

        // std::cout << nfoundK  << std::endl;

        // Add hits per detector from the maximum energy to the lower one, but limiting the number
        // of clusters per detector to fMaxNumClusters
        if (nfoundK > 0 && nfoundS > 0)
        {
            for (Int_t mul = 0; mul < std::min(std::min(nfoundK, nfoundS), fMaxNumClusters); mul++)
            {
                /*   if (i == 0)
                   {                               // top
                       z = 14. + clusterS[mul][1]; // FIXME:Fix offsets for s444
                       y = 20. + 1.;
                       x = 20. - clusterK[mul][1];
                   }
                   else if (i == 1)
                   { // right
                       z = 14. + clusterS[mul][1];
                       x = -20. - 1.;
                       y = 20. - 1. * clusterK[mul][1];
                   }
                   else if (i == 2)
                   { // bottom
                       z = 14. + clusterS[mul][1];
                       y = -20. - 1.;
                       x = clusterK[mul][1] - 20.;
                   }
                   else if (i == 3)
                   { // left
                       z = 14. + clusterS[mul][1];
                       x = 20. + 1.;
                       y = clusterK[mul][1] - 20.;
                   }
   */
                TVector3 master(x, y, z);
                AddHitData(i, mul, clusterS[mul][1], clusterK[mul][1], master, clusterS[mul][0], clusterK[mul][0]);
            }
        }
    }

    for (Int_t i = 0; i < fMaxNumDet * 2; i++)
        hams[i]->Reset();
    if (calData)
        delete calData;
    delete ss;
    return;
}

// -----   Protected method Finish   --------------------------------------------
void R3BAmsStripCal2Hit::Finish() {}

// -----   Protected method to define clusters   --------------------------------
void R3BAmsStripCal2Hit::DefineClusters(Int_t* nfoundhits,
                                        Double_t fPitch,
                                        Double_t* fChannels,
                                        TH1F* hsst,
                                        Double_t cluster[][2])
{
    // std::cout << "Search " << std::endl;

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
            energy = hsst->GetBinContent(strip + 1);
            if (hsst->GetBinContent(strip + 2) > 0)
                finalstrip++;
            // std::cout<< strip <<" "<< energy <<std::endl;
            CoG[0] = CoG[0] + energy * strip;
            CoG[1] = CoG[1] + energy;
            SumEnergy[i] = SumEnergy[i] + energy;
            hsst->SetBinContent(strip + 1, 0.);
        }
        Position[i] = CoG[0] / CoG[1] * fPitch / 1000.;
    }
    // for(Int_t i = 0; i < nfound; i++)std::cout<< i <<" " << SumEnergy[i] <<" "<< Position[i] <<std::endl;
    Int_t v = 0;
    for (Int_t j = 0; j < nfound; j++)
    {
        if (SumEnergy[j] > 50.)
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
void R3BAmsStripCal2Hit::Reset()
{
    LOG(DEBUG) << "Clearing HitData Structure";
    if (fAmsHitDataCA)
        fAmsHitDataCA->Clear();
}

// -----   Private method AddHitData  --------------------------------------------
R3BAmsHitData* R3BAmsStripCal2Hit::AddHitData(Int_t detid,
                                              Int_t numhit,
                                              Double_t x,
                                              Double_t y,
                                              TVector3 master,
                                              Double_t energy_x,
                                              Double_t energy_y)
{
    // It fills the R3BAmsHitData
    TClonesArray& clref = *fAmsHitDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BAmsHitData(detid, numhit, x, y, master, energy_x, energy_y);
}

ClassImp(R3BAmsStripCal2Hit)
