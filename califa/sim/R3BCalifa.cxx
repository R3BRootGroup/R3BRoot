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

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairVolume.h"

#include "R3BCalifa.h"
#include "R3BCalifaGeometry.h"
#include "R3BCalifaPoint.h"
#include "R3BLogger.h"
#include "R3BMCStack.h"

#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoNode.h"
#include "TParticle.h"
#include "TVirtualMC.h"

#include <iostream>
#include <stdlib.h>

#define U_MEV 931.4940954

inline double BETA(const double M, const double E_kin) { return sqrt(1. - M * M / ((M + E_kin) * (M + E_kin))); }

inline double GAMMA(const double M, const double E_kin) { return 1. + E_kin / M; }

R3BCalifa::R3BCalifa()
    : R3BCalifa("")
{
}

R3BCalifa::R3BCalifa(const TString& geoFile, const TGeoTranslation& trans, const TGeoRotation& rot)
    : R3BCalifa(geoFile, { trans, rot })
{
}

R3BCalifa::R3BCalifa(const TString& geoFile, const TGeoCombiTrans& combi)
    : R3BDetector("R3BCalifa", kCALIFA, geoFile, combi)
    , fCsIDensity(0.)
    , fCalifaGeo(NULL)
{
    ResetParameters();
    fCalifaCollection = new TClonesArray("R3BCalifaPoint");
    flGeoPar = new TList();
    flGeoPar->SetName(GetName());
    fGeometryVersion = 2020; // final BARREL+iPhos: 2020
}

R3BCalifa::~R3BCalifa()
{
    if (flGeoPar)
    {
        delete flGeoPar;
    }
    if (fCalifaCollection)
    {
        fCalifaCollection->Delete();
        delete fCalifaCollection;
    }
}

void R3BCalifa::Initialize()
{
    FairDetector::Initialize();

    R3BLOG(INFO, " ");

    TGeoVolume* vol = gGeoManager->GetVolume("CalifaWorld");
    vol->SetVisibility(kFALSE);

    if (!R3BCalifaGeometry::Instance()->Init(fGeometryVersion))
    {
        R3BLOG(ERROR, "Califa geometry not found");
    }
    return;
}

Bool_t R3BCalifa::ProcessHits(FairVolume* vol)
{
    Int_t crystalId = fCalifaGeo->GetCrystalId(gMC->CurrentVolPath());
    if (!fCsIDensity) // fill it in the first crystal
        fCsIDensity = gGeoManager->GetCurrentVolume()->GetMaterial()->GetDensity();

    if (gMC->IsTrackEntering())
    {
        fELoss = 0.;
        fNf = 0.;
        fNs = 0.;
        fNSteps = 0; // FIXME
        fTime = gMC->TrackTime() * 1.0e09;
        fLength = gMC->TrackLength();
        gMC->TrackPosition(fPosIn);
        gMC->TrackMomentum(fMomIn);
        fEinc = gMC->Etot() - gMC->TrackMass(); // be aware!! Relativistic mass!
    }

    // Sum energy loss for all steps in the active volume
    Double_t dE = gMC->Edep() * 1000.;                          // in MeV
    Double_t post_E = (gMC->Etot() - gMC->TrackMass()) * 1000.; // in MeV
    TString ptype = gMC->GetStack()->GetCurrentTrack()->GetName();
    Double_t dx = gMC->TrackStep() * fCsIDensity;

    Double_t M_in = gMC->TrackMass() * 1000.;
    Double_t A_in = M_in / U_MEV;
    Double_t Z_in = gMC->TrackCharge();

    const double Z_CsI = 54.;
    const double A_CsI = 129.905;   // g/mol
    const double E_delta = 5.30227; // MeV
    const double m_e = .5109989461; // MeV
    const double slope_e = 1.33055;
    const double K = .307075; // MeV cm**2/mol
    // quenching
    const double q_1 = 0.0396113;
    const double q_2 = -0.0828619;
    const double q_3 = 0.780435;

    fELoss += dE / 1000.; // back to GeV

    // Note: TF1s and friends are evil, because they will generally break the
    // stack when they crash (e.g. because of SIGFPE, which GEANT4 helpfully
    // activates on Tuesdays and Debug builds (->G4FPE_DEBUG).

    auto makeCompFun = [](std::array<double, 5> p) {
        return [p](double x) { return (x > 0.0) ? 1. / (p[0] + p[1] * pow(x, p[2]) + p[3] / pow(x, p[4])) : 0.0; };
    };
    auto tf_dNf_dE = makeCompFun({ -1.79, 1.36e-2, 7.84e-1, 4.97, 1.75e-1 });
    auto tf_dNs_dE = makeCompFun({ -1.24e2, 6.3e-3, 1.27, 1.262e2, 2.3e-3 });

    if (dE > 0 && dx > 0)
    {
        if (ptype != "gamma" && post_E >= A_in * E_delta)
        {
            double beta_cut = BETA(M_in, A_in * E_delta);
            double gamma_cut = GAMMA(M_in, A_in * E_delta);
            double beta = BETA(M_in, post_E);
            double gamma = GAMMA(M_in, post_E);
            double T_cut = 2. * m_e * beta_cut * beta_cut * gamma_cut * gamma_cut /
                           (1. + 2. * gamma_cut * m_e / M_in + (m_e / M_in) * (m_e / M_in));
            double T_max =
                2. * m_e * beta * beta * gamma * gamma / (1. + 2. * gamma * m_e / M_in + (m_e / M_in) * (m_e / M_in));
            double C = 0.5 * K * Z_in * Z_in * Z_CsI / (A_CsI * beta * beta);

            // quenching
            double part1 =
                q_1 / q_2 * (1 / T_max - 1 / T_cut + (log(T_cut / T_max) + log((T_max - q_2) / (T_cut - q_2)) / q_2));
            double part2 = q_1 * beta * beta / T_max * (log(T_cut / T_max) + log((T_max - q_2) / (T_cut - q_2)) / q_2);
            double N = 1 / T_cut - 1 / T_max - (beta * beta) / T_max * log(T_max / T_cut);
            double part3 = q_3 * N;
            double scaling = 1.;

            double dE_dxe = C * (log(T_max / T_cut) - beta * beta * (T_max - T_cut) / T_max);
            double dE_e = dE_dxe * dx;
            if (dE_e > dE)
            {
                dE_e = dE;
            }
            if (T_max < 2.)
            {
                scaling = (part1 + part2 + part3) / N;
            }
            else
            {
                scaling = q_3;
            }
            fNf += (dE_e * scaling / (1 + slope_e)) / 1000.;
            fNs += (dE_e * scaling / (1. / slope_e + 1)) / 1000.;
            dE -= dE_e;
        }
        fNf += tf_dNf_dE(dE / dx) * dE / 1000.;
        fNs += tf_dNs_dE(dE / dx) * dE / 1000.;
    }

    fNSteps++;

    // Set additional parameters at exit of active volume. Create R3BCalifaPoint.
    if (gMC->IsTrackExiting() || gMC->IsTrackStop() || gMC->IsTrackDisappeared())
    {
        fTrackID = gMC->GetStack()->GetCurrentTrackNumber();
        fParentTrackID = gMC->GetStack()->GetCurrentParentTrackNumber();
        fVolumeID = vol->getMCid();
        fTrackPID = gMC->TrackPid();
        fUniqueID = gMC->GetStack()->GetCurrentTrack()->GetUniqueID();
        // updating the value of the track length when exiting or stopping
        fLength = gMC->TrackLength();

        if (fELoss == 0.)
            return kFALSE;

        AddPoint(fTrackID,
                 fVolumeID,
                 crystalId,
                 TVector3(fPosIn.X(), fPosIn.Y(), fPosIn.Z()),
                 TVector3(fMomIn.Px(), fMomIn.Py(), fMomIn.Pz()),
                 fTime,
                 fLength,
                 fELoss,
                 fNf,
                 fNs,
                 gMC->CurrentEvent());

        // Increment number of CalifaPoints for this track
        R3BStack* stack = (R3BStack*)gMC->GetStack();
        stack->AddPoint(kCALIFA);
        ResetParameters();
    }
    return kTRUE;
}

void R3BCalifa::EndOfEvent()
{
    // if (fVerboseLevel > 1)
    Print();
    fCalifaCollection->Clear();
    ResetParameters();
}

void R3BCalifa::Register()
{
    FairRootManager::Instance()->Register("CrystalPoint", GetName(), fCalifaCollection, kTRUE);
}

TClonesArray* R3BCalifa::GetCollection(Int_t iColl) const
{
    if (iColl == 0)
    {
        return fCalifaCollection;
    }
    else
        return NULL;
}

void R3BCalifa::Print(Option_t* option) const
{
    Int_t nPoints = fCalifaCollection->GetEntriesFast();
    LOG(INFO) << "R3BCalifa: " << nPoints << " points registered in this event";
}

void R3BCalifa::Reset()
{
    fCalifaCollection->Clear();
    ResetParameters();
}

void R3BCalifa::CopyClones(TClonesArray* cl1, TClonesArray* cl2, Int_t offset)
{
    Int_t nEntries = cl1->GetEntriesFast();
    LOG(INFO) << "R3BCalifa: " << nEntries << " entries to add";
    TClonesArray& clref = *cl2;
    R3BCalifaPoint* oldpoint = NULL;
    for (Int_t i = 0; i < nEntries; i++)
    {
        oldpoint = (R3BCalifaPoint*)cl1->At(i);
        Int_t index = oldpoint->GetTrackID() + offset;
        oldpoint->SetTrackID(index);
        new (clref[fPosIndex]) R3BCalifaPoint(*oldpoint);
        fPosIndex++;
    }
    LOG(INFO) << "R3BCalifa: " << cl2->GetEntriesFast() << " merged entries";
}

R3BCalifaPoint* R3BCalifa::AddPoint(Int_t trackID,
                                    Int_t detID,
                                    Int_t ident,
                                    TVector3 posIn,
                                    TVector3 momIn,
                                    Double_t time,
                                    Double_t length,
                                    Double_t eLoss,
                                    Double_t Nf,
                                    Double_t Ns,
                                    UInt_t EventId)
{
    TClonesArray& clref = *fCalifaCollection;
    Int_t size = clref.GetEntriesFast();
    if (fVerboseLevel > 1)
    {
        LOG(INFO) << "R3BCalifa: Adding Point at (" << posIn.X() << ", " << posIn.Y() << ", " << posIn.Z()
                  << ") cm,  detector " << detID << ", track " << trackID << ", energy loss " << eLoss * 1e06 << " keV";
    }
    return new (clref[size]) R3BCalifaPoint(trackID, detID, ident, posIn, momIn, time, length, eLoss, Nf, Ns, EventId);
}

void R3BCalifa::SelectGeometryVersion(Int_t version) { fGeometryVersion = version; }

Bool_t R3BCalifa::CheckIfSensitive(std::string name)
{
    if (TString(name).Contains("Crystal_"))
    {
        return kTRUE;
    }
    return kFALSE;
}

ClassImp(R3BCalifa);
