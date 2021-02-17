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

#include "TClonesArray.h"
#include "TF1.h"
#include "TGeoMatrix.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TMath.h"
#include "TObjArray.h"
#include "TRandom.h"
#include "TSpectrum.h"
#include "TVector3.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TGeoManager.h"

#include "R3BCalifaCrystalCal2TotCalPar.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaMappingPar.h"
#include "R3BCalifaTotCalPar.h"
#include "R3BEventHeader.h"

#include <iostream>
#include <stdlib.h>

using namespace std;

R3BCalifaCrystalCal2TotCalPar::R3BCalifaCrystalCal2TotCalPar()
    : FairTask("R3B CALIFA Tot Calibration Parameters Finder ", 1)
    , fMap_Par(NULL)
    , fTotCal_Par(NULL)
    , fCrystalCalDataCA(NULL)
    , fNumCrystals(0)
    , fNumParam(2)
    , fMinStadistics(0)
    , fThreshold(0)
{
}

R3BCalifaCrystalCal2TotCalPar::R3BCalifaCrystalCal2TotCalPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMap_Par(NULL)
    , fTotCal_Par(NULL)
    , fCrystalCalDataCA(NULL)
    , fNumCrystals(0)
    , fNumParam(2)
    , fMinStadistics(0)
    , fThreshold(0)
{
}

R3BCalifaCrystalCal2TotCalPar::~R3BCalifaCrystalCal2TotCalPar()
{
    LOG(INFO) << "R3BCalifaCrystalCal2TotCalPar: Delete instance";
    if (fCrystalCalDataCA)
        delete fCrystalCalDataCA;
}

void R3BCalifaCrystalCal2TotCalPar::SetParContainers()
{
    // Parameter Container
    // Reading califaMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        LOG(ERROR) << "FairRuntimeDb not opened!";
    }
    fMap_Par = (R3BCalifaMappingPar*)rtdb->getContainer("califaMappingPar");
    if (!fMap_Par)
    {
        LOG(ERROR) << "R3BCalifaCrystalCal2TotCalPar::Init() Couldn't get handle on califamappingpar container";
    }
    else
    {
        LOG(INFO) << "R3BCalifaCrystalCal2TotCalPar:: califamappingpar container open";
    }
}

void R3BCalifaCrystalCal2TotCalPar::SetParameter()
{
    if (!fMap_Par)
    {
        LOG(ERROR) << "R3BCalifaMapped2CrystalCalPar::Container califaMappingPar not found.";
    }
    //--- Parameter Container ---
    fNumCrystals = fMap_Par->GetNumCrystals(); // Number of crystals x 2
    LOG(INFO) << "R3BCalifaMapped2CrystalCalPar::NumCry " << fNumCrystals;
    // fMap_Par->printParams();
}

InitStatus R3BCalifaCrystalCal2TotCalPar::Init()
{
    LOG(INFO) << "R3BCalifaCrystalCal2TotCalPar::Init";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        return kFATAL;
    }

    fCrystalCalDataCA = (TClonesArray*)rootManager->GetObject("CalifaCrystalCalData");
    if (!fCrystalCalDataCA)
    {
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        return kFATAL;
    }

    fTotCal_Par = (R3BCalifaTotCalPar*)rtdb->getContainer("CalifaTotCalPar");
    if (!fTotCal_Par)

    {
        LOG(ERROR) << "R3BCalifaCrystalCal2TotCalPar::Init() Couldn't get handle on CalifaTotCalPar container";
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    // Create histograms for crystal calibration
    char name[100];
    energy_vs_tot_crystal = new TGraph*[fNumCrystals];
    // std::cout << "fNumCrystals = " << fNumCrystals << std::endl;
    for (Int_t i = 0; i < fNumCrystals; i++)
        if (fMap_Par->GetInUse(i + 1) == 1)
        {
            sprintf(name, "energy_vs_tot_crystal_%d", i + 1);
            energy_vs_tot_crystal[i] = new TGraph();
            energy_vs_tot_crystal[i]->SetName(name);
        }

    return kSUCCESS;
}

InitStatus R3BCalifaCrystalCal2TotCalPar::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BCalifaCrystalCal2TotCalPar::Reset() {}

void R3BCalifaCrystalCal2TotCalPar::FinishEvent() {}

void R3BCalifaCrystalCal2TotCalPar::FinishTask() { Search_TotParams(); }

void R3BCalifaCrystalCal2TotCalPar::Exec(Option_t* opt)
{
    Int_t nHits = fCrystalCalDataCA->GetEntries();
    if (!nHits)
        return;

    R3BCalifaCrystalCalData** MapHit = new R3BCalifaCrystalCalData*[nHits];
    Int_t crystalId = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        MapHit[i] = (R3BCalifaCrystalCalData*)(fCrystalCalDataCA->At(i));
        crystalId = MapHit[i]->GetCrystalId();
        // Fill histograms
        energy_vs_tot_crystal[crystalId - 1]->SetPoint(
            energy_vs_tot_crystal[crystalId - 1]->GetN(), MapHit[i]->GetToT_Energy(), MapHit[i]->GetEnergy());
        // std::cout << "Tot = " << MapHit[i]->GetToT_Energy() << std::endl;
        // std::cout << "Energy = " << MapHit[i]->GetEnergy() << std::endl;
    }

    if (MapHit)
        delete MapHit;
    return;
}

void R3BCalifaCrystalCal2TotCalPar::Search_TotParams()
{

    fTotCal_Par->SetNumCrystals(fNumCrystals);
    fTotCal_Par->SetNumParametersFit(fNumParam);
    fTotCal_Par->GetCryCalParams()->Set(fNumParam * fNumCrystals);

    TF1* f1 = nullptr;
    if (fNumParam)
    {

        if (fNumParam == 2)
        {
            f1 = new TF1("f1", "[0]*TMath::Exp(x/[1])", 500, 1400);
            f1->SetParameter(0, 7700);
            f1->SetParameter(1, 1000);
        }
        if (fNumParam == 3)
        {
            f1 = new TF1("f1", "[0]*TMath::Exp(x/[1]) + [2]", 500, 1400);
            f1->SetParameter(0, 7700);
            f1->SetParameter(1, 1000);
            f1->SetParameter(2, 100);
        }

        if (fNumParam > 3)
        {
            LOG(ERROR) << "R3BCalifaCrystalCal2TotCalPar:: The number of fit parameters can not be higher than 3";
        }
    }
    else
    {
        LOG(INFO) << "R3BCalifaCrystalCal2TotCalPar:: No imput number of fit parameters, therefore, by default "
                     "NumberParameters=2";
        f1 = new TF1("f1", "[0]*TMath::Exp(x/[1])", 500, 1400);
        f1->SetParameter(0, 7700);
        f1->SetParameter(1, 1000);
    }

    for (Int_t i = 0; i < fNumCrystals; i++)
        if (fMap_Par->GetInUse(i + 1) == 1)
        {
            energy_vs_tot_crystal[i]->Fit("f1", "R0");
            for (Int_t h = 0; h < fNumParam; h++)
            {
                fTotCal_Par->SetCryCalParams(f1->GetParameter(h), fNumParam * i + h);
            }
        }

        else
        {
            LOG(WARNING) << "R3BCalifaCrystalCal2TotCalPar::Histogram NO Fitted number " << i + 1;
        }

    fTotCal_Par->setChanged();
    return;
}

ClassImp(R3BCalifaCrystalCal2TotCalPar)
