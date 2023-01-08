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

#include "R3BFi31DigitizerCal.h"
#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"

// includes for modeling
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"
#include "TGeoManager.h"
#include "TGeoMaterial.h"
#include "TGeoMatrix.h"
#include "TGeoMedium.h"
#include "TGeoShapeAssembly.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

#include "R3BFibPoint.h"
#include "R3BMCTrack.h"

using std::cout;
using std::endl;

R3BFi31DigitizerCal::R3BFi31DigitizerCal()
    : FairTask("R3B Fi31 Digitization scheme ", 1)
    , fFi31Points(NULL)
{
}

R3BFi31DigitizerCal::R3BFi31DigitizerCal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fFi31Points(NULL)
{
}

R3BFi31DigitizerCal::~R3BFi31DigitizerCal() {}

InitStatus R3BFi31DigitizerCal::Init()
{

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init:No FairRootManager";
    fFi31Points = (TClonesArray*)ioman->GetObject("Fi31Point");

    fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

    // Register output array Fib13Digi
    fFi31Cals = new TClonesArray("R3BFiberMAPMTCalData", 1000);
    ioman->Register("Fi31Cal", "Digital response in Fi31", fFi31Cals, kTRUE);

    fFi31TriggerCals = new TClonesArray("R3BFiberMAPMTCalData", 1000);
    ioman->Register("Fi31TriggerCal", "Digital response in Fi31", fFi31TriggerCals, kTRUE);

    // for sigmas
    prnd = new TRandom3();

    return kSUCCESS;
}

void R3BFi31DigitizerCal::Exec(Option_t* opt)
{
    Reset();

    auto Digitize = [this](TClonesArray* Points, TClonesArray* Hits, TClonesArray* TriggerHits, Int_t NumOfFibers) {
        Int_t entryNum = Points->GetEntries();

        if (!entryNum)
            return;

        // creating the storage for energy and time for each fiber
        std::vector<Double_t>* energy = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* time = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* y = new std::vector<Double_t>[NumOfFibers];

        for (Int_t i = 0; i < NumOfFibers; ++i)
        {

            time[i].push_back(99999);
            energy[i].push_back(0);
            y[i].push_back(0);
        }
        // Sim data:
        struct TempHit
        {
            Int_t fiberID;
            Double_t Energy;
            Double_t Time;
            Double_t Y;

            TempHit(Int_t id, Double_t e, Double_t t, Double_t y)
                : fiberID(id)
                , Energy(e)
                , Time(t)
                , Y(y)
            {
            }
        };

        // ordering the hits in time

        std::vector<TempHit> TempHits;

        for (Int_t i = 0; i < entryNum; ++i)
        {
            R3BFibPoint* data_element = (R3BFibPoint*)Points->At(i);

            TempHits.push_back(TempHit(data_element->GetDetectorID(), // fiber nummer
                                       data_element->GetEnergyLoss(),
                                       data_element->GetTime(),
                                       data_element->GetYIn()));
        }

        std::sort(TempHits.begin(), TempHits.end(), [](const TempHit& lhs, const TempHit& rhs) {
            return lhs.Time < rhs.Time;
        });

        // registering the hits in the PMTs

        for (TempHit& Hit : TempHits) // loop over full range of TempHits; Hit is reference to each TempHit
        {
            if (Hit.Energy < 0.0001)
            {
                continue;
            }

            Int_t fiberID = Hit.fiberID;

            // pile-up simulation:
            if (Hit.Time - time[fiberID].back() < 30.)
            {
                energy[fiberID].back() += Hit.Energy;

                y[fiberID].back() = (time[fiberID].back() > Hit.Time) ? Hit.Y : y[fiberID].back();

                time[fiberID].back() = (time[fiberID].back() > Hit.Time) ? Hit.Time : time[fiberID].back();
            }
            else
            {
                energy[fiberID].push_back(Hit.Energy);
                time[fiberID].push_back(Hit.Time);
                y[fiberID].push_back(Hit.Y);
            }
        }

        // creating the final hits

        Int_t ichanMA = -10, ichanSA = -10;
        Double_t yrnd, yns, ToT_MA, ToT_SA, ernd, ens, timernd, timeMA_lead = -1., timeMA_trail = -1.,
                                                                timeSA_lead = -1., timeSA_trail = -1.;
        Bool_t first = true;
        for (Int_t i = 0; i < NumOfFibers; ++i)
        {
            for (Double_t& energyl : energy[i])
            {
                if (energyl > 0.0001)
                {

                    /* ifib num -> ichan num: Based on what Hans did in BunchedFiberCal2Hit:
                     * How to get a fiber ID for a fiber detector defined as:
                     *  SubNum = 2
                     *  MAPMT = 256
                     *  SPMT = 2
                     * This means we'll receive 512 MAPMT channels as 1..512, and 4 SPMT
                     * channels, but the first half (sub-detector) is completely
                     * decoupled from the second half. The sequence of all fibers in
                     * order is then, as (MAPMT,SPMT)-pairs:
                     *  (1,1), (1,2), (2,1), ... (256,1), (256,2),
                     *  (257,3), (257,4), (258,3), ... (512,3), (512,4)
                     */

                    ichanMA = i + 1; // iFib = 0...., iCha=1...
                    ichanSA = ichanMA;
                    /*
                    if (i < 256)
                        ichanSA = 1;
                    if (i > 255)
                        ichanSA = 2;
                      */

                    /* From y-position get ToT_MA and ToT_SA only considering absorption;
                     * ToT_MA = energyl*exp(-l/labs), labs assume 100cm
                     *
                     */
                    yrnd = prnd->Gaus((y[i].at(&energyl - energy[i].data())), ysigma);
                    ernd = prnd->Gaus(energyl, esigma) * 1000. * 2.; // GeV->MeV
                    ToT_MA =
                        ernd * exp(-(25. - yrnd) / 100.); // just to come to the tot region we measure in experiment
                    ToT_SA = ernd * exp(-(25. + yrnd) / 100.);
                    ens = sqrt(ToT_MA * ToT_SA);

                    /* Now, make from ToT and time leading and trailing times:
                     * time=(timeMA_lead+timeSA_lead)/2, and timeMA_lead-timeSA_lead=y => timeMA_lead, timeSA_lead
                     * time_trail = time_lead+ToT
                     */
                    timernd = prnd->Gaus(time[i].at(&energyl - energy[i].data()),
                                         tsigma); //*3. just to get some larger values
                    Double_t veff = 12.;
                    timeMA_lead = timernd - 25. / veff + (25. - yrnd) / veff;
                    timeSA_lead = timernd - 25. / veff + (25. + yrnd) / veff;
                    timeMA_trail = timeMA_lead + ToT_MA;
                    timeSA_trail = timeSA_lead + ToT_SA;

                    /* CalData format:                      --> in Tree
                        fIsMAPMT(a_is_mapmt) 0-1            --> FibXYCal.fIsMAPMT
                       ,fChannel(a_channel)  1-SubNum*256   --> FibXYCal.fChannel
                       ,fIsLeading(a_is_leading) 0-1        --> FibXY.fIsLeading
                       ,fTime_ns(a_time_ns)                 --> FibXY.fTime_ns
                    0-bottom, 1-top, 2-trigger
                    */

                    new ((*Hits)[Hits->GetEntries()]) // MAPMT leading
                        R3BFiberMAPMTCalData(1, ichanMA, 1, timeMA_lead);

                    new ((*Hits)[Hits->GetEntries()]) // MAPMT trailing
                        R3BFiberMAPMTCalData(1, ichanMA, 0, timeMA_trail);

                    new ((*Hits)[Hits->GetEntries()]) // SAPMT leading
                        R3BFiberMAPMTCalData(0, ichanMA, 1, timeSA_lead);

                    new ((*Hits)[Hits->GetEntries()]) // SAPMT trailing
                        R3BFiberMAPMTCalData(0, ichanMA, 0, timeSA_trail);
                    if (first)
                    {
                        for (Int_t j = 0; j < 8; j++)
                        {
                            new ((*TriggerHits)[TriggerHits->GetEntries()]) R3BFiberMAPMTCalData(2, j + 1, 1, 0.);
                        }
                        first = false;
                    }
                }
            }
        }

        delete[] energy;
        delete[] time;
        delete[] y;
    };

    // running the digitizer for the Fi detectors

    if (fFi31Points)
    {
        Digitize(fFi31Points, fFi31Cals, fFi31TriggerCals, fiber_nbr);
    }
}
// -------------------------------------------------------------------------

void R3BFi31DigitizerCal::Reset()
{
    if (fFi31Cals)
        fFi31Cals->Clear();
    if (fFi31TriggerCals)
        fFi31TriggerCals->Clear();
}

void R3BFi31DigitizerCal::Finish() {}

ClassImp(R3BFi31DigitizerCal)
