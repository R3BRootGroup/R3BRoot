/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <TClonesArray.h>
#include <TF1.h>
#include <TGraph.h>
#include <TH1F.h>
#include <TMath.h>
#include <TObjArray.h>
#include <TRandom.h>
#include <TSpectrum.h>
#include <iostream>
#include <memory>
#include <stdlib.h>

#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRuntimeDb.h>

#include "R3BCalifaCrystalCalPar.h"
#include "R3BCalifaMapped2CrystalCalPar.h"
#include "R3BCalifaMappedData.h"
#include "R3BCalifaMappingPar.h"
#include "R3BLogger.h"
#include "R3BShared.h"

R3BCalifaMapped2CrystalCalPar::R3BCalifaMapped2CrystalCalPar()
    : R3BCalifaMapped2CrystalCalPar("R3BCalifaMapped2CrystalCalPar", 1)
{
}

R3BCalifaMapped2CrystalCalPar::R3BCalifaMapped2CrystalCalPar(const std::string& name, int iVerbose)
    : FairTask(name.c_str(), iVerbose)
{
}

R3BCalifaMapped2CrystalCalPar::~R3BCalifaMapped2CrystalCalPar()
{
    R3BLOG(debug, "Delete instance");
    if (fEnergyPeaks)
        delete fEnergyPeaks;
}

void R3BCalifaMapped2CrystalCalPar::SetParContainers()
{
    // Parameter Container
    // Reading califaMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BCalifaMappingPar*>(rtdb->getContainer("califaMappingPar"));
    if (!fMap_Par)
    {
        R3BLOG(error, "Couldn't get handle on califaMappingPar container");
    }
    else
    {
        R3BLOG(info, "califaMappingPar container open");
    }
}

void R3BCalifaMapped2CrystalCalPar::SetParameter()
{
    R3BLOG_IF(warn, !fMap_Par, "Container califaMappingPar not found.");

    //--- Parameter Container ---
    fNumCrystals = fMap_Par->GetNumCrystals(); // Number of crystals x 2
    R3BLOG(info, "NumCry " << fNumCrystals);
    // fMap_Par->printParams();
}

InitStatus R3BCalifaMapped2CrystalCalPar::Init()
{
    R3BLOG(info, "");
    if (!fEnergyPeaks)
    {
        fEnergyPeaks = new TArrayF;
        fEnergyPeaks->Set(fNumPeaks);
    }

    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    fCalifaMappedDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("CalifaMappedData"));
    if (!fCalifaMappedDataCA)
    {
        R3BLOG(fatal, "CalifaMappedData not found");
        return kFATAL;
    }

    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    if (!rtdb)
    {
        R3BLOG(fatal, "FairRuntimeDb not found");
        return kFATAL;
    }

    fCal_Par = dynamic_cast<R3BCalifaCrystalCalPar*>(rtdb->getContainer("califaCrystalCalPar"));
    if (!fCal_Par)
    {
        R3BLOG(fatal, "Couldn't get handle on califaCrystalCalPar container");
        return kFATAL;
    }

    // Set container with mapping parameters
    SetParameter();

    // Create histograms for crystal calibration
    Int_t fright, fleft, fbins;
    fh1_Map_energy_crystal = new TH1F*[fNumCrystals];
    for (Int_t i = 0; i < fNumCrystals; i++)
        if (fMap_Par->GetInUse(i + 1) == 1)
        {
            std::string name = "fh1_Map_energy_crystal_" + std::to_string(i + 1);
            if (i < fMap_Par->GetNumCrystals() / 2)
            {
                fright = fMapHistos_right;
                fleft = fMapHistos_left;
                fbins = fMapHistos_bins;
            }
            else
            {
                fright = fMapHistos_rightp;
                fleft = fMapHistos_leftp;
                fbins = fMapHistos_binsp;
            }
            fh1_Map_energy_crystal[i] = R3B::root_owned<TH1F>(name.c_str(), name.c_str(), fbins, fleft, fright);
        }

    return kSUCCESS;
}

InitStatus R3BCalifaMapped2CrystalCalPar::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BCalifaMapped2CrystalCalPar::Exec(Option_t* /*opt*/)
{
    auto nHits = fCalifaMappedDataCA->GetEntriesFast();
    for (size_t i = 0; i < nHits; i++)
    {
        auto MapData = dynamic_cast<R3BCalifaMappedData*>(fCalifaMappedDataCA->At(i));
        auto crystalId = MapData->GetCrystalId();
        // Fill histograms
        if (fMap_Par->GetInUse(crystalId) == 1)
            fh1_Map_energy_crystal[crystalId - 1]->Fill(MapData->GetEnergy());
    }
    return;
}

void R3BCalifaMapped2CrystalCalPar::FinishTask()
{
    // Look for the peaks in each spectrum
    this->SearchPeaks();
    // fCal_Par->printParams();
    if (fDebugMode)
    {
        for (Int_t i = 0; i < fNumCrystals; i++)
        {
            if (fMap_Par->GetInUse(i + 1) == 1 && fh1_Map_energy_crystal[i]->GetEntries() > fMinStadistics)
                fh1_Map_energy_crystal[i]->Write();
        }
    }
}

void R3BCalifaMapped2CrystalCalPar::SearchPeaks()
{
    Int_t nfound = 0;
    Int_t numPars = 2; // Number of parameters=2 by default
    if (fNumParam > 0)
    {
        numPars = fNumParam;
    }

    fCal_Par->SetNumCrystals(fNumCrystals);
    fCal_Par->SetNumParametersFit(fNumParam);
    fCal_Par->GetCryCalParams()->Set(numPars * fNumCrystals);

    auto spectrum = std::make_unique<TSpectrum>(fNumPeaks);

    Int_t fright, fleft;

    for (size_t i = 0; i < fNumCrystals; i++)
        if (fMap_Par->GetInUse(i + 1) == 1)
        {
            if (fh1_Map_energy_crystal[i]->GetEntries() > fMinStadistics)
            {
                if (fDebugMode)
                    nfound = spectrum->Search(fh1_Map_energy_crystal[i], fSigma, "", fThreshold); // number of peaks
                else
                    nfound = spectrum->Search(fh1_Map_energy_crystal[i], fSigma, "goff", fThreshold);
                R3BLOG(debug, "CrystalId=" << i + 1 << " " << nfound << " " << fThreshold);
                fChannelPeaks = (Double_t*)spectrum->GetPositionX();

                Int_t idx[nfound];
                TMath::Sort(nfound, fChannelPeaks, idx, kTRUE);

                // Calibrated Spectrum
                Double_t X[nfound + 1];
                Double_t Y[nfound + 1];

                for (Int_t j = 0; j < nfound; j++)
                {
                    X[j] = fChannelPeaks[idx[nfound - j - 1]];
                    Y[j] = fEnergyPeaks->GetAt(nfound - j - 1);
                    LOG(debug) << "CrystalId=" << i + 1 << " " << j + 1 << " " << X[j + 1];
                }
                X[nfound] = 0.;
                Y[nfound] = 0.;

                if (i < fMap_Par->GetNumCrystals() / 2)
                {
                    fright = fMapHistos_right;
                    fleft = fMapHistos_left;
                }
                else
                {
                    fright = fMapHistos_rightp;
                    fleft = fMapHistos_leftp;
                }

                std::unique_ptr<TF1> f1;

                if (fNumParam > 0)
                {
                    if (fNumParam == 1)
                    {
                        f1 = std::make_unique<TF1>("f1", "[0]*x", fleft, fright);
                    }
                    else if (fNumParam == 2)
                    {
                        f1 = std::make_unique<TF1>("f1", "[0]+[1]*x", fleft, fright);
                    }
                    else if (fNumParam == 3)
                    {
                        f1 = std::make_unique<TF1>("f1", "[0]+[1]*x+[2]*pow(x,2)", fleft, fright);
                    }
                    else if (fNumParam == 4)
                    {
                        f1 = std::make_unique<TF1>("f1", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)", fleft, fright);
                    }
                    else if (fNumParam == 5)
                    {
                        f1 = std::make_unique<TF1>(
                            "f1", "[0]+[1]*x+[2]*pow(x,2)+[3]*pow(x,3)+[4]*pow(x,4)", fleft, fright);
                    }
                    else
                    {
                        R3BLOG(warn, "The number of fit parameters cannot be higher than 5");
                        return;
                    }
                }
                else
                {
                    R3BLOG(warn, "No input number of fit parameters, defaulting to number of parameters = 2");
                    f1 = std::make_unique<TF1>("f1", "[0]+[1]*x", fleft, fright);
                }

                auto graph = new TGraph(fNumPeaks, X, Y);
                graph->Fit(f1.get(), "Q");

                for (Int_t h = 0; h < numPars; h++)
                {
                    fCal_Par->SetCryCalParams(f1->GetParameter(h), numPars * i + h + 1); // 1-base
                }
            }
            else
            {
                R3BLOG(warn, "Histogram number " << i + 1 << "not Fitted");
            }
        }

    fCal_Par->setChanged();
    return;
}

ClassImp(R3BCalifaMapped2CrystalCalPar)
