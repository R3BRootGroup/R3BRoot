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

#include "R3BNeulandCal2Hit.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "R3BEventHeader.h"
#include "R3BNeulandHitPar.h"
#include "R3BTCalPar.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TGraph2D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include <cmath>

R3BNeulandCal2Hit::R3BNeulandCal2Hit()
    : FairTask("R3BNeulandCal2Hit", 0)
    , fEventHeader(nullptr)
    , fCalData("NeulandCalData")
    , fHits("NeulandHits")
    , fFirstPlaneHorizontal(true)
{
}

InitStatus R3BNeulandCal2Hit::Init()
{
    auto ioman = FairRootManager::Instance();
    if (ioman == nullptr)
    {
        throw std::runtime_error("R3BNeulandCal2Hit: No FairRootManager");
    }

    fEventHeader = (R3BEventHeader*)ioman->GetObject("R3BEventHeader");
    if (fEventHeader == nullptr)
    {
        throw std::runtime_error("R3BNeulandCal2Hit: No R3BEventHeader");
    }

    fCalData.Init();
    fHits.Init();

    SetParameter();
    return kSUCCESS;
}

void R3BNeulandCal2Hit::SetParContainers()
{
    fPar = (R3BNeulandHitPar*)FairRuntimeDb::instance()->getContainer("NeulandHitPar");
}

void R3BNeulandCal2Hit::SetParameter()
{
    std::map<Int_t, Bool_t> tempMapIsSet;
    std::map<Int_t, Double_t> tempMapVeff;
    std::map<Int_t, Double_t> tempMapTSync;
    std::map<Int_t, Double_t> tempMapEGain;

    for (Int_t i = 0; i < fPar->GetNumModulePar(); i++)
    {
        R3BNeulandHitModulePar* fModulePar = fPar->GetModuleParAt(i);
        Int_t id = fModulePar->GetModuleId() * 2 + fModulePar->GetSide() - 3;
        tempMapIsSet[id] = kTRUE;
        // ig tempMapVeff[id] = std::abs(fModulePar->GetEffectiveSpeed());
        tempMapVeff[id] = fModulePar->GetEffectiveSpeed();
        tempMapTSync[id] = fModulePar->GetTimeOffset() + fPar->GetGlobalTimeOffset();
        tempMapEGain[id] = fModulePar->GetEnergieGain();
    }

    LOG(INFO) << "R3BNeulandCal2Hit::SetParameter : Number of Parameters: " << fPar->GetNumModulePar();

    fMapIsSet = tempMapIsSet;
    fMapVeff = tempMapVeff;
    fMapTSync = tempMapTSync;
    fMapEGain = tempMapEGain;
}

InitStatus R3BNeulandCal2Hit::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

void R3BNeulandCal2Hit::Exec(Option_t*)
{
    fHits.Reset();

    Int_t id = 0;
    if (fFirstPlaneHorizontal)
    {
        id = 1;
    }

    auto calData = fCalData.Retrieve();

    const auto start = fEventHeader->GetTStart();
    const bool beam = !std::isnan(start);

    // Sides 1 and two mixed in container. Group side 1 and side 2 together, get iterator to first side 2 data
    const auto endSide1StartSide2 =
        std::partition(calData.begin(), calData.end(), [](const R3BNeulandCalData* c) { return c->GetSide() == 1; });

    Int_t bar_fired[801] = { 0 };

    // Loop over side 1
    for (auto c1 = calData.begin(); c1 != endSide1StartSide2; c1++)
    {
        const auto pmt1 = *c1;

        const Int_t barId = pmt1->GetBarId();
        if (!fMapIsSet[(barId - 1) * 2])
        {
            continue;
        }

        // Find matching side 2
        // ig auto c2 = std::find_if(
        // ig     endSide1StartSide2, calData.end(), [&](const R3BNeulandCalData* c) { return c->GetBarId() == barId;
        // }); ig if (c2 == calData.end()) ig {
        // No matching side 2 found
        // ig    continue;
        // ig}

        // igconst auto pmt2 = *c2;

        Int_t skip = 0;

        for (auto c2 = endSide1StartSide2; c2 != calData.end(); c2++) // ig
        {
            const auto pmt2 = *c2;
            if (pmt2->GetBarId() != barId)
            {
                continue;
            }
            // skip .....
            if (skip < bar_fired[barId])
            {
                skip++;
                continue;
            }

            bar_fired[barId]++;

            // According to the NeuLAND nomenclature sheet, 1 -> Right, 2 -> Left
            // TODO: Check everywhere
            // ig instead checking everywhere keep labels 1 and 2 (labels R and L
            // anyhow don't apply for up and down PMTs
            const Double_t qdc1 = pmt1->GetQdc() * fMapEGain[barId * 2 - 2];
            const Double_t qdc2 = pmt2->GetQdc() * fMapEGain[barId * 2 - 1];
            const Double_t qdc = TMath::Sqrt(qdc1 * qdc2);

            Double_t tdc1 = pmt1->GetTime() + fMapTSync[barId * 2 - 2];
            Double_t tdc2 = pmt2->GetTime() + fMapTSync[barId * 2 - 1];

            if (tdc1 - tdc2 < -0.5 * 5. * 2048)
                tdc2 = tdc2 - 5. * 2048;
            if (tdc1 - tdc2 > 0.5 * 5. * 2048)
                tdc1 = tdc1 - 5. * 2048;

            Double_t tdc = (tdc1 + tdc2) / 2. - fGlobalTimeOffset;

            if (beam)
            {
                // the shift is to get fmod to work as indented: 4 peaks -> 1 peak w/o stray data (e.g. at 5 * 2048)
                // tdc = fmod(tdc - start - 3000, 5 * 2048) + 3000;
                tdc = remainder(tdc - start - 3000, 5 * 2048) + 3000; // fmod 3000
            }
            else
            {
                tdc = std::numeric_limits<double>::quiet_NaN();
            }

            const Double_t veff = fMapVeff[(barId - 1) * 2];

            const Int_t plane = ((barId - 1) / 50) + 1;
            const Int_t normalizedBarID = barId % 50 + 1; // ig

            Double_t x, y, z;
            Double_t xx, yy, zz;
            if (id == plane % 2)
            {
                x = veff * (tdc2 - tdc1);
                xx = std::min(std::max(0., x / 5. + 25), 49.); // [-:+] -> [0:49]

                y = normalizedBarID * 5. - 127.5; // [1:50] -> [-122.5:122.5]
                yy = normalizedBarID - 1;         // [1:50] -> [0:49]
            }
            else
            {
                x = normalizedBarID * 5. - 127.5; // [1:50] -> [-122.5:122.5]
                xx = normalizedBarID - 1;         // [1:50] -> [0:49]

                y = veff * (tdc2 - tdc1);
                yy = std::min(std::max(0., y / 5. + 25), 49.); // [-:+] -> [0:49]
            }
            z = (plane - 0.5) * 5. + fDistanceToTarget;
            zz = plane - 1;

            fHits.Insert({ barId, tdc1, tdc2, tdc, qdc1, qdc2, qdc, { x, y, z }, { xx, yy, zz } });
        }
    }
}

ClassImp(R3BNeulandCal2Hit)
