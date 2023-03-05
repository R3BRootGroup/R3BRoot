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

#include "R3BFi60Digitizer.h"
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
using namespace std;

R3BFi60Digitizer::R3BFi60Digitizer()
    : FairTask("R3B Fi60 Digitization scheme ")
    , fFi60Points(NULL)
{

    esigma = 0.001;
    tsigma = 0.01;
    ysigma = 1;
}

R3BFi60Digitizer::R3BFi60Digitizer(Double_t e, Double_t t, Double_t y)
    : FairTask("R3B Fi60 Digitization scheme ")
    , fFi60Points(NULL)
{

    esigma = e;
    tsigma = t;
    ysigma = y;
}

R3BFi60Digitizer::~R3BFi60Digitizer() {}

void R3BFi60Digitizer::SetEnergyResolution(Double_t e) { esigma = e; }

void R3BFi60Digitizer::SetTimeResolution(Double_t t) { tsigma = t; }

void R3BFi60Digitizer::SetYPositionResolution(Double_t y) { ysigma = y; }

InitStatus R3BFi60Digitizer::Init()
{

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init:No FairRootManager";
    fFi60Points = (TClonesArray*)ioman->GetObject("Fi60Point");

    fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

    // Register output array DchDigi
    fFi60Hits = new TClonesArray("R3BBunchedFiberHitData", 1000);
    ioman->Register("Fi60Hit", "Digital response in Fi60", fFi60Hits, kTRUE);

    // for sigmas
    prnd = new TRandom3();

    return kSUCCESS;
}

void R3BFi60Digitizer::Exec(Option_t* opt)
{
    Reset();

    auto Digitize = [this](TClonesArray* Points, TClonesArray* Hits, Int_t NumOfFibers) {
        Int_t entryNum = Points->GetEntries();

        if (!entryNum)
            return;
        /*
                for (Int_t l = 0; l < entryNum; l++)
                {
                    R3BFibPoint* aTrack = (R3BFibPoint*)fFi60Points->At(l);

                if(aTrack->GetModule()==43 || aTrack->GetModule()==45 || aTrack->GetModule()==52)
           cout<<setprecision(10)<<"From Fi60Points: "<<l<<", "<<aTrack->GetModule()<<", "<<aTrack->GetXIn()<<";
           "<<aTrack->GetXOut()<<endl;

              }
        */
        // creating the storage for energy and time for each PMT
        std::vector<Double_t>* energy = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* time = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* y = new std::vector<Double_t>[NumOfFibers];

        for (Int_t i = 0; i < NumOfFibers; ++i)
        {
            time[i].push_back(99999);
            energy[i].push_back(0);
            y[i].push_back(0);
        }

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

            TempHits.push_back(TempHit(data_element->GetDetectorID(),
                                       data_element->GetEnergyLoss(),
                                       data_element->GetTime(),
                                       data_element->GetYIn()));
        }

        std::sort(TempHits.begin(), TempHits.end(), [](const TempHit& lhs, const TempHit& rhs) {
            return lhs.Time < rhs.Time;
        });

        // registering the hits in the PMTs

        for (TempHit& Hit : TempHits)
        {
            if (Hit.Energy < 0.0000001)
            {
                continue;
            }

            Int_t fiberID = Hit.fiberID;

            if (Hit.Time - time[fiberID].back() < 30)
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

        for (Int_t i = 0; i < NumOfFibers; ++i)
        {
            for (Double_t& energyl : energy[i])
            {
                if (energyl > 0.0000001)
                {

                    Double_t fiber_id = i;
                    Double_t xpos = -detector_width / 2. + fiber_thickness * (1 + air_layer) / 2. +
                                    fiber_id * (1 + air_layer) * fiber_thickness;

                    //        if(fiber_id==43 || fiber_id==45 || fiber_id==52) cout<<setprecision(10)<< "Test: Det = 30"
                    //        << " x = " << xpos << "lab x: "<<xpos-61.224460462455166<<" fiber = " << fiber_id <<" ,
                    //        "<<energyl<<endl;

                    new ((*Hits)[Hits->GetEntries()])
                        R3BBunchedFiberHitData(6,
                                               xpos,
                                               prnd->Gaus((y[i].at(&energyl - energy[i].data())), ysigma),
                                               prnd->Gaus(energyl, esigma),
                                               prnd->Gaus(time[i].at(&energyl - energy[i].data()), tsigma),
                                               i,
                                               0.,
                                               0.,
                                               0.,
                                               0.);
                }
            }
        }

        delete[] energy;
        delete[] time;
        delete[] y;
    };

    // running the digitizer for the Fi detectors

    if (fFi60Points)
    {
        Digitize(fFi60Points, fFi60Hits, fiber_nbr);
    }
}
// -------------------------------------------------------------------------

void R3BFi60Digitizer::Reset()
{
    if (fFi60Hits)
        fFi60Hits->Clear();
}

void R3BFi60Digitizer::Finish() {}

ClassImp(R3BFi60Digitizer)
