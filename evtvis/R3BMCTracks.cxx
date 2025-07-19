/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include "R3BMCTracks.h"
#include "R3BEventManager.h"

#include <FairEventManager.h> // for FairEventManager
#include <FairLogger.h>
#include <FairRootManager.h> // for FairRootManager

#include <TClonesArray.h>        // for TClonesArray
#include <TEveManager.h>         // for TEveManager, gEve
#include <TEvePathMark.h>        // for TEvePathMark
#include <TEveTrack.h>           // for TEveTrackList, TEveTrack
#include <TEveTrackPropagator.h> // for TEveTrackPropagator
#include <TEveVector.h>          // for TEveVector, TEveVectorT
#include <TGeoTrack.h>           // for TGeoTrack
#include <TMathBase.h>           // for Max, Min
#include <TObjArray.h>           // for TObjArray
#include <TParticle.h>           // for TParticle
#include <cstring>               // for strcmp
#include <iostream>
#include <sstream>
#include <string>

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
R3BMCTracks::R3BMCTracks(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fEveTrList(new TObjArray(16))
{
}

InitStatus R3BMCTracks::Init()
{
    if (fVerbose > 1)
        cout << "R3BMCTracks::Init()" << endl;

    auto fManager = FairRootManager::Instance();
    fTrackList = dynamic_cast<TClonesArray*>(fManager->GetObject("GeoTracks"));
    if (fTrackList == 0)
    {
        cout << "FairMCPointDraw::Init()  branch " << GetName() << " Not found! Task will be deactivated " << endl;
        SetActive(kFALSE);
    }
    if (fVerbose > 2)
        cout << "FairMCTracks::Init() get track list" << fTrackList << endl;
    if (fVerbose > 2)
        cout << "FairMCTracks::Init()  create propagator" << endl;

    fEventManager = R3BEventManager::Instance();
    if (fVerbose > 2)
        cout << "FairMCTracks::Init() get instance of FairEventManager " << endl;
    fEvent = "Current Event";
    MinEnergyLimit = fEventManager->GetEvtMinEnergy();
    MaxEnergyLimit = fEventManager->GetEvtMaxEnergy();
    PEnergy = 0;
    if (IsActive())
        return kSUCCESS;
    else
        return kERROR;
}

void R3BMCTracks::Exec(Option_t*)
{
    if (IsActive())
    {
        if (fVerbose > 1)
            cout << " FairMCTracks::Exec " << endl;
        TGeoTrack* tr;
        const Double_t* point;

        Reset();

        for (size_t i = 0; i < fTrackList->GetEntriesFast(); i++)
        {
            if (fVerbose > 2)
                cout << "FairMCTracks::Exec " << i << endl;
            tr = dynamic_cast<TGeoTrack*>(fTrackList->At(i));
            auto Par = dynamic_cast<TParticle*>(tr->GetParticle());

            PEnergy = (Par->Energy() - Par->GetCalcMass()) * 1000; //[MeV]
            MinEnergyLimit = TMath::Min(PEnergy - 10, MinEnergyLimit);
            MinEnergyLimit = TMath::Max(0.0, MinEnergyLimit);
            MaxEnergyLimit = TMath::Max(PEnergy + 10, MaxEnergyLimit);

            fEventManager->SetMaxEnergy(MaxEnergyLimit + 1);

            if (fVerbose > 2)
                cout << "MinEnergyLimit " << MinEnergyLimit << " MaxEnergyLimit " << MaxEnergyLimit << endl;
            if (fEventManager->IsPriOnly() && Par->GetMother(0) > -1)
                continue;
            if (fEventManager->GetCurrentPDG() != 0 && fEventManager->GetCurrentPDG() != tr->GetPDG())
                continue;

            if (fVerbose > 2)
                cout << "PEnergy " << PEnergy << " Min " << fEventManager->GetMinEnergy() << " Max "
                     << fEventManager->GetMaxEnergy() << endl;
            if ((PEnergy < fEventManager->GetMinEnergy()) || (PEnergy > fEventManager->GetMaxEnergy()))
                continue;

            (dynamic_cast<R3BEventManager*>(fEventManager))->AddParticlesToPdgDataBase(tr->GetPDG());
            if (fVerbose > 3)
                cout << "Particle with PDG " << tr->GetPDG() << " added to DataBase " << endl;
            if (fVerbose > 3)
                cout << "Particle  " << Par << " and propagator " << fTrPr << endl;

            Int_t Np = tr->GetNpoints();
            fTrList = GetTrGroup(Par);
            if (fVerbose > 3)
                cout << "Track list: " << fTrList << " - " << fTrList->GetLimP() << " - " << fTrList->GetMaxP() << endl;
            auto track = new TEveTrack(Par, tr->GetPDG(), fTrPr);
            if (fVerbose > 3)
                cout << "Track: " << track << " - " << track->GetPdg() << " - " << track->GetLabel() << endl;
            if (tr->GetPDG() > 5000000)
            {
                track->SetLineColor(12);
            }
            else
            {
                track->SetLineColor(fEventManager->Color(tr->GetPDG()));
            }
            track->SetLineStyle(9);

            // Set Title / Tooltip
            std::ostringstream oss;
            if (tr->GetPDG() < 5000000)
            {
                oss << "PDG: " << tr->GetPDG() << ", name: " << Par->GetTitle() << "\nTrackID: " << i
                    << ", MotherID: " << Par->GetMother(0) << "\nMass: " << Par->GetMass() * 1000. << " MeV"
                    << "\nE: " << PEnergy << " MeV"
                    << "\nStrangeness: " << Par->Strangeness() << "\nT: " << Par->T() / 1e-9 << " ns";
            }
            else
            {
                auto mass = (tr->GetPDG() / 10) % 1000;
                oss << "PDG: " << tr->GetPDG() << ", name: " << Par->GetTitle() << "\nTrackID: " << i
                    << ", MotherID: " << Par->GetMother(0) << "\nMass: " << mass << " (A)"
                    << "\nE: " << PEnergy / mass << " MeV/u"
                    << "\nStrangeness: " << Par->Strangeness() << "\nT: " << Par->T() / 1e-9 << " ns";
            }
            track->SetTitle(oss.str().c_str());

            // Set the line width depending on energy
            if ((dynamic_cast<R3BEventManager*>(fEventManager))->IsScaleByEnergy())
            {
                auto lineWidth =
                    (Int_t)(PEnergy / TMath::Min(fEventManager->GetMaxEnergy(), (Float_t)MaxEnergyLimit) * 15.0);
                if (fVerbose > 3)
                    cout << "lineWidth: " << lineWidth << " for track " << track->GetPdg() << " - " << Par->GetTitle()
                         << endl;

                if (lineWidth > 0)
                {
                    track->SetLineWidth(lineWidth);
                    if (Par->GetMother(0) > -1)
                        track->SetLineStyle(2);
                }
                else
                {
                    track->SetLineStyle(1);
                    track->SetLineWidth(1);
                }
            }

            for (size_t n = 0; n < Np; n++)
            {
                point = tr->GetPoint(n);
                track->SetPoint(n, point[0], point[1], point[2]);
                auto pos = TEveVector(point[0], point[1], point[2]);
                auto path = std::make_unique<TEvePathMark>();
                path->fV = pos;
                path->fTime = point[3];
                if (n == 0)
                {
                    auto Mom = TEveVector(Par->Px(), Par->Py(), Par->Pz());
                    path->fP = Mom;
                }
                if (fVerbose > 3)
                    cout << "Path marker added " << path.get() << endl;

                track->AddPathMark(*path);

                if (fVerbose > 3)
                    cout << "Path marker added " << path.get() << endl;
            }
            fTrList->AddElement(track);
            if (fVerbose > 3)
                cout << "track added " << track->GetName() << endl;
        }
        fEventManager->SetEvtMaxEnergy(MaxEnergyLimit);
        fEventManager->SetEvtMinEnergy(MinEnergyLimit);
        gEve->Redraw3D(kFALSE);
    }
}

void R3BMCTracks::Reset()
{
    for (size_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
    {
        auto ele = static_cast<TEveTrackList*>(fEveTrList->At(i));
        gEve->RemoveElement(ele, fEventManager);
    }
    fEveTrList->Clear();
}

TEveTrackList* R3BMCTracks::GetTrGroup(TParticle* P)
{
    fTrList = 0;
    for (size_t i = 0; i < fEveTrList->GetEntriesFast(); i++)
    {
        auto TrListIn = static_cast<TEveTrackList*>(fEveTrList->At(i));
        if (strcmp(TrListIn->GetName(), P->GetName()) == 0)
        {
            fTrList = TrListIn;
            break;
        }
    }
    if (fTrList == 0)
    {
        fTrPr = new TEveTrackPropagator();
        fTrList = new TEveTrackList(P->GetName(), fTrPr);
        fTrList->SetMainColor(fEventManager->Color(P->GetPdgCode()));
        fEveTrList->Add(fTrList);
        gEve->AddElement(fTrList, fEventManager);
        fTrList->SetRnrLine(kTRUE);
    }
    return fTrList;
}

ClassImp(R3BMCTracks)
