/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
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
// -----        R3BIncomingTrackingFootOnlineSpectra      -----
// -----    Created 05/05/25 by J.L. Rodriguez-Sanchez    -----
// -----         Fill tracking online histograms          -----
// ------------------------------------------------------------

/*
 * This task should fill histograms for tracking before GLAD
 */

#include "R3BIncomingTrackingFootOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BFootHitData.h"
#include "R3BFootMappingPar.h"
#include "R3BFrsData.h"
#include "R3BLogger.h"
#include "R3BMwpcHitData.h"
#include "R3BShared.h"
#include "R3BTGeoPar.h"

#include <FairRootManager.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>

#include <TArrow.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TFolder.h>
#include <TH1F.h>
#include <TH2F.h>
#include <THttpServer.h>
#include <TLatex.h>
#include <TLine.h>
#include <TMath.h>
#include <TRandom.h>
#include <TVector3.h>
#include <cmath>
#include <iostream>

R3BIncomingTrackingFootOnlineSpectra::R3BIncomingTrackingFootOnlineSpectra()
    : R3BIncomingTrackingFootOnlineSpectra("IncomingTrackingFootOnlineSpectra", 1)
{
}

R3BIncomingTrackingFootOnlineSpectra::R3BIncomingTrackingFootOnlineSpectra(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

R3BIncomingTrackingFootOnlineSpectra::R3BIncomingTrackingFootOnlineSpectra(const TString& name,
                                                                           float zmin,
                                                                           float zmax,
                                                                           float aqmin,
                                                                           float aqmax)
    : FairTask("IncomingTrackingOnlineSpectra_" + name, 1)
    , fNameCut(name)
{
    fCutIncoming = std::make_unique<TCutG>(name, 5);
    fCutIncoming->SetPoint(0, zmin, aqmin);
    fCutIncoming->SetPoint(1, zmin, aqmax);
    fCutIncoming->SetPoint(2, zmax, aqmax);
    fCutIncoming->SetPoint(3, zmax, aqmin);
    fCutIncoming->SetPoint(4, zmin, aqmin);
}

// -----   Public method SetParContainers   --------------------------------
void R3BIncomingTrackingFootOnlineSpectra::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, NULL == rtdb, "FairRuntimeDb not found");

    fMw0GeoPar = dynamic_cast<R3BTGeoPar*>(rtdb->getContainer("Mwpc0GeoPar"));
    R3BLOG_IF(warn, !fMw0GeoPar, "Could not get access to Mwpc0GeoPar container.");

    fFootMappingPar = dynamic_cast<R3BFootMappingPar*>(rtdb->getContainer("footMappingPar"));
    R3BLOG_IF(fatal, !fFootMappingPar, "Could not get access to footMappingPar container.");

    return;
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BIncomingTrackingFootOnlineSpectra::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

InitStatus R3BIncomingTrackingFootOnlineSpectra::Init()
{
    R3BLOG(info, "");
    auto mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, mgr == nullptr, "FairRootManager not found");

    fHitFootData = dynamic_cast<TClonesArray*>(mgr->GetObject("FootHitData"));
    R3BLOG_IF(fatal, fHitFootData == nullptr, "FootHitData not found");

    fMwpc0HitData = dynamic_cast<TClonesArray*>(mgr->GetObject("Mwpc0HitData"));
    R3BLOG_IF(warn, fMwpc0HitData == nullptr, "Mwpc0HitData not found");

    fFrsHitData = dynamic_cast<TClonesArray*>(mgr->GetObject("FrsData"));
    R3BLOG_IF(warn, fFrsHitData == nullptr, "FrsData not found");

    // Get the Z position from the parameter container
    for (size_t i = 0; i < fFootZPos.size(); i++)
        fFootZPos[i] = fFootMappingPar->GetDist2target(i + 1);

    // Create histograms for detectors
    TString Name1;
    TString Name2;

    // Hit data, tracking plane X-Z
    cTrackingXZ = new TCanvas(
        "Tracking_before_GLAD_XZ_" + fNameCut, "Tracking (Lab.) plane XZ info " + fNameCut, 10, 10, 800, 700);

    Name1 = "fh2_tracking_planeXZ_" + fNameCut;
    Name2 = "Tracking (Lab.) plane XZ info " + fNameCut;
    Int_t histoYlim = 150;
    fh2_tracking_planeXZ =
        R3B::root_owned<TH2F>(Name1, Name2, 400, 0., fDist_acelerator_glad, 400, -1. * histoYlim, histoYlim);
    fh2_tracking_planeXZ->GetXaxis()->SetTitle("Beam direction-Z [mm]");
    fh2_tracking_planeXZ->GetYaxis()->SetTitle("(Wixhausen) <---  X [mm]  ---> (Messel)");
    fh2_tracking_planeXZ->GetYaxis()->SetTitleOffset(1.1);
    fh2_tracking_planeXZ->GetXaxis()->CenterTitle(true);
    fh2_tracking_planeXZ->GetYaxis()->CenterTitle(true);
    fh2_tracking_planeXZ->GetXaxis()->SetLabelSize(0.045);
    fh2_tracking_planeXZ->GetXaxis()->SetTitleSize(0.045);
    fh2_tracking_planeXZ->GetYaxis()->SetLabelSize(0.045);
    fh2_tracking_planeXZ->GetYaxis()->SetTitleSize(0.045);
    fh2_tracking_planeXZ->Draw("colz");

    // Target indicated as a hole
    auto l1 = new TLine(fPosTarget, fWidthTarget / 2., fPosTarget, histoYlim);
    l1->SetLineColor(2);
    l1->SetLineWidth(2);
    l1->Draw();
    auto l2 = new TLine(fPosTarget, -1. * fWidthTarget / 2., fPosTarget, -1. * histoYlim);
    l2->SetLineColor(2);
    l2->SetLineWidth(2);
    l2->Draw();

    auto arrow = new TArrow(200., 120., 800., 120., 0.02, ">");
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
        "Tracking_before_GLAD_YZ_" + fNameCut, " Tracking (Lab.) plane YZ info " + fNameCut, 10, 10, 800, 700);
    Name1 = "fh2_tracking_planeYZ_" + fNameCut;
    Name2 = "Tracking (Lab.) plane YZ info " + fNameCut;
    fh2_tracking_planeYZ =
        R3B::root_owned<TH2F>(Name1, Name2, 400, 0., fDist_acelerator_glad, 400, -1. * histoYlim, histoYlim);
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

    // Hit data, Beam profile X-Y at target position (with FOOT before target)
    cBeamProfileBeforeTarget = new TCanvas("Beam_profile_XY_at_before_target_" + fNameCut,
                                           "Beam profile XY (before target) info " + fNameCut,
                                           10,
                                           10,
                                           800,
                                           700);
    Name1 = "fh2_beam_profile_XY_before" + fNameCut;
    Name2 = "Beam profile-XY (Lab.) at before target position " + fNameCut;
    fh2_before_target_PosXY = R3B::root_owned<TH2F>(Name1, Name2, 200, -100., 100., 200, -100., 100.);
    fh2_before_target_PosXY->GetXaxis()->SetTitle("(Wixhausen) <---  X [mm]  ---> (Messel)");
    fh2_before_target_PosXY->GetYaxis()->SetTitle("Y [mm]");
    fh2_before_target_PosXY->GetYaxis()->SetTitleOffset(1.1);
    fh2_before_target_PosXY->GetXaxis()->CenterTitle(true);
    fh2_before_target_PosXY->GetYaxis()->CenterTitle(true);
    fh2_before_target_PosXY->GetXaxis()->SetLabelSize(0.045);
    fh2_before_target_PosXY->GetXaxis()->SetTitleSize(0.045);
    fh2_before_target_PosXY->GetYaxis()->SetLabelSize(0.045);
    fh2_before_target_PosXY->GetYaxis()->SetTitleSize(0.045);
    fh2_before_target_PosXY->Draw("colz");

    // Hit data, Beam profile X-Y at target position (with FOOT before target)
    cBeamProfileAfterTarget = new TCanvas("Beam_profile_XY_at_after_target_" + fNameCut,
                                          "Beam profile XY (after target) info " + fNameCut,
                                          10,
                                          10,
                                          800,
                                          700);
    Name1 = "fh2_beam_profile_XY_after" + fNameCut;
    Name2 = "Beam profile-XY (Lab.) at after target position " + fNameCut;
    fh2_after_target_PosXY = R3B::root_owned<TH2F>(Name1, Name2, 200, -100., 100., 200, -100., 100.);
    fh2_after_target_PosXY->GetXaxis()->SetTitle("(Wixhausen) <---  X [mm]  ---> (Messel)");
    fh2_after_target_PosXY->GetYaxis()->SetTitle("Y [mm]");
    fh2_after_target_PosXY->GetYaxis()->SetTitleOffset(1.1);
    fh2_after_target_PosXY->GetXaxis()->CenterTitle(true);
    fh2_after_target_PosXY->GetYaxis()->CenterTitle(true);
    fh2_after_target_PosXY->GetXaxis()->SetLabelSize(0.045);
    fh2_after_target_PosXY->GetXaxis()->SetTitleSize(0.045);
    fh2_after_target_PosXY->GetYaxis()->SetLabelSize(0.045);
    fh2_after_target_PosXY->GetYaxis()->SetTitleSize(0.045);
    fh2_after_target_PosXY->Draw("colz");

    // AngleX and positionX on the target position
    auto cAPX = new TCanvas(
        "AngleX_vs_positionX_target_" + fNameCut, "Angle_XZ vs position X on target " + fNameCut, 10, 10, 800, 700);
    fh2_angvsposx = R3B::root_owned<TH2F>("AngXvsPosX" + fNameCut,
                                          "Angle vs position on target " + fNameCut,
                                          500,
                                          -fWidthTarget,
                                          fWidthTarget,
                                          500,
                                          -10.,
                                          10.);
    fh2_angvsposx->GetXaxis()->SetTitle("(Wixhausen) <---  X [mm]  ---> (Messel)");
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
        "AngleY_vs_positionY_target_" + fNameCut, "Angle_YZ vs position Y on target " + fNameCut, 10, 10, 800, 700);
    fh2_angvsposy = R3B::root_owned<TH2F>("AngYvsPosY" + fNameCut,
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
    auto mainfol = new TFolder("Tracking_Cave" + fNameCut, "Tracking info " + fNameCut);
    mainfol->Add(cTrackingXZ);
    mainfol->Add(cTrackingYZ);
    mainfol->Add(cBeamProfileBeforeTarget);
    mainfol->Add(cBeamProfileAfterTarget);
    mainfol->Add(cAPX);
    mainfol->Add(cAPY);

    auto run = FairRunOnline::Instance();
    R3BLOG_IF(fatal, run == nullptr, "FairRunOnline not found");
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);

    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Cave_Tracking_HIST" + fNameCut,
                                          Form("/Objects/%s/->Reset_Histo()", GetName()));

    return kSUCCESS;
}

void R3BIncomingTrackingFootOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");
    fh2_tracking_planeXZ->Reset();
    fh2_tracking_planeYZ->Reset();
    fh2_before_target_PosXY->Reset();
    fh2_after_target_PosXY->Reset();
    fh2_angvsposx->Reset();
    fh2_angvsposy->Reset();
}

void R3BIncomingTrackingFootOnlineSpectra::Exec(Option_t* /*option*/)
{

    double zrand = 0.;
    double xtarget = std::nan(""), ytarget = std::nan("");

    if (fCutIncoming && fFrsHitData)
    {
        auto nHits = fFrsHitData->GetEntriesFast();
        float z = 0., aq = 0.;
        for (size_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = dynamic_cast<R3BFrsData*>(fFrsHitData->At(ihit));
            if (!hit)
                continue;
            z = hit->GetZ();
            aq = hit->GetAq();
        }
        if (fCutIncoming->IsInside(z, aq) == false)
            return;
    }

    // Fill hit data
    if (fHitFootData && fHitFootData->GetEntriesFast() > 0 && fYearConf == 2025)
    {
        std::vector<double> footPos(fNbDet, std::nan(""));

        auto nHits = fHitFootData->GetEntriesFast();
        for (size_t ihit = 0; ihit < nHits; ihit++)
        {
            auto hit = dynamic_cast<R3BFootHitData*>(fHitFootData->At(ihit));
            auto detId = hit->GetDetId() - 1;
            if (detId >= fNbDet)
                continue;

            if (std::find(fDetIdX.begin(), fDetIdX.end(), detId) != fDetIdX.end())
                if (!std::isfinite(footPos[detId]))
                    footPos[detId] = hit->GetPosLab()[0];

            if (std::find(fDetIdY.begin(), fDetIdY.end(), detId) != fDetIdY.end())
                if (!std::isfinite(footPos[detId]))
                    footPos[detId] = hit->GetPosLab()[1];
        }

        // Calculations for tracking with X before target
        if (std::isfinite(footPos[fDetIds[0]]) && std::isfinite(footPos[fDetIds[2]]))
        {
            auto angX = std::atan((footPos[fDetIds[2]] - footPos[fDetIds[0]]) / (fFootZPos[2] - fFootZPos[0]) / 10.);
            if (TMath::Abs(angX) < 0.1)
            {
                zrand = gRandom->Uniform(0., fPosTarget);
                fh2_tracking_planeXZ->Fill(zrand, footPos[fDetIds[2]] - angX * fFootZPos[2] * 10. + angX * zrand); // mm
                xtarget = footPos[fDetIds[2]] - angX * fFootZPos[2] * 10. + angX * fPosTarget;
                fh2_angvsposx->Fill(xtarget, angX * 1000.);
            }
        }

        // Calculations for tracking with Y before target
        if (std::isfinite(footPos[fDetIds[1]]) && std::isfinite(footPos[fDetIds[3]]))
        {
            auto angY = std::atan((footPos[fDetIds[3]] - footPos[fDetIds[1]]) / (fFootZPos[3] - fFootZPos[1]) / 10.);
            if (TMath::Abs(angY) < 0.1)
            {
                zrand = gRandom->Uniform(0., fPosTarget);
                fh2_tracking_planeYZ->Fill(zrand, footPos[fDetIds[3]] - angY * fFootZPos[3] * 10. + angY * zrand); // mm
                ytarget = footPos[fDetIds[3]] - angY * fFootZPos[3] * 10. + angY * fPosTarget;
                fh2_angvsposy->Fill(ytarget, angY * 1000.);
            }
        }

        // Beam profile at target position (before)
        if (std::isfinite(xtarget) && std::isfinite(ytarget))
        {
            fh2_before_target_PosXY->Fill(xtarget, ytarget);
        }

        // Calculations for tracking with X after target
        if (std::isfinite(footPos[fDetIds[5]]) && std::isfinite(footPos[fDetIds[7]]))
        {
            auto angX = std::atan((footPos[fDetIds[7]] - footPos[fDetIds[5]]) / (fFootZPos[7] - fFootZPos[5]) / 10.);
            if (TMath::Abs(angX) < 0.1)
            {
                zrand = gRandom->Uniform(fPosTarget, fDist_acelerator_glad);
                fh2_tracking_planeXZ->Fill(zrand, footPos[fDetIds[7]] - angX * fFootZPos[7] * 10. + angX * zrand); // mm
                xtarget = footPos[fDetIds[7]] - angX * fFootZPos[7] * 10. + angX * fPosTarget;
            }
        }

        // Calculations for tracking with Y after target
        if (std::isfinite(footPos[fDetIds[6]]) && std::isfinite(footPos[fDetIds[4]]))
        {
            auto angY = std::atan((footPos[fDetIds[6]] - footPos[fDetIds[4]]) / (fFootZPos[6] - fFootZPos[4]) / 10.);
            if (TMath::Abs(angY) < 0.1)
            {
                zrand = gRandom->Uniform(fPosTarget, fDist_acelerator_glad);
                fh2_tracking_planeYZ->Fill(zrand, footPos[fDetIds[6]] - angY * fFootZPos[6] * 10. + angY * zrand); // mm
                ytarget = footPos[fDetIds[6]] - angY * fFootZPos[6] * 10. + angY * fPosTarget;
            }
        }

        // Beam profile at target position (after)
        if (std::isfinite(xtarget) && std::isfinite(ytarget))
        {
            fh2_after_target_PosXY->Fill(xtarget, ytarget);
        }
    }

    fNEvents += 1;
}

void R3BIncomingTrackingFootOnlineSpectra::FinishEvent()
{
    if (fHitFootData)
    {
        fHitFootData->Clear();
    }

    if (fMwpc0HitData)
    {
        fMwpc0HitData->Clear();
    }

    if (fFrsHitData)
    {
        fFrsHitData->Clear();
    }
}

void R3BIncomingTrackingFootOnlineSpectra::FinishTask()
{
    if (fHitFootData)
    {
        cTrackingXZ->Write();
        cTrackingYZ->Write();
        cBeamProfileBeforeTarget->Write();
        cBeamProfileAfterTarget->Write();
        fh2_angvsposx->Write();
        fh2_angvsposy->Write();
    }
}

ClassImp(R3BIncomingTrackingFootOnlineSpectra)
