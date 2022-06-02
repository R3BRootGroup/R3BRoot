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

// ------------------------------------------------------------
// -----          R3BIncomingTrackingOnlineSpectra        -----
// -----    Created 25/02/22 by J.L. Rodriguez-Sanchez    -----
// -----         Fill tracking online histograms          -----
// ------------------------------------------------------------

/*
 * This task should fill histograms for tracking before GLAD
 */

#include "R3BIncomingTrackingOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BFrsData.h"
#include "R3BLogger.h"
#include "R3BMusicHitData.h"
#include "R3BMwpcHitData.h"
#include "R3BTGeoPar.h"
#include "R3BTwimHitData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TArrow.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"

R3BIncomingTrackingOnlineSpectra::R3BIncomingTrackingOnlineSpectra()
    : R3BIncomingTrackingOnlineSpectra("IncomingTrackingOnlineSpectra", 1)
{
}

R3BIncomingTrackingOnlineSpectra::R3BIncomingTrackingOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fMwpc0HitDataCA(NULL)
    , fMwpc1HitDataCA(NULL)
    , fFrsHitDataCA(NULL)
    , fDist_acelerator_glad(3855.0)
    , fPosTarget(2773.0)
    , fWidthTarget(40.0)
    , fCutIncoming(NULL)
    , fNameCut("")
{
}

R3BIncomingTrackingOnlineSpectra::R3BIncomingTrackingOnlineSpectra(const TString& name,
                                                                   float zmin,
                                                                   float zmax,
                                                                   float aqmin,
                                                                   float aqmax)
    : FairTask("IncomingTrackingOnlineSpectra_" + name, 1)
    , fNEvents(0)
    , fMwpc0HitDataCA(NULL)
    , fMwpc1HitDataCA(NULL)
    , fFrsHitDataCA(NULL)
    , fDist_acelerator_glad(3855.0)
    , fPosTarget(2773.0)
    , fWidthTarget(40.0)
    , fNameCut(name)
{
    fCutIncoming = new TCutG(name, 5);
    fCutIncoming->SetPoint(0, zmin, aqmin);
    fCutIncoming->SetPoint(1, zmin, aqmax);
    fCutIncoming->SetPoint(2, zmax, aqmax);
    fCutIncoming->SetPoint(3, zmax, aqmin);
    fCutIncoming->SetPoint(4, zmin, aqmin);
}

R3BIncomingTrackingOnlineSpectra::~R3BIncomingTrackingOnlineSpectra()
{
    R3BLOG(DEBUG1, "Destructor");
    if (fMwpc0HitDataCA)
    {
        delete fMwpc0HitDataCA;
    }
    if (fMwpc1HitDataCA)
    {
        delete fMwpc1HitDataCA;
    }
    if (fFrsHitDataCA)
    {
        delete fFrsHitDataCA;
    }
}

// -----   Public method SetParContainers   --------------------------------
void R3BIncomingTrackingOnlineSpectra::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(FATAL, NULL == rtdb, "FairRuntimeDb not found");

    fMw0GeoPar = (R3BTGeoPar*)rtdb->getContainer("Mwpc0GeoPar");
    R3BLOG_IF(ERROR, !fMw0GeoPar, "Could not get access to Mwpc0GeoPar container.");

    // fTargetGeoPar = (R3BTGeoPar*)rtdb->getContainer("TargetGeoPar");
    // R3BLOG_IF(ERROR, !fTargetGeoPar, "Could not get access to TargetGeoPar container.");

    fMw1GeoPar = (R3BTGeoPar*)rtdb->getContainer("Mwpc1GeoPar");
    R3BLOG_IF(ERROR, !fMw1GeoPar, "Could not get access to Mwpc1GeoPar container.");
    return;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BIncomingTrackingOnlineSpectra::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

InitStatus R3BIncomingTrackingOnlineSpectra::Init()
{
    R3BLOG(INFO, "");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(FATAL, NULL == mgr, "FairRootManager not found");

    fMwpc0HitDataCA = (TClonesArray*)mgr->GetObject("Mwpc0HitData");
    if (!fMwpc0HitDataCA)
    {
        R3BLOG(FATAL, "Mwpc0HitData not found");
        return kFATAL;
    }

    fMwpc1HitDataCA = (TClonesArray*)mgr->GetObject("Mwpc1HitData");
    if (!fMwpc1HitDataCA)
    {
        R3BLOG(FATAL, "Mwpc1HitData not found");
        return kFATAL;
    }

    fFrsHitDataCA = (TClonesArray*)mgr->GetObject("FrsData");
    if (!fFrsHitDataCA)
    {
        R3BLOG(warning, "FrsData not found");
        // return kFATAL;
    }

    // Create histograms for detectors
    TString Name1;
    TString Name2;

    // Hit data, tracking plane X-Z
    cTrackingXZ = new TCanvas(
        "Tracking_before_GLAD_XZ" + fNameCut, "Tracking (Lab.) plane XZ info " + fNameCut, 10, 10, 800, 700);

    Name1 = "fh2_tracking_planeXZ" + fNameCut;
    Name2 = "Tracking (Lab.) plane XZ info " + fNameCut;
    Int_t histoYlim = 150;
    fh2_tracking_planeXZ = new TH2F(Name1, Name2, 400, 0., fDist_acelerator_glad, 400, -1. * histoYlim, histoYlim);
    fh2_tracking_planeXZ->GetXaxis()->SetTitle("Beam direction-Z [mm]");
    fh2_tracking_planeXZ->GetYaxis()->SetTitle("(Wixhausen)<---  X [mm]  ---> (Messel)");
    fh2_tracking_planeXZ->GetYaxis()->SetTitleOffset(1.1);
    fh2_tracking_planeXZ->GetXaxis()->CenterTitle(true);
    fh2_tracking_planeXZ->GetYaxis()->CenterTitle(true);
    fh2_tracking_planeXZ->GetXaxis()->SetLabelSize(0.045);
    fh2_tracking_planeXZ->GetXaxis()->SetTitleSize(0.045);
    fh2_tracking_planeXZ->GetYaxis()->SetLabelSize(0.045);
    fh2_tracking_planeXZ->GetYaxis()->SetTitleSize(0.045);
    fh2_tracking_planeXZ->Draw("colz");

    // Target indicated as a hole
    TLine* l1 = new TLine(fPosTarget, fWidthTarget / 2., fPosTarget, histoYlim);
    l1->SetLineColor(2);
    l1->SetLineWidth(2);
    l1->Draw();
    TLine* l2 = new TLine(fPosTarget, -1. * fWidthTarget / 2., fPosTarget, -1. * histoYlim);
    l2->SetLineColor(2);
    l2->SetLineWidth(2);
    l2->Draw();

    TArrow* arrow = new TArrow(200., 120., 800., 120., 0.02, ">");
    arrow->SetLineColor(3);
    arrow->SetFillStyle(1001);
    arrow->SetLineWidth(3);
    arrow->Draw();

    TLatex latex;
    latex.SetTextSize(0.045);
    latex.SetTextAlign(13);
    latex.SetTextColor(3);
    latex.DrawLatex(200., histoYlim - 10., "Beam");
    latex.SetTextColor(1);
    latex.DrawLatex(100., -1. * histoYlim + 20., "Accelerator");
    latex.SetTextColor(2);
    latex.DrawLatex(fPosTarget + 28., 1. * histoYlim - 20., "Target pos.");
    latex.SetTextColor(1);
    latex.DrawLatex(fDist_acelerator_glad - 600., -1. * histoYlim + 20., "GLAD wind.");

    // Hit data, tracking plane Y-Z
    cTrackingYZ = new TCanvas(
        "Tracking_before_GLAD_YZ" + fNameCut, "Tracking (Lab.) plane YZ info " + fNameCut, 10, 10, 800, 700);
    Name1 = "fh2_tracking_planeYZ" + fNameCut;
    Name2 = "Tracking (Lab.) plane YZ info " + fNameCut;
    fh2_tracking_planeYZ = new TH2F(Name1, Name2, 400, 0., fDist_acelerator_glad, 400, -1. * histoYlim, histoYlim);
    fh2_tracking_planeYZ->GetXaxis()->SetTitle("Beam direction-Z [mm]");
    fh2_tracking_planeYZ->GetYaxis()->SetTitle("Y [mm]");
    fh2_tracking_planeYZ->GetYaxis()->SetTitleOffset(1.1);
    fh2_tracking_planeYZ->GetXaxis()->CenterTitle(true);
    fh2_tracking_planeYZ->GetYaxis()->CenterTitle(true);
    fh2_tracking_planeYZ->GetXaxis()->SetLabelSize(0.045);
    fh2_tracking_planeYZ->GetXaxis()->SetTitleSize(0.045);
    fh2_tracking_planeYZ->GetYaxis()->SetLabelSize(0.045);
    fh2_tracking_planeYZ->GetYaxis()->SetTitleSize(0.045);
    fh2_tracking_planeYZ->Draw("colz");

    latex.SetTextSize(0.045);
    latex.SetTextAlign(13);
    latex.SetTextColor(3);
    latex.DrawLatex(200., histoYlim - 10., "Beam");
    latex.SetTextColor(1);
    latex.DrawLatex(100., -1. * histoYlim + 20., "Accelerator");
    latex.SetTextColor(2);
    latex.DrawLatex(fPosTarget + 28., 1. * histoYlim - 20., "Target pos.");
    latex.SetTextColor(1);
    latex.DrawLatex(fDist_acelerator_glad - 600., -1. * histoYlim + 20., "GLAD wind.");
    l1->Draw();
    l2->Draw();
    arrow->Draw();

    // Hit data, Beam profile X-Y at target position
    cBeamProfileTarget =
        new TCanvas("Beam_profile_XY_at_target" + fNameCut, "Beam profile XY info " + fNameCut, 10, 10, 800, 700);
    Name1 = "fh2_beam_profile_XY" + fNameCut;
    Name2 = "Beam profile-XY (Lab.) at target position " + fNameCut;
    fh2_target_PosXY = new TH2F(Name1, Name2, 200, -100., 100., 200, -100., 100.);
    fh2_target_PosXY->GetXaxis()->SetTitle("(Wixhausen)<---  X [mm]  ---> (Messel)");
    fh2_target_PosXY->GetYaxis()->SetTitle("Y [mm]");
    fh2_target_PosXY->GetYaxis()->SetTitleOffset(1.1);
    fh2_target_PosXY->GetXaxis()->CenterTitle(true);
    fh2_target_PosXY->GetYaxis()->CenterTitle(true);
    fh2_target_PosXY->GetXaxis()->SetLabelSize(0.045);
    fh2_target_PosXY->GetXaxis()->SetTitleSize(0.045);
    fh2_target_PosXY->GetYaxis()->SetLabelSize(0.045);
    fh2_target_PosXY->GetYaxis()->SetTitleSize(0.045);
    fh2_target_PosXY->Draw("colz");

    // AngleX and positionX on the target position
    auto cAPX = new TCanvas(
        "AngleX_vs_positionX_target" + fNameCut, "Angle_XZ vs position X on target " + fNameCut, 10, 10, 800, 700);
    fh2_angvsposx = new TH2F("AngXvsPosX" + fNameCut,
                             "Angle vs position on target " + fNameCut,
                             500,
                             -fWidthTarget,
                             fWidthTarget,
                             500,
                             -10.,
                             10.);
    fh2_angvsposx->GetXaxis()->SetTitle("(Wixhausen)<---  X [mm]  ---> (Messel)");
    fh2_angvsposx->GetYaxis()->SetTitle("Angle plane_XZ [mrad]");
    fh2_angvsposx->GetYaxis()->SetTitleOffset(1.1);
    fh2_angvsposx->GetXaxis()->CenterTitle(true);
    fh2_angvsposx->GetYaxis()->CenterTitle(true);
    fh2_angvsposx->GetXaxis()->SetLabelSize(0.045);
    fh2_angvsposx->GetXaxis()->SetTitleSize(0.045);
    fh2_angvsposx->GetYaxis()->SetLabelSize(0.045);
    fh2_angvsposx->GetYaxis()->SetTitleSize(0.045);
    cAPX->cd();
    fh2_angvsposx->Draw("colz");

    // AngleY and positionY on the target position
    auto cAPY = new TCanvas(
        "AngleY_vs_positionY_target" + fNameCut, "Angle_YZ vs position Y on target " + fNameCut, 10, 10, 800, 700);
    fh2_angvsposy = new TH2F("AngYvsPosY" + fNameCut,
                             "Angle vs position on target " + fNameCut,
                             500,
                             -fWidthTarget,
                             fWidthTarget,
                             500,
                             -10.,
                             10.);
    fh2_angvsposy->GetXaxis()->SetTitle("Y [mm]");
    fh2_angvsposy->GetYaxis()->SetTitle("Angle plane_YZ [mrad]");
    fh2_angvsposy->GetYaxis()->SetTitleOffset(1.1);
    fh2_angvsposy->GetXaxis()->CenterTitle(true);
    fh2_angvsposy->GetYaxis()->CenterTitle(true);
    fh2_angvsposy->GetXaxis()->SetLabelSize(0.045);
    fh2_angvsposy->GetXaxis()->SetTitleSize(0.045);
    fh2_angvsposy->GetYaxis()->SetLabelSize(0.045);
    fh2_angvsposy->GetYaxis()->SetTitleSize(0.045);
    cAPY->cd();
    fh2_angvsposy->Draw("colz");

    // MAIN FOLDER
    TFolder* mainfol = new TFolder("Tracking_Cave" + fNameCut, "Tracking info " + fNameCut);
    mainfol->Add(cTrackingXZ);
    mainfol->Add(cTrackingYZ);
    mainfol->Add(cBeamProfileTarget);
    mainfol->Add(cAPX);
    mainfol->Add(cAPY);

    FairRunOnline* run = FairRunOnline::Instance();
    R3BLOG_IF(FATAL, NULL == run, "FairRunOnline not found");
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Cave_Tracking_HIST" + fNameCut,
                                          Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BIncomingTrackingOnlineSpectra::Reset_Histo()
{
    R3BLOG(INFO, "");
    fh2_tracking_planeXZ->Reset();
    fh2_tracking_planeYZ->Reset();
    fh2_target_PosXY->Reset();
    fh2_angvsposx->Reset();
    fh2_angvsposy->Reset();
}

void R3BIncomingTrackingOnlineSpectra::Exec(Option_t* option)
{
    Double_t mwpc0x = -300., mwpc0y = -300., zrand = 0.;
    Double_t xtarget = -500., ytarget = -500.;

    if (fCutIncoming && fFrsHitDataCA)
    {
        Int_t nHits = fFrsHitDataCA->GetEntriesFast();
        float z = 0., aq = 0.;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BFrsData*)fFrsHitDataCA->At(ihit);
            if (!hit)
                continue;
            z = hit->GetZ();
            aq = hit->GetAq();
        }
        if (fCutIncoming->IsInside(z, aq) == false)
            return;
    }

    // Fill Mwpc0 Hit data
    if (fMwpc0HitDataCA && fMwpc0HitDataCA->GetEntriesFast() > 0)
    {
        Int_t nHits = fMwpc0HitDataCA->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = (R3BMwpcHitData*)fMwpc0HitDataCA->At(ihit);
            if (!hit)
                continue;
            mwpc0x = hit->GetX() + fMw0GeoPar->GetPosX() * 10.; // mm
            mwpc0y = hit->GetY() + fMw0GeoPar->GetPosY() * 10.; // mm
        }

        // Fill Mwpc1 Hit data
        if (fMwpc1HitDataCA && fMwpc1HitDataCA->GetEntriesFast() > 0 && mwpc0y > -100.)
        {
            nHits = fMwpc1HitDataCA->GetEntriesFast();
            Float_t mwpc1x = -150.;
            Float_t mwpc1y = -150.;
            Float_t angX = -500.;
            Float_t angY = -500.;
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hit = (R3BMwpcHitData*)fMwpc1HitDataCA->At(ihit);
                if (!hit)
                    continue;
                mwpc1x = hit->GetX() + fMw1GeoPar->GetPosX() * 10.;
                mwpc1y = hit->GetY() + fMw1GeoPar->GetPosY() * 10.;
                // Float_t angX = (mwpc1x - mwpc0x) / (fMw1GeoPar->GetPosZ() - fMw0GeoPar->GetPosZ()) / 10.;
                // Float_t angY = (mwpc1y - mwpc0y) / (fMw1GeoPar->GetPosZ() - fMw0GeoPar->GetPosZ()) / 10.;
                angX = (mwpc0x - mwpc1x) / (fMw0GeoPar->GetPosZ() - fMw1GeoPar->GetPosZ()) / 10.;
                angY = (mwpc0y - mwpc1y) / (fMw0GeoPar->GetPosZ() - fMw1GeoPar->GetPosZ()) / 10.;
                if (TMath::Abs(angX) < 0.075 && TMath::Abs(angY) < 0.075 && mwpc1x > -150.)
                {
                    zrand = gRandom->Uniform(0., fDist_acelerator_glad);
                    // fh2_tracking_planeYZ->Fill(zrand, mwpc0y - angY * fMw0GeoPar->GetPosZ() * 10. + angY * zrand); //
                    // mm ytarget = mwpc0y - angY * fMw0GeoPar->GetPosZ() * 10. + angY * fPosTarget;
                    // fh2_tracking_planeXZ->Fill(zrand, mwpc0x - angX * fMw0GeoPar->GetPosZ() * 10. + angX * zrand); //
                    // mm xtarget = mwpc0x - angX * fMw0GeoPar->GetPosZ() * 10. + angX * fPosTarget;
                    fh2_tracking_planeYZ->Fill(zrand, mwpc1y - angY * fMw1GeoPar->GetPosZ() * 10. + angY * zrand); // mm
                    ytarget = mwpc1y - angY * fMw1GeoPar->GetPosZ() * 10. + angY * fPosTarget;
                    fh2_tracking_planeXZ->Fill(zrand, mwpc1x - angX * fMw1GeoPar->GetPosZ() * 10. + angX * zrand); // mm
                    xtarget = mwpc1x - angX * fMw1GeoPar->GetPosZ() * 10. + angX * fPosTarget;
                }
            }
            if (xtarget > -500. && ytarget > -500.)
            {
                fh2_target_PosXY->Fill(xtarget, ytarget);
                fh2_angvsposx->Fill(xtarget, angX * 1000.);
                fh2_angvsposy->Fill(ytarget, angY * 1000.);
            }
        }
    }

    fNEvents += 1;
}

void R3BIncomingTrackingOnlineSpectra::FinishEvent()
{
    if (fMwpc0HitDataCA)
    {
        fMwpc0HitDataCA->Clear();
    }

    if (fMwpc1HitDataCA)
    {
        fMwpc1HitDataCA->Clear();
    }

    if (fFrsHitDataCA)
    {
        fFrsHitDataCA->Clear();
    }
}

void R3BIncomingTrackingOnlineSpectra::FinishTask()
{
    if (fMwpc0HitDataCA)
    {
        cTrackingXZ->Write();
        cTrackingYZ->Write();
        cBeamProfileTarget->Write();
        fh2_angvsposx->Write();
        fh2_angvsposy->Write();
    }
}

ClassImp(R3BIncomingTrackingOnlineSpectra);
