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

// ----------------------------------------------------------------
// -----         R3BMwpc0Cal2Hit source file                  -----
// -----     Created 09/10/19 by J.L. Rodriguez-Sanchez       -----
// ----------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TMath.h>

// FAIR headers
#include <FairRootManager.h>
#include <FairRuntimeDb.h>

// R3B headers
#include "R3BLogger.h"
#include "R3BMwpc0Cal2Hit.h"
#include "R3BMwpcCalData.h"
#include "R3BMwpcHitData.h"

// R3BMwpc0Cal2Hit: Default Constructor --------------------------
R3BMwpc0Cal2Hit::R3BMwpc0Cal2Hit()
    : R3BMwpc0Cal2Hit("R3BMwpc0Cal2Hit", 1)
{
}

// R3BMwpc0Cal2Hit: Standard Constructor --------------------------
R3BMwpc0Cal2Hit::R3BMwpc0Cal2Hit(const std::string& name, int iVerbose)
    : FairTask(name.c_str(), iVerbose)
{
}

// Virtual R3BMwpc0Cal2Hit: Destructor
R3BMwpc0Cal2Hit::~R3BMwpc0Cal2Hit()
{
    R3BLOG(debug, "Delete instance");
    if (fMwpcHitDataCA)
        delete fMwpcHitDataCA;
}

// -----   Public method Init   --------------------------------------------
InitStatus R3BMwpc0Cal2Hit::Init()
{
    R3BLOG(info, "");

    // INPUT DATA
    FairRootManager* rootManager = FairRootManager::Instance();
    if (!rootManager)
    {
        R3BLOG(fatal, "FairRootManager not found");
        return kFATAL;
    }

    fMwpcCalDataCA = dynamic_cast<TClonesArray*>(rootManager->GetObject("Mwpc0CalData"));
    if (!fMwpcCalDataCA)
    {
        R3BLOG(fatal, "Mwpc0CalData not found");
        return kFATAL;
    }

    // OUTPUT DATA
    // Hit data
    fMwpcHitDataCA = new TClonesArray("R3BMwpcHitData");
    rootManager->Register("Mwpc0HitData", "MWPC0 Hit", fMwpcHitDataCA, !fOnline);

    return kSUCCESS;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BMwpc0Cal2Hit::ReInit() { return kSUCCESS; }

// -----   Public method Execution   --------------------------------------------
void R3BMwpc0Cal2Hit::Exec(Option_t* /*opt*/)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Reading the Input -- Cal Data --
    Int_t nHits = fMwpcCalDataCA->GetEntriesFast();
    if (nHits == 0)
        return;

    // Data from cal level
    Int_t padmx = -1, padmy = -1;
    Double_t qmx = 0., qmy = 0., qleft = 0., qright = 0., qdown = 0., qup = 0.;
    Double_t x = NAN, y = NAN;

    for (Int_t i = 0; i < Mw0PadsX; i++)
        fx[i] = 0;
    for (Int_t i = 0; i < Mw0PadsY; i++)
        fy[i] = 0;

    for (Int_t i = 0; i < nHits; i++)
    {
        auto calData = dynamic_cast<R3BMwpcCalData*>(fMwpcCalDataCA->At(i));
        auto planeId = calData->GetPlane();
        auto padId = calData->GetPad() - 1;
        auto q = calData->GetQ();
        if (planeId == 1)
            fx[padId] = q;
        else
            fy[padId] = q;
        if (q > qmx && planeId == 1)
        {
            qmx = q;
            padmx = padId;
        }
        if (q > qmy && planeId == 3)
        {
            qmy = q;
            padmy = padId;
        }
    }
    // Add Hit data ----
    if (padmx > 1 && padmy > 1 && padmx + 1 < Mw0PadsX && padmy + 1 < Mw0PadsY && qmx > 0 && qmy > 0)
    {
        // Obtain position X ----
        qleft = (Double_t)fx[padmx - 1];
        qright = (Double_t)fx[padmx + 1];
        // std::cout<<qleft<<" "<<qright<<std::endl;
        if (qleft > 0 && qright > 0)
            x = GetPositionX(qmx, padmx, qleft, qright);

        // Obtain position Y ----
        qdown = fy[padmy - 1];
        qup = fy[padmy + 1];
        if (qdown > 0 && qup > 0)
            y = GetPositionY(qmy, padmy, qdown, qup);
        // std::cout << x << " " << y << std::endl;
        AddHitData(x, y);
    }
    return;
}

// -----   Protected method to obtain the position X ----------------------------
Double_t R3BMwpc0Cal2Hit::GetPositionX(Double_t qmax, Int_t padmax, Double_t qleft, Double_t qright)
{
    Double_t a3 = TMath::Pi() * fwx / (TMath::ACosH(0.5 * (TMath::Sqrt(qmax / qleft) + TMath::Sqrt(qmax / qright))));
    // Double_t a2 =gRandom->Uniform(-fwx / 2, fwx / 2);
    Double_t a2 = (a3 / TMath::Pi()) * TMath::ATanH((TMath::Sqrt(qmax / qleft) - TMath::Sqrt(qmax / qright)) /
                                                    (2 * TMath::SinH(TMath::Pi() * fwx / a3)));

    return (-1. * padmax * fwx + (fSize / 2) - (fwx / 2) - a2); // Left is positive and right negative
}

// -----   Protected method to obtain the position Y ----------------------------
Double_t R3BMwpc0Cal2Hit::GetPositionY(Double_t qmax, Int_t padmax, Double_t qdown, Double_t qup)
{
    Double_t a3 = TMath::Pi() * fwy / (TMath::ACosH(0.5 * (TMath::Sqrt(qmax / qdown) + TMath::Sqrt(qmax / qup))));
    // Double_t a2 =gRandom->Uniform(-fwy / 2, fwy / 2);
    Double_t a2 = (a3 / TMath::Pi()) * TMath::ATanH((TMath::Sqrt(qmax / qdown) - TMath::Sqrt(qmax / qup)) /
                                                    (2 * TMath::SinH(TMath::Pi() * fwy / a3)));

    return (padmax * fwy - (fSize / 2) + (fwy / 2) + a2);
}

// -----   Public method Reset   ------------------------------------------------
void R3BMwpc0Cal2Hit::Reset()
{
    R3BLOG(debug, "Clearing Mwpc0HitData Structure");
    if (fMwpcHitDataCA)
        fMwpcHitDataCA->Clear();
}

// -----   Private method AddHitData  --------------------------------------------
R3BMwpcHitData* R3BMwpc0Cal2Hit::AddHitData(Double_t x, Double_t y)
{
    // It fills the R3BMwpcHitData
    TClonesArray& clref = *fMwpcHitDataCA;
    Int_t size = clref.GetEntriesFast();
    return new (clref[size]) R3BMwpcHitData(x, y);
}

ClassImp(R3BMwpc0Cal2Hit)
