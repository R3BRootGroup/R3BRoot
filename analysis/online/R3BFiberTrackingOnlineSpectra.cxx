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
// -----          R3BFiberTrackingOnlineSpectra        -----
// -----    Created 18/05/22 by J.L. Rodriguez-Sanchez & Enis Lorenz    -----
// -----         Fill tracking online histograms          -----
// ------------------------------------------------------------

/*
 * This task should fill histograms for tracking after GLAD
 */

#include "R3BFiberTrackingOnlineSpectra.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMappingPar.h"
#include "R3BTofdHitData.h"
#include "R3BLosCalData.h"
//#include "R3BMwpcHitData.h"
#include "R3BTGeoPar.h"
//#include "R3BTwimHitData.h"
#include "R3BFrsData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"

#include "TArrow.h"
#include "TStyle.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TClonesArray.h"
#include "TFolder.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3D.h"
#include "TNtuple.h"
#include "THttpServer.h"
#include "TLatex.h"
#include "TLine.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector3.h"

#define IS_NAN(x) TMath::IsNaN(x)

R3BFiberTrackingOnlineSpectra::R3BFiberTrackingOnlineSpectra()
    : R3BFiberTrackingOnlineSpectra("FiberTrackingOnlineSpectra", 1)
{
}

R3BFiberTrackingOnlineSpectra::R3BFiberTrackingOnlineSpectra(const TString& name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fNEvents(0)
    , fTrigger(-1)
    , fTPatMask(0xffff)//16bits, all TPats
    , fFi31MapPar(NULL)
    , fFi32MapPar(NULL)
    , fFi33MapPar(NULL)
    , fFi30HitDataCA(NULL)
    , fFi31HitDataCA(NULL)
    , fFi32HitDataCA(NULL)
    , fFi33HitDataCA(NULL)
    , fHitTofdItems(NULL)
    , fNbFibers31(512)
    , fNbFibers32(512)
    , fNbFibers33(512)
    , fNbTofdPlanes(1)
    , fNbTofdPaddlesPerPlane(44)
    , fDist_acelerator_glad(3718.0)
    , fPosTarget(2656.)
    , fWidthTarget(40.)
    , fZ_max(40.)
    , fZ_min(0.)
    , fFRSGATE(false)
    , fAQmin(0.)
    , fAQmax(100.)
    , fQmin(0.)
    , fQmax(100.)
    , fTpat1(-1)
    , fTpat2(-1)
{
}

R3BFiberTrackingOnlineSpectra::~R3BFiberTrackingOnlineSpectra()
{
    R3BLOG(debug1, "Destructor");
    if (fFi31HitDataCA)
    {
        delete fFi31HitDataCA;
    }
    if (fFi32HitDataCA)
    {
        delete fFi32HitDataCA;
    }
    if (fFi33HitDataCA)
    {
        delete fFi33HitDataCA;
    }
    if(fFi30HitDataCA)
    {
        delete fFi30HitDataCA;
    }
    if (fHitFrs)
        delete fHitFrs;
}

// -----   Public method SetParContainers   --------------------------------
void R3BFiberTrackingOnlineSpectra::SetParContainers()
{
    // Parameter Container
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, NULL == rtdb, "FairRuntimeDb not found");

   // fMw0GeoPar = (R3BTGeoPar*)rtdb->getContainer("FiberGeoPar");
   // R3BLOG_IF(error, !FiberGeoPar, "Could not get access to FiberGeoPar container.");

    // fTargetGeoPar = (R3BTGeoPar*)rtdb->getContainer("TargetGeoPar");
    // R3BLOG_IF(error, !fTargetGeoPar, "Could not get access to TargetGeoPar container.");

    fMw1GeoPar = (R3BTGeoPar*)rtdb->getContainer("Mwpc1GeoPar");
    R3BLOG_IF(error, !fMw1GeoPar, "Could not get access to Mwpc1GeoPar container.");
    return;
    
    fFi31MapPar = (R3BFiberMappingPar*)FairRuntimeDb::instance()->getContainer("Fi31MappingPar");
    R3BLOG_IF(error, !fFi31MapPar, "Couldn't get " << "Fi31" << "MappingPar");
    if (fFi31MapPar)
    {
        fNbFibers31 = fFi31MapPar->GetNbChannels();
        R3BLOG(info, "Fi31 " << "MappingPar found with " << fNbFibers31 << " fibers");
    }
    fFi32MapPar = (R3BFiberMappingPar*)FairRuntimeDb::instance()->getContainer("Fi32MappingPar");
    R3BLOG_IF(error, !fFi32MapPar, "Couldn't get " << "Fi32" << "MappingPar");
    if (fFi32MapPar)
    {
        fNbFibers32 = fFi32MapPar->GetNbChannels();
        R3BLOG(info, "Fi32 " << "MappingPar found with " << fNbFibers32 << " fibers");
    }
    fFi33MapPar = (R3BFiberMappingPar*)FairRuntimeDb::instance()->getContainer("Fi33MappingPar");
    R3BLOG_IF(error, !fFi33MapPar, "Couldn't get " << "Fi33" << "MappingPar");
    if (fFi33MapPar)
    {
        fNbFibers33 = fFi33MapPar->GetNbChannels();
        R3BLOG(info, "Fi33 " << "MappingPar found with " << fNbFibers33 << " fibers");
    }
    
}

// -----   Public method ReInit   ----------------------------------------------
InitStatus R3BFiberTrackingOnlineSpectra::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

InitStatus R3BFiberTrackingOnlineSpectra::Init()
{
    R3BLOG(info, "For Fibers 31, 32, 33");
    FairRootManager* mgr = FairRootManager::Instance();
    R3BLOG_IF(fatal, NULL == mgr, "FairRootManager not found");

    header = (R3BEventHeader*)mgr->GetObject("EventHeader.");
    if (!header)
    {
        R3BLOG(warning, "EventHeader. not found");
        header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    }
    else
        R3BLOG(info, " EventHeader. found");

    // uncomment lines below when ucesb avaliable
   // FairRunOnline* run = FairRunOnline::Instance();
   // run->GetHttpServer()->Register("", this);

    fFi31HitDataCA = (TClonesArray*)mgr->GetObject("Fi31Hit");
    if (!fFi31HitDataCA)
    {
        R3BLOG(warning, "Fi31HitData not found");
       // return kwarning;
    }

    fFi32HitDataCA = (TClonesArray*)mgr->GetObject("Fi32Hit");
    if (!fFi32HitDataCA)
    {
        R3BLOG(warning, "Fi32HitData not found");
      //  return kwarning;
    }
    
    fFi33HitDataCA = (TClonesArray*)mgr->GetObject("Fi33Hit");
    if (!fFi33HitDataCA)
    {
        R3BLOG(warning, "Fi33HitData not found");
      //  return kwarning;
    }
    fFi30HitDataCA = (TClonesArray*)mgr->GetObject("Fi30Hit");
    if(!fFi30HitDataCA)
    {
        R3BLOG(warning, "Fi30HitData not found");
    }
    
    fHitTofdItems = (TClonesArray*)mgr->GetObject("TofdHit");
    R3BLOG_IF(warning, NULL == fHitTofdItems, "TofdHit not found");
    
    fHitFrs = (TClonesArray*)mgr->GetObject("FrsData");
    R3BLOG_IF(warning, !fHitFrs, "Branch FrsData not found");
    
    fCalLos = (TClonesArray*)mgr->GetObject("LosCal");
    R3BLOG_IF(warning, !fCalLos, "Branch LosCal not found");
    
    // Create histograms for detectors
    TString Name1;
    TString Name2;

    cFibInfo = new TCanvas("FibInfo", "FibInfo", 10, 10, 800, 700);

        fh_ToT_Fib = new TH2F(
            "Fib32Hit_ToT_iFib", "Fib32Hit ToT vs Fib", 512, 1., 512 + 1, 200, 0., 100.);
        fh_ToT_Fib->GetXaxis()->SetTitle("Fiber number");
        fh_ToT_Fib->GetYaxis()->SetTitle("ToT / ns");

        gPad->SetLogz();
        fh_ToT_Fib->Draw("colz");

    // Hit data, fiber tracking plane X-Z
    cTrackingXZ = new TCanvas("Tracking_after_GLAD_XZ", "Fragment Tracking (Fibers) plane XZ Fibers + ToFD", 10, 10, 800, 700);

    Name1 = "fh2_fibtracking_planeXZ";
    Name2 = "Fragment Tracking (Fibers) plane XZ Fibers + ToFD";
    Int_t histoYlim = 150;
    fh2_fibtracking_planeXZ = new TH2F(Name1, Name2, 250, 0., 1250, 1400, -1. * 70., 70.);
    fh2_fibtracking_planeXZ->GetXaxis()->SetTitle("Beam direction-Z [cm]");
    fh2_fibtracking_planeXZ->GetYaxis()->SetTitle("X Position [cm]");
    fh2_fibtracking_planeXZ->GetYaxis()->SetTitleOffset(1.1);
    fh2_fibtracking_planeXZ->GetXaxis()->CenterTitle(true);
    fh2_fibtracking_planeXZ->GetYaxis()->CenterTitle(true);
    fh2_fibtracking_planeXZ->GetXaxis()->SetLabelSize(0.045);
    fh2_fibtracking_planeXZ->GetXaxis()->SetTitleSize(0.045);
    fh2_fibtracking_planeXZ->GetYaxis()->SetLabelSize(0.045);
    fh2_fibtracking_planeXZ->GetYaxis()->SetTitleSize(0.045);
    gPad->SetLogz();
    fh2_fibtracking_planeXZ->Draw("colz");

    // Target indicated as a hole
    TLine* l1 = new TLine(594.5, -0.6, 594.5, 50.6);//Fi33
    l1->SetLineColor(2);
    l1->SetLineWidth(2);
    l1->Draw();
    TLine* l3 = new TLine(611.4, 0.6, 611.4, -50.6);//Fi31
    l3->SetLineColor(2);
    l3->SetLineWidth(2);
    l3->Draw();
    TLine* l2 = new TLine(434.1, -25.6, 434.1, 25.6);//Fi32
    l2->SetLineColor(2);
    l2->SetLineWidth(2);
    l2->Draw();
    
    TLine* l4 = new TLine(0., 0., 1250., 0.);//14° line
    l4->SetLineColor(1);
    l4->SetLineWidth(2);
    l4->Draw();
    
    TLine* l5 = new TLine(1138.5, -60., 1138.5, 60);//ToFD
    l5->SetLineColor(1);
    l5->SetLineWidth(2);
    l5->Draw();

    TArrow* arrow = new TArrow(100., 58., 300., 58., 0.02, ">");
    arrow->SetLineColor(3);
    arrow->SetFillStyle(1001);
    arrow->SetLineWidth(3);
    arrow->Draw();

    TLatex latex;
    latex.SetTextSize(0.045);
    latex.SetTextAlign(13);
    latex.SetTextColor(3);
    latex.DrawLatex(100., 65., "Beam");
    latex.SetTextColor(2);
    latex.DrawLatex(434.1, -26.5, "Fi32");
    latex.SetTextColor(2);
    latex.DrawLatex(631.4, -45., "Fi31");
    latex.SetTextColor(2);
    latex.DrawLatex(604.5, 50., "Fi33");
    latex.SetTextColor(1);
    latex.DrawLatex(1150., -50., "ToFD");
    latex.SetTextColor(1);
    latex.DrawLatex(50., -2., "14 deg");
    latex.SetTextColor(1);
    latex.DrawLatex(50., -50., "Umlenkpunkt");
     
  

    cslopeVsX = new TCanvas("slopeX_vs_positionX_Fibers", "slope_XZ vs position X of Fibers", 10, 10, 800, 700);
    fh2_fib_slope_vs_x =
        new TH2F("slopeXvsPosX", "slope vs position on Fibers", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_slope_vs_x->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_slope_vs_x->GetYaxis()->SetTitle("NL <- slope rel. to 14 deg [mrad] -> RPC");
    fh2_fib_slope_vs_x->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_slope_vs_x->GetXaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x->GetYaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_slope_vs_x->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x->GetYaxis()->SetTitleSize(0.04);

    fh2_fib_slope_vs_x4 =
        new TH2F("slopeXvsPosX4", "slope vs position on Fibers Charge 4", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_slope_vs_x4->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_slope_vs_x4->GetYaxis()->SetTitle("NL <- slope rel. to 14 deg [mrad] -> RPC");
    fh2_fib_slope_vs_x4->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_slope_vs_x4->GetXaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x4->GetYaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x4->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x4->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_slope_vs_x4->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x4->GetYaxis()->SetTitleSize(0.04);

    fh2_fib_slope_vs_x5 =
        new TH2F("slopeXvsPosX5", "slope vs position on Fibers Charge 5", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_slope_vs_x5->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_slope_vs_x5->GetYaxis()->SetTitle("NL <- slope rel. to 14 deg [mrad] -> RPC");
    fh2_fib_slope_vs_x5->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_slope_vs_x5->GetXaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x5->GetYaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x5->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x5->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_slope_vs_x5->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x5->GetYaxis()->SetTitleSize(0.04); 

    fh2_fib_slope_vs_x6 =
        new TH2F("slopeXvsPosX6", "slope vs position on Fibers Charge 6", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_slope_vs_x6->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_slope_vs_x6->GetYaxis()->SetTitle("NL <- slope rel. to 14 deg [mrad] -> RPC");
    fh2_fib_slope_vs_x6->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_slope_vs_x6->GetXaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x6->GetYaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x6->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x6->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_slope_vs_x6->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x6->GetYaxis()->SetTitleSize(0.04);       

    cslopeVsX->Divide(2,2);
    gStyle->SetPalette(kRainBow);
    cslopeVsX->cd(1);
    gPad->SetLogz();
    fh2_fib_slope_vs_x->Draw("colz,cp55");
    cslopeVsX->cd(2);
    gPad->SetLogz();
    fh2_fib_slope_vs_x4->Draw("colz,cp55");
    cslopeVsX->cd(3);
    gPad->SetLogz();
    fh2_fib_slope_vs_x5->Draw("colz,cp55");
    cslopeVsX->cd(4);
    gPad->SetLogz();
    fh2_fib_slope_vs_x6->Draw("colz,cp55");

   

    // AngleX and positionX on Fibers relative to center / 14° line - positive angles mean higher bending, vice versa
    cAngVsX = new TCanvas("AngleX_vs_positionX_Fibers", "Angle_XZ vs position X of Fibers", 10, 10, 800, 700);
    fh2_fib_ang_vs_x =
        new TH2F("AngXvsPosX", "Angle vs position on Fibers", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_ang_vs_x->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_ang_vs_x->GetYaxis()->SetTitle("NL <- Angle rel. to 14 deg [mrad] -> RPC");
    fh2_fib_ang_vs_x->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_ang_vs_x->GetXaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x->GetYaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_ang_vs_x->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x->GetYaxis()->SetTitleSize(0.04);

    fh2_fib_ang_vs_x4 =
        new TH2F("AngXvsPosX4", "Angle vs position on Fibers Charge 4", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_ang_vs_x4->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_ang_vs_x4->GetYaxis()->SetTitle("NL <- Angle rel. to 14 deg [mrad] -> RPC");
    fh2_fib_ang_vs_x4->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_ang_vs_x4->GetXaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x4->GetYaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x4->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x4->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_ang_vs_x4->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x4->GetYaxis()->SetTitleSize(0.04);

    fh2_fib_ang_vs_x5 =
        new TH2F("AngXvsPosX5", "Angle vs position on Fibers Charge 5", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_ang_vs_x5->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_ang_vs_x5->GetYaxis()->SetTitle("NL <- Angle rel. to 14 deg [mrad] -> RPC");
    fh2_fib_ang_vs_x5->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_ang_vs_x5->GetXaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x5->GetYaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x5->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x5->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_ang_vs_x5->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x5->GetYaxis()->SetTitleSize(0.04); 

    fh2_fib_ang_vs_x6 =
        new TH2F("AngXvsPosX6", "Angle vs position on Fibers Charge 6", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_ang_vs_x6->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_ang_vs_x6->GetYaxis()->SetTitle("NL <- Angle rel. to 14 deg [mrad] -> RPC");
    fh2_fib_ang_vs_x6->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_ang_vs_x6->GetXaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x6->GetYaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x6->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x6->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_ang_vs_x6->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x6->GetYaxis()->SetTitleSize(0.04);       

    cAngVsX->Divide(2,2);
    cAngVsX->cd(1);
    gPad->SetLogz();
    fh2_fib_ang_vs_x->Draw("colz,cp55");
    cAngVsX->cd(2);
    gPad->SetLogz();
    fh2_fib_ang_vs_x4->Draw("colz,cp55");
    cAngVsX->cd(3);
    gPad->SetLogz();
    fh2_fib_ang_vs_x5->Draw("colz,cp55");
    cAngVsX->cd(4);
    gPad->SetLogz();
    fh2_fib_ang_vs_x6->Draw("colz,cp55");
    
    
        //----------------------------fh_ang_vs_pos_rot
    cAngRot = new TCanvas("AngX_vs_positionX_Fibers rotated", "Angle_XZ vs position X of Fibers", 10, 10, 800, 700);

    fh_ang_vs_pos_rot =
        new TH2F("AngXvsPosXRot", "Angle vs Position on Fibers rotated", 600, -60., 60., 1000, -50., 50.);
    fh_ang_vs_pos_rot->GetXaxis()->SetTitle("Arb. X");
    fh_ang_vs_pos_rot->GetYaxis()->SetTitle("Arb. Y");
    fh_ang_vs_pos_rot->GetYaxis()->SetTitleOffset(1.1);
    fh_ang_vs_pos_rot->GetXaxis()->CenterTitle(true);
    fh_ang_vs_pos_rot->GetYaxis()->CenterTitle(true);
    fh_ang_vs_pos_rot->GetXaxis()->SetLabelSize(0.045);
    fh_ang_vs_pos_rot->GetXaxis()->SetTitleSize(0.04);
    fh_ang_vs_pos_rot->GetYaxis()->SetLabelSize(0.045);
    fh_ang_vs_pos_rot->GetYaxis()->SetTitleSize(0.04);
    
    fh_pid =
        new TH2F("PID_QvsProj", "Charge vs Trajectory by angle vs position", 240, -60., 60., 100, 0., 10.);
    fh_pid->GetXaxis()->SetTitle("Arb. X / projection of angular correlation against position");
    fh_pid->GetYaxis()->SetTitle("Charge (ToFD)");
    fh_pid->GetYaxis()->SetTitleOffset(1.1);
    fh_pid->GetXaxis()->CenterTitle(true);
    fh_pid->GetYaxis()->CenterTitle(true);
    fh_pid->GetXaxis()->SetLabelSize(0.045);
    fh_pid->GetXaxis()->SetTitleSize(0.04);
    fh_pid->GetYaxis()->SetLabelSize(0.045);
    fh_pid->GetYaxis()->SetTitleSize(0.04); 

    fh_pid_slope =
        new TH2F("PID_QvsProjSlope", "Charge vs Linear Fitted Trajectory", 600, -60., 60., 750, 0., 30.);
    fh_pid_slope->GetXaxis()->SetTitle("Arb. X / projection of slope against intercept");
    fh_pid_slope->GetYaxis()->SetTitle("Charge (ToFD)");
    fh_pid_slope->GetYaxis()->SetTitleOffset(1.1);
    fh_pid_slope->GetXaxis()->CenterTitle(true);
    fh_pid_slope->GetYaxis()->CenterTitle(true);
    fh_pid_slope->GetXaxis()->SetLabelSize(0.045);
    fh_pid_slope->GetXaxis()->SetTitleSize(0.04);
    fh_pid_slope->GetYaxis()->SetLabelSize(0.045);
    fh_pid_slope->GetYaxis()->SetTitleSize(0.04);

    fh_pid_3d = new TH3D("pid3d","pid3d",400, -40., 40., 200, -10., 10., 80, 0., 8.);
    fh_pid_3d->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh_pid_3d->GetYaxis()->SetTitle("NL <- Angle rel. to 14 deg [mrad] -> RPC");
    fh_pid_3d->GetZaxis()->SetTitle("Charge (ToFD)");
    fh_pid_3d->GetYaxis()->SetTitleOffset(1.1);
    fh_pid_3d->GetXaxis()->CenterTitle(true);
    fh_pid_3d->GetYaxis()->CenterTitle(true);
    fh_pid_3d->GetZaxis()->CenterTitle(true);
    fh_pid_3d->GetXaxis()->SetLabelSize(0.045);
    fh_pid_3d->GetXaxis()->SetTitleSize(0.04);
    fh_pid_3d->GetYaxis()->SetLabelSize(0.045);
    fh_pid_3d->GetYaxis()->SetTitleSize(0.04); 
    fh_pid_3d->GetZaxis()->SetLabelSize(0.045);
    fh_pid_3d->GetZaxis()->SetTitleSize(0.04); 
   
    

    cAngRot->Divide(2,2);
    gStyle->SetPalette(kRainBow);
    cAngRot->cd(2);
    gPad->SetLogz();
    fh_ang_vs_pos_rot->Draw("colz,cp55"); 
    gStyle->SetPalette(kRainBow);
    cAngRot->cd(1);
    gPad->SetLogz();
    fh2_fib_ang_vs_x->Draw("colz");   
    cAngRot->cd(3);
    gPad->SetLogz();
    fh_pid->Draw("colz");
    cAngRot->cd(4);
    gPad->SetLogy();
    //fh_pid_3d->Draw("col");
    
    cPID_3D = new TCanvas("PID", "PID", 10, 10, 800, 700);
    //ntuple = new TNtuple("ntuple","data from ascii file","x:y:z:count");
    gStyle->SetPalette(kRainBow);
    //ntuple->Draw("x:y:z:cont","","COLZ");
    fh_pid_3d->Draw("colt");


    cSlopeRot = new TCanvas("Slope_vs_positionX_Fibers rotated", "Angle_XZ vs position X of Fibers", 10, 10, 800, 700);

    fh_slope_vs_pos_rot =
        new TH2F("SlopevsPosXRot", "Slope vs Position on Fibers rotated", 600, -60., 60., 1000, -50., 50.);
    fh_slope_vs_pos_rot->GetXaxis()->SetTitle("Arb. X");
    fh_slope_vs_pos_rot->GetYaxis()->SetTitle("Arb. Y");
    fh_slope_vs_pos_rot->GetYaxis()->SetTitleOffset(1.1);
    fh_slope_vs_pos_rot->GetXaxis()->CenterTitle(true);
    fh_slope_vs_pos_rot->GetYaxis()->CenterTitle(true);
    fh_slope_vs_pos_rot->GetXaxis()->SetLabelSize(0.045);
    fh_slope_vs_pos_rot->GetXaxis()->SetTitleSize(0.04);
    fh_slope_vs_pos_rot->GetYaxis()->SetLabelSize(0.045);
    fh_slope_vs_pos_rot->GetYaxis()->SetTitleSize(0.04);


    cSlopeRot->Divide(2,2);
    gStyle->SetPalette(kRainBow);
    cSlopeRot->cd(2);
    gPad->SetLogz();
    fh_slope_vs_pos_rot->Draw("colz,cp55"); 
    gStyle->SetPalette(kRainBow);
    cSlopeRot->cd(1);
    gPad->SetLogz();
    fh2_fib_slope_vs_x->Draw("colz");   
    cSlopeRot->cd(3);
    gPad->SetLogz();
    fh_pid_slope->Draw("colz");
    /*
    // AngleY and positionY on the target position
    auto cAPY = new TCanvas("AngleY_vs_positionY_target", "Angle_YZ vs position Y on target", 10, 10, 800, 700);
    fh2_angvsposy =
        new TH2F("AngYvsPosY", "Angle vs position on target", 500, -fWidthTarget, fWidthTarget, 500, -10., 10.);
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
*/
    // Hit data, Z versus beta
    
    cToFD_Z = new TCanvas("Charge_ToFD", "Q in ToFD", 10, 10, 800, 700);
    fh_ToFD_Charge =
        new TH1F("Charge Number", "Counts", 1200, 0., 12.);
    fh_ToFD_Charge->GetXaxis()->SetTitle("Charge-Z");
    fh_ToFD_Charge->GetYaxis()->SetTitle("Counts");
    fh_ToFD_Charge->GetYaxis()->SetTitleOffset(1.1);
    fh_ToFD_Charge->GetXaxis()->CenterTitle(true);
    fh_ToFD_Charge->GetYaxis()->CenterTitle(true);
    fh_ToFD_Charge->GetXaxis()->SetLabelSize(0.045);
    fh_ToFD_Charge->GetXaxis()->SetTitleSize(0.045);
    fh_ToFD_Charge->GetYaxis()->SetLabelSize(0.045);
    fh_ToFD_Charge->GetYaxis()->SetTitleSize(0.045);
    cToFD_Z->cd();
    gPad->SetLogy();
    fh_ToFD_Charge->Draw("colz");
    
    AngCorrFibToFD = new TCanvas("Angular_Correlation_Fib_ToFD","Angle fibers vs angle ToFD",10,10,800,700);
    fh2_ang_Fib_vs_ToFD = new TH2F("Angles Fiber","Angles ToFD",400,-20.,20.,400,-20.,20.);
        fh2_ang_Fib_vs_ToFD->GetXaxis()->SetTitle("Angle Fiber32 -> Fibers31/33 [mrad]");
    fh2_ang_Fib_vs_ToFD->GetYaxis()->SetTitle("Angle Fiber mean -> ToFD [mrad]");
    fh2_ang_Fib_vs_ToFD->GetYaxis()->SetTitleOffset(1.1);
    fh2_ang_Fib_vs_ToFD->GetXaxis()->CenterTitle(true);
    fh2_ang_Fib_vs_ToFD->GetYaxis()->CenterTitle(true);
    fh2_ang_Fib_vs_ToFD->GetXaxis()->SetLabelSize(0.045);
    fh2_ang_Fib_vs_ToFD->GetXaxis()->SetTitleSize(0.045);
    fh2_ang_Fib_vs_ToFD->GetYaxis()->SetLabelSize(0.045);
    fh2_ang_Fib_vs_ToFD->GetYaxis()->SetTitleSize(0.045);
    //AngCorrFibToFD->cd();
    //gPad->SetLogz();
    //fh2_ang_Fib_vs_ToFD->Draw("colz");

    fh2_ang_Fib_vs_ToFD_nocuts = new TH2F("Angles Fiber no cuts","Angles ToFD",400,-20.,20.,400,-20.,20.);
        fh2_ang_Fib_vs_ToFD_nocuts->GetXaxis()->SetTitle("Angle Fiber32 -> Fibers31/33 [mrad]");
    fh2_ang_Fib_vs_ToFD_nocuts->GetYaxis()->SetTitle("Angle Fiber mean -> ToFD [mrad]");
    fh2_ang_Fib_vs_ToFD_nocuts->GetYaxis()->SetTitleOffset(1.1);
    fh2_ang_Fib_vs_ToFD_nocuts->GetXaxis()->CenterTitle(true);
    fh2_ang_Fib_vs_ToFD_nocuts->GetYaxis()->CenterTitle(true);
    fh2_ang_Fib_vs_ToFD_nocuts->GetXaxis()->SetLabelSize(0.045);
    fh2_ang_Fib_vs_ToFD_nocuts->GetXaxis()->SetTitleSize(0.045);
    fh2_ang_Fib_vs_ToFD_nocuts->GetYaxis()->SetLabelSize(0.045);
    fh2_ang_Fib_vs_ToFD_nocuts->GetYaxis()->SetTitleSize(0.045);
    AngCorrFibToFD->Divide(2,1);
    AngCorrFibToFD->cd(1);
    gPad->SetLogz();
    fh2_ang_Fib_vs_ToFD_nocuts->Draw("colz");
    AngCorrFibToFD->cd(2);
    gPad->SetLogz();
    fh2_ang_Fib_vs_ToFD->Draw("colz");

 

    cFibToT_Q = new TCanvas("FibToT_Charge_ToFD", "FibToT vs Q ToFD", 10, 10, 800, 700);
    
    fh2_FibToT_vs_QToFD = new TH2F("FibToTvsQToFD", "FibToT vs QToFD", 400, 0., 40.,200, 0.,20.);
    fh2_FibToT_vs_QToFD->GetXaxis()->SetTitle("Fib32 ToT [ns]");
    fh2_FibToT_vs_QToFD->GetYaxis()->SetTitle("Charge (ToFD)");
    fh2_FibToT_vs_QToFD->GetYaxis()->SetTitleOffset(1.1);
    fh2_FibToT_vs_QToFD->GetXaxis()->CenterTitle(true);
    fh2_FibToT_vs_QToFD->GetYaxis()->CenterTitle(true);
    fh2_FibToT_vs_QToFD->GetXaxis()->SetLabelSize(0.045);
    fh2_FibToT_vs_QToFD->GetXaxis()->SetTitleSize(0.045);
    fh2_FibToT_vs_QToFD->GetYaxis()->SetLabelSize(0.045);
    fh2_FibToT_vs_QToFD->GetYaxis()->SetTitleSize(0.045);
    cFibToT_Q->cd();
    gPad->SetLogz();
    fh2_FibToT_vs_QToFD->Draw("colz");

    cFibToTCorr = new TCanvas("FibToT_Corr", "Fib ToT Corr", 10, 10, 800, 700);

    fh2_ToT_Fib32_vs_Fib31 = new TH2F("ToTFib32vsFib31", "ToT Fib32 vs Fib31", 400, 0., 40.,400, 0., 40.);
    fh2_ToT_Fib32_vs_Fib31->GetXaxis()->SetTitle("Fib32 ToT [ns]");
    fh2_ToT_Fib32_vs_Fib31->GetYaxis()->SetTitle("Fib31 ToT [ns]");
    fh2_ToT_Fib32_vs_Fib31->GetYaxis()->SetTitleOffset(1.1);
    fh2_ToT_Fib32_vs_Fib31->GetXaxis()->CenterTitle(true);
    fh2_ToT_Fib32_vs_Fib31->GetYaxis()->CenterTitle(true);
    fh2_ToT_Fib32_vs_Fib31->GetXaxis()->SetLabelSize(0.045);
    fh2_ToT_Fib32_vs_Fib31->GetXaxis()->SetTitleSize(0.045);
    fh2_ToT_Fib32_vs_Fib31->GetYaxis()->SetLabelSize(0.045);
    fh2_ToT_Fib32_vs_Fib31->GetYaxis()->SetTitleSize(0.045);
    
    fh2_ToT_Fib32_vs_Fib30 = new TH2F("ToTFib32vsFib30", "ToT Fib32 vs Fib30", 400, 0., 40.,400, 0., 40.);
    fh2_ToT_Fib32_vs_Fib30->GetXaxis()->SetTitle("Fib32 ToT [ns]");
    fh2_ToT_Fib32_vs_Fib30->GetYaxis()->SetTitle("Fib30 ToT [ns]");
    fh2_ToT_Fib32_vs_Fib30->GetYaxis()->SetTitleOffset(1.1);
    fh2_ToT_Fib32_vs_Fib30->GetXaxis()->CenterTitle(true);
    fh2_ToT_Fib32_vs_Fib30->GetYaxis()->CenterTitle(true);
    fh2_ToT_Fib32_vs_Fib30->GetXaxis()->SetLabelSize(0.045);
    fh2_ToT_Fib32_vs_Fib30->GetXaxis()->SetTitleSize(0.045);
    fh2_ToT_Fib32_vs_Fib30->GetYaxis()->SetLabelSize(0.045);
    fh2_ToT_Fib32_vs_Fib30->GetYaxis()->SetTitleSize(0.045);

    fh2_ToT_Fib32_vs_Fib33 = new TH2F("ToTFib32vsFib33", "ToT Fib32 vs Fib33", 400, 0., 40.,400, 0., 40.);
    fh2_ToT_Fib32_vs_Fib33->GetXaxis()->SetTitle("Fib32 ToT [ns]");
    fh2_ToT_Fib32_vs_Fib33->GetYaxis()->SetTitle("Fib33 ToT [ns]");
    fh2_ToT_Fib32_vs_Fib33->GetYaxis()->SetTitleOffset(1.1);
    fh2_ToT_Fib32_vs_Fib33->GetXaxis()->CenterTitle(true);
    fh2_ToT_Fib32_vs_Fib33->GetYaxis()->CenterTitle(true);
    fh2_ToT_Fib32_vs_Fib33->GetXaxis()->SetLabelSize(0.045);
    fh2_ToT_Fib32_vs_Fib33->GetXaxis()->SetTitleSize(0.045);
    fh2_ToT_Fib32_vs_Fib33->GetYaxis()->SetLabelSize(0.045);
    fh2_ToT_Fib32_vs_Fib33->GetYaxis()->SetTitleSize(0.045);

    fh_pos_TPat = new TH2F("X_vs_TPat", "X vs Tpat", 600, -60., 60.,17, 0., 17.);
    fh_pos_TPat->GetXaxis()->SetTitle("X [cm]");
    fh_pos_TPat->GetYaxis()->SetTitle("TPat");
    fh_pos_TPat->GetYaxis()->SetTitleOffset(1.1);
    fh_pos_TPat->GetXaxis()->CenterTitle(true);
    fh_pos_TPat->GetYaxis()->CenterTitle(true);
    fh_pos_TPat->GetXaxis()->SetLabelSize(0.045);
    fh_pos_TPat->GetXaxis()->SetTitleSize(0.045);
    fh_pos_TPat->GetYaxis()->SetLabelSize(0.045);
    fh_pos_TPat->GetYaxis()->SetTitleSize(0.045);

    fh_x_vs_tof = new TH2F("X_vs_ToF", "X vs ToF", 600, -60., 60., 1000, -200., 0.);
    fh_x_vs_tof->GetXaxis()->SetTitle("X [cm]");
    fh_x_vs_tof->GetYaxis()->SetTitle("ToF [ns]");
    fh_x_vs_tof->GetYaxis()->SetTitleOffset(1.1);
    fh_x_vs_tof->GetXaxis()->CenterTitle(true);
    fh_x_vs_tof->GetYaxis()->CenterTitle(true);
    fh_x_vs_tof->GetXaxis()->SetLabelSize(0.045);
    fh_x_vs_tof->GetXaxis()->SetTitleSize(0.045);
    fh_x_vs_tof->GetYaxis()->SetLabelSize(0.045);
    fh_x_vs_tof->GetYaxis()->SetTitleSize(0.045);    

    fh_x_vs_q = new TH2F("X_vs_Q", "X vs Q", 600, -60., 60., 300, 4., 7.);
    fh_x_vs_q->GetXaxis()->SetTitle("X [cm]");
    fh_x_vs_q->GetYaxis()->SetTitle("charge");
    fh_x_vs_q->GetYaxis()->SetTitleOffset(1.1);
    fh_x_vs_q->GetXaxis()->CenterTitle(true);
    fh_x_vs_q->GetYaxis()->CenterTitle(true);
    fh_x_vs_q->GetXaxis()->SetLabelSize(0.045);
    fh_x_vs_q->GetXaxis()->SetTitleSize(0.045);
    fh_x_vs_q->GetYaxis()->SetLabelSize(0.045);
    fh_x_vs_q->GetYaxis()->SetTitleSize(0.045);    

    cFibToTCorr->Divide(2,2);
    cFibToTCorr->cd(1);
    gPad->SetLogz();
    fh2_ToT_Fib32_vs_Fib30->Draw("colz");
    cFibToTCorr->cd(2);
    gPad->SetLogz();
    //fh2_ToT_Fib32_vs_Fib31->Draw("colz");
    fh_x_vs_q->Draw("colz");
    cFibToTCorr->cd(3);
    gPad->SetLogz();
    //fh2_ToT_Fib32_vs_Fib33->Draw("colz");
    fh_x_vs_tof->Draw("colz");
    cFibToTCorr->cd(4);
    gPad->SetLogz();
    fh_pos_TPat->Draw("colz");

    cFibToFDnocuts = new TCanvas("NoCuts", "NoCuts", 10, 10, 800, 700);
    fh2_fib_ang_vs_x_nocuts =
        new TH2F("AngXvsPosXnocuts", "Angle vs position on Fibers no cuts", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_ang_vs_x_nocuts->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_ang_vs_x_nocuts->GetYaxis()->SetTitle("NL <- Angle rel. to 14 deg [mrad] -> RPC");
    fh2_fib_ang_vs_x_nocuts->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_ang_vs_x_nocuts->GetXaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x_nocuts->GetYaxis()->CenterTitle(true);
    fh2_fib_ang_vs_x_nocuts->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x_nocuts->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_ang_vs_x_nocuts->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_ang_vs_x_nocuts->GetYaxis()->SetTitleSize(0.04);

    fh2_fib_slope_vs_x_nocuts =
        new TH2F("slopeXvsPosXnocuts", "slope vs position on Fibers", 600, -60., 60., 1000, -50., 50.);
    fh2_fib_slope_vs_x_nocuts->GetXaxis()->SetTitle("RPC <-- X Position [cm] --> NeuLand");
    fh2_fib_slope_vs_x_nocuts->GetYaxis()->SetTitle("NL <- slope rel. to 14 deg [mrad] -> RPC");
    fh2_fib_slope_vs_x_nocuts->GetYaxis()->SetTitleOffset(1.1);
    fh2_fib_slope_vs_x_nocuts->GetXaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x_nocuts->GetYaxis()->CenterTitle(true);
    fh2_fib_slope_vs_x_nocuts->GetXaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x_nocuts->GetXaxis()->SetTitleSize(0.04);
    fh2_fib_slope_vs_x_nocuts->GetYaxis()->SetLabelSize(0.045);
    fh2_fib_slope_vs_x_nocuts->GetYaxis()->SetTitleSize(0.04);

    fh2_fibtracking_planeXZ_nocuts = new TH2F(Name1 +"nocuts", Name2, 250, 0., 1250, 1400, -1. * 70., 70.);
    fh2_fibtracking_planeXZ_nocuts->GetXaxis()->SetTitle("Beam direction-Z [cm]");
    fh2_fibtracking_planeXZ_nocuts->GetYaxis()->SetTitle("X Position [cm]");
    fh2_fibtracking_planeXZ_nocuts->GetYaxis()->SetTitleOffset(1.1);
    fh2_fibtracking_planeXZ_nocuts->GetXaxis()->CenterTitle(true);
    fh2_fibtracking_planeXZ_nocuts->GetYaxis()->CenterTitle(true);
    fh2_fibtracking_planeXZ_nocuts->GetXaxis()->SetLabelSize(0.045);
    fh2_fibtracking_planeXZ_nocuts->GetXaxis()->SetTitleSize(0.045);
    fh2_fibtracking_planeXZ_nocuts->GetYaxis()->SetLabelSize(0.045);
    fh2_fibtracking_planeXZ_nocuts->GetYaxis()->SetTitleSize(0.045);
    
    fh_Rsqr = new TH1F("R_Sqr", "R Squared", 1100, 0., 1.1);
    fh_Rsqr->GetXaxis()->SetTitle("R Squared");
    fh_Rsqr->GetYaxis()->SetTitle("Counts");
    fh_Rsqr->GetYaxis()->SetTitleOffset(1.1);
    fh_Rsqr->GetXaxis()->CenterTitle(true);
    fh_Rsqr->GetYaxis()->CenterTitle(true);
    fh_Rsqr->GetXaxis()->SetLabelSize(0.045);
    fh_Rsqr->GetXaxis()->SetTitleSize(0.045);
    fh_Rsqr->GetYaxis()->SetLabelSize(0.045);
    fh_Rsqr->GetYaxis()->SetTitleSize(0.045);

    cFibToFDnocuts->Divide(2,2);
    cFibToFDnocuts->cd(1);
    gPad->SetLogz();
    fh2_fib_ang_vs_x_nocuts->Draw("colz");

    cFibToFDnocuts->cd(2);
    gPad->SetLogz();
    fh2_fib_slope_vs_x_nocuts->Draw("colz");
    cFibToFDnocuts->cd(3);
    gPad->SetLogz();
    fh2_fibtracking_planeXZ_nocuts->Draw("colz");   
    cFibToFDnocuts->cd(4);
    gPad->SetLogy();
    fh_Rsqr->Draw();


    cFib30Rel = new TCanvas("Fib30Rel", "Fib30Rel", 10, 10, 800, 700);

    fh_f32X_vs_f30Y = new TH2F("Fib32XvsFib30Y", "Fib32X vs Fib30Y", 600, -30., 30.,600, -30., 30.);
    fh_f32X_vs_f30Y->GetXaxis()->SetTitle("Fib32 X [cm]");
    fh_f32X_vs_f30Y->GetYaxis()->SetTitle("Fib30 Y [cm]");
    fh_f32X_vs_f30Y->GetYaxis()->SetTitleOffset(1.1);
    fh_f32X_vs_f30Y->GetXaxis()->CenterTitle(true);
    fh_f32X_vs_f30Y->GetYaxis()->CenterTitle(true);
    fh_f32X_vs_f30Y->GetXaxis()->SetLabelSize(0.045);
    fh_f32X_vs_f30Y->GetXaxis()->SetTitleSize(0.045);
    fh_f32X_vs_f30Y->GetYaxis()->SetLabelSize(0.045);
    fh_f32X_vs_f30Y->GetYaxis()->SetTitleSize(0.045);

    fh_f32Y_vs_f30Y = new TH2F("Fib32YvsFib30Y", "Fib32Y vs Fib30Y", 600, -30., 30.,600, -30., 30.);
    fh_f32Y_vs_f30Y->GetXaxis()->SetTitle("Fib32 Y [cm]");
    fh_f32Y_vs_f30Y->GetYaxis()->SetTitle("Fib30 Y [cm]");
    fh_f32Y_vs_f30Y->GetYaxis()->SetTitleOffset(1.1);
    fh_f32Y_vs_f30Y->GetXaxis()->CenterTitle(true);
    fh_f32Y_vs_f30Y->GetYaxis()->CenterTitle(true);
    fh_f32Y_vs_f30Y->GetXaxis()->SetLabelSize(0.045);
    fh_f32Y_vs_f30Y->GetXaxis()->SetTitleSize(0.045);
    fh_f32Y_vs_f30Y->GetYaxis()->SetLabelSize(0.045);
    fh_f32Y_vs_f30Y->GetYaxis()->SetTitleSize(0.045);

    fh_f32tot_vs_f30Y = new TH2F("Fib32totvsFib30Y", "Fib32ToT vs Fib30Y", 400, 0., 40.,600, -30., 30.);
    fh_f32tot_vs_f30Y->GetXaxis()->SetTitle("Fib32 ToT [ns]");
    fh_f32tot_vs_f30Y->GetYaxis()->SetTitle("Fib30 Y [cm]");
    fh_f32tot_vs_f30Y->GetYaxis()->SetTitleOffset(1.1);
    fh_f32tot_vs_f30Y->GetXaxis()->CenterTitle(true);
    fh_f32tot_vs_f30Y->GetYaxis()->CenterTitle(true);
    fh_f32tot_vs_f30Y->GetXaxis()->SetLabelSize(0.045);
    fh_f32tot_vs_f30Y->GetXaxis()->SetTitleSize(0.045);
    fh_f32tot_vs_f30Y->GetYaxis()->SetLabelSize(0.045);
    fh_f32tot_vs_f30Y->GetYaxis()->SetTitleSize(0.045);

    fh_tofdq_vs_f30Y = new TH2F("ToFDQvsFib30Y", "ToFDQ vs Fib30Y", 100, 0., 10.,600, -30., 30.);
    fh_tofdq_vs_f30Y->GetXaxis()->SetTitle("ToFD Charge");
    fh_tofdq_vs_f30Y->GetYaxis()->SetTitle("Fib30 Y [cm]");
    fh_tofdq_vs_f30Y->GetYaxis()->SetTitleOffset(1.1);
    fh_tofdq_vs_f30Y->GetXaxis()->CenterTitle(true);
    fh_tofdq_vs_f30Y->GetYaxis()->CenterTitle(true);
    fh_tofdq_vs_f30Y->GetXaxis()->SetLabelSize(0.045);
    fh_tofdq_vs_f30Y->GetXaxis()->SetTitleSize(0.045);
    fh_tofdq_vs_f30Y->GetYaxis()->SetLabelSize(0.045);
    fh_tofdq_vs_f30Y->GetYaxis()->SetTitleSize(0.045);

    cFib30Rel->Divide(2,2);
    cFib30Rel->cd(1);
    gPad->SetLogz();
    fh_f32X_vs_f30Y->Draw("colz");

    cFib30Rel->cd(2);
    gPad->SetLogz();
    fh_f32Y_vs_f30Y->Draw("colz");
    cFib30Rel->cd(3);
    gPad->SetLogz();
    fh_f32tot_vs_f30Y->Draw("colz");   
    cFib30Rel->cd(4);
    gPad->SetLogz();
    fh_tofdq_vs_f30Y->Draw("colz");

    cXcorr = new TCanvas("Xcorr", "Xcorr", 10, 10, 800, 700);
    
    fh_X_fib_vs_tofd_no_cut = new TH2F("X_corr_fib_tofd_fit_nocut", "X corr fib tofd fit no cut", 600, -60., 60., 600, -60., 60.);
    fh_X_fib_vs_tofd_no_cut->GetXaxis()->SetTitle("ToFD X pos [cm]");
    fh_X_fib_vs_tofd_no_cut->GetYaxis()->SetTitle("Extrap. X pos [cm]");
    fh_X_fib_vs_tofd_no_cut->GetYaxis()->SetTitleOffset(1.1);
    fh_X_fib_vs_tofd_no_cut->GetXaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd_no_cut->GetYaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd_no_cut->GetXaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd_no_cut->GetXaxis()->SetTitleSize(0.045);
    fh_X_fib_vs_tofd_no_cut->GetYaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd_no_cut->GetYaxis()->SetTitleSize(0.045);
    
    fh_X_fib_vs_tofd = new TH2F("X_corr_fib_tofd_fit", "X corr fib tofd fit", 600, -60., 60., 600, -60., 60.);
    fh_X_fib_vs_tofd->GetXaxis()->SetTitle("ToFD X pos [cm]");
    fh_X_fib_vs_tofd->GetYaxis()->SetTitle("Extrap. X pos [cm]");
    fh_X_fib_vs_tofd->GetYaxis()->SetTitleOffset(1.1);
    fh_X_fib_vs_tofd->GetXaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd->GetYaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd->GetXaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd->GetXaxis()->SetTitleSize(0.045);
    fh_X_fib_vs_tofd->GetYaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd->GetYaxis()->SetTitleSize(0.045);
    
    fh_X_fib_vs_tofd_ang = new TH2F("X_corr_fib_tofd_ang", "X corr fib tofd ang", 600, -60., 60., 600, -60., 60.);
    fh_X_fib_vs_tofd_ang->GetXaxis()->SetTitle("ToFD X pos [cm]");
    fh_X_fib_vs_tofd_ang->GetYaxis()->SetTitle("Extrap. X pos [cm]");
    fh_X_fib_vs_tofd_ang->GetYaxis()->SetTitleOffset(1.1);
    fh_X_fib_vs_tofd_ang->GetXaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd_ang->GetYaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd_ang->GetXaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd_ang->GetXaxis()->SetTitleSize(0.045);
    fh_X_fib_vs_tofd_ang->GetYaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd_ang->GetYaxis()->SetTitleSize(0.045);   

    fh_X_fib_vs_tofd_ang_no_cut = new TH2F("X_corr_fib_tofd_ang_no_cut", "X corr fib tofd ang no cut", 600, -60., 60., 600, -60., 60.);
    fh_X_fib_vs_tofd_ang_no_cut->GetXaxis()->SetTitle("ToFD X pos [cm]");
    fh_X_fib_vs_tofd_ang_no_cut->GetYaxis()->SetTitle("Extrap. X pos [cm]");
    fh_X_fib_vs_tofd_ang_no_cut->GetYaxis()->SetTitleOffset(1.1);
    fh_X_fib_vs_tofd_ang_no_cut->GetXaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd_ang_no_cut->GetYaxis()->CenterTitle(true);
    fh_X_fib_vs_tofd_ang_no_cut->GetXaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd_ang_no_cut->GetXaxis()->SetTitleSize(0.045);
    fh_X_fib_vs_tofd_ang_no_cut->GetYaxis()->SetLabelSize(0.045);
    fh_X_fib_vs_tofd_ang_no_cut->GetYaxis()->SetTitleSize(0.045);

    cXcorr->Divide(2,2);
    cXcorr->cd(1);
    gPad->SetLogz();
    fh_X_fib_vs_tofd_no_cut->Draw("colz");
    cXcorr->cd(3);
    gPad->SetLogz();
    fh_X_fib_vs_tofd->Draw("colz");
    cXcorr->cd(2);
    gPad->SetLogz();
    fh_X_fib_vs_tofd_ang_no_cut->Draw("colz");
    cXcorr->cd(4);
    gPad->SetLogz();
    fh_X_fib_vs_tofd_ang->Draw("colz");
    

    cDeltaQ = new TCanvas("ToFDDeltaQ", "ToFDDeltaQ", 10, 10, 800, 700);
    cDeltaQ->Divide(3,2);
    int g = 0;
    for(int i = 0; i<4; i++){
        for(int j=i+1;j<4;j++){
            char strName1[255];
            sprintf(strName1, "tofd_Charge_plane_%d_vs_%d",i+1, j+1);
            char strName2[255];
            sprintf(strName2, "tofd Charge plane %d vs %d",i+1, j+1);
            fh_tofd_charge_planes[g] = new TH2F(strName1, strName2, 45, 0., 45., 200,-10.,10.);
            fh_tofd_charge_planes[g]->GetXaxis()->SetTitle("Bar");
            fh_tofd_charge_planes[g]->GetYaxis()->SetTitle("Delta Q");
            fh_tofd_charge_planes[g]->GetYaxis()->SetTitleOffset(1.);
            fh_tofd_charge_planes[g]->GetXaxis()->CenterTitle(true);
            fh_tofd_charge_planes[g]->GetYaxis()->CenterTitle(true);
            fh_tofd_charge_planes[g]->GetXaxis()->SetLabelSize(0.045);
            fh_tofd_charge_planes[g]->GetXaxis()->SetTitleSize(0.045);
            fh_tofd_charge_planes[g]->GetYaxis()->SetLabelSize(0.045);
            fh_tofd_charge_planes[g]->GetYaxis()->SetTitleSize(0.045);
            fh_tofd_charge_planes[g]->SetFillColor(31);

            cDeltaQ->cd(g+1);
            gPad->SetLogz();
            fh_tofd_charge_planes[g]->Draw("colz");
            g++;
        }
    }

    cMult = new TCanvas("Mult", "Mult", 10, 10, 800, 700);
    fh_mult_coinc = new TH1D("mult_coinc", "Multiplicity in coincidence", 151, -1, 150.);
    fh_mult_coinc->GetXaxis()->SetTitle("Multiplicity");
    fh_mult_coinc->GetYaxis()->SetTitle("Counts");
    fh_mult_coinc->GetYaxis()->SetTitleOffset(1.1);
    fh_mult_coinc->GetXaxis()->CenterTitle(true);
    fh_mult_coinc->GetYaxis()->CenterTitle(true);
    fh_mult_coinc->GetXaxis()->SetLabelSize(0.045);
    fh_mult_coinc->GetXaxis()->SetTitleSize(0.045);
    fh_mult_coinc->GetYaxis()->SetLabelSize(0.045);
    fh_mult_coinc->GetYaxis()->SetTitleSize(0.045);

    fh_mult_minus_los = new TH1D("mult_coinc_LOS", "Multiplicity in coincidence minus LOS", 151, -1., 150.);
    fh_mult_minus_los->GetXaxis()->SetTitle("Multiplicity");
    fh_mult_minus_los->GetYaxis()->SetTitle("Counts");
    fh_mult_minus_los->GetYaxis()->SetTitleOffset(1.1);
    fh_mult_minus_los->GetXaxis()->CenterTitle(true);
    fh_mult_minus_los->GetYaxis()->CenterTitle(true);
    fh_mult_minus_los->GetXaxis()->SetLabelSize(0.045);
    fh_mult_minus_los->GetXaxis()->SetTitleSize(0.045);
    fh_mult_minus_los->GetYaxis()->SetLabelSize(0.045);
    fh_mult_minus_los->GetYaxis()->SetTitleSize(0.045);
    
    fh_mult_fib30 = new TH1D("mult_fib30", "Multiplicity in fib30", 151, -1, 150.);
    fh_mult_fib30->GetXaxis()->SetTitle("Multiplicity");
    fh_mult_fib30->GetYaxis()->SetTitle("Counts");
    fh_mult_fib30->GetYaxis()->SetTitleOffset(1.1);
    fh_mult_fib30->GetXaxis()->CenterTitle(true);
    fh_mult_fib30->GetYaxis()->CenterTitle(true);
    fh_mult_fib30->GetXaxis()->SetLabelSize(0.045);
    fh_mult_fib30->GetXaxis()->SetTitleSize(0.045);
    fh_mult_fib30->GetYaxis()->SetLabelSize(0.045);
    fh_mult_fib30->GetYaxis()->SetTitleSize(0.045);

    fh_mult_fib31 = new TH1D("mult_fib31", "Multiplicity in fib31", 151, -1, 150.);
    fh_mult_fib31->GetXaxis()->SetTitle("Multiplicity");
    fh_mult_fib31->GetYaxis()->SetTitle("Counts");
    fh_mult_fib31->GetYaxis()->SetTitleOffset(1.1);
    fh_mult_fib31->GetXaxis()->CenterTitle(true);
    fh_mult_fib31->GetYaxis()->CenterTitle(true);
    fh_mult_fib31->GetXaxis()->SetLabelSize(0.045);
    fh_mult_fib31->GetXaxis()->SetTitleSize(0.045);
    fh_mult_fib31->GetYaxis()->SetLabelSize(0.045);
    fh_mult_fib31->GetYaxis()->SetTitleSize(0.045);   

    fh_mult_fib32 = new TH1D("mult_fib32", "Multiplicity in fib32", 151, -1, 150.);
    fh_mult_fib32->GetXaxis()->SetTitle("Multiplicity");
    fh_mult_fib32->GetYaxis()->SetTitle("Counts");
    fh_mult_fib32->GetYaxis()->SetTitleOffset(1.1);
    fh_mult_fib32->GetXaxis()->CenterTitle(true);
    fh_mult_fib32->GetYaxis()->CenterTitle(true);
    fh_mult_fib32->GetXaxis()->SetLabelSize(0.045);
    fh_mult_fib32->GetXaxis()->SetTitleSize(0.045);
    fh_mult_fib32->GetYaxis()->SetLabelSize(0.045);
    fh_mult_fib32->GetYaxis()->SetTitleSize(0.045);
        
    fh_mult_fib33 = new TH1D("mult_fib33", "Multiplicity in fib33", 151, -1, 150.);
    fh_mult_fib33->GetXaxis()->SetTitle("Multiplicity");
    fh_mult_fib33->GetYaxis()->SetTitle("Counts");
    fh_mult_fib33->GetYaxis()->SetTitleOffset(1.1);
    fh_mult_fib33->GetXaxis()->CenterTitle(true);
    fh_mult_fib33->GetYaxis()->CenterTitle(true);
    fh_mult_fib33->GetXaxis()->SetLabelSize(0.045);
    fh_mult_fib33->GetXaxis()->SetTitleSize(0.045);
    fh_mult_fib33->GetYaxis()->SetLabelSize(0.045);
    fh_mult_fib33->GetYaxis()->SetTitleSize(0.045);
        
    fh_mult_fibAll = new TH1D("mult_fibAll", "Multiplicity in fibAll", 151, -1, 150.);
    fh_mult_fibAll->GetXaxis()->SetTitle("Multiplicity");
    fh_mult_fibAll->GetYaxis()->SetTitle("Counts");
    fh_mult_fibAll->GetYaxis()->SetTitleOffset(1.1);
    fh_mult_fibAll->GetXaxis()->CenterTitle(true);
    fh_mult_fibAll->GetYaxis()->CenterTitle(true);
    fh_mult_fibAll->GetXaxis()->SetLabelSize(0.045);
    fh_mult_fibAll->GetXaxis()->SetTitleSize(0.045);
    fh_mult_fibAll->GetYaxis()->SetLabelSize(0.045);
    fh_mult_fibAll->GetYaxis()->SetTitleSize(0.045);

    fh_ToF_vs_charge = new TH2F("tof_vs_q", "tof vs q",100, 0.,10., 2000,-200.,0.);
    fh_ToF_vs_charge->GetXaxis()->SetTitle("Charge");
    fh_ToF_vs_charge->GetYaxis()->SetTitle("ToF ns");
    fh_ToF_vs_charge->GetYaxis()->SetTitleOffset(1.);
    fh_ToF_vs_charge->GetXaxis()->CenterTitle(true);
    fh_ToF_vs_charge->GetYaxis()->CenterTitle(true);
    fh_ToF_vs_charge->GetXaxis()->SetLabelSize(0.045);
    fh_ToF_vs_charge->GetXaxis()->SetTitleSize(0.045);
    fh_ToF_vs_charge->GetYaxis()->SetLabelSize(0.045);
    fh_ToF_vs_charge->GetYaxis()->SetTitleSize(0.045);
    
    cMult->Divide(3,3);
    cMult->cd(1);
    gPad->SetLogy();
    fh_mult_coinc->Draw();
    cMult->cd(2);
    gPad->SetLogy();
    fh_mult_minus_los->Draw();
    cMult->cd(3);
    gPad->SetLogy();
    fh_mult_fib30->Draw();
    cMult->cd(4);
    gPad->SetLogy();
    fh_mult_fib31->Draw();
    cMult->cd(5);
    gPad->SetLogy();
    fh_mult_fib32->Draw();
    cMult->cd(6);
    gPad->SetLogy();
    fh_mult_fib33->Draw();
    cMult->cd(7);
    gPad->SetLogy();
    fh_mult_fibAll->Draw();
    cMult->cd(8);
    gPad->SetLogz();
    fh_ToF_vs_charge->Draw("colz");



    // MAIN FOLDER
    TFolder* mainfol = new TFolder("Tracking_Fibers", "Tracking info");
    mainfol->Add(cTrackingXZ);
    mainfol->Add(cAngVsX);
    mainfol->Add(cslopeVsX);
    mainfol->Add(AngCorrFibToFD);
    mainfol->Add(cPID_3D);
    mainfol->Add(cToFD_Z);
    mainfol->Add(cAngRot);
    mainfol->Add(cSlopeRot);
    mainfol->Add(cFibToT_Q);
    mainfol->Add(cFib30Rel);
    mainfol->Add(cFibToTCorr);
    mainfol->Add(cFibToFDnocuts);
    mainfol->Add(cFibInfo);
    mainfol->Add(cXcorr);
    mainfol->Add(cDeltaQ);
    mainfol->Add(cMult);
    
    FairRunOnline* run = FairRunOnline::Instance();
    R3BLOG_IF(fatal, NULL == run, "FairRunOnline not found");
    run->GetHttpServer()->Register("", this);
    run->AddObject(mainfol);
   
    // Register command to reset histograms
    run->GetHttpServer()->RegisterCommand("Reset_Fiber_Tracking_HIST", Form("/Objects/%s/->Reset_Histo()", GetName()));
    
    return kSUCCESS;
}

void R3BFiberTrackingOnlineSpectra::Reset_Histo()
{
    R3BLOG(info, "");
    fh2_fibtracking_planeXZ->Reset();
    fh2_fib_ang_vs_x->Reset();
    fh2_fib_ang_vs_x4->Reset();
    fh2_fib_ang_vs_x5->Reset();
    fh2_fib_ang_vs_x6->Reset();
    fh2_fib_slope_vs_x->Reset();
    fh2_fib_slope_vs_x4->Reset();
    fh2_fib_slope_vs_x5->Reset();
    fh2_fib_slope_vs_x6->Reset();
    // fh2_ZvsBeta->Reset();
    fh2_FibToT_vs_QToFD->Reset();
    fh_ang_vs_pos_rot->Reset();
    fh_slope_vs_pos_rot->Reset();
    fh_pid->Reset();
    fh_pid_slope->Reset();
    fh2_ang_Fib_vs_ToFD->Reset();
    fh2_ang_Fib_vs_ToFD_nocuts->Reset();
    fh_ToFD_Charge->Reset();
    fh_pid_3d->Reset();
}

void R3BFiberTrackingOnlineSpectra::Exec(Option_t* option)
{   
 
    fNEvents += 1;
    Bool_t ToFDgate = true;

    fh_mult_fib30->Fill(-1);
    fh_mult_fib31->Fill(-1);
    fh_mult_fib32->Fill(-1);
    fh_mult_fib33->Fill(-1);
    fh_mult_fibAll->Fill(-1);

    if ((fTrigger >= 0) && header && (header->GetTrigger() != fTrigger)) return;
    bool onspill = true;
    //fTpat = 1-16; fTpat_bit = 0-15
    fTpat1 = 16; fTpat2 = 13;
    if (fTpat1 > -1 && fTpat2 > -1)
    {
        Int_t fTpat_bit1 = fTpat1 - 1;
        Int_t fTpat_bit2 = fTpat2 - 1;
        Int_t tpatbin;
        for (int i = 0; i < 16; i++)
        {
            tpatbin = (header->GetTpat() & (1 << i));
            if (tpatbin != 0 && (i < fTpat_bit1 && i > fTpat_bit2))
            {
                onspill = false;
            }
        }
    }

    //cout<<"Tpat "<<header->GetTpat()<<" mask "<<fTPatMask<<endl;

    
    if(/*!(fHitFrs) || */!(fFi32HitDataCA) || !(fHitTofdItems)) return;
    
    //cout<<"FRS hit "<<fHitFrs->GetEntriesFast()<<endl;
//if(header->GetTpat() & fTPatMask){
if(onspill){
    fh_mult_fib30->Fill(0);
    fh_mult_fib31->Fill(0);
    fh_mult_fib32->Fill(0);
    fh_mult_fib33->Fill(0);
    fh_mult_fibAll->Fill(0);
    if(fFi31HitDataCA && fFi33HitDataCA){
        if(
            fFi32HitDataCA->GetEntriesFast() == fFi31HitDataCA->GetEntriesFast() + fFi33HitDataCA->GetEntriesFast() && 4 * fFi32HitDataCA->GetEntriesFast() == fHitTofdItems->GetEntriesFast()
        ){
            fh_mult_coinc->Fill(fFi32HitDataCA->GetEntriesFast());
            fh_mult_minus_los->Fill(fCalLos->GetEntriesFast() - fFi32HitDataCA->GetEntriesFast());
           // if(fCalLos->GetEntriesFast() - fFi32HitDataCA->GetEntriesFast() < 0) cout<<"Los "<<fCalLos->GetEntriesFast()<<" fib "<<fFi32HitDataCA->GetEntriesFast()<<endl;
        }
        else fh_mult_coinc->Fill(-1);
    }
}
    if((header && !(header->GetTpat() & fTPatMask))) return;
    if((/*fHitFrs->GetEntriesFast() != fFi32HitDataCA->GetEntriesFast() ||*/fFi32HitDataCA->GetEntriesFast() != 1 ) || (4.*(fFi32HitDataCA->GetEntriesFast()) != fHitTofdItems->GetEntriesFast())) return;
    if(!(fFi32HitDataCA->GetEntriesFast() == 1)) return;
    if(!(4.*(fFi32HitDataCA->GetEntriesFast()) == fHitTofdItems->GetEntriesFast())) return;
    if(fHitTofdItems->GetEntriesFast() != 4) return;

    Double_t xS2=0.,Aq=0.,ToF=0.,Beta=0.,Brho=0.,Q_FRS=0.;
 if(fFRSGATE){ 
    
    //FRS HIT DATA
    if (fHitFrs && fHitFrs->GetEntriesFast() > 0)
    {
        //cout<<"Hit FRS"<<endl;
        Int_t nHits = fHitFrs->GetEntriesFast();
        //cout<<"Hit FRS "<<nHits<<endl;
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BFrsData* hit = (R3BFrsData*)fHitFrs->At(ihit);
            if (!hit)
                continue;
            //if (hit->GetStaId() != fStaId)
            if (hit->GetStaId() != 1)
                continue;
            xS2 = hit->GetXS2();
            Aq = hit->GetAq();
            ToF = hit->GetTof();
            Beta = hit->GetBeta();
            Brho = hit->GetBrho();
            Q_FRS = hit->GetZ();
            //cout<<"i: "<<ihit<<" xs2 "<<xS2<<" Aq "<<Aq<<" ToF "<<ToF<<" Beta "<<Beta<<" Brho "<<Brho<<" Q "<<Q_FRS<<endl;
        }

    }
 }

//cout<<"LOS "<<fCalLos->GetEntriesFast()<<endl;
//if(fCalLos->GetEntriesFast()!=1) return;

#if 0
////Start LOS
    Int_t fNofLosDetectors = 1;
    Double_t timeTofd = 0;

    Double_t timeLosV[fNofLosDetectors][32];
    Double_t LosTresV[fNofLosDetectors][32];
    Double_t timeLosT[fNofLosDetectors][32];
    Double_t LosTresT[fNofLosDetectors][32];
    Double_t timeLos[fNofLosDetectors][32];
    Double_t totsum[fNofLosDetectors][32];
    Double_t xT_cm[fNofLosDetectors][32];
    Double_t yT_cm[fNofLosDetectors][32];
    Double_t xToT_cm[fNofLosDetectors][32];
    Double_t yToT_cm[fNofLosDetectors][32];
    Double_t xV_cm[fNofLosDetectors][32];
    Double_t yV_cm[fNofLosDetectors][32];

    Double_t time_V[fNofLosDetectors][32][8]; // [det][multihit][pm]
    Double_t time_L[fNofLosDetectors][32][8];
    Double_t time_T[fNofLosDetectors][32][8];
    Double_t tot[fNofLosDetectors][32][8];
    Double_t time_MTDC[32][8] = { 0. };
    Double_t LosTresMTDC[32];

    for (Int_t idet = 0; idet < fNofLosDetectors; idet++)
    {
        for (Int_t imult = 0; imult < 32; imult++)
        {
            timeLosV[idet][imult] = 0.0;
            LosTresV[idet][imult] = 0.0 / 0.0;
            timeLosT[idet][imult] = 0.0;
            LosTresT[idet][imult] = 0.0 / 0.0;
            timeLos[idet][imult] = 0.0;
            totsum[idet][imult] = 0.0;
            xT_cm[idet][imult] = 0.0 / 0.0;
            yT_cm[idet][imult] = 0.0 / 0.0;
            xToT_cm[idet][imult] = -100000.;
            yToT_cm[idet][imult] = -100000.;
            xV_cm[idet][imult] = 0.0 / 0.0;
            yV_cm[idet][imult] = 0.0 / 0.0;
            for (Int_t icha = 0; icha < 8; icha++)
            {
                time_V[idet][imult][icha] = 0.0 / 0.0; // [det][multihit][pm]
                time_L[idet][imult][icha] = 0.0 / 0.0;
                time_T[idet][imult][icha] = 0.0 / 0.0;
                tot[idet][imult][icha] = 0.0 / 0.0;
            }
        }
    }
    Int_t nPartLOS = 0;
    Int_t nPartc[fNofLosDetectors];
    for (Int_t d = 0; d < fNofLosDetectors; d++) nPartc[d] = 0;

    Bool_t iLOSType[fNofLosDetectors][32];
    Bool_t iLOSPileUp[fNofLosDetectors][32];
    for (Int_t idet = 0; idet < fNofLosDetectors; idet++)
    {
        for (Int_t imult = 0; imult < 32; imult++)
        {
            iLOSType[idet][imult] = false;
            iLOSPileUp[idet][imult] = false;
        }
    }   

        nPartLOS = fCalLos->GetEntriesFast();

        Int_t iDet = 0;
        Double_t time_V_LOS1[32][8] = { 0. };
        Double_t time_V_LOS2[32][8] = { 0. };

        for (Int_t iPart = 0; iPart < nPartLOS; iPart++)
        {
            /*
             * nPart is the number of particle passing through LOS detector in one event
             */
            R3BLosCalData* calData = (R3BLosCalData*)fCalLos->At(iPart);
            iDet = calData->GetDetector();

            Double_t sumvtemp = 0, sumltemp = 0, sumttemp = 0;
            for (Int_t iCha = 0; iCha < 8; iCha++)
            {
                sumvtemp += calData->GetTimeV_ns(iCha);
                sumltemp += calData->GetTimeL_ns(iCha);
                sumttemp += calData->GetTimeT_ns(iCha);
            }
            if (!(IS_NAN(sumvtemp)) && !(IS_NAN(sumltemp)) && !(IS_NAN(sumltemp)))
            {
                nPartc[iDet - 1]++;
            }
            else
            {
                continue;
            }

            for (Int_t iCha = 0; iCha < 8; iCha++)
            {
                if (iDet == 1)
                {
                    time_V_LOS1[nPartc[iDet - 1] - 1][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V_LOS1[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeV_ns(iCha);
                    }
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeL_ns(iCha);
                    }

                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeT_ns(iCha);
                    }
                    time_MTDC[nPartc[iDet - 1]][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeM_ns(iCha))))
                    { // MTDC
                        time_MTDC[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeM_ns(iCha);
                    }
                }
                if (iDet == 2)
                {
                    time_V_LOS2[nPartc[iDet - 1] - 1][iCha] = 0. / 0.;
                    if (!(IS_NAN(calData->GetTimeV_ns(iCha))))
                    { // VFTX
                        time_V_LOS2[nPartc[iDet - 1] - 1][iCha] = calData->GetTimeV_ns(iCha);
                    }
                    if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                    { // TAMEX leading
                        time_L[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeL_ns(iCha);
                    }
                    if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                    { // TAMEX trailing
                        time_T[iDet - 1][nPartc[iDet - 1] - 1][iCha] = calData->GetTimeT_ns(iCha);
                    }
                }
            }

            if (!calData)
            {
                cout << "LOS !calData" << endl;
                continue; // can this happen?
            }
        }

        // Sorting VFTX data:

        // detector 1
        if (nPartc[0] > 0)
        {
            std::qsort(time_V_LOS1, nPartc[0], sizeof(*time_V_LOS1), [](const void* arg1, const void* arg2) -> int {
                double const* lhs = static_cast<double const*>(arg1);
                double const* rhs = static_cast<double const*>(arg2);

                return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
            });
            for (Int_t iPart = 0; iPart < nPartc[0]; iPart++)
            {
                for (int ipm = 0; ipm < 8; ipm++)
                {
                    time_V[0][iPart][ipm] = time_V_LOS1[iPart][ipm];
                }
            }
        }
        else if(nPartc[0] > 1) return;

        // detector 2
        if (fNofLosDetectors > 1 && nPartc[1] > 0)
        {
            std::qsort(time_V_LOS2, nPartc[1], sizeof(*time_V_LOS2), [](const void* arg1, const void* arg2) -> int {
                double const* lhs = static_cast<double const*>(arg1);
                double const* rhs = static_cast<double const*>(arg2);

                return (lhs[0] < rhs[0]) ? -1 : ((rhs[0] < lhs[0]) ? 1 : 0);
            });
            for (Int_t iPart = 0; iPart < nPartc[1]; iPart++)
            {
                for (int ipm = 0; ipm < 8; ipm++)
                {
                    time_V[1][iPart][ipm] = time_V_LOS2[iPart][ipm];
                }
            }
        }

        // End sorting

        std::vector<double_t> time_first(fNofLosDetectors, -1.);
        std::vector<double_t> time0(fNofLosDetectors, -1.);
        std::vector<double_t> time1(fNofLosDetectors, -1.);
        std::vector<double_t> time_abs(fNofLosDetectors, -1.);

        for (iDet = 1; iDet <= fNofLosDetectors; iDet++)
        {
            for (Int_t iPart = 0; iPart < nPartc[iDet - 1]; iPart++)
            {
                Bool_t iLOSTypeMCFD = false;
                Bool_t iLOSTypeTAMEX = false;

                if (time_V[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][0])) &&
                    time_V[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][1])) &&
                    time_V[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][2])) &&
                    time_V[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][3])) &&
                    time_V[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][4])) &&
                    time_V[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][5])) &&
                    time_V[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][6])) &&
                    time_V[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][7])))
                {
                    iLOSTypeMCFD = true; // all 8 MCFD times
                }

                if (time_L[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][0])) &&
                    time_L[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][1])) &&
                    time_L[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][2])) &&
                    time_L[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][3])) &&
                    time_L[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][4])) &&
                    time_L[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][5])) &&
                    time_L[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][6])) &&
                    time_L[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][7])) &&

                    time_T[iDet - 1][iPart][0] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][0])) &&
                    time_T[iDet - 1][iPart][1] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][1])) &&
                    time_T[iDet - 1][iPart][2] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][2])) &&
                    time_T[iDet - 1][iPart][3] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][3])) &&
                    time_T[iDet - 1][iPart][4] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][4])) &&
                    time_T[iDet - 1][iPart][5] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][5])) &&
                    time_T[iDet - 1][iPart][6] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][6])) &&
                    time_T[iDet - 1][iPart][7] > 0. && !(IS_NAN(time_T[iDet - 1][iPart][7])))
                {
                    iLOSTypeTAMEX = true; // all 8 leading and trailing times
                }

                // We will consider only events in which booth MCFD and TAMEX see same number of channels:
                if (iLOSTypeTAMEX && iLOSTypeMCFD)
                    iLOSType[iDet - 1][iPart] = true;

                if (iLOSType[iDet - 1][iPart])
                {
                    int nPMT = 0;
                    int nPMV = 0;

                    for (int ipm = 0; ipm < 8; ipm++)
                    {

                        if (time_T[iDet - 1][iPart][ipm] > 0. && time_L[iDet - 1][iPart][ipm] > 0. &&
                            !(IS_NAN(time_T[iDet - 1][iPart][ipm])) && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                        {
                            while (time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm] <= 0.)
                            {
                                time_T[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] + 2048. * 5./*fClockFreq*/;
                            }

                            nPMT = nPMT + 1;
                            tot[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm];

                            // pileup rejection
                           // if (tot[iDet - 1][iPart][ipm] > fEpileup)
                             //   iLOSPileUp[iDet - 1][iPart] = true;
                        }

                        if (tot[iDet - 1][iPart][ipm] != 0. && !(IS_NAN(tot[iDet - 1][iPart][ipm])))
                            totsum[iDet - 1][iPart] += tot[iDet - 1][iPart][ipm];

                        if (time_L[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_L[iDet - 1][iPart][ipm])))
                            timeLosT[iDet - 1][iPart] += time_L[iDet - 1][iPart][ipm];

                        // Calculate detector time
                        if (time_V[iDet - 1][iPart][ipm] > 0. && !(IS_NAN(time_V[iDet - 1][iPart][ipm])))
                        {
                            timeLosV[iDet - 1][iPart] += time_V[iDet - 1][iPart][ipm];
                            nPMV = nPMV + 1;
                        }
                    }

                    totsum[iDet - 1][iPart] = totsum[iDet - 1][iPart] / nPMT;

                    timeLosV[iDet - 1][iPart] = timeLosV[iDet - 1][iPart] / nPMV;

                    timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] / nPMT;

                    timeLos[iDet - 1][iPart] = timeLosV[iDet - 1][iPart];

                    LosTresV[iDet - 1][iPart] = ((time_V[iDet - 1][iPart][0] + time_V[iDet - 1][iPart][2] +
                                                  time_V[iDet - 1][iPart][4] + time_V[iDet - 1][iPart][6]) -
                                                 (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][3] +
                                                  time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][7])) /
                                                4.;

                    LosTresT[iDet - 1][iPart] = ((time_L[iDet - 1][iPart][0] + time_L[iDet - 1][iPart][2] +
                                                  time_L[iDet - 1][iPart][4] + time_L[iDet - 1][iPart][6]) -
                                                 (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][3] +
                                                  time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][7])) /
                                                4.;

                    // right koord.-system, Z-axis beam direction:
                    // Position from tamex:
            //        xT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][2]) / 2. -
            //                                 (time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][6]) / 2.;
            //        yT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][3] + time_L[iDet - 1][iPart][4]) / 2. -
            //                                 (time_L[iDet - 1][iPart][7] + time_L[iDet - 1][iPart][0]) / 2.;
            //        xT_cm[iDet - 1][iPart] = (xT_cm[iDet - 1][iPart] - flosOffsetXT[iDet - 1]) * flosVeffXT[iDet - 1];
            //        yT_cm[iDet - 1][iPart] = (yT_cm[iDet - 1][iPart] - flosOffsetYT[iDet - 1]) * flosVeffYT[iDet - 1];
//
            //        // Position from VFTX:
            //        xV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][2]) / 2. -
            //                                 (time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][6]) / 2.;
            //        yV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][3] + time_V[iDet - 1][iPart][4]) / 2. -
            //                                 (time_V[iDet - 1][iPart][7] + time_V[iDet - 1][iPart][0]) / 2.;
            //        xV_cm[iDet - 1][iPart] = (xV_cm[iDet - 1][iPart] - flosOffsetXV[iDet - 1]) * flosVeffXV[iDet - 1];
            //        yV_cm[iDet - 1][iPart] = (yV_cm[iDet - 1][iPart] - flosOffsetYV[iDet - 1]) * flosVeffYV[iDet - 1];
//
            //        // Position from ToT:
            //        if (tot[iDet - 1][iPart][1] > 0. && tot[iDet - 1][iPart][2] > 0. && tot[iDet - 1][iPart][5] > 0. &&
            //            tot[iDet - 1][iPart][6] > 0. && tot[iDet - 1][iPart][0] > 0. && tot[iDet - 1][iPart][3] > 0. &&
            //            tot[iDet - 1][iPart][4] > 0. && tot[iDet - 1][iPart][7] > 0.)
            //        {
            //            xToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) / 2. -
            //                                         (tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2]) / 2.) /
            //                                        ((tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2] +
            //                                          tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) /
            //                                         4.));

            //            yToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7]) / 2. -
            //                                         (tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) / 2.) /
            //                                        ((tot[iDet - 1][iPart][7] + tot[iDet - 1][iPart][0] +
            //                                          tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) /
            //                                         4.));
            //        }
//
            //        xToT_cm[iDet - 1][iPart] =
            //            (xToT_cm[iDet - 1][iPart] - flosOffsetXQ[iDet - 1]) * flosVeffXQ[iDet - 1];
            //        yToT_cm[iDet - 1][iPart] =
            //            (yToT_cm[iDet - 1][iPart] - flosOffsetYQ[iDet - 1]) * flosVeffYQ[iDet - 1];
//
            //        if (timeLosV[iDet - 1][iPart] > 0. && timeLosV[iDet - 1][iPart] < 8192. * 5. &&
            //            !(IS_NAN(timeLosV[iDet - 1][iPart])))
            //        {
            //            while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] < 2048. * 5. / 2.)
            //            {
            //                timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] + 2048. * 5.;
            //            }
            //            while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] > 2048. * 5. / 2.)
            //            {
            //                timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] - 2048. * 5.;
            //            }
//
            //       
            //        }
                    //cout<<"los tot "<<totsum[iDet - 1][iPart]<<endl;
                    //if(totsum[iDet - 1][iPart] < 64.4 || totsum[iDet - 1][iPart] > 70.) return;//C16 Setting
                    if(totsum[iDet - 1][iPart] < 68. || totsum[iDet - 1][iPart] > 73.6) return;//C12 Setting
                    
                } // if iLosType
            }     // for iPart
        }         // for iDet

     // if fCallItems

////End LOS
#endif





///B13 13.9°, C16 13.6°, 
 if(!fFRSGATE ||
    (Aq < fAQmax && Aq > fAQmin && Q_FRS > fQmin && Q_FRS < fQmax)
    ) 
 {
//cout<<"FOUND ISOTOPE OF INTEREST"<<endl;

    if(fFi31HitDataCA && fFi32HitDataCA && fFi33HitDataCA)
    {   
        Bool_t CentralHit = false;
        Bool_t LeftHit = false;
        Bool_t RightHit = false;

        if
            (
            (fFi32HitDataCA->GetEntriesFast() > 0 &&
            (fFi31HitDataCA->GetEntriesFast() > 0 ||
            fFi33HitDataCA->GetEntriesFast() > 0))
            )//First x det (Fi32) needs to be hit. Second row (Fi31 or Fi33) needs at least in 1 det a hit.
        {
            Double_t xpos32 = 0./0., xpos31 = 0./0., xpos33 = 0./0., xposback = 0./0., xpos30=0./0.;
            Double_t ypos32 = 0./0., ypos31 = 0./0., ypos33 = 0./0., ypos30=0./0.;
            Double_t tot32 = 0., tot31 = 0., tot33 = 0., tot30 = 0., t32 = 0.;
            Int_t nHits32 = fFi32HitDataCA->GetEntriesFast();
            Int_t Fib32 = 0;
          
           //cout<<"nHits32: "<<nHits32<<endl;
            if(nHits32 >1) return;
            for(Int_t ihit=0; ihit<nHits32; ihit++)
            {
                auto hit = (R3BFiberMAPMTHitData*)fFi32HitDataCA->At(ihit);
                if(!hit)
                    continue;
                
                //cout<<"ihit: "<<ihit<<" ToT: "<<hit->GetEloss()<<" fID: "<<hit->GetFiberId()<<endl;
                if (hit->GetEloss() > tot32)
                {
                    tot32 = hit->GetEloss();
                    xpos32 = hit->GetX() /*cm*/;
                   //cout<<"ihit: "<<<<hit->GetX()<<endl;
                    ypos32= hit->GetY();
                    Fib32 = hit->GetFiberId();
                    t32 = hit->GetTime();
                }
            }

            Int_t nHits31 = fFi31HitDataCA->GetEntriesFast();
            Int_t nHits33 = fFi33HitDataCA->GetEntriesFast();
            Int_t nHits30 = fFi30HitDataCA->GetEntriesFast();

            if( !( (nHits31 == 1 && nHits33 == 0) || (nHits31 == 0 && nHits33 == 1) ) ) return;
            
            //cout<<"before if: n31 "<<nHits31<<" n33 "<<nHits33<<endl;
            bool overlap = false;
            if(nHits31 == 1 && nHits33 == 1)
                overlap = true;
            else if(nHits31 == 1){
                if(nHits33 !=0) return;
            }
            else if(nHits33 == 1){
                if(nHits31 !=0) return;
            }
            else return;
            //cout<<"after if: n31 "<<nHits31<<" n33 "<<nHits33<<endl;
            


            //fh2_ToT_Fib32_vs_Fib30
            
            if(nHits30 == 1){
                for(Int_t ihit=0; ihit<nHits30; ihit++)
                {
                    auto hit = (R3BFiberMAPMTHitData*)fFi30HitDataCA->At(ihit);
                    if(!hit)
                        continue;
                    if (hit->GetEloss() > tot30)
                    {
                        tot30 = hit->GetEloss();
                        xpos30 = hit->GetX() - 25.6 /*cm*/;
                        ypos30= hit->GetY();
                    }
                }
            }

            for(Int_t ihit=0; ihit<nHits31; ihit++)
            {
                auto hit = (R3BFiberMAPMTHitData*)fFi31HitDataCA->At(ihit);
                if(!hit)
                    continue;
                if (hit->GetEloss() > tot31)
                {
                    tot31 = hit->GetEloss();
                    xpos31 = hit->GetX() - 25.6 /*cm*/;
                    ypos31= hit->GetY();
                }
            }

            for(Int_t ihit=0; ihit<nHits33; ihit++)
            {
                auto hit = (R3BFiberMAPMTHitData*)fFi33HitDataCA->At(ihit);
                if(!hit)
                    continue;
                if (hit->GetEloss() > tot33)
                {
                    tot33 = hit->GetEloss();
                    xpos33 = 25. + hit->GetX() /*cm*/;
                    ypos33= hit->GetY();
                }
            }
            if(overlap){
                if(xpos33 > 0.3 || xpos31 < -0.3) return;
            }

            if(tot33 > tot31){
                xposback = xpos33;
                LeftHit = true;
            }
            else if(tot33 <= tot31){
                xposback = xpos31;
                RightHit = true;
            }
            
            if(!IS_NAN(xposback)){
            
                Double_t angX = 0./0.;
                Double_t totback = 0.;
                if(LeftHit){
                    angX = -100.*(xpos32 - xposback) / (160.4) /*cm*/;
                    totback = tot33;
                }
                else if(RightHit){
                    angX = -100.*(xpos32 - xposback) / (177.3) /*cm*/;
                    totback = tot31;
                }




                 Double_t tof = 0./0.;
                        Double_t tofdq[fNbTofdPlanes]; 
                        Double_t tofdtof[fNbTofdPlanes]; 
                        Int_t bar[fNbTofdPlanes];
                if(!IS_NAN(angX) || 1){
                    //     fh2_fib_ang_vs_x->Fill(xpos32,angX);
                    Double_t xpos2[fNbTofdPlanes];
                    Double_t q = 0./0.;
                    Double_t q1= 0./0.;
                    Double_t xmean = 0.;
                    if(ToFDgate && fHitTofdItems && fHitTofdItems->GetEntriesFast()>0){
                        
                        Double_t xpos1 = 0. / 0.;
                        Double_t ypos1 = 0. / 0., ypos2[fNbTofdPlanes];
                        //Double_t tot = 0.; 
                       

                        for (Int_t i = 0; i < fNbTofdPlanes; i++)
                        {
                            xpos2[i] = 0. / 0.;
                            ypos2[i] = 0. / 0.;
                            tofdq[i] = 0.;
                            bar[i] = -1;
                        }
                        
                        Int_t nHits2 = fHitTofdItems->GetEntriesFast();
                        //cout<<"nHitsToFD: "<<nHits2<<endl;
                       // cout<<"_________________"<<endl;
                        for (Int_t ihit = 0; ihit < nHits2; ihit++)
                        {
                            auto hitToFD = (R3BTofdHitData*)fHitTofdItems->At(ihit);
                            if (!hitToFD)
                                continue;
                            // Looking for the maximum
                            Int_t iPlane = hitToFD->GetDetId() - 1;
                            if(iPlane > 1) continue;
                           if(iPlane==0){
                             cout<<"ToFDQ: "<<hitToFD->GetEloss()<<" Ch: "<<hitToFD->GetBarId()<<endl;
                             q1 = hitToFD->GetEloss();
                           }
                            if (hitToFD->GetEloss() > tofdq[iPlane])
                            {
                                tofdq[iPlane] = hitToFD->GetEloss();
                                xpos2[iPlane] = hitToFD->GetX();
                                ypos2[iPlane] = hitToFD->GetY();
                                bar[iPlane] = hitToFD->GetBarId();
                                tofdtof[iPlane] = hitToFD->GetTof();
                            }
                        }
                            tof = tofdtof[0];

                        if(bar[3] == 5 || bar[3] == 8){
                            //cout<<"bar "<<bar[3]<<" xpos "<<xpos2[3]<<" q "<<tofdq[3]<<endl;
                            //tofdq[3] = 6.;
                        }
                        //if(bar[2] == 17) tofdq[2] = 0.;

                        if(xpos2[0] > -40 && xpos2[0] < -38.8){
                            if(tofdq[0]> 5. || tofdq[1]> 5. || tofdq[2]> 5. ||tofdq[3]> 5.) for(int x = 0; x<4; x++) cout<<"x "<<xpos2[x]<<" q "<<tofdq[x]<<" bar "<<bar[x]<<endl;
                            cout<<"q "<<(tofdq[0]+tofdq[1]+tofdq[2]+tofdq[3])/4. <<endl<<"_________"<<endl;
                        }

                        bool badcharge = false;
                        int badcharge_ctr = 0;

                        Double_t tofdqV[4];
                        Int_t barV[4];
                        for(int t2 = 0; t2<4;t2++){
                             tofdqV[t2] = tofdq[t2];
                             barV[t2] = bar[t2];
                        }


                        //loop to observe tof values
                        bool nonzero = false;
                       // for(int j=0; j<4;j++){
                       //    if(!IS_NAN(tofdtof[j])){
                       //     cout<<"tof["<<j<<"] "<<tofdtof[j]<<endl;
                       //    nonzero = true;
                       //    }
                       // }
                       //    if(nonzero) cout<<"t32: "<<t32<<" tlos "<<timeLos[0][0]<<endl<<"___________"<<endl;
                        //if(t32 > 0. ) return;

                        Int_t SetPlane = 2; 
                        //if(q<2. || 1){ 
                        for(int i = 0; i<SetPlane;i++){
                            if(tofdq[i]==0){ 
                                tofdq[i] = 200.;
                                bar[i] = -100;
                                badcharge = true;
                                badcharge_ctr++;
                            }
                            //cout<<"tofdq["<<i<<"] "<<tofdq[i]<<" bar["<<i<<"] "<<bar[i]<<endl;
                        }

                        #if 0
                        int ck=0;
                        for(int j=0;j<SetPlane;j++){
                            if(bar[j] != -100) xmean += xpos2[j];
                            else continue;
                            ck++;
                        }
                        xmean = xmean/(double)ck;
                        #else
                        xmean = xpos2[0];
                        #endif

                    ///    cout<<"______________"<<endl;
                    ///    cout<<"Fib32tot "<<tot32<<endl;
                        //if(badcharge) cout<<"badcharge "<<badcharge_ctr<<endl;

                        //check if bars align, or if there are stragglers
                        bool straggler = false;
                        for(int i=0;i<SetPlane;i++){
                            if(abs(bar[i] - bar[i+1] > 2)){ //for now, accept +-2 bars across planes
                                straggler = true;
                            }
                        }

                        int bararray[4] = {0,0,0,0};
                        int badbar_ctr = 0;
                        if(straggler){
                   ///         cout<<"straggler"<<endl;
                            
                    ///        for(int i = 0; i<4;i++) cout<<"tofdq["<<i<<"] "<<tofdq[i]<<" bar["<<i<<"] "<<bar[i]<<endl;
                            
                            for(int k=0;k<SetPlane;k++){
                                    if(bar[k] == -100){
                                        badbar_ctr++;
                                        continue;
                                    }
                                for(int j=k+1;j<SetPlane;j++){
                                    if(abs(bar[k] - bar[j]<3)){
                                        bararray[k]++;
                                        bararray[j]++;
                                    }
                                }
                            }
                        }
                        
                    

                        #if 0
                          if(straggler){  
                            for(int g=0; g<4;g++){
                                cout<<"bararray["<<g<<"] "<<bararray[g]<<endl;
                                
                            
                            }
                          }
                        #endif

                        cout<<"tof "<<tofdtof[0]<<endl;
                        for(int a=0; a<SetPlane; a++){
                            if(badbar_ctr == SetPlane-1){
                                if(bar[a] == -100) tofdq[a] = 200.;
                            }
                            else if(straggler && bararray[a] == 0){
                                 tofdq[a] = 200.;
                                 badcharge_ctr++;
                            }
                        }

                        //}
                        if(!IS_NAN(tofdq[3]) && 0) q = tofdq[3];
                        else{
                            int b, k, tmpb;
                            double tmp, tmpt;
                            for (k=1; k < 4; k++){
                                //if(IS_NAN((double)tofdq[k])) tofdq[k] = 1e+4;
                                if(tofdq[k]==0) tofdq[k] = 1e+4;
                                for(b = 0; b < (SetPlane-k); b++){
                                    if(tofdq[b] > tofdq[b+1]) {
                                        tmp=tofdq[b+1];
                                        tofdq[b+1]=tofdq[b];
                                        tofdq[b]=tmp;
                                        tmpb = bar[b+1];
                                        bar[b+1] = bar[b];
                                        bar[b] = tmpb;
                                        tmpt = tofdtof[b+1];
                                        tofdtof[b+1] = tofdtof[b];
                                        tofdtof[b] = tmpt;
                                    }
                                }
                            }
                        }
                        ///    for(int i = 0; i<4;i++) cout<<"tofdq["<<i<<"] "<<tofdq[i]<<" bar["<<i<<"] "<<bar[i]<<endl;

                        
                            int ctr = 0;
                            int qctr = 0;
                            int tctr = 0;
                            //do{
                                q = tofdq[0];
                            //    ctr++;
                            //    if(ctr == 4) break;
                            //}while(q>0.04 && !IS_NAN(q));

                            //for(int i=0; i<4;i++) tofdq[i] = (double)tofdq[i];
                        ///    cout<<"badcharge_ctr "<<badcharge_ctr<<endl;
                        ///    cout<<"badbar_ctr "<<badbar_ctr<<endl;
                            while(ctr<3-badbar_ctr){ 
                                //if(tofdq[ctr] == 0){
                                //    ctr++;
                                //    continue;
                                //}
                                if((abs((tofdq[ctr] - tofdq[ctr+1])/tofdq[ctr]) <0.5) || (1 && tofdq[ctr+1]!=200)){ 
                                    q += tofdq[ctr+1];
                                    if(tofdtof[ctr+1] != 0){
                                        tofdtof[0] += tofdtof[ctr+1];
                                        tctr++;
                                    }
                                    ctr++;
                                    qctr++;
                                }
                                else break;  
                            }
                            //cout<<"qctr "<<qctr<<endl;
                        //    q = 2.*q/((double)qctr + 1.);
                        //    tofdtof[0] = 2. * tofdtof[0] / ((double)tctr + 1.);
                           // cout<<"mean tof "<<tofdtof[0]<<endl;

                        //if(xmean < -21.6 || xmean > -12.5) 
                                                

                       q = q1;
                       cout<<"Mean ToFD charge: "<<q<<endl;
                        if(badcharge_ctr>3) q=0.;
                        //if(q>10.){ //exit(0);
                        //    cout<<"q = "<<q<<endl;
                        //}
                       
                        if(q>5.93 && q<6.18 && /*tot32 < 14. &&*/ nHits32 == 1){
                                fh_ToT_Fib->Fill(Fib32,tot32);
                                //cout<<"Fib mult "<<nHits32<<endl;
                        }

                        //if(q>0.) 
                         //   fh_ToFD_Charge->Fill(q);

                    
                        if(q>4.46 && q<4.94){
                            int gh = 0;
                            for(int l= 0; l<4; l++){
                                for(int p = l+1; p<4; p++){
                                    fh_tofd_charge_planes[gh]->Fill(barV[l], tofdqV[l] - tofdqV[p]);
                                    gh++;
                                }
                            }
                        }
                    
                    
                    }              
                    
                    if(q>4.){
                        if(nHits30 > 0 && tot30 > 14.) fh_mult_fib30->Fill(nHits30);
                        if(nHits31 > 0 && tot31 > 14.) fh_mult_fib31->Fill(nHits31);
                        if(nHits32 > 0 && tot32 > 14.) fh_mult_fib32->Fill(nHits32);
                        if(nHits33 > 0 && tot33 > 14.) fh_mult_fib33->Fill(nHits33);
                        if(nHits30 > 0 && tot30 > 14. &&
                           nHits32 > 0 && tot32 > 14.  &&
                           ((nHits31 > 0 && tot31 > 14.) ||
                            (nHits33 > 0 && tot33 > 14.))) fh_mult_fibAll->Fill(1);
                    }


                    double xdiff = 0./0.;
                    double yy = 0./0.;
                    if(RightHit){ 
                        yy = 611.4;
                        xdiff = 527.1;
                        LeftHit = false;
                    }
                    if(LeftHit){
                         yy = 594.5;
                         xdiff = 544.;
                         RightHit = false;
                    }

                    Double_t angFibToFD = ((100*((xposback - xmean) / xdiff)) + (100*((xpos32 - xmean) / 704.4))) / 2.;
                    Double_t angMean = (-angX + angFibToFD)/2.;

                    
                    double xsum = 0., ysum = 0., x2sum = 0., xysum = 0.,y2sum=0.,r=0.;
                    double xpoints[3] = {434.1, yy, 1138.5};
                    double ypoints[3] = {xpos32, xposback, xmean};
                    //double ypoints = 0.;
                    
                    for (int i=0;i<3;i++)
                    {   
                        //if(0==i) ypoints = xpos32;
                        //if(1==i) ypoints = xposback;
                        //if(2==i) ypoints = xmean;

                        xsum=xsum+xpoints[i];                        //calculate sigma(xi)
                        ysum=ysum+ypoints[i];                        //calculate sigma(yi)
                        x2sum=x2sum+pow(xpoints[i],2);                //calculate sigma(x^2i)
                        y2sum=y2sum+pow(ypoints[i],2);
                        xysum=xysum+xpoints[i]*ypoints[i];                    //calculate sigma(xi*yi)
                    }
                    double slope =(2.*xysum-xsum*ysum)/(2.*x2sum-xsum*xsum);            //calculate slope
                    double intercept =(x2sum*ysum-xsum*xysum)/(x2sum*2.-xsum*xsum);            //calculate intercept
                    
                    //cout<<"linfit: slope = "<<slope<<" intercept = "<<intercept<<endl;
                    double x_linreg[3] = {434.1, 602.95, 1138.5};
                    double y_linreg[3] = {xpos32, xposback, xmean};

                    if (r==0.) {
                        r = (xysum - xsum * ysum / 3.) /    /* compute correlation coeff */
                        sqrt((x2sum - pow(xsum,2.0)/3.) *
                        (y2sum - pow(ysum,2.0)/3.));
                    }
                    
                    
                    //double m,b,r;
//
                    //linreg(3,x_linreg,y_linreg,&m,&b,&r);

                    //cout<<"linreg: m = "<<m<<" b = "<<b<<" r = "<<r<<endl;

                    fh_ToFD_Charge->Fill(q);
                    fh_ToF_vs_charge->Fill(q,tof);

                    double qmin = 3.54/*4.6*//*5.93*/;
                    double qmax = 6.4;

                    if(tot32 < 14. || tot32 > 24.6) return;
                    //if(RightHit && (tot31 < 14. || tot31 > 24.6)) return; 
                    //if(LeftHit && (tot33 < 14. || tot33 > 24.6)) return;
                    if(totback < 14. || totback > 24.6) return;


                           TVector3 v1(xposback, angMean -0.0904221,0.);
                           //TVector3 v2(intercept, 100*slope + 1.40554,0.);
                           //TVector3 v2(xposback, -100*atan(slope) + 1.40554,0.);
                           TVector3 v2(xposback, -100*atan(slope) + 0.057777928,0.);
                           //double rotAngle2 = -0.079104685; //radiants
                           double rotAngle2 = atan(-0.15555556);
                           double rotAngle1 = -0.18909835 + 0.050518777; //radiants
                          // double rotAngle = -10.834537; //degree
                           v1.RotateZ(-rotAngle1);
                           v2.RotateZ(-rotAngle2);

                    fh2_fib_ang_vs_x_nocuts->Fill(xposback,angMean);
                           fh_pid->Fill(v1(0),q);

                    if(q<qmin || q>qmax) return;
                    
                    fh_Rsqr->Fill(r*r);
                    
                    if(r*r < 0.8){
                        //cout<<"low certainty"<<endl;
                        //cout<<"x32 "<<xpos32<<" "<<tot32<<" xb "<<xposback<<" "<<totback<<" xm "<<xmean<<" "<<q<<" r² "<<r*r<<endl; 
                        // for(int h=0; h<4; h++) cout<<"q["<<h<<"] "<<tofdq[h]<<" bar["<<h<<"] "<<bar[h]<<endl;
                //         return;  
                    }
                    
                    if(r*r > 0.993){
                     //cout<<"x32 "<<xpos32<<" "<<tot32<<" xb "<<xposback<<" "<<totback<<" xm "<<xmean<<" "<<q<<" r² "<<r*r<<endl; 
                     //    for(int h=0; h<4; h++) cout<<"q["<<h<<"] "<<tofdq[h]<<" bar["<<h<<"] "<<bar[h]<<endl;  
                         //return;
                    }

                    #if 1
                    Int_t tpatbin;
                        for (int i = 0; i < 16; i++)
                        {
                            tpatbin = (header->GetTpat() & (1 << i));
                            if (tpatbin != 0 && (i < 16 || i > -1))
                            {   
                                //if(i+1 == 3 || i+1 == 4 || i+1 == 5 || i+1 == 9 || i+1 == 10 || i+1 == 11) return;
                                if(i+1 == 1 || i+1 == 2 || i+1 == 5 || i+1 == 6 || i+1 == 7 || i+1 == 8 || i+1 == 11 || i+1==12) return;
                                fh_pos_TPat->Fill(xmean, i+1);
                            }
                        }
                    #endif

                    fh_x_vs_tof->Fill(xposback,tofdtof[0]);
                    fh_x_vs_q->Fill(xposback,q);

                    fh_X_fib_vs_tofd_no_cut->Fill(xmean, intercept+slope*1138.5);
                    fh_X_fib_vs_tofd_ang_no_cut->Fill(xmean, xposback - (angX) * 602.95 /100. + (angX) * 1138.5 /100.);
                    
                    /*x1=-3.3 y1=-2.8 | x2=5.1 y2=5.8
                    s = (5.8 + 2.8)/(5.1 + 3.3) = 1.0238095
                    b= 0.57857155
                    x1=-34, y1=-2 | x2=19.40 ,y2=0.7 
                    s=0.050561798
                    b= -0.28089888
                    x1=-33.8 , y1=5.2 | x2=29.2 ,y2=-4.6 
                    s = -0.15555556
                    b = -0.057777928
                    */
                    
                    //For better PID: Rotate angle vs pos plot via TVector3, project that on "new" x-axis, then plot against Charge ID
                    //ToDo: Get rotation angle more dynamically. Now precalculated fixed angle.
//                           TVector3 v1(xposback, angMean -0.0904221,0.);
//                           //TVector3 v2(intercept, 100*slope + 1.40554,0.);
//                           //TVector3 v2(xposback, -100*atan(slope) + 1.40554,0.);
//                           TVector3 v2(xposback, -100*atan(slope) + 0.057777928,0.);
//                           //double rotAngle2 = -0.079104685; //radiants
//                           double rotAngle2 = atan(-0.15555556);
//                           double rotAngle1 = -0.18909835 + 0.050518777; //radiants
//                          // double rotAngle = -10.834537; //degree
//                           v1.RotateZ(-rotAngle1);
//                           v2.RotateZ(-rotAngle2);
            if(q>qmin && q<qmax/*q>5.6 && q<6.5*/){
                fh2_ang_Fib_vs_ToFD_nocuts->Fill(-angX,angFibToFD); //Fill before applying any cuts
                fh2_fib_slope_vs_x_nocuts->Fill(xposback, -1000.*(atan(slope)/*+0.2443461014° in rad*/));
               // fh2_fib_ang_vs_x_nocuts->Fill(xposback,angMean);
            }
                for (int i=0; i<100;i++){
                    Double_t zrand = gRandom->Uniform(0., 1230.5/*cm*/);//Umlenkpunkt at 0. Fib32 at 434.1 cm, ToFD at 1138.5cm
                    //fh2_fibtracking_planeXZ->Fill(zrand, xpos32 - angX * 434.1 /1000. + angX * zrand /1000.);
                   //if(q>5.93 && q<6.18/*q>5.6 && q<6.5*/) fh2_fibtracking_planeXZ_nocuts->Fill(zrand, intercept + slope * zrand );
                }
               

                if((q>4.6 && q<6.5)|| !ToFDgate){
                fh2_fib_ang_vs_x->Fill(xposback,-angX);
                fh2_fib_slope_vs_x->Fill(xposback, -1000.*(atan(slope)/*+0.2443461014° in rad*/));
                if(q>=3.8 && q < 4.7){ 
                    fh2_fib_ang_vs_x4->Fill(xposback,angMean);
                    fh2_fib_slope_vs_x4->Fill(xposback, -1000.*(atan(slope)/*+0.2443461014° in rad*/));
                }
                if(q>=4.6 && q < 5.6){
                    fh2_fib_ang_vs_x5->Fill(xposback,angMean);
                    fh2_fib_slope_vs_x5->Fill(xposback, -1000.*(atan(slope)/*+0.2443461014° in rad*/));
                }
                if(q>5.93 && q<6.18){
                    fh2_fib_ang_vs_x6->Fill(xposback,angMean);
                    fh2_fib_slope_vs_x6->Fill(xposback, -1000.*(atan(slope)/*+0.2443461014° in rad*/));
                }
                }

                //if(1 != check_fit(slope,intercept,434.1, yy, 1138.5,RightHit)) return;
                    

                    //if(r*r < 0.9) return;
                
                if(!IS_NAN(ypos30) /*&& q>5.93 && q<6.18*/){
                    fh_f32X_vs_f30Y->Fill(xpos32,ypos30);
                    fh_f32Y_vs_f30Y->Fill(ypos32,ypos30);
                    fh_f32tot_vs_f30Y->Fill(tot32,ypos30);
                    fh_tofdq_vs_f30Y->Fill(q,ypos30);
                }
                

                if(!(q>qmin && q<qmax/*q>5.6 && q<6.5*/)/*r*r < 0.9*/) return;
                fh_X_fib_vs_tofd->Fill(xmean, intercept+slope*1138.5);
                fh_X_fib_vs_tofd_ang->Fill(xmean, xposback - (angX) * 602.95 /100. + (angX) * 1138.5 /100.);

                if((v2(1) > -0.2 && v2(1) < 0.2) || 1){    

                    //double ang_check = (angX * 1.0238095 + 0.57857155 - angFibToFD)/angFibToFD;
                    double ang_check = angX * 1.0238095 + 0.57857155 - angFibToFD;
                    if( ( ((tot32 > 12. && totback > 12.) || 1) && !IS_NAN(slope) /*&& ang_check >= -1. && ang_check <= 1. */&& intercept > -4. && intercept <3.) || 1){
                        fh2_ang_Fib_vs_ToFD->Fill(-angX,angFibToFD);
                        for (int i=0; i<100;i++){
                            Double_t zrand = gRandom->Uniform(0., 1230.5/*cm*/);//Umlenkpunkt at 0. Fib32 at 434.1 cm, ToFD at 1138.5cm
                            //fh2_fibtracking_planeXZ->Fill(zrand, xpos32 - angX * 434.1 /1000. + angX * zrand /1000.);
                            //fh2_fibtracking_planeXZ->Fill(zrand, b + m * zrand);   
                            fh2_fibtracking_planeXZ_nocuts->Fill(zrand, xposback - (angX) * 602.95 /100. + (angX) * zrand /100.);
                            fh2_fibtracking_planeXZ->Fill(zrand, intercept + slope * zrand );
                        }

                    }

                    if( ( ((tot32 > 12. && totback > 12.) || 1) && intercept > -4. && intercept <3.
                        && !IS_NAN(slope) /*&& ang_check >= -1. && ang_check <= 1. */) || 1){
                         fh2_FibToT_vs_QToFD->Fill(tot32,q);

                        fh2_ToT_Fib32_vs_Fib30->Fill(tot32,tot30);
                        fh2_ToT_Fib32_vs_Fib31->Fill(tot32,tot31);
                        fh2_ToT_Fib32_vs_Fib33->Fill(tot32,tot33);


                      //  cout<<"Q going into cut: "<<q<<endl;

                           // cout<<"Q cut: "<<q<<endl;
                           // cout<<"ToT32: "<<tot32<<" ToTback: "<<totback<<endl;

                           fh_ang_vs_pos_rot->Fill(v1(0),v1(1));
                           fh_slope_vs_pos_rot->Fill(v2(0),v2(1));
                           fh_pid->Fill(v1(0),q);
                           fh_pid_slope->Fill(v2(0),q);
                           fh_pid_3d->Fill(xposback,angMean,q);
                           //ntuple->Fill(xposback,angMean,q);


                        
                        }
                    
                    
                    
                    
                    }
                }
            }
        
        

        }

    }

}//End FRS GATE

    //fNEvents += 1;//old
}

void R3BFiberTrackingOnlineSpectra::FinishEvent()
{
    if (fFi30HitDataCA)
    {
        fFi30HitDataCA->Clear();
    }

    if (fFi31HitDataCA)
    {
        fFi31HitDataCA->Clear();
    }

    if (fFi32HitDataCA)
    {
        fFi32HitDataCA->Clear();
    }

    if (fFi33HitDataCA)
    {
        fFi33HitDataCA->Clear();
    }

    if(fHitFrs)
    {
        fHitFrs->Clear();
    }
}

void R3BFiberTrackingOnlineSpectra::FinishTask()
{
    if (fFi32HitDataCA && (fFi33HitDataCA || fFi31HitDataCA))
    {
        cTrackingXZ->Write();
        cAngVsX->Write();
        cslopeVsX->Write();
        cToFD_Z->Write();
        AngCorrFibToFD->Write();
        cAngRot->Write();
        cFibToT_Q->Write();
        cSlopeRot->Write();
        
        fh2_fibtracking_planeXZ->Write();
        fh2_fib_ang_vs_x->Write();
        fh2_fib_ang_vs_x4->Write();
        fh2_fib_ang_vs_x5->Write();
        fh2_fib_ang_vs_x6->Write();
        fh2_fib_slope_vs_x->Write();
        fh_pid->Write();
        fh_pid_slope->Write();
        fh2_fib_slope_vs_x->Write();
        fh2_fib_slope_vs_x4->Write();
        fh2_fib_slope_vs_x5->Write();
        fh2_fib_slope_vs_x6->Write();
        fh2_FibToT_vs_QToFD->Write();
        fh_ang_vs_pos_rot->Write();
        fh_slope_vs_pos_rot->Write();
        fh2_ang_Fib_vs_ToFD->Write();
        fh_ToFD_Charge->Write();
        fh_pid_3d->Write();

    }
}

ClassImp(R3BFiberTrackingOnlineSpectra);
