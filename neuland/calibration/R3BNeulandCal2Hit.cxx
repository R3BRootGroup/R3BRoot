#include "R3BNeulandCal2Hit.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "R3BNeulandHitPar.h"
#include "R3BTCalPar.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TGraph2D.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

R3BNeulandCal2Hit::R3BNeulandCal2Hit()
    : FairTask("R3BNeulandCal2Hit", 0)
    , fCalData("NeulandCalData")
    , fHits("NeulandHits")
    , fFirstPlaneHorizontal(false)
{
}

InitStatus R3BNeulandCal2Hit::Init()
{
    fCalData.Init();
    fHits.Init();

    SetParameter();
    return kSUCCESS;
}

void R3BNeulandCal2Hit::SetParContainers()
{
    FairRuntimeDb* rtdb = FairRunAna::Instance()->GetRuntimeDb();
    fPar = (R3BNeulandHitPar*)rtdb->getContainer("NeulandHitPar");
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
        tempMapVeff[id] = fModulePar->GetEffectiveSpeed();
        tempMapTSync[id] = fModulePar->GetTimeOffset();
        tempMapEGain[id] = fModulePar->GetEnergieGain();
    }

    LOG(INFO) << "R3BNeulandCal2Hit::SetParameter : Number of Parameters: " << fPar->GetNumModulePar()
              << FairLogger::endl;

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
    Int_t id = 0;
    if (fFirstPlaneHorizontal)
    {
        id = 1;
    }

    auto calData = fCalData.Retrieve();

    // Sides 1 and two mixed in container. Group side 1 and side 2 together, get iterator to first side 2 data
    const auto endSide1StartSide2 =
        std::partition(calData.begin(), calData.end(), [](const R3BNeulandCalData* c) { return c->GetSide() == 1; });

    std::vector<R3BNeulandHit> hits;
    // Loop over side 1
    for (auto c1 = calData.begin(); c1 != endSide1StartSide2; c1++)
    {
        const auto pmt1 = *c1;

        Int_t barId = pmt1->GetBarId();
        if (!fMapIsSet[(barId - 1) * 2])
        {
            continue;
        }

        // Find matching side 2
        auto c2 = std::find_if(
            endSide1StartSide2, calData.end(), [&](const R3BNeulandCalData* c) { return c->GetBarId() == barId; });
        if (c2 == calData.end())
        {
            // No matching side 2 found
            continue;
        }

        const auto pmt2 = *c2;

        const Double_t qdcL = pmt1->GetQdc() * fMapEGain[pmt1->GetBarId() * 2 - 2];
        const Double_t qdcR = pmt2->GetQdc() * fMapEGain[pmt1->GetBarId() * 2 - 1];
        const Double_t qdc = TMath::Sqrt(qdcL * qdcR);

        const Double_t tdcL = pmt1->GetTime() + fMapTSync[pmt1->GetBarId() * 2 - 2];
        const Double_t tdcR = pmt2->GetTime() + fMapTSync[pmt2->GetBarId() * 2 - 1];
        const Double_t tdc = (tdcL + tdcR) / 2.;

        const Double_t veff = fMapVeff[(barId - 1) * 2];
        const Int_t plane = ((barId - 1) / 50) + 1;

        Double_t x, y, z;
        if (id == plane % 2)
        {
            x = veff * (tdcR - tdcL);
            y = (barId - 0.5 - (plane - 1) * 50) * 5. - 125.;
        }
        else
        {
            x = (barId - 0.5 - (plane - 1) * 50) * 5. - 125.;
            y = veff * (tdcR - tdcL);
        }
        z = (plane - 0.5) * 5. + fDistanceToTarget;

        hits.emplace_back(R3BNeulandHit{ barId, tdcL, tdcR, tdc, qdcL, qdcR, qdc, { x, y, z }, {} });
    }
    fHits.Store(hits);
}

ClassImp(R3BNeulandCal2Hit)
