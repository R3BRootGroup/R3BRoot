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

#include "R3BCalifaDigitizer.h"
#include "FairRootManager.h"
#include "R3BCalifa.h"
#include "R3BCalifaCrystalCalData.h"
#include "R3BCalifaPoint.h"
#include "TArrayD.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <iostream>
#include <stdlib.h>

using std::cerr;
using std::cout;
using std::endl;

R3BCalifaDigitizer::R3BCalifaDigitizer()
    : FairTask("R3B CALIFA Digitizer")
    , fCalifaPointDataCA(NULL)
    , fCalifaCryCalDataCA(NULL)
    , fNonUniformity(0)
{
    fNonUniformity = 0.; // perfect crystals
    fResolution = 0.;    // perfect crystals
    fComponentRes = 0.;  // perfect crystals
    fThreshold = 0.;     // no threshold
}

R3BCalifaDigitizer::R3BCalifaDigitizer(const TString& geoFile)
    : FairTask("R3B CALIFA Digitizer")
    , fCalifaPointDataCA(NULL)
    , fCalifaCryCalDataCA(NULL)
    , fNonUniformity(0)
{
    fNonUniformity = 0.; // perfect crystals
    fResolution = 0.;    // perfect crystals
    fComponentRes = 0.;  // perfect crystals
    fThreshold = 0.;     // no threshold
}

R3BCalifaDigitizer::~R3BCalifaDigitizer()
{
    LOG(INFO) << "R3BCalifaDigitizer: Delete instance";

    if (fCalifaPointDataCA)
    {
        fCalifaPointDataCA->Delete();
        delete fCalifaPointDataCA;
    }
    if (fCalifaCryCalDataCA)
    {
        fCalifaCryCalDataCA->Delete();
        delete fCalifaCryCalDataCA;
    }
}

InitStatus R3BCalifaDigitizer::Init()
{
    LOG(INFO) << "R3BCalifaDigitizer::Init ";

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
        LOG(fatal) << "Init: No FairRootManager";

    fCalifaPointDataCA = (TClonesArray*)rootManager->GetObject("CrystalPoint");
    if (!fCalifaPointDataCA)
    {
        LOG(fatal) << "Init: No CrystalPoint CA";
        return kFATAL;
    }

    fCalifaCryCalDataCA = new TClonesArray("R3BCalifaCrystalCalData", 10);
    rootManager->Register("CalifaCrystalCalData", "CALIFA Crystal Cal", fCalifaCryCalDataCA, kTRUE);

    return kSUCCESS;
}

void R3BCalifaDigitizer::Exec(Option_t* option)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Point data --
    Int_t nHits = fCalifaPointDataCA->GetEntries();
    if (!nHits)
        return;

    R3BCalifaPoint** pointData = NULL;
    pointData = new R3BCalifaPoint*[nHits];
    for (Int_t i = 0; i < nHits; i++)
        pointData[i] = (R3BCalifaPoint*)(fCalifaPointDataCA->At(i));

    Int_t crystalId;
    Double_t Nf;
    Double_t Ns;
    Double_t time;
    Double_t energy;

    for (Int_t i = 0; i < nHits; i++)
    {
        crystalId = pointData[i]->GetCrystalId();
        Nf = pointData[i]->GetNf();
        Ns = pointData[i]->GetNs();
        time = pointData[i]->GetTime();
        energy = pointData[i]->GetEnergyLoss();

        Int_t nCrystalCals = fCalifaCryCalDataCA->GetEntriesFast();
        Bool_t existHit = 0;
        if (nCrystalCals == 0)
            AddCrystalCal(crystalId, NUSmearing(energy), Nf, Ns, time, 0);
        else
        {
            for (Int_t j = 0; j < nCrystalCals; j++)
            {
                if (((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(j)))->GetCrystalId() == crystalId)
                {
                    ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(j)))->AddMoreEnergy(NUSmearing(energy));
                    ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(j)))->AddMoreNf(Nf);
                    ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(j)))->AddMoreNs(Ns);
                    if (((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(j)))->GetTime() > time)
                    {
                        ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(j)))->SetTime(time);
                    }
                    existHit = 1; // to avoid the creation of a new CrystalHit

                    break;
                }
            }
            if (!existHit)
                AddCrystalCal(crystalId, NUSmearing(energy), Nf, Ns, time, 0);
        }
        existHit = 0;
    }

    if (pointData)
        delete[] pointData;

    Int_t nCrystalCals = fCalifaCryCalDataCA->GetEntriesFast();
    if (nCrystalCals == 0)
        return;

    Double_t temp = 0;

    for (Int_t i = 0; i < nCrystalCals; i++)
    {
        temp = ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(i)))->GetEnergy();
        if (temp < fThreshold)
        {
            fCalifaCryCalDataCA->RemoveAt(i); // remove from CalData those below threshold
            continue;
        }
        if (fResolution > 0)
            ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(i)))->SetEnergy(ExpResSmearing(temp));
        if (fComponentRes > 0)
        {
            temp = ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(i)))->GetNf();
            ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(i)))->SetNf(CompSmearing(temp));
            temp = ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(i)))->GetNs();
            ((R3BCalifaCrystalCalData*)(fCalifaCryCalDataCA->At(i)))->SetNs(CompSmearing(temp));
        }
    }
}

// -----   Public method EndOfEvent   -----------------------------------------
void R3BCalifaDigitizer::EndOfEvent()
{
    // fCalifaPointDataCA->Clear();
    // fCalifaCryCalDataCA->Clear();
    ResetParameters();
}

void R3BCalifaDigitizer::Register()
{
    FairRootManager::Instance()->Register("CrystalCal", GetName(), fCalifaCryCalDataCA, kTRUE);
}

void R3BCalifaDigitizer::Reset()
{
    // Clear the CA structure
    LOG(DEBUG) << "Clearing CalifaCrystalCalData Structure";
    if (fCalifaCryCalDataCA)
        fCalifaCryCalDataCA->Clear();

    ResetParameters();
}

void R3BCalifaDigitizer::FinishEvent() {}

void R3BCalifaDigitizer::SetDetectionThreshold(Double_t thresholdEne)
{
    fThreshold = thresholdEne;
    LOG(INFO) << "R3BCalifaDigitizer::SetDetectionThreshold to " << fThreshold << " GeV.";
}

R3BCalifaCrystalCalData* R3BCalifaDigitizer::AddCrystalCal(Int_t ident,
                                                           Double_t energy,
                                                           Double_t Nf,
                                                           Double_t Ns,
                                                           ULong64_t time,
                                                           Double_t tot_energy)
{
    TClonesArray& clref = *fCalifaCryCalDataCA;
    Int_t size = clref.GetEntriesFast();
    if (fVerbose > 1)
        LOG(INFO) << "-I- R3BCalifaDigitizer: Adding CrystalCalData "
                  << " with unique identifier " << ident << " entering with " << energy * 1e06 << " keV Nf=" << Nf
                  << " Ns=" << Ns << " Time=" << time << " tot_energy=" << tot_energy;

    return new (clref[size]) R3BCalifaCrystalCalData(ident, energy, Nf, Ns, time, tot_energy);
}

void R3BCalifaDigitizer::SetExpEnergyRes(Double_t crystalRes)
{
    fResolution = crystalRes;
    LOG(INFO) << "R3BCalifaDigitizer::SetExpEnergyRes to " << fResolution << "% @ 1 MeV.";
}

void R3BCalifaDigitizer::SetComponentRes(Double_t componentRes)
{
    fComponentRes = componentRes;
    LOG(INFO) << "R3BCalifaDigitizer::SetComponentRes to " << fComponentRes;
}

Double_t R3BCalifaDigitizer::NUSmearing(Double_t inputEnergy)
{
    // Very simple preliminary scheme where the NU is introduced as a flat random
    // distribution with limits fNonUniformity (%) of the energy value.
    //
    return gRandom->Uniform(inputEnergy - inputEnergy * fNonUniformity / 100,
                            inputEnergy + inputEnergy * fNonUniformity / 100);
}

void R3BCalifaDigitizer::SetNonUniformity(Double_t nonU)
{
    fNonUniformity = nonU;
    LOG(INFO) << "R3BCalifaDigitizer::SetNonUniformity to " << fNonUniformity << " %";
}

Double_t R3BCalifaDigitizer::ExpResSmearing(Double_t inputEnergy)
{
    // Smears the energy according to some Experimental Resolution distribution
    // Very simple  scheme where the Experimental Resolution
    // is introduced as a gaus random distribution with a width given by the
    // parameter fResolution(in % @ MeV). Scales according to 1/sqrt(E)
    //
    // The formula is   TF1("name","0.058*x/sqrt(x)",0,10) for 3% at 1MeV (3.687 @ 662keV)
    //  ( % * energy ) / sqrt( energy )
    // and then the % is given at 1 MeV!!
    //
    if (fResolution == 0)
        return inputEnergy;
    else
    {
        // Energy in MeV, that is the reason for the factor 1000...
        Double_t randomIs = gRandom->Gaus(0, inputEnergy * fResolution * 1000 / (235 * sqrt(inputEnergy * 1000)));
        return inputEnergy + randomIs / 1000;
    }
}

Double_t R3BCalifaDigitizer::CompSmearing(Double_t inputComponent)
{
    // Smears the components Ns and Nf according to fComponentRes
    //
    if (fComponentRes == 0)
        return inputComponent;
    else
    {
        Double_t randomIs =
            gRandom->Gaus(0, inputComponent * fComponentRes * 1000 / (235 * sqrt(inputComponent * 1000)));
        return inputComponent + randomIs / 1000;
    }
}
