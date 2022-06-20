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

// ---------------------------------------------------------------
// -----                    R3BFiberDigitizer                -----
// -----          Created 10/05/2022 by J.L. Rodriguez       -----
// ---------------------------------------------------------------

#include "R3BFiberDigitizer.h"
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "R3BLogger.h"
#include "TClonesArray.h"
#include "R3BTGeoPar.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"
#include <iostream>
#include <string>
#include "R3BFibPoint.h"
#include "R3BMCTrack.h"

R3BFiberDigitizer::R3BFiberDigitizer(const TString& name)
    : FairTask("R3B" + name + "Digitizer")
    , fName(name)
    , fFiPoints(NULL)
    , fFiHits(NULL)
{
    esigma = 0.001;
    tsigma = 0.01;
    ysigma = 1;
    fiber_thickness = 0.10000;
    fiber_nbr = 512;
    air_layer = 0.01; // relative to fiber_thickness
    detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);
}

R3BFiberDigitizer::R3BFiberDigitizer(const TString& name, Double_t e, Double_t t, Double_t y)
    : FairTask("R3B" + name + "Digitizer")
    , fName(name)
    , fFiPoints(NULL)
    , fFiHits(NULL)
{
    esigma = e;
    tsigma = t;
    ysigma = y;
    fiber_thickness = 0.10000;
    fiber_nbr = 512;
    air_layer = 0.01; // relative to fiber_thickness
    detector_width = fiber_nbr * fiber_thickness * (1 + air_layer);
}

R3BFiberDigitizer::~R3BFiberDigitizer()
{
    if (fFiPoints)
        delete[] fFiPoints;
}

void R3BFiberDigitizer::SetEnergyResolution(Double_t e) { esigma = e; }

void R3BFiberDigitizer::SetTimeResolution(Double_t t) { tsigma = t; }

void R3BFiberDigitizer::SetYPositionResolution(Double_t y) { ysigma = y; }

void R3BFiberDigitizer::SetParContainers()
{
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    fFiGeoPar = (R3BTGeoPar*)rtdb->getContainer(fName+"GeoPar");
    if (!fFiGeoPar)
    {
        LOG(ERROR) << "R3BFiberDigitizer::SetParContainers() : Could not get access to "+fName+"GeoPar container.";
        return;
    }
    else
        LOG(INFO) << "R3BFiberDigitizer::SetParContainers() : Container "+fName+"GeoPar found.";
}

void R3BFiberDigitizer::SetParameter()
{
    if (fFiGeoPar){
      ysigma = fFiGeoPar->GetSigmaY();
      xsigma = fFiGeoPar->GetSigmaX();
      fRot.RotateX(-fFiGeoPar->GetRotX() * TMath::DegToRad());
      fRot.RotateY(-fFiGeoPar->GetRotY() * TMath::DegToRad());
      fRot.RotateZ(-fFiGeoPar->GetRotZ() * TMath::DegToRad());

      fTrans.SetXYZ(fFiGeoPar->GetPosX(), fFiGeoPar->GetPosY(), fFiGeoPar->GetPosZ());
    }
}

InitStatus R3BFiberDigitizer::Init()
{
    R3BLOG(INFO, "for fiber " << fName);
    // Get input array
    FairRootManager* ioman = FairRootManager::Instance();
    R3BLOG_IF(FATAL, !ioman, "FairRootManager not found");

    fFiPoints = (TClonesArray*)ioman->GetObject(fName + "Point");
    R3BLOG_IF(FATAL, !fFiPoints, fName + "Point not found");

    fMCTrack = (TClonesArray*)ioman->GetObject("MCTrack");

    // Register output array DchDigi
    fFiHits = new TClonesArray("R3BBunchedFiberHitData");
    ioman->Register(fName + "Hit", "Digital response in" + fName, fFiHits, kTRUE);

    // for sigmas
    rand = new TRandom3();

    SetParameter();
    return kSUCCESS;
}

void R3BFiberDigitizer::Exec(Option_t* opt)
{
    /*Reset();

    auto Digitize = [this](TClonesArray* Points, TClonesArray* Hits, Int_t NumOfFibers) {
        Int_t entryNum = Points->GetEntries();

        if (!entryNum)
            return;

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
                    Float_t xpos =
                        -detector_width / 2. + fiber_thickness / 2. + (i + (i * air_layer)) * fiber_thickness;
                    // cout << "Test: Det = 30" << " x = " << xpos << " fiber = " << i << endl;

                    new ((*Hits)[Hits->GetEntries()])
                        R3BBunchedFiberHitData(1,
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

    if (fFiPoints)
    {
        Digitize(fFiPoints, fFiHits, fiber_nbr);
    }
    */

    Reset();
    // Reading the Input -- Point Data --
    Int_t nHits = fFiPoints->GetEntriesFast();
    if (!nHits)
        return;
    // Data from Point level
    R3BFibPoint** pointData;
    pointData = new R3BFibPoint*[nHits];
    Int_t fiber = 0;
    Int_t TrackId = 0, PID = 0, mother = -1;
    Double_t x = 0., y = 0., z = 0., time = 0.;
    TVector3 vpos;
    for (Int_t i = 0; i < nHits; i++)
    {
        pointData[i] = (R3BFibPoint*)(fFiPoints->At(i));
        TrackId = pointData[i]->GetTrackID();

        R3BMCTrack* Track = (R3BMCTrack*)fMCTrack->At(TrackId);
        PID = Track->GetPdgCode();
        // mother = Track->GetMotherId();

        if (PID > 1000401000) // Z=40 and A=100
        {
            x = (pointData[i]->GetXIn() + pointData[i]->GetXOut())/2.;
            y = (pointData[i]->GetYIn() + pointData[i]->GetYOut())/2.;
            z = (pointData[i]->GetZIn() + pointData[i]->GetZOut())/2.;
            vpos.SetXYZ(x, y, z);

            vpos = fRot * (vpos - fTrans);
            //vpos = fRot * (vpos);
            //time = pointData[i]->GetTime() + rand->Gaus(0., fsigma_t);

            fiber = (int) std::round(vpos.X() / fiber_thickness);
            // Add hit data
            AddHitData(1, vpos.X()*10.+rand->Gaus(0., 0.1), vpos.Y()*10.+rand->Gaus(0., 0.1),
                       1, 1, fiber, 1,1,1,1);
        }
    }
    if (pointData)
        delete[] pointData;
    return;

}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BFiberDigitizer::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}
void R3BFiberDigitizer::Reset()
{
    if (fFiHits)
        fFiHits->Clear();
}

ClassImp(R3BFiberDigitizer);
