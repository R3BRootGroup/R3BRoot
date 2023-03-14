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

#include "R3BFi63DigitizerHit.h"
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

R3BFi63DigitizerHit::R3BFi63DigitizerHit()
    : FairTask("R3B Fi63 Digitization scheme ")
    , fFi63Points(NULL)
{

    esigma = 0.001;
    tsigma = 0.01;
    ysigma = 1;
}

R3BFi63DigitizerHit::R3BFi63DigitizerHit(Double_t e, Double_t t, Double_t x, Double_t y)
    : FairTask("R3B Fi63 Digitization scheme ")
    , fFi63Points(NULL)
{

    esigma = e;
    tsigma = t;
    ysigma = y;
}

R3BFi63DigitizerHit::~R3BFi63DigitizerHit() {}

void R3BFi63DigitizerHit::SetEnergyResolution(Double_t e) { esigma = e; }
void R3BFi63DigitizerHit::SetTimeResolution(Double_t t) { tsigma = t; }
void R3BFi63DigitizerHit::SetXPositionResolution(Double_t x) { xsigma = x; }
void R3BFi63DigitizerHit::SetYPositionResolution(Double_t y) { ysigma = y; }

InitStatus R3BFi63DigitizerHit::Init()
{

    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    if (!ioman)
        LOG(fatal) << "Init:No FairRootManager";
    fFi63Points = (TClonesArray*)ioman->GetObject("Fi63Point");

    fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

    // Register output array DchDigi
    fFi63Hits = new TClonesArray("R3BFiberMAPMTHitData", 1000);
    ioman->Register("Fi63Hit", "Digital response in Fi63", fFi63Hits, kTRUE);

    // for sigmas
    prnd = new TRandom3();

    return kSUCCESS;
}

void R3BFi63DigitizerHit::Exec(Option_t* opt)
{
    Reset();

    auto Digitize = [this](TClonesArray* Points, TClonesArray* Hits, Int_t NumOfFibers) {
        Int_t entryNum = Points->GetEntries();

        if (!entryNum)
            return;

        // creating the storage for energy and time for each PMT
        std::vector<Double_t>* energy = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* time = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* x = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* y = new std::vector<Double_t>[NumOfFibers];
        std::vector<Double_t>* z = new std::vector<Double_t>[NumOfFibers];

        for (Int_t i = 0; i < NumOfFibers; ++i)
        {
            time[i].push_back(99999);
            energy[i].push_back(0);
            x[i].push_back(0);
            y[i].push_back(0);
            z[i].push_back(0);
        }

        struct TempHit
        {
            Int_t fiberID;
            Double_t Energy;
            Double_t Time;
            Double_t X;
            Double_t Y;
            Double_t Z;

            TempHit(Int_t id, Double_t e, Double_t t, Double_t x, Double_t y, Double_t z)
                : fiberID(id)
                , Energy(e)
                , Time(t)
                , X(x)
                , Y(y)
                , Z(z)
            {
            }
        };

        // ordering the hits in time

        std::vector<TempHit> TempHits;

        for (Int_t i = 0; i < entryNum; ++i)
        {
            R3BFibPoint* data_element = (R3BFibPoint*)Points->At(i);

            LOG(debug) << "Point Data fi63 - x: " << (data_element->GetXIn() + data_element->GetXOut()) / 2.
                       << ", y: " << (data_element->GetYIn() + data_element->GetYOut()) / 2.
                       << ", z: " << (data_element->GetZIn() + data_element->GetZOut()) / 2. << endl;

            TempHits.push_back(TempHit(data_element->GetDetectorID(),
                                       data_element->GetEnergyLoss(),
                                       data_element->GetTime(),
                                       (data_element->GetXIn() + data_element->GetXOut()) / 2.,
                                       (data_element->GetYIn() + data_element->GetYOut()) / 2.,
                                       (data_element->GetZIn() + data_element->GetZOut()) / 2.));
        }

        std::sort(TempHits.begin(), TempHits.end(), [](const TempHit& lhs, const TempHit& rhs) {
            return lhs.Time < rhs.Time;
        });

        // registering the hits in the PMTs

        for (TempHit& Hit : TempHits)
        {
            if (Hit.Energy < 1.e-12)
            {
                continue;
            }

            Int_t fiberID = Hit.fiberID;

            LOG(debug) << "Hit Fi63 in: fiber: " << Hit.fiberID << " x: " << Hit.X << " y: " << Hit.Y << " z: " << Hit.Z
                       << " Eloss: " << Hit.Energy << " t: " << Hit.Time << endl;

            if (Hit.Time - time[fiberID].back() < 30)
            {
                energy[fiberID].back() += Hit.Energy;
                x[fiberID].back() = (time[fiberID].back() > Hit.Time) ? Hit.X : x[fiberID].back();
                y[fiberID].back() = (time[fiberID].back() > Hit.Time) ? Hit.Y : y[fiberID].back();
                z[fiberID].back() = (time[fiberID].back() > Hit.Time) ? Hit.Z : z[fiberID].back();
                time[fiberID].back() = (time[fiberID].back() > Hit.Time) ? Hit.Time : time[fiberID].back();
            }
            else
            {
                energy[fiberID].push_back(Hit.Energy);
                time[fiberID].push_back(Hit.Time);
                x[fiberID].push_back(Hit.X);
                y[fiberID].push_back(Hit.Y);
                z[fiberID].push_back(Hit.Z);
            }
        }

        // creating the final hits

        for (Int_t i = 0; i < NumOfFibers; ++i)
        {
            for (Double_t& energyl : energy[i])
            {
                if (energyl > 1.e-12)
                {
                    Double_t fiber_id = i;

                    LOG(debug) << "Hit Fi63 out: fiber: " << i << " x: " << (x[i].at(&energyl - energy[i].data()))
                               << " y: " << (y[i].at(&energyl - energy[i].data()))
                               << " z: " << (z[i].at(&energyl - energy[i].data())) << " Eloss: " << energyl
                               << " t: " << time[i].at(&energyl - energy[i].data()) << endl;

                    Double_t PositionX = -145.;
                    Double_t PositionY = 0.;
                    Double_t PositionZ = 643.;
                    Double_t RotationY = -14.;

                    TVector3 posGlobal(x[i].at(&energyl - energy[i].data()),
                                       y[i].at(&energyl - energy[i].data()),
                                       z[i].at(&energyl - energy[i].data()));
                    TVector3 posDet(PositionX, PositionY, PositionZ);

                    TVector3 local = posGlobal - posDet;
                    local.RotateY(-RotationY * TMath::DegToRad());
                    Double_t x_local = local.X();
                    Double_t y_local = local.Y();

                    Double_t fiber_width = 0.1;
                    Double_t air_layer = 0.01; // relative to fiber_thickness
                    Double_t detector_width = fiber_nbr * fiber_width * (1. + air_layer);

                    Double_t xx = -detector_width / 2. + fiber_width * (1. + air_layer) / 2. +
                                  double(fiber_id) * (1. + air_layer) * fiber_width;

                    // cout<<setprecision(10) << "Hit Fi63 local-x from fiberNum: " << xx<<" from transf: " << x_local
                    // <<", diff: "<< x_local-xx<<", fID: "<<fiber_id<<
                    //", detWidth: "<<detector_width<<endl;

                    TVector3 posGlobalcheck(x_local, y_local, 0.);
                    posGlobalcheck.RotateY(RotationY * TMath::DegToRad());
                    posGlobalcheck = posGlobalcheck + posDet;

                    // cout<<setprecision(10)<< "Hit Fi63 global-dx,dy,dz: "<<posGlobalcheck.X()-posGlobal.X()<<",
                    // "<<posGlobalcheck.Y()-posGlobal.Y()<<", "<<posGlobalcheck.Z()-posGlobal.Z()<<endl;

                    Bool_t granularity = false;
                    if (granularity)
                    {
                        LOG(debug) << "y before granularity: " << y_local;
                        Double_t fiber_gran = 0.05; // cm
                        y_local = (int)((y_local + fiber_gran / 2.) / fiber_gran) * fiber_gran;
                        LOG(debug) << "y after granularity: " << y_local;
                    }

                    Int_t qcharge = (int)(470.61775 * energyl + 1.5642724 + 0.5);

                    new ((*Hits)[Hits->GetEntries()])
                        R3BFiberMAPMTHitData(1,
                                             prnd->Gaus(x_local, xsigma),
                                             prnd->Gaus(y_local, ysigma),
                                             qcharge,
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
        delete[] x;
        delete[] y;
    };

    // running the digitizer for the Fi detectors

    if (fFi63Points)
    {
        Digitize(fFi63Points, fFi63Hits, fiber_nbr);
    }
}
// -------------------------------------------------------------------------

void R3BFi63DigitizerHit::Reset()
{
    if (fFi63Hits)
        fFi63Hits->Clear();
}

void R3BFi63DigitizerHit::Finish() {}

ClassImp(R3BFi63DigitizerHit)
