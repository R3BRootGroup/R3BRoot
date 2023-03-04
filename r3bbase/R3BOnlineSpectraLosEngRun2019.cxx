// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraLosEngRun2019.h"
#include "R3BLosCalData.h"
#include "R3BLosMappedData.h"

#include "R3BRoluCalData.h"
#include "R3BRoluMappedData.h"

#include "R3BSci8CalData.h"
#include "R3BSci8MappedData.h"

#include "R3BBeamMonitorMappedData.h"

#include "R3BTofdCalData.h"
#include "R3BTofdMappedData.h"

#include "R3BPaddleCalData.h"
#include "R3BPaddleTamexMappedData.h"

#include "R3BPspxCalData.h"
#include "R3BPspxHitData.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxPrecalData.h"

#include "../sofia/sofdata/twimData/R3BSofTwimCalData.h"
#include "../sofia/sofdata/twimData/R3BSofTwimHitData.h"
#include "../sofia/sofdata/twimData/R3BSofTwimMappedData.h"

#include "R3BEventHeader.h"
#include "R3BSamplerMappedData.h"
#include "R3BTCalEngine.h"

#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberMappedData.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THttpServer.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <algorithm>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

R3BOnlineSpectraLosEngRun2019::R3BOnlineSpectraLosEngRun2019()
    : FairTask("OnlineSpectraLosEngRun2019", 1)
    , fMappedItemsTwim(NULL)
    , fCalItemsTwim(NULL)
    , fHitItemsTwim(NULL)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraLosEngRun2019::R3BOnlineSpectraLosEngRun2019(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItemsTwim(NULL)
    , fCalItemsTwim(NULL)
    , fHitItemsTwim(NULL)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraLosEngRun2019::~R3BOnlineSpectraLosEngRun2019()
{
    //	delete fhTpat ;
    //	delete fhTrigger;
    //	delete fh_SEETRAM;
    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        delete fh_channels_Fib[i];
        delete fh_fibers_Fib[i];
        delete fh_mult_Fib[i];
        delete fh_time_Fib[i];
        delete fh_multihit_m_Fib[i];
        delete fh_multihit_s_Fib[i];
        delete fh_ToT_m_Fib[i];
        delete fh_ToT_s_Fib[i];
        delete fh_ToT_single_Fib[i];
        delete fh_channels_single_Fib[i];
    }
    /*
      LOG(info) << "R3BSofTwimOnlineSpectra::Delete instance";
       if (fMappedItemsTwim)
           delete fMappedItemsTwim;
       if (fCalItemsTwim)
           delete fCalItemsTwim;
       if (fHitItemsTwim)
           delete fHitItemsTwim;
    */
}

InitStatus R3BOnlineSpectraLosEngRun2019::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BOnlineSpectraLosEngRun2019::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    //   FairRunOnline* run = FairRunOnline::Instance();

    //   run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels
    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    printf("Have %d fiber detectors.\n", NOF_FIB_DET);
    for (int det = 0; det < DET_MAX; det++)
    {
        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));

        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }
        fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCal", fDetectorNames[det])));
        if (NULL == fCalItems.at(det))
        {
            printf("Could not find Cal data for '%s'.\n", fDetectorNames[det]);
        }
        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
        if (NULL == fHitItems.at(det))
        {
            printf("Could not find hit data for '%s'.\n", fDetectorNames[det]);
        }
    }

    // get access to mapped data of the TWIM
    fMappedItemsTwim = (TClonesArray*)mgr->GetObject("TwimMappedData");
    if (!fMappedItemsTwim)
    {
        LOG(warning) << "R3BOnlineSpectra: TwimMappedData not found"; // return kFATAL;
    }

    // get access to cal data of the TWIM
    fCalItemsTwim = (TClonesArray*)mgr->GetObject("TwimCalData");
    if (!fCalItemsTwim)
        LOG(warning) << "R3BOnlineSpectra: TwimCalData not found";

    // get access to hit data of the TWIM
    fHitItemsTwim = (TClonesArray*)mgr->GetObject("TwimHitData");
    if (!fHitItemsTwim)
        LOG(warning) << "R3BOnlineSpectra: TwimHitData not found";

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    // Trigger and Tpat
    fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fhTpat->GetXaxis()->SetTitle("Tpat value");

    fhTrigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fhTrigger->GetXaxis()->SetTitle("Trigger value");
    /*
        TCanvas* cTrigg = new TCanvas("Trigg", "Triggers", 10, 10, 650, 350);
        cTrigg->Divide(2, 1);
        cTrigg->cd(1);
        gPad->SetLogy();
        fhTrigger->Draw();
        cTrigg->cd(2);
        gPad->SetLogy();
        fhTpat->Draw();
        cTrigg->cd(0);
    */

    //------------------------------------------------------------------------
    // Sci8 detector
    if (fMappedItems.at(DET_SCI8))
    {
        TCanvas* cSci8 = new TCanvas("Sci8", "SCI8", 10, 10, 850, 850);

        fh_sci8_channels = new TH1F("sci8_channels", "SCI8 channels", 4, 0., 4.);
        fh_sci8_channels->GetXaxis()->SetTitle("Channel number");
        fh_sci8_multihit = new TH1F("sci8_multihit", "SCI8 multihit", 20, 0., 20.);
        fh_sci8_multihit->GetXaxis()->SetTitle("Multihit");
        fh_sci8_multihitVFTX = new TH2F("sci8_vftx_multihit", "SCI8 VFTX multihit", 4, 0., 4., 20, 0., 20.);
        fh_sci8_multihitVFTX->GetYaxis()->SetTitle("Multihit ");
        fh_sci8_multihitVFTX->GetXaxis()->SetTitle("Channel number");
        fh_sci8_multihitLEAD = new TH2F("sci8_lead_multihit", "SCI8 TAMEX Leading multihit", 4, 0., 4., 20, 0., 20.);
        fh_sci8_multihitLEAD->GetYaxis()->SetTitle("Multihit ");
        fh_sci8_multihitLEAD->GetXaxis()->SetTitle("Channel number");
        fh_sci8_multihitTRAI = new TH2F("sci8_trai_multihit", "SCI8 TAMEX Trailing multihit", 4, 0., 4., 20, 0., 20.);
        fh_sci8_multihitTRAI->GetYaxis()->SetTitle("Multihit ");
        fh_sci8_multihitTRAI->GetXaxis()->SetTitle("Channel number");
        fh_sci8_dt_hits = new TH1F("sci8_dt_hits", "SCI8 dt multihits", 40000, -2000, 2000);
        fh_sci8_dt_hits->GetXaxis()->SetTitle("dt_VFTX between two hits / ns");
        fh_sci8_dt_hits_l = new TH1F("sci8_dt_hits_l", "SCI8 dt multihits leading", 40000, -2000, 2000);
        fh_sci8_dt_hits_l->GetXaxis()->SetTitle("dt_LEAD between two hits / ns");
        fh_sci8_dt_hits_t = new TH1F("sci8_dt_hits_t", "SCI8 dt multihits trailing", 40000, -2000, 2000);
        fh_sci8_dt_hits_t->GetXaxis()->SetTitle("dt_TRAIL between two hits / ns");
        fh_sci8_tres_MCFD = new TH1F("sci8_time_res_MCFD", "SCI8 MCFD Time resolution - raw", 10000, -10., 10.);
        fh_sci8_tres_MCFD->GetXaxis()->SetTitle("Time MCFD / ns");
        fh_sci8_tres_TAMEX = new TH1F("sci8_time_res_TAMEX", "SCI8 TAMEX Time resolution -raw ", 8000, -4., 4.);
        fh_sci8_tres_TAMEX->GetXaxis()->SetTitle("Time TAMEX / ns");
        fh_sci8_tot = new TH2F("sci8_tot", "SCI8 ToT vs PM", 4, 0, 4, 3000, 0., 300.);
        fh_sci8_tot->GetXaxis()->SetTitle("PMT number");
        fh_sci8_tot->GetYaxis()->SetTitle("ToT / ns");
        fh_sci8_tot_mean = new TH1F("sci8_tot_mean", "SCI8 mean ToT", 30000, 0., 300.);
        fh_sci8_tot_mean->GetYaxis()->SetTitle("Counts");
        fh_sci8_tot_mean->GetXaxis()->SetTitle("ToT / ns");
        fh_tof_sci8 = new TH1F("sci8_tof", "SCI8-LOS ToF", 40000, -200, 200);
        fh_tof_sci8->GetXaxis()->SetTitle("ToF / ns");

        cSci8->Divide(2, 2);
        cSci8->cd(1);
        fh_sci8_channels->Draw();
        cSci8->cd(2);
        gPad->SetLogy();
        fh_sci8_multihit->Draw();
        // cSci8->cd(3);gPad->SetLogz();
        // fh_sci8_tot->Draw("colz");
        cSci8->cd(3);
        fh_sci8_tres_MCFD->Draw();
        // cSci8->cd(5);
        // fh_sci8_tres_TAMEX->Draw();
        cSci8->cd(4);
        fh_tof_sci8->Draw();
        // cSci8->cd(7);
        // fh_sci8_tot_mean->Draw();

        cSci8->cd(0);
        //  run->AddObject(cSci8);

        //       run->GetHttpServer()->RegisterCommand("Reset_SCI8", Form("/Tasks/%s/->Reset_SCI8_Histo()", GetName()));
    }

    //------------------------------------------------------------------------
    // Los detector
    cout << "No of LOS DETECTORS = " << fNofLosDetectors << endl;
    if (fMappedItems.at(DET_LOS))
    {
        TCanvas* cLos[fNofLosDetectors];

        if (fNofLosDetectors == 2)
        {

            fh_los_ToT_vs_ToT = new TH2F("los_tot_vs_tot", "LOS ToT vs ToT", 3000, 0., 300., 3000, 0., 300.);
            fh_los_ToT_vs_ToT->GetXaxis()->SetTitle("ToT LOS1 / ns");
            fh_los_ToT_vs_ToT->GetYaxis()->SetTitle("ToT LOS2 / ns");
            fh_los_nPart_vs_nPart = new TH2F("los_nPart_vs_nPart", "LOS nPart vs nPart", 20, 0., 20., 20, 0., 20.);
            fh_los_nPart_vs_nPart->GetXaxis()->SetTitle("nPart LOS1");
            fh_los_nPart_vs_nPart->GetYaxis()->SetTitle("nPart LOS2");
            fh_dt_los1_los2 = new TH1F("los_dt_los1_los2", "LOS dt los1 los2", 20000, -10, 10);
            fh_dt_los1_los2->GetXaxis()->SetTitle("dt / ns");
            fh_dt_los1_los2->GetYaxis()->SetTitle("Counts");

            /*
                        for (Int_t i = 0; i < 8; i++)
                        {
                            char strName1[255];
                            sprintf(strName1, "test_PM%d", i + 1);
                            char strName2[255];
                            sprintf(strName2, "test PM%d", i + 1);
                            fh_test[i] = new TH2F(strName1, strName2, 1000, 0, 10, 400, 1, 400.);
                            char strName3[255];
                            sprintf(strName3, "test_corr_PM%d", i + 1);
                            char strName4[255];
                            sprintf(strName4, "test_corr PM%d", i + 1);
                            fh_test_corr[i] = new TH2F(strName3, strName4, 1000, 0, 10, 400, 1, 400.);
                        }
            */

            TCanvas* cLosCor = new TCanvas("Los_correl", "Los correl", 10, 10, 910, 810);
            cLosCor->Divide(4, 5);
            cLosCor->cd(1);
            gPad->SetLogz();
            fh_los_ToT_vs_ToT->Draw("colz");
            cLosCor->cd(2);
            gPad->SetLogz();
            fh_los_nPart_vs_nPart->Draw("colz");
            cLosCor->cd(3);
            gPad->SetLogy();
            fh_dt_los1_los2->Draw();
            cLosCor->cd(4);
            gPad->SetLogz();

            /*
                        fh_test[0]->Draw("colz");
                        cLosCor->cd(5);
                        gPad->SetLogz();
                        fh_test[1]->Draw("colz");
                        cLosCor->cd(6);
                        gPad->SetLogz();
                        fh_test[2]->Draw("colz");
                        cLosCor->cd(7);
                        gPad->SetLogz();
                        fh_test[3]->Draw("colz");
                        cLosCor->cd(8);
                        gPad->SetLogz();
                        fh_test[4]->Draw("colz");
                        cLosCor->cd(9);
                        gPad->SetLogz();
                        fh_test[5]->Draw("colz");
                        cLosCor->cd(10);
                        gPad->SetLogz();
                        fh_test[6]->Draw("colz");
                        cLosCor->cd(11);
                        gPad->SetLogz();
                        fh_test[7]->Draw("colz");
                        cLosCor->cd(13);
                        gPad->SetLogz();
                        fh_test_corr[0]->Draw("colz");
                        cLosCor->cd(14);
                        gPad->SetLogz();
                        fh_test_corr[1]->Draw("colz");
                        cLosCor->cd(15);
                        gPad->SetLogz();
                        fh_test_corr[2]->Draw("colz");
                        cLosCor->cd(16);
                        gPad->SetLogz();
                        fh_test_corr[3]->Draw("colz");
                        cLosCor->cd(17);
                        gPad->SetLogz();
                        fh_test_corr[4]->Draw("colz");
                        cLosCor->cd(18);
                        gPad->SetLogz();
                        fh_test_corr[5]->Draw("colz");
                        cLosCor->cd(19);
                        gPad->SetLogz();
                        fh_test_corr[6]->Draw("colz");
                        cLosCor->cd(20);
                        gPad->SetLogz();
                        fh_test_corr[7]->Draw("colz");
            */
        }

        //	fh_los_tres_MTDC = new TH1F("los_time_res_MTDC", "LOS MTDC Time resolution - raw", 4000, -4, 4);
        //	fh_los_tres_MTDC->GetXaxis()->SetTitle("Time MTDC / ns");

        for (Int_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        {
            char detName[255];
            sprintf(detName, "LOS%d", iloscount + 1);

            cLos[iloscount] = new TCanvas(detName, detName, 10, 10, 1010, 810);

            fh_los_twin[iloscount] = new TH2F(
                Form("%s_ToT_TWIN", detName), Form("%s ToT vs dE Twin ", detName), 800, 0, 400., 10000, 0., 5000.);
            fh_los_twin[iloscount]->GetXaxis()->SetTitle("LOS ToT / ns");
            fh_los_twin[iloscount]->GetYaxis()->SetTitle("dE TWIN / channel");

            fh_los_channels[iloscount] =
                new TH1F(Form("%s_channels", detName), Form("%s channels", detName), 10, 0., 10.);
            fh_los_channels[iloscount]->GetXaxis()->SetTitle("Channel number");

            fh_los_multihit[iloscount] =
                new TH1F(Form("%s_multihit", detName), Form("%s multihit && all 8 PMs", detName), 30, 0., 30.);
            fh_los_multihit[iloscount]->GetXaxis()->SetTitle("Multihit");

            fh_los_pos_MCFD[iloscount] =
                new TH2F(Form("%s_pos_MCFD", detName), Form("%s MCFD Position ", detName), 500, -5., 5., 500, -5., 5.);
            fh_los_pos_MCFD[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_MCFD[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_MCFD_cond[iloscount] = new TH2F(
                Form("%s_pos_MCFD_cond", detName), Form("%s MCFD Position_cond ", detName), 500, -5., 5., 500, -5., 5.);
            fh_los_pos_MCFD_cond[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_MCFD_cond[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_TAMEX[iloscount] = new TH2F(
                Form("%s_pos_TAMEX", detName), Form("%s ln(ToT) Position cond ", detName), 500, -5., 5., 500, -5., 5.);
            fh_los_pos_TAMEX[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_TAMEX[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_ToT[iloscount] = new TH2F(
                Form("%s_pos_ToT", detName), Form("%s ToT Position ", detName), 1000, -10., 10., 1000, -10., 10.);
            fh_los_pos_ToT[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_ToT[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_los_pos_ToT_cond[iloscount] = new TH2F(Form("%s_pos_ToT_cond", detName),
                                                      Form("%s ToT Position cond", detName),
                                                      1000,
                                                      -10.,
                                                      10.,
                                                      1000,
                                                      -10.,
                                                      10.);
            fh_los_pos_ToT_cond[iloscount]->GetXaxis()->SetTitle("X position / cm");
            fh_los_pos_ToT_cond[iloscount]->GetYaxis()->SetTitle("Y position / cm");

            fh_xlos_xfib[iloscount] =
                new TH2F(Form("%s_xlos_xfib", detName), Form("%s xLOS vs xFIB ", detName), 500, -5., 5., 500, -5., 5.);
            fh_xlos_xfib[iloscount]->GetXaxis()->SetTitle("X FIB / cm");
            fh_xlos_xfib[iloscount]->GetYaxis()->SetTitle("X LOS / cm");

            fh_ylos_yfib[iloscount] =
                new TH2F(Form("%s_ylos_yfib", detName), Form("%s yLOS vs yFIB ", detName), 500, -5., 5., 500, -5., 5.);
            fh_ylos_yfib[iloscount]->GetXaxis()->SetTitle("Y FIB / cm");
            fh_ylos_yfib[iloscount]->GetYaxis()->SetTitle("Y LOS / cm");

            fh_los_dt_hits_ToT[iloscount] =
                new TH2F(Form("%s_dt_ToT", detName), Form("%s ToT dt ", detName), 1000, 0, 10, 300, 0, 300.);
            fh_los_dt_hits_ToT[iloscount]->GetXaxis()->SetTitle(
                "dt between two hits / micros");                             // dt between two hits / ns
            fh_los_dt_hits_ToT[iloscount]->GetYaxis()->SetTitle("ToT / ns"); // ToT / ns

            fh_los_tres_MCFD[iloscount] = new TH1F(
                Form("%s_time_res_MCFD", detName), Form("%s MCFD Time resolution - raw", detName), 4000, -4., 4.);
            fh_los_tres_MCFD[iloscount]->GetXaxis()->SetTitle("Time MCFD / ns");

            fh_los_tres_MCFD_cond[iloscount] = new TH1F(
                Form("%s_time_res_MCFD_cond", detName), Form("%s MCFD Time resolution - cond", detName), 4000, -4., 4.);
            fh_los_tres_MCFD_cond[iloscount]->GetXaxis()->SetTitle("Time MCFD / ns");

            fh_los_tres_TAMEX[iloscount] = new TH1F(
                Form("%s_time_res_TAMEX", detName), Form("%s TAMEX Time resolution -raw ", detName), 4000, -4., 4.);
            fh_los_tres_TAMEX[iloscount]->GetXaxis()->SetTitle("Time TAMEX / ns");

            fh_los_tot[iloscount] =
                new TH2F(Form("%s_tot", detName), Form("%s ToT vs PMT", detName), 10, 0, 10, 1500, 0., 300.);
            fh_los_tot[iloscount]->GetXaxis()->SetTitle("PMT number");
            fh_los_tot[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            fh_los_tot_cond[iloscount] =
                new TH2F(Form("%s_tot_cond", detName), Form("%s ToT vs PMT cond", detName), 10, 0, 10, 1500, 0., 300.);
            fh_los_tot_cond[iloscount]->GetXaxis()->SetTitle("PMT number");
            fh_los_tot_cond[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            fh_los_tot_mean[iloscount] =
                new TH1F(Form("%s_tot_mean", detName), Form("%s mean ToT", detName), 1500, 0., 300.);
            fh_los_tot_mean[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_tot_mean[iloscount]->GetXaxis()->SetTitle("ToT / ns");

            fh_los_tot_mean_cond[iloscount] =
                new TH1F(Form("%s_tot_mean_cond", detName), Form("%s mean ToT cond", detName), 1500, 0., 300.);
            fh_los_tot_mean_cond[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_tot_mean_cond[iloscount]->GetXaxis()->SetTitle("ToT / ns");

            fh_los_tot_mean_corr[iloscount] =
                new TH1F(Form("%s_tot_mean_cor", detName), Form("%s mean ToT_cor", detName), 2000, 1., 400.);
            fh_los_tot_mean_corr[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_tot_mean_corr[iloscount]->GetXaxis()->SetTitle("ToT_cor / ns");

            fh_los_ihit_ToT[iloscount] =
                new TH2F(Form("%s_tot_ihit", detName), Form("%s ToT vs ihit", detName), 10, 0, 10, 600, 0., 300.);
            // fh_los_ihit_ToT[iloscount]= new TH2F(Form("%s_tot_ihit",detName),Form("%s ToT vs
            // ihit",detName),15,0,15,300,0.,300.);
            fh_los_ihit_ToT[iloscount]->GetXaxis()->SetTitle("iHit");
            fh_los_ihit_ToT[iloscount]->GetYaxis()->SetTitle("ToT / ns");

            fh_losToT_vs_Events[iloscount] = new TH2F(Form("%s_tot_vs_event", detName),
                                                      Form("%s ToT vs. Event #", detName),
                                                      10000,
                                                      0,
                                                      10000000,
                                                      300,
                                                      0.,
                                                      300.);
            fh_losToT_vs_Events[iloscount]->GetYaxis()->SetTitle("ToT / ns");
            fh_losToT_vs_Events[iloscount]->GetXaxis()->SetTitle("Event number");

            fh_losTAMEX_vs_Events[iloscount] = new TH2F(Form("%s_dtTAMEX_vs_event", detName),
                                                        Form("%s dtTAMEX vs. Event #", detName),
                                                        10000,
                                                        0,
                                                        10000000,
                                                        1000,
                                                        -4.,
                                                        4.);
            fh_losTAMEX_vs_Events[iloscount]->GetYaxis()->SetTitle("Time TAMEX  / ns");
            fh_losTAMEX_vs_Events[iloscount]->GetXaxis()->SetTitle("Event number");

            fh_losMCFD_vs_Events[iloscount] = new TH2F(Form("%s_dtMCFD_vs_event", detName),
                                                       Form("%s dtMCFD vs. Event #", detName),
                                                       10000,
                                                       0,
                                                       10000000,
                                                       1000,
                                                       -4.,
                                                       4.);
            fh_losMCFD_vs_Events[iloscount]->GetYaxis()->SetTitle("Time MCFD  / ns");
            fh_losMCFD_vs_Events[iloscount]->GetXaxis()->SetTitle("Event number");

            fh_los_rw[iloscount] = new TH1F(Form("%s_rw", detName), Form("%s rw", detName), 600, 100, 400.);
            fh_los_rw[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_rw[iloscount]->GetXaxis()->SetTitle("Emission angle / deg");

            fh_los_vftx_tamex[iloscount] =
                new TH1F(Form("%s_vftx_tamex", detName), Form("%s vftx_tamex", detName), 100000, -5000, 5000.);
            fh_los_vftx_tamex[iloscount]->GetYaxis()->SetTitle("Counts");
            fh_los_vftx_tamex[iloscount]->GetXaxis()->SetTitle("Ttamex-Tvftx / ns");

            fh_los_tot_S8[iloscount] = new TH2F(
                Form("%s_tot_dtS8", detName), Form("%s ToTcorr vs. dT-S8", detName), 2000, -5, 5, 400, 1., 401.);

            cLos[iloscount]->Divide(4, 4);
            cLos[iloscount]->cd(1);
            fh_los_channels[iloscount]->Draw();
            cLos[iloscount]->cd(2);
            gPad->SetLogy();
            fh_los_multihit[iloscount]->Draw();
            cLos[iloscount]->cd(3);
            gPad->SetLogz();
            fh_los_tot[iloscount]->Draw("colz");
            cLos[iloscount]->cd(4);
            gPad->SetLogz();
            fh_los_tot_cond[iloscount]->Draw("colz");
            cLos[iloscount]->cd(5);
            fh_los_tot_mean[iloscount]->Draw();
            cLos[iloscount]->cd(6);
            fh_los_tot_mean_cond[iloscount]->Draw();
            cLos[iloscount]->cd(7); // gPad->SetLogy();
            fh_los_tres_MCFD[iloscount]->Draw();
            cLos[iloscount]->cd(8); // gPad->SetLogy();
            fh_los_tres_MCFD_cond[iloscount]->Draw();
            cLos[iloscount]->cd(9);
            gPad->SetLogz();
            fh_los_pos_ToT[iloscount]->Draw("colz");
            cLos[iloscount]->cd(10);
            gPad->SetLogz();
            fh_los_pos_ToT_cond[iloscount]->Draw("colz");
            cLos[iloscount]->cd(11);
            gPad->SetLogz();
            fh_los_pos_MCFD[iloscount]->Draw("colz");
            cLos[iloscount]->cd(12);
            gPad->SetLogz();
            fh_los_pos_MCFD_cond[iloscount]->Draw("colz");
            cLos[iloscount]->cd(13);
            gPad->SetLogz();
            fh_xlos_xfib[iloscount]->Draw("colz");
            cLos[iloscount]->cd(14);
            gPad->SetLogz();
            fh_ylos_yfib[iloscount]->Draw("colz");
            cLos[iloscount]->cd(15);
            gPad->SetLogz();
            fh_los_twin[iloscount]->Draw("colz");
            cLos[iloscount]->cd(0);

            //  run->AddObject(cLos[iloscount]);
        }

        //        run->GetHttpServer()->RegisterCommand("Reset_LOS", Form("/Tasks/%s/->Reset_LOS_Histo()", GetName()));
    }

    if (fMappedItemsTwim)
    {
        // TWIM: Map data for E in the first (anodes from 0 to 7) and final sections (anodes from 8 to 15)
        TCanvas* cTwimMap_ESum = new TCanvas("twim_ESum", "twim ESum", 10, 10, 800, 700);
        cTwimMap_ESum->Divide(3, 2);

        cTwimMap_ESum->cd(1);
        gPad->SetLogy();
        fh1_twim_ESum[0] = new TH1F("fh1_twim_ESum1", "twim:ESum:first", 8192, 0, 8192);
        fh1_twim_ESum[0]->GetXaxis()->SetTitle("Energy [channels]");
        fh1_twim_ESum[0]->GetYaxis()->SetTitle("Counts");
        fh1_twim_ESum[0]->GetYaxis()->SetTitleOffset(1.1);
        fh1_twim_ESum[0]->GetXaxis()->CenterTitle(true);
        fh1_twim_ESum[0]->GetYaxis()->CenterTitle(true);
        fh1_twim_ESum[0]->GetXaxis()->SetLabelSize(0.045);
        fh1_twim_ESum[0]->GetXaxis()->SetTitleSize(0.045);
        fh1_twim_ESum[0]->GetYaxis()->SetLabelSize(0.045);
        fh1_twim_ESum[0]->GetYaxis()->SetTitleSize(0.045);
        fh1_twim_ESum[0]->Draw("");
        cTwimMap_ESum->cd(2);
        gPad->SetLogy();
        fh1_twim_ESum[1] = new TH1F("fh1_twim_ESum2", "twim:ESum:Second", 8192, 0, 8192);
        fh1_twim_ESum[1]->GetXaxis()->SetTitle("Energy [channels]");
        fh1_twim_ESum[1]->GetYaxis()->SetTitle("Counts");
        fh1_twim_ESum[1]->GetYaxis()->SetTitleOffset(1.1);
        fh1_twim_ESum[1]->GetXaxis()->CenterTitle(true);
        fh1_twim_ESum[1]->GetYaxis()->CenterTitle(true);
        fh1_twim_ESum[1]->GetXaxis()->SetLabelSize(0.045);
        fh1_twim_ESum[1]->GetXaxis()->SetTitleSize(0.045);
        fh1_twim_ESum[1]->GetYaxis()->SetLabelSize(0.045);
        fh1_twim_ESum[1]->GetYaxis()->SetTitleSize(0.045);
        fh1_twim_ESum[1]->Draw("");
        cTwimMap_ESum->cd(3);
        gPad->SetLogy();
        fh1_twim_ESum[2] = new TH1F("fh1_twim_ESum", "twim:ESum", 8192, 0, 8192);
        fh1_twim_ESum[2]->GetXaxis()->SetTitle("Energy [channels]");
        fh1_twim_ESum[2]->GetYaxis()->SetTitle("Counts");
        fh1_twim_ESum[2]->GetYaxis()->SetTitleOffset(1.1);
        fh1_twim_ESum[2]->GetXaxis()->CenterTitle(true);
        fh1_twim_ESum[2]->GetYaxis()->CenterTitle(true);
        fh1_twim_ESum[2]->GetXaxis()->SetLabelSize(0.045);
        fh1_twim_ESum[2]->GetXaxis()->SetTitleSize(0.045);
        fh1_twim_ESum[2]->GetYaxis()->SetLabelSize(0.045);
        fh1_twim_ESum[2]->GetYaxis()->SetTitleSize(0.045);
        fh1_twim_ESum[2]->Draw("");
        cTwimMap_ESum->cd(4);
        gPad->SetLogz();
        fh2_twim_ESum = new TH2F("fh2_twim_ESum", "twim: ESum1 vs Esum2", 2192, 0, 8192, 2192, 0, 8192);
        fh2_twim_ESum->GetXaxis()->SetTitle("Energy1 [channels]");
        fh2_twim_ESum->GetYaxis()->SetTitle("Energy2 [channels]");
        fh2_twim_ESum->GetYaxis()->SetTitleOffset(1.1);
        fh2_twim_ESum->GetXaxis()->CenterTitle(true);
        fh2_twim_ESum->GetYaxis()->CenterTitle(true);
        fh2_twim_ESum->GetXaxis()->SetLabelSize(0.045);
        fh2_twim_ESum->GetXaxis()->SetTitleSize(0.045);
        fh2_twim_ESum->GetYaxis()->SetLabelSize(0.045);
        fh2_twim_ESum->GetYaxis()->SetTitleSize(0.045);
        fh2_twim_ESum->Draw("col");
        cTwimMap_ESum->cd(5);
        fh1_Twimmap_mult = new TH1F("twin_multipl", "Twin multiplicity", 17, 0, 17);
        fh1_Twimmap_mult->GetXaxis()->SetTitle("Anode");
        fh1_Twimmap_mult->GetYaxis()->SetTitle("Counts");
        fh1_Twimmap_mult->GetYaxis()->SetTitleOffset(1.1);
        fh1_Twimmap_mult->GetXaxis()->CenterTitle(true);
        fh1_Twimmap_mult->GetYaxis()->CenterTitle(true);
        fh1_Twimmap_mult->GetXaxis()->SetLabelSize(0.045);
        fh1_Twimmap_mult->GetXaxis()->SetTitleSize(0.045);
        fh1_Twimmap_mult->GetYaxis()->SetLabelSize(0.045);
        fh1_Twimmap_mult->GetYaxis()->SetTitleSize(0.045);
        fh1_Twimmap_mult->Draw("");

        //   run->AddObject(cTwimMap_ESum);

        //       run->GetHttpServer()->RegisterCommand("Reset_TWIN", Form("/Tasks/%s/->Reset_TWIN_Histo()", GetName()));
    }

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    TCanvas* FibCanvas[NOF_FIB_DET];
    fh_Fib_xy = new TH2F("XY_Fib", "XY Fiber ", 500, -5., 5., 500, -5., 5.);
    fh_Fib_xy->GetXaxis()->SetTitle("X position / cm");
    fh_Fib_xy->GetYaxis()->SetTitle("Y position / cm");

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {

        std::stringstream fibhitobj;
        std::stringstream fibmapobj;
        std::stringstream FiName;
        std::string temp;

        if (fMappedItems.at(DET_FI_FIRST + ifibcount))
        {

            const char* chistName;
            const char* chistTitle;
            const char* detName;
            std::string tempTitle;
            std::string tempName;
            std::stringstream tempFibName;
            std::string tempFibNames;
            std::stringstream tempCanvName;

            detName = fDetectorNames[DET_FI_FIRST + ifibcount];

            FibCanvas[ifibcount] = new TCanvas(detName, detName, 10, 10, 910, 910);

            // Channels:
            fh_channels_Fib[ifibcount] =
                new TH1F(Form("%s_channels", detName), Form("%s channels", detName), 520, 0., 520.);
            fh_channels_Fib[ifibcount]->GetXaxis()->SetTitle("Channel number");
            fh_channels_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Channels:
            fh_channels_single_Fib[ifibcount] =
                new TH1F(Form("%s_channels_single", detName), Form("%s channels of single PMTs", detName), 10, 0., 10.);
            fh_channels_single_Fib[ifibcount]->GetXaxis()->SetTitle("Channel number");
            fh_channels_single_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Fibers:
            fh_fibers_Fib[ifibcount] =
                new TH1F(Form("%s_fibers", detName), Form("%s fibers", detName), N_FIBER_PLOT, 0., N_FIBER_PLOT);
            fh_fibers_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_fibers_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multiplicity (number of hit fibers):
            fh_mult_Fib[ifibcount] = new TH1F(Form("%s_mult", detName), Form("%s mult", detName), 100, 0., 100.);
            fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multihit MAPMT:
            fh_multihit_m_Fib[ifibcount] = new TH2F(
                Form("%s_multihit_m", detName), Form("%s multihits MAPMT", detName), 520, 0., 520., 20, 0., 20.);
            fh_multihit_m_Fib[ifibcount]->GetXaxis()->SetTitle("MAPMT channel");
            fh_multihit_m_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // Multihit SAPMT:
            fh_multihit_s_Fib[ifibcount] = new TH2F(
                Form("%s_multihit_s", detName), Form("%s multihits single PMT", detName), 16, 0., 16., 20, 0., 20.);
            fh_multihit_s_Fib[ifibcount]->GetXaxis()->SetTitle("Single PMT channel");
            fh_multihit_s_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // ToT MAPMT:
            fh_ToT_m_Fib[ifibcount] = new TH2F(Form("%s_tot_m", detName),
                                               Form("%s ToT of MAPMT", detName),
                                               N_FIBER_PLOT,
                                               0.,
                                               N_FIBER_PLOT,
                                               600,
                                               0.,
                                               200.);
            fh_ToT_m_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_m_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToT SAPMT:
            fh_ToT_s_Fib[ifibcount] = new TH2F(Form("%s_tot_s", detName),
                                               Form("%s ToT of single PMT", detName),
                                               N_FIBER_PLOT,
                                               0.,
                                               N_FIBER_PLOT,
                                               600,
                                               0.,
                                               200.);
            fh_ToT_s_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_s_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // Time of fiber:
            fh_time_Fib[ifibcount] = new TH2F(Form("%s_TimevsFiber", detName),
                                              Form("%s Time vs Fiber", detName),
                                              N_FIBER_PLOT,
                                              0.,
                                              N_FIBER_PLOT,
                                              2048,
                                              -1024.,
                                              1024.);
            fh_time_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_time_Fib[ifibcount]->GetYaxis()->SetTitle("tMAPMT-tSPMT");

            // ToF LOS -> Fiber:
            fh_Fib_ToF[ifibcount] = new TH2F(Form("%s_tof", detName),
                                             Form("%s ToF LOS to Fiber", detName),
                                             N_FIBER_PLOT,
                                             0.,
                                             N_FIBER_PLOT,
                                             10000,
                                             -5000.,
                                             5000.);
            fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
            fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("Fiber number");

            // Not-calibrated position:
            fh_Fib_pos[ifibcount] = new TH2F(
                Form("%s_pos", detName), Form("%s Not-calibrated position", detName), 2000, -100, 100, 2000, -100, 100);
            fh_Fib_pos[ifibcount]->GetXaxis()->SetTitle("x position");
            fh_Fib_pos[ifibcount]->GetYaxis()->SetTitle("y position");

            // hit fiber number vs. event number:
            fh_Fib_vs_Events[ifibcount] = new TH2F(Form("%s_fib_vs_event", detName),
                                                   Form("%s Fiber # vs. Event #", detName),
                                                   10000,
                                                   0,
                                                   5e6,
                                                   1100,
                                                   0.,
                                                   1100.);
            fh_Fib_vs_Events[ifibcount]->GetYaxis()->SetTitle("Fiber number");
            fh_Fib_vs_Events[ifibcount]->GetXaxis()->SetTitle("Event number");

            fh_ToTmax_Fibmax[ifibcount] = new TH2F(Form("%s_totMax_iFibMax", detName),
                                                   Form("%s ToTMax vs iFIbMax", detName),
                                                   N_FIBER_PLOT,
                                                   0.,
                                                   N_FIBER_PLOT,
                                                   600,
                                                   0.,
                                                   200.);
            fh_ToT_s_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_s_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");
            // ToT SAPMT:
            fh_ToT_single_Fib[ifibcount] = new TH2F(
                Form("%s_tot_single", detName), Form("%s ToT of single PMTs", detName), 10, 0., 10., 600, 0., 200.);
            fh_ToT_single_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_single_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            FibCanvas[ifibcount]->Divide(3, 4);
            FibCanvas[ifibcount]->cd(1);
            fh_channels_Fib[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(2);
            gPad->SetLogz();
            fh_multihit_m_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(3);
            gPad->SetLogz();
            fh_multihit_s_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(4);
            fh_fibers_Fib[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(5);
            fh_mult_Fib[ifibcount]->Draw();
            FibCanvas[ifibcount]->cd(6);
            gPad->SetLogz();
            fh_ToT_m_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(7);
            gPad->SetLogz();
            fh_ToT_s_Fib[ifibcount]->Draw("colz");
            // FibCanvas[ifibcount]->cd(8);gPad->SetLogz();
            // fh_time_Fib[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(8);
            gPad->SetLogz();
            fh_Fib_xy->Draw("colz");
            FibCanvas[ifibcount]->cd(9);
            fh_Fib_ToF[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(10);
            fh_Fib_vs_Events[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(11);
            fh_ToTmax_Fibmax[ifibcount]->Draw("colz");
            FibCanvas[ifibcount]->cd(12);
            fh_channels_single_Fib[ifibcount]->Draw();

            FibCanvas[ifibcount]->cd(0);
            //  run->AddObject(FibCanvas[ifibcount]);

        } // end if(Mapped)

    } // end for(ifibcount)

    // -------------------------------------------------------------------------

    return kSUCCESS;
}

void R3BOnlineSpectraLosEngRun2019::Exec(Option_t* option)
{
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        cout << "Events: " << fNEvents << flush << '\r';

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(error) << "FairRootManager not found";
        return;
    }

    //    ofstream myFile("data_eng_run40.dat", ios_base::out | ios_base::app);

    time = header->GetTimeStamp() / 1.6; // divided by 1.6 for stand alone daq with Vulom generating time stamps

    // time = 0;

    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }

    if (header->GetTrigger() == 12)
        time_spill_start = time; // header->GetTimeStamp();    // spill start in nsec
    if (header->GetTrigger() == 13)
        time_spill_end = time; // header->GetTimeStamp();    // spill end  in nsec
                               /*
                                   if (header->GetTrigger() == 12)
                                       cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
                                   if (header->GetTrigger() == 13)
                                       cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec" << endl;
                               */
    fhTrigger->Fill(header->GetTrigger());

    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    Int_t tpatbin;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0)
            fhTpat->Fill(i + 1);
    }

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit = fTpat - 1;
    Int_t itpat;
    Int_t tpatvalue;
    if (fTpat_bit >= 0)
    {
        itpat = header->GetTpat();
        tpatvalue = (itpat && (1 << fTpat_bit)) >> fTpat_bit;
        if ((tpatvalue == 0))
            return;
    }

    // TWIN MUSIC
    Double_t dE_TWIN;
    Bool_t twin = false;
    // Fill mapped data
    if (fMappedItemsTwim && fMappedItemsTwim->GetEntriesFast() > 0)
    {
        Double_t e1 = 0., e2 = 0., E[NbSections][NbAnodes], DT[NbSections][NbAnodes + NbSections];
        Double_t n1 = 0., n2 = 0.;
        for (Int_t j = 0; j < NbSections; j++)
            for (Int_t i = 0; i < NbAnodes; i++)
                E[j][i] = 0.; // mult=1 !!!
        for (Int_t j = 0; j < NbSections; j++)
            for (Int_t i = 0; i < NbAnodes + NbSections; i++)
                DT[j][i] = 0.; // mult=1 !!!
        Int_t nHits = fMappedItemsTwim->GetEntriesFast();
        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BSofTwimMappedData* hit = (R3BSofTwimMappedData*)fMappedItemsTwim->At(ihit);
            if (!hit)
                continue;
            fh1_Twimmap_mult->Fill(hit->GetAnodeID());
            //  fh1_twimmap_E[hit->GetSecID()][hit->GetAnodeID()]->Fill(hit->GetEnergy());
            if (DT[hit->GetSecID()][hit->GetAnodeID()] == 0 && E[hit->GetSecID()][hit->GetAnodeID()] == 0 &&
                hit->GetEnergy() < 8192 && hit->GetEnergy() > 0)
            {
                E[hit->GetSecID()][hit->GetAnodeID()] = hit->GetEnergy(); // mult=1 !!!
                DT[hit->GetSecID()][hit->GetAnodeID()] = hit->GetTime();  // mult=1 !!!
            }

            if (hit->GetAnodeID() < NbAnodes / 2 && hit->GetAnodeID() != 7)
            {
                e1 = e1 + hit->GetEnergy();
                n1++;
            }
            else if (hit->GetAnodeID() >= NbAnodes / 2 && hit->GetAnodeID() < NbAnodes && hit->GetAnodeID() != 15)
            {
                e2 = e2 + hit->GetEnergy();
                n2++;
            }
        }
        if ((e1 / n1 < 5000) && abs(e1 / n1 - e2 / n2) < 200.)
        {
            twin = true;
            fh1_twim_ESum[0]->Fill(e1 / n1);
            fh1_twim_ESum[1]->Fill(e2 / n2);
            fh1_twim_ESum[2]->Fill((e1 + e2) / (n1 + n2));
            fh2_twim_ESum->Fill(e1 / n1, e2 / n2);
        }
        // cout<<"TWIN: "<< e1/n1<<", "<<e2/n2<<endl;
        dE_TWIN = (e1 + e2) / (n1 + n2);
    }

    //----------------------------------------------------------------------
    // LOS detector
    //----------------------------------------------------------------------
    // Time nomenclature for LOS:
    // lt=0, l=1,lb=2,b=3,rb=4,r=5,rt=6,t=7
    //   t / \lt
    //   rt| |l
    //   r | |lb
    //   rb\ /b

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
                tot[idet][imult][icha] = -1000. * float(icha + 1); // 0.0 / 0.0;
            }
        }
    }
    Int_t nPartLOS = 0;
    Int_t nPartc[fNofLosDetectors] = { 0 };
    Bool_t iLOSType[fNofLosDetectors][32];
    Bool_t iLOSPileUp[fNofLosDetectors][32];
    for (Int_t idet = 0; idet < fNofLosDetectors; idet++)
    {
        for (Int_t imult = 0; imult < 33; imult++)
        {
            iLOSType[idet][imult] = false;
            iLOSPileUp[idet][imult] = false;
        }
    }

    Int_t Multip;

    if (fMappedItems.at(DET_LOS))
    {
        auto det = fMappedItems.at(DET_LOS);
        Int_t nHits = det->GetEntriesFast();

        Multip = nHits;

        //    cout<<"LOS: nHits"<<nHits<<endl;

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BLosMappedData* hit = (R3BLosMappedData*)det->At(ihit);
            if (!hit)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hit->GetDetector(); // 1..
            Int_t iCha = hit->GetChannel();  // 1..
            Int_t iTyp = hit->GetType();     // 0,1,2,3
        }
    }

    if (fCalItems.at(DET_LOS))
    {
        auto det = fCalItems.at(DET_LOS);
        nPartLOS = det->GetEntriesFast();

        Int_t iDet = 0;
        Double_t time_V_LOS1[32][8] = { 0. };
        Double_t time_V_LOS2[32][8] = { 0. };

        for (Int_t iPart = 0; iPart < nPartLOS; iPart++)
        {
            /*
             * nPart is the number of particle passing through LOS detector in one event
             */
            R3BLosCalData* calData = (R3BLosCalData*)det->At(iPart);
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

                // if(iDet < fNofLosDetectors+1)
                // {

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
                                time_T[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] + 2048. * fClockFreq;
                            }

                            nPMT = nPMT + 1;
                            tot[iDet - 1][iPart][ipm] = time_T[iDet - 1][iPart][ipm] - time_L[iDet - 1][iPart][ipm];
                        }
                        // pileup rejection
                        if (tot[iDet - 1][iPart][ipm] > pile[ipm])
                            iLOSPileUp[iDet - 1][iPart] = true;

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

                        /*  if (fNEvents < 5000000)
                              myFile << setprecision(10) << iDet << " " << iPart << " " << ipm << " "
                                     << time_V[iDet - 1][iPart][ipm] << " " << time_L[iDet - 1][iPart][ipm] << " "
                                     << tot[iDet - 1][iPart][ipm] << endl;*/
                    }

                    totsum[iDet - 1][iPart] = totsum[iDet - 1][iPart] / nPMT;

                    timeLosV[iDet - 1][iPart] = timeLosV[iDet - 1][iPart] / nPMV;

                    timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] / nPMT;

                    timeLos[iDet - 1][iPart] = timeLosV[iDet - 1][iPart];

                    // cout<<"LOS: "<<iDet<<", "<<iPart<<"; "<<timeLos[iDet-1][iPart]<<endl;

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

                    //   LosTresMTDC[iPart] =
                    //   ((time_MTDC[iPart][0]+time_MTDC[iPart][2]+time_MTDC[iPart][4]+time_MTDC[iPart][6]) -
                    //      (time_MTDC[iPart][1]+time_MTDC[iPart][3]+time_MTDC[iPart][5]+time_MTDC[iPart][7]))/4.;

                    LosTresMTDC[iPart] =
                        ((time_MTDC[iPart][1] + time_MTDC[iPart][3]) - (time_MTDC[iPart][5] + time_MTDC[iPart][7])) /
                        4.;

                    // Position from tamex -> ln(ToT):
                    /*
                       xT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][1] + time_L[iDet - 1][iPart][2]) / 2. -
                                                (time_L[iDet - 1][iPart][5] + time_L[iDet - 1][iPart][6]) / 2.;
                       yT_cm[iDet - 1][iPart] = (time_L[iDet - 1][iPart][3] + time_L[iDet - 1][iPart][4]) / 2. -
                                                (time_L[iDet - 1][iPart][7] + time_L[iDet - 1][iPart][0]) / 2.;
                     */
                    xT_cm[iDet - 1][iPart] = log((tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) /
                                                 (tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2]));
                    yT_cm[iDet - 1][iPart] = log((tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7]) /
                                                 (tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][3]));

                    xT_cm[iDet - 1][iPart] = (xT_cm[iDet - 1][iPart] - flosOffsetXT[iDet - 1]) * flosVeffXT[iDet - 1];
                    yT_cm[iDet - 1][iPart] = (yT_cm[iDet - 1][iPart] - flosOffsetYT[iDet - 1]) * flosVeffYT[iDet - 1];

                    // Position from VFTX:
                    xV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][1] + time_V[iDet - 1][iPart][2]) / 2. -
                                             (time_V[iDet - 1][iPart][5] + time_V[iDet - 1][iPart][6]) / 2.;
                    yV_cm[iDet - 1][iPart] = (time_V[iDet - 1][iPart][3] + time_V[iDet - 1][iPart][4]) / 2. -
                                             (time_V[iDet - 1][iPart][7] + time_V[iDet - 1][iPart][0]) / 2.;
                    xV_cm[iDet - 1][iPart] = (xV_cm[iDet - 1][iPart] - flosOffsetXV[iDet - 1]) * flosVeffXV[iDet - 1];
                    yV_cm[iDet - 1][iPart] = (yV_cm[iDet - 1][iPart] - flosOffsetYV[iDet - 1]) * flosVeffYV[iDet - 1];

                    // Position from ToT:
                    if (tot[iDet - 1][iPart][1] > 0. && tot[iDet - 1][iPart][2] > 0. && tot[iDet - 1][iPart][5] > 0. &&
                        tot[iDet - 1][iPart][6] > 0. && tot[iDet - 1][iPart][0] > 0. && tot[iDet - 1][iPart][3] > 0. &&
                        tot[iDet - 1][iPart][4] > 0. && tot[iDet - 1][iPart][7] > 0.)
                    {

                        // For runs below 35
                        yToT_cm[iDet - 1][iPart] = (-1.) * (((tot[iDet - 1][iPart][6] + tot[iDet - 1][iPart][5]) / 2. -
                                                             (tot[iDet - 1][iPart][2] + tot[iDet - 1][iPart][1]) / 2.) /
                                                            ((tot[iDet - 1][iPart][2] + tot[iDet - 1][iPart][1] +
                                                              tot[iDet - 1][iPart][6] + tot[iDet - 1][iPart][5]) /
                                                             4.));

                        xToT_cm[iDet - 1][iPart] = (-1.) * (((tot[iDet - 1][iPart][4] + tot[iDet - 1][iPart][3]) / 2. -
                                                             (tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7]) / 2.) /
                                                            ((tot[iDet - 1][iPart][0] + tot[iDet - 1][iPart][7] +
                                                              tot[iDet - 1][iPart][4] + tot[iDet - 1][iPart][3]) /
                                                             4.));
                        // For runs above 35
                        /*
                                           xToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][5] + tot[iDet -
                           1][iPart][6]) / 2. - (tot[iDet - 1][iPart][1] + tot[iDet - 1][iPart][2]) / 2.) /
                                                                        ((tot[iDet - 1][iPart][1] + tot[iDet -
                           1][iPart][2] + tot[iDet - 1][iPart][5] + tot[iDet - 1][iPart][6]) / 4.));

                                            yToT_cm[iDet - 1][iPart] = (((tot[iDet - 1][iPart][0] + tot[iDet -
                           1][iPart][7]) / 2. - (tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) / 2.) /
                                                                        ((tot[iDet - 1][iPart][7] + tot[iDet -
                           1][iPart][0] + tot[iDet - 1][iPart][3] + tot[iDet - 1][iPart][4]) / 4.));
                        */
                    }

                    xToT_cm[iDet - 1][iPart] =
                        (xToT_cm[iDet - 1][iPart] - flosOffsetXQ[iDet - 1]) * flosVeffXQ[iDet - 1];
                    yToT_cm[iDet - 1][iPart] =
                        (yToT_cm[iDet - 1][iPart] - flosOffsetYQ[iDet - 1]) * flosVeffYQ[iDet - 1];

                    //        if(xToT_cm[iDet - 1][iPart] == 0. && yToT_cm[iDet - 1][iPart] == 0.)
                    //        cout<<tot[iDet - 1][iPart][1]<<", "<<tot[iDet - 1][iPart][2]<<", "<<tot[iDet -
                    //        1][iPart][5]<<", "<<tot[iDet - 1][iPart][6] <<", "<<
                    //          tot[iDet - 1][iPart][0]<<", "<<tot[iDet - 1][iPart][3]<<", "<<tot[iDet -
                    //          1][iPart][4]<<", "<<tot[iDet - 1][iPart][7] <<endl;

                    // time difference between two hits
                    if (time0[iDet - 1] < 0)
                        time0[iDet - 1] = timeLosV[iDet - 1][iPart];
                    time1[iDet - 1] = timeLosV[iDet - 1][iPart];
                    time_abs[iDet - 1] = time1[iDet - 1] - time0[iDet - 1] + (double)(time - time_prev[iDet - 1]);
                    if (time_abs[iDet - 1] > 0 && time_abs[iDet - 1] < 5.E8)
                    {
                        fh_los_dt_hits_ToT[iDet - 1]->Fill(time_abs[iDet - 1] / 1000., totsum[iDet - 1][iPart]);
                    }

                    time_prev[iDet - 1] = time;
                    time0[iDet - 1] = time1[iDet - 1];

                    if (timeLosV[iDet - 1][iPart] > 0. && timeLosV[iDet - 1][iPart] < 8192. * 5. &&
                        !(IS_NAN(timeLosV[iDet - 1][iPart])))
                    {
                        while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] < 2048. * 5. / 2.)
                        {
                            timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] + 2048. * 5.;
                        }
                        while (timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart] > 2048. * 5. / 2.)
                        {
                            timeLosT[iDet - 1][iPart] = timeLosT[iDet - 1][iPart] - 2048. * 5.;
                        }

                        if (twin)
                            fh_los_vftx_tamex[iDet - 1]->Fill(timeLosT[iDet - 1][iPart] - timeLosV[iDet - 1][iPart]);
                    }

                    fh_losMCFD_vs_Events[iDet - 1]->Fill(fNEvents, LosTresV[iDet - 1][iPart]);
                    fh_losTAMEX_vs_Events[iDet - 1]->Fill(fNEvents, LosTresT[iDet - 1][iPart]);
                    fh_losToT_vs_Events[iDet - 1]->Fill(fNEvents, totsum[iDet - 1][iPart]);

                    for (int ipm = 0; ipm < 8; ipm++)
                    {
                        fh_los_tot[iDet - 1]->Fill(ipm + 1, tot[iDet - 1][iPart][ipm]);
                    }
                    if (1 == 1) // !(iLOSPileUp[iDet - 1][iPart]))    // desregard pile-up events
                    {
                        fh_los_tot_mean[iDet - 1]->Fill(totsum[iDet - 1][iPart]);
                        fh_los_tres_MCFD[iDet - 1]->Fill(LosTresV[iDet - 1][iPart]);
                        fh_los_tres_TAMEX[iDet - 1]->Fill(LosTresT[iDet - 1][iPart]);
                        fh_los_pos_MCFD[iDet - 1]->Fill(xV_cm[iDet - 1][iPart], yV_cm[iDet - 1][iPart]);
                        //  fh_los_pos_TAMEX[iDet - 1]->Fill(xT_cm[iDet - 1][iPart], yT_cm[iDet - 1][iPart]);
                        fh_los_pos_ToT[iDet - 1]->Fill(xToT_cm[iDet - 1][iPart], yToT_cm[iDet - 1][iPart]);
                        //  fh_los_twin[iDet - 1]->Fill(totsum[iDet - 1][iPart], dE_TWIN);
                        fh_los_ihit_ToT[iDet - 1]->Fill(iPart + 1, totsum[iDet - 1][iPart]);
                        fh_los_multihit[iDet - 1]->Fill(iPart + 1);
                    }
                } // if iLosType

                // }
                // else
                // {
                // cout<<"Wrong detector ID for LOS!"<<endl;
                // }
            } // for iPart
        }     // for iDet

    } // if fCallItems

    if (fCalItems.at(DET_LOS) && fNofLosDetectors == 2)
    {
        fh_los_nPart_vs_nPart->Fill(nPartc[0], nPartc[1]);

        for (Int_t iPart1 = 0; iPart1 < nPartc[0]; iPart1++)
        {
            for (Int_t iPart2 = 0; iPart2 < nPartc[1]; iPart2++)
            {
                if (timeLos[1][iPart2] > 0. && timeLos[0][iPart1] > 0.)
                {
                    fh_dt_los1_los2->Fill(timeLos[1][iPart2] - timeLos[0][iPart1]);
                    if ((timeLos[1][iPart2] - timeLos[0][iPart1]) > 2. &&
                        (timeLos[1][iPart2] - timeLos[0][iPart1] < 2.4))
                        fh_los_ToT_vs_ToT->Fill(totsum[0][iPart1], totsum[1][iPart2]);
                }
            }
        }
    }

    //----------------------------------------------------------------------
    // SCI8 detector
    //----------------------------------------------------------------------
    std::vector<double_t> time_first(fNofLosDetectors, -1.);

    Double_t timeS8_V[10][2], timeS8_L[10][2], timeS8_T[10][2], timeSci8M[10], Sci8TresM[10], timeSci8T[10],
        Sci8TresT[10], timeSci8[10], totsumS8[10], totS8[10][8]; // [multihit][pm]
    for (Int_t i = 0; i < 10; i++)
    {
        timeSci8M[i] = 0.0;
        Sci8TresM[i] = 0.0 / 0.0;
        timeSci8T[i] = 0.0;
        Sci8TresT[i] = 0.0 / 0.0;
        timeSci8[i] = 0.0;
        totsumS8[i] = 0.0;
        for (Int_t j = 0; j < 2; j++)
        {
            timeS8_L[i][j] = 0.0 / 0.0;
            timeS8_T[i][j] = 0.0 / 0.0;
            totS8[i][j] = 0.0 / 0.0;
        }
    }

    Int_t MultipS8;
    Int_t nPartS8;
    Int_t lhitmem[fNofLosDetectors] = { 0 };
    Double_t tof_mem[fNofLosDetectors];
    for (Int_t i = 0; i < fNofLosDetectors; i++)
    {
        tof_mem[i] = 0. / 0.;
    }

    if (fMappedItems.at(DET_SCI8))
    {
        auto det = fMappedItems.at(DET_SCI8);
        Int_t nHits = det->GetEntriesFast();

        MultipS8 = nHits;

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BSci8MappedData* hit = (R3BSci8MappedData*)det->At(ihit);
            if (!hit)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hit->GetDetector(); // 1..
            Int_t iCha = hit->GetChannel();  // 1..

            fh_sci8_channels->Fill(iCha);
        }
    }

    if (fCalItems.at(DET_SCI8))
    {
        auto det = fCalItems.at(DET_SCI8);
        nPartS8 = det->GetEntriesFast();

        fh_sci8_multihit->Fill(nPartS8);

        Int_t iDet = 0;
        Int_t nPartS8_VFTX[2] = { 0 };
        Int_t nPartS8_LEAD[2] = { 0 };
        Int_t nPartS8_TRAI[2] = { 0 };

        for (Int_t iPart = 0; iPart < nPartS8; iPart++)
        {

            /*
             * nPart is the number of particle passing through Sci8 detector in one event
             */

            R3BSci8CalData* calDataS8 = (R3BSci8CalData*)det->At(iPart);
            iDet = calDataS8->GetDetector();

            // VFTX Channels 1-2:
            if (!(IS_NAN(calDataS8->fTimeV_r_ns)))
            {
                timeS8_V[iPart][0] = calDataS8->fTimeV_r_ns;
                nPartS8_VFTX[0] += 1;
            }
            if (!(IS_NAN(calDataS8->fTimeV_l_ns)))
            {
                timeS8_V[iPart][1] = calDataS8->fTimeV_l_ns;
                nPartS8_VFTX[1] += 1;
            }

            // TAMEX Channels 1-2:
            if (!(IS_NAN(calDataS8->fTimeL_r_ns)))
            {
                timeS8_L[iPart][0] = calDataS8->fTimeL_r_ns;
                nPartS8_LEAD[0] += 1;
            }
            if (!(IS_NAN(calDataS8->fTimeT_r_ns)))
            {
                timeS8_T[iPart][0] = calDataS8->fTimeT_r_ns;
                nPartS8_TRAI[0] += 1;
            }
            if (!(IS_NAN(calDataS8->fTimeL_l_ns)))
            {
                timeS8_L[iPart][1] = calDataS8->fTimeL_l_ns;
                nPartS8_LEAD[1] += 1;
            }
            if (!(IS_NAN(calDataS8->fTimeT_l_ns)))
            {
                timeS8_T[iPart][1] = calDataS8->fTimeT_l_ns;
                nPartS8_TRAI[1] += 1;
            }

            if (iPart > 0 && MultipS8 % 2 == 0)
            {
                for (int k = 0; k < 2; k++)
                {
                    if (timeS8_V[iPart][k] > 0. && timeS8_V[iPart - 1][k] > 0. && !(IS_NAN(timeS8_V[iPart][k])) &&
                        !(IS_NAN(timeS8_V[iPart - 1][k])))
                    {
                        fh_sci8_dt_hits->Fill(timeS8_V[iPart][k] - timeS8_V[iPart - 1][k]);
                    }
                    if (timeS8_L[iPart][k] > 0. && timeS8_L[iPart - 1][k] > 0. && !(IS_NAN(timeS8_L[iPart][k])) &&
                        !(IS_NAN(timeS8_L[iPart - 1][k])))
                    {
                        fh_sci8_dt_hits_l->Fill(timeS8_L[iPart][k] - timeS8_L[iPart - 1][k]);
                    }
                    if (timeS8_T[iPart][k] > 0. && timeS8_T[iPart - 1][k] > 0. && !(IS_NAN(timeS8_T[iPart][k])) &&
                        !(IS_NAN(timeS8_T[iPart - 1][k])))
                    {
                        fh_sci8_dt_hits_t->Fill(timeS8_T[iPart][k] - timeS8_T[iPart - 1][k]);
                    }
                }
            }

            if (iDet == 1)
            {

                // if(iPart >= 0)
                if (1 == 1)
                {
                    int nPMT = 0;
                    int nPMV = 0;
                    int ilc = iPart;

                    for (int ipm = 0; ipm < 2; ipm++)
                    {

                        if (timeS8_T[iPart][ipm] > 0. && timeS8_L[iPart][ipm] > 0. && !(IS_NAN(timeS8_T[iPart][ipm])) &&
                            !(IS_NAN(timeS8_L[iPart][ipm])))
                        {
                            while (timeS8_T[iPart][ipm] - timeS8_L[iPart][ipm] < 0.)
                            {
                                timeS8_T[iPart][ipm] = timeS8_T[iPart][ipm] + 2048. * fClockFreq;
                            }

                            nPMT = nPMT + 1;
                            totS8[iPart][ipm] = timeS8_T[iPart][ipm] - timeS8_L[iPart][ipm];
                        }

                        totsumS8[iPart] += totS8[iPart][ipm];

                        if (totS8[iPart][ipm] != 0. && !(IS_NAN(totS8[iPart][ipm])))
                            fh_sci8_tot->Fill(ipm + 1, totS8[iPart][ipm]);

                        if (timeS8_L[iPart][ipm] > 0. && !(IS_NAN(timeS8_L[iPart][ipm])))
                            timeSci8T[iPart] += timeS8_L[iPart][ipm];

                        if (timeS8_V[iPart][ipm] > 0. && !(IS_NAN(timeS8_V[iPart][ipm])))
                        {
                            timeSci8M[iPart] += timeS8_V[iPart][ipm];
                            nPMV = nPMV + 1;
                        }
                    }

                    totsumS8[iPart] = totsumS8[iPart] / nPMT;
                    /*
                    if(totsum[iPart] < 88.) cout<<fNEvents<<"; "<<nPart<<"; "<<iPart<<",
                    "<<totsum[iPart]<<tot[iPart][0]<<
                    ", "<<tot[iPart][1]<<", " <<tot[iPart][2]<<", "<<tot[iPart][3]<<", "<<tot[iPart][4]<<", "
                    <<tot[iPart][5]<<", "<<tot[iPart][6]<<", "<<tot[iPart][7]<<endl;
                    */
                    timeSci8M[iPart] = timeSci8M[iPart] / nPMV;
                    timeSci8T[iPart] = timeSci8T[iPart] / nPMT;

                    timeSci8[iPart] = timeSci8M[iPart];

                    Double_t tof = 0. / 0.;
                    // at first, take the first LOS-hit

                    if (!IS_NAN(timeLos[1][0]) && timeLos[1][0] > 0.)
                    {
                        if (timeSci8[iPart] - timeLos[1][0] < 8192. * 5. / 2.)
                            timeSci8[iPart] += 8192. * 5.;
                        if (timeSci8[iPart] - timeLos[1][0] > 8192. * 5. / 2.)
                            timeSci8[iPart] -= 8192. * 5.;
                        tof = timeSci8[iPart] - timeLos[1][0];
                    }

                    // cout<<"tof "<<tof<<"  "<<timeSci8[iPart]<<"  "<<timeLos[1][0]<<endl;
                    // loop over multihits of LOS
                    for (Int_t ilos = 0; ilos < fNofLosDetectors; ilos++)
                    {
                        for (Int_t lhit = 0; lhit < nPartc[1]; lhit++)
                        {
                            if (IS_NAN(timeLos[ilos][lhit]))
                                continue;
                            if (timeSci8[iPart] - timeLos[ilos][lhit] < 8192. * 5. / 2.)
                                timeSci8[iPart] += 8192. * 5.;
                            if (timeSci8[iPart] - timeLos[ilos][lhit] > 8192. * 5. / 2.)
                                timeSci8[iPart] -= 8192. * 5.;
                            Double_t tofi = timeSci8[iPart] - timeLos[ilos][lhit];
                            // if hit is closer to expected value then take it
                            if (abs(tofi - 7852.) < abs(tof - 7852.))
                            {
                                tof = tofi;
                                lhitmem[ilos] = lhit;
                                tof_mem[ilos] = tof;
                                // cout<<"tofi "<<tof<<"  "<<timeSci8[iPart]<<"  "<<timeLos[1][0]<<endl;
                            }

                            fh_los_tot_S8[ilos]->Fill(tof_mem[ilos] - 7852.64, totsum[ilos][lhit]);
                        }

                        // cout<<"lhitmem "<<ilos<<", "<<lhitmem[ilos]<<", "<<tof_mem[ilos]<<endl;

                        if (ilos == 1)
                            fh_tof_sci8->Fill(tof - 7852.);
                    }

                    if (nPMV == 2)
                        Sci8TresM[iPart] = (timeS8_V[iPart][1] - timeS8_V[iPart][0]);
                    if (nPMT == 2)
                        Sci8TresT[iPart] = (timeS8_L[iPart][1] - timeS8_L[iPart][0]);

                    if (nPMV == 2)
                        fh_sci8_tres_MCFD->Fill(Sci8TresM[iPart]);
                    if (nPMT == 2)
                        fh_sci8_tres_TAMEX->Fill(Sci8TresT[iPart]);
                    if (nPMT == 2)
                        fh_sci8_tot_mean->Fill(totsumS8[iPart]);
                }
            }
            else
            {
                cout << "Wrong detector ID for Sci8!" << endl;
            }
        }

        for (int ik = 0; ik < 2; ik++)
        {
            fh_sci8_multihitVFTX->Fill(ik + 1, nPartS8_VFTX[ik]);
            fh_sci8_multihitLEAD->Fill(ik + 1, nPartS8_LEAD[ik]);
            fh_sci8_multihitTRAI->Fill(ik + 1, nPartS8_TRAI[ik]);
        }
    }

    //----------------------------------------------------------------------
    // Fiber detectors
    //----------------------------------------------------------------------
    Double_t dtime = 0.0 / 0.0, dtimeMax;
    Bool_t fib1a_cut = false;
    Bool_t fib1b_cut = false;
    Double_t yfib = -100000., xfib = -100000., tfibMax = 0. / 0.;
    Double_t rfib = 0.0 / 0.0;
    Bool_t LosFibCorr = false;
    Bool_t los_fib_cor[NOF_FIB_DET];
    Int_t iFibMax;
    Double_t totMax;
    Double_t tofMax;
    Int_t iCha = 0;
    Double_t tFib = 0. / 0.;

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        los_fib_cor[ifibcount] = false;
        Int_t iFib = 0;

        auto detMapped = fMappedItems.at(DET_FI_FIRST + ifibcount);
        auto detHit = fHitItems.at(DET_FI_FIRST + ifibcount);

        if (detMapped)
        {
            Int_t nHits = detMapped->GetEntriesFast();
            std::vector<UInt_t> mapmt_num(512);
            std::vector<UInt_t> spmt_num(16);
            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                R3BBunchedFiberMappedData* hit = (R3BBunchedFiberMappedData*)detMapped->At(ihit);
                if (!hit)
                    continue;

                // channel numbers are stored 1-based (1..n)
                iCha = hit->GetChannel(); // 1..

                if (hit->IsMAPMT() && hit->IsLeading() && hit->GetSide() == 0)
                {
                    fh_channels_Fib[ifibcount]->Fill(iCha); // Fill which clockTDC channel has events
                    ++mapmt_num.at(hit->GetChannel() - 1);  // multihit of a given clockTDC channel
                }

                if (!hit->IsMAPMT() && hit->IsLeading() && hit->GetSide() == 1)
                {
                    fh_channels_single_Fib[ifibcount]->Fill(iCha); // Fill which single PMT channel has events
                    ++spmt_num.at(hit->GetChannel() - 1);          // multihit of a given PADI channel
                }
            }

            for (int i = 0; i < 512; ++i)
            {
                auto m = mapmt_num.at(i);
                if (m > 0)
                    fh_multihit_m_Fib[ifibcount]->Fill(i + 1, m); // multihit of a given clockTDC channel
            }

            for (int i = 0; i < 16; ++i)
            {
                auto s = spmt_num.at(i);
                if (s > 0)
                {
                    fh_multihit_s_Fib[ifibcount]->Fill(i + 1, s);
                } // multihit of a given PADI cha
            }
        }

        if (detHit)
        {
            Int_t nHits = detHit->GetEntriesFast();
            Double_t xpos = 0. / 0.;
            Double_t ypos = 0. / 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0., tof_fib_s = 0. / 0., tof_fib_m = 0. / 0.;
            Double_t randx;
            Double_t totMax_MA = 0.;
            Double_t spmtMax;
            Double_t mapmtMax;
            Double_t posfib = 0. / 0.;
            Double_t totMax_S = 0.;
            Double_t yposMax = -10000;
            Double_t tof_fibMax = -10000;
            iFibMax = -1000;
            totMax = 0.;
            dtimeMax = 0. / 0.;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                Double_t tMAPMT = 0. / 0.;
                Double_t tSPMT = 0. / 0.;

                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)detHit->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..

                // times
                tMAPMT = hit->GetMAPMTTime_ns();
                tSPMT = hit->GetSPMTTime_ns();
                // "Push" two times in the same clock cycle:
                // MAPMT coarse counter has 4096 channels with 1000/150=6.67 ns each
                // SPMT coarse counter has 2048 channels with 5 ns each
                dtime = tMAPMT - tSPMT;

                while (dtime > 256. / 2. * 1000. / 150.)
                {
                    tMAPMT = tMAPMT - 256. * 1000. / 150.;
                    dtime = tMAPMT - tSPMT;
                }
                while (dtime < -256. / 2. * 1000. / 150.)
                {
                    tMAPMT = tMAPMT + 256. * 1000. / 150.;
                    dtime = tMAPMT - tSPMT;
                }

                // Not-calibrated ToF:
                tfib = (tMAPMT + tSPMT) / 2.;
                // tfib = tSPMT;

                // Not-calibrated position:
                if (ifibcount == 0)
                {
                    xpos = iFib;
                    ypos = dtime * 3;
                }
                if (ifibcount == 0)
                {
                    ypos = iFib;
                    xpos = dtime * 3;
                }
                // if not resonable y-position, go to next
                //  if (ypos < 70. || ypos > 100.)  continue;

                // find fiber with most light which has resonable ToF and y-position
                Double_t ToT_MA = hit->GetMAPMTToT_ns();
                Double_t ToT_SA = hit->GetSPMTToT_ns();
                Double_t ToT = hit->GetMAPMTToT_ns(); // sqrt(ToT_MA*ToT_SA);
                //  if(ypos>70. && ypos<100.){
                if (ToT > totMax)
                {
                    totMax = ToT;
                    iFibMax = iFib;
                    tfibMax = tfib;
                    dtimeMax = dtime;
                    //    }
                }

                //    if(hit->GetSPMTToT_ns() > 0){
                fh_fibers_Fib[ifibcount]->Fill(iFib);
                fh_ToT_s_Fib[ifibcount]->Fill(iFib, ToT_SA);
                fh_ToT_m_Fib[ifibcount]->Fill(iFib, ToT_MA);
                fh_time_Fib[ifibcount]->Fill(iFib, dtime);
                fh_Fib_vs_Events[ifibcount]->Fill(fNEvents, iFib);
                fh_ToT_single_Fib[ifibcount]->Fill(1, ToT_SA);

                // }

                // if not both times present, go to next
                //    if (!(tMAPMT > 0.) || !(tSPMT > 0.))  continue;
            } // end for(ihit)

            fh_ToTmax_Fibmax[ifibcount]->Fill(iFibMax, totMax);
            // cout<<"TEST: "<<fNofLosDetectors<<", "<<fNofLosDetectors-1<<endl;
            // loop over all hits of LOS1:
            for (Int_t lhit = 0; lhit < nPartc[fNofLosDetectors - 1]; lhit++)
            {
                //  if (IS_NAN(timeLos[1][lhit])) continue;

                // if not multiplicity 8 or pile-up, go to next event
                //    if (!iLOSType[1][lhit])  continue;

                // "Push" the Fib times in the same cycle with LOS:
                // Los coarse counter has 8192 channels with 5 ns each
                // SPMT coarse counter has 2048 channels with 5 ns each
                if (timeLos[fNofLosDetectors - 1][lhit] > 0. &&
                    !(IS_NAN(timeLos[fNofLosDetectors - 1][lhit]))) // timeLos[1][lhit] < 8192. * 5. &&
                {
                    while (tfibMax - timeLos[fNofLosDetectors - 1][lhit] < 2048. / 2.)
                    {
                        tfibMax = tfibMax + 2048.;
                    }
                    while (tfibMax - timeLos[fNofLosDetectors - 1][lhit] > 2048. / 2.)
                    {
                        tfibMax = tfibMax - 2048.;
                    }
                }
                tof_fib = tfibMax - timeLos[fNofLosDetectors - 1][lhit];
                fh_Fib_ToF[ifibcount]->Fill(iFibMax, tof_fib);

            } // end for (lhit)
            randx = (std::rand() / (float)RAND_MAX);

            // Not-calibrated position:
            if (ifibcount == 0)
            {
                xpos = (-n_fiber[ifibcount] / 2. + iFibMax + (0.5 - randx)) * 0.221 / 10.; // iFibMax;
                ypos = dtimeMax * 3;
            }
            if (ifibcount == 1)
            {
                ypos = (-n_fiber[ifibcount] / 2. + iFibMax + (0.5 - randx)) * 0.221 / 10.; // iFibMax;
                xpos = dtimeMax * 3;
            }
            fh_Fib_pos[ifibcount]->Fill(xpos, ypos);

            // LOS correlated to ifibMax
            //    if (tof_fibMax >  - 10. && tof_fibMax <  10.) los_fib_cor[ifibcount] = true;

            if (iFibMax > 0)
                posfib = (-n_fiber[ifibcount] / 2. + iFibMax + (0.5 - randx)) * 0.221 / 10.; // in cm

            if (ifibcount == 0)
                yfib = posfib;
            if (ifibcount == 1)
                xfib = posfib;
            if (ifibcount == 1)
                tFib = tfibMax;

            //	if(iFibMax > 0)cout<<"FIB POS: "<<fNEvents<<", "<<ifibcount<<"; "<<iFibMax<<",
            //"<<n_fiber[ifibcount]<<"; "<<posfib<<endl;

            // if(ifibcount==0 && iFibMax%32 == 0) fib1a_cut = true;
            // if(ifibcount==1 && iFibMax%32 == 0) fib1b_cut = true;

            // if(ifibcount==0 && (iFibMax == 100 || iFibMax == 200)) fib1a_cut = true;
            // if(ifibcount==1 && (iFibMax == 100 || iFibMax == 200)) fib1b_cut = true;

            // For position calibration:
            /*
                     if (ifibcount==0  && ((iFibMax >67 && iFibMax<88) || (iFibMax >119 && iFibMax<130) ||
                        (iFibMax>167 && iFibMax<188))){
                      fib1a_cut=true;
                  }
                     if (ifibcount==1  && ((iFibMax >67 && iFibMax<88) || (iFibMax >119 && iFibMax<130) ||
                        (iFibMax >167 && iFibMax<188))){
                      fib1b_cut=true;
                  }
            */
            // Netz
            /*
                               if (ifibcount==0  && (iFibMax == 32 || (iFibMax == 126) || (iFibMax == 220))){
                                fib1a_cut=true;
                            }
                               if (ifibcount==1  && (iFibMax == 32 || (iFibMax == 126) || (iFibMax == 220))){
                                fib1b_cut=true;
                            }
            */
            // Full fib1a, 1b:

            if (ifibcount == 0 && (iFibMax > 0 && iFibMax < 256))
            {
                fib1a_cut = true;
            }
            if (ifibcount == 1 && (iFibMax > 0 && iFibMax < 256))
            {
                fib1b_cut = true;
            }

            // Center of LOS detector:
            /*
                if (ifibcount==0  && (iFibMax >110 && iFibMax<146)){
                    fib1a_cut=true;
                }
                   if (ifibcount==1  && (iFibMax >110 && iFibMax<146)){
                    fib1b_cut=true;
                }
            */
            if (nHits > 0)
                fh_mult_Fib[ifibcount]->Fill(nHits);

        } // end if(aHit[ifibcount])
    }     // end for(ifibcount)

    //   fh_Fib_xy->Fill(xfib,yfib);

    // LOS1 and fib events are correlated:
    //  if (los_fib_cor[0] && los_fib_cor[1])
    LosFibCorr = true;

    rfib = sqrt(xfib * xfib + yfib * yfib);

    if ((fib1a_cut == true || fib1b_cut == true) && twin)
    {
        fh_Fib_xy->Fill(xfib, yfib);

        for (Int_t ilos = 0; ilos < fNofLosDetectors; ilos++)
        {
            for (Int_t imultlos = 0; imultlos < nPartc[ilos]; imultlos++)
            {

                Double_t dx = (xfib - xToT_cm[ilos][imultlos]);
                Double_t dy = (yfib - yToT_cm[ilos][imultlos]);

                if (iLOSType[ilos][imultlos] //&& !(iLOSPileUp[ilos][imultlos])
                    &&
                    ((tFib - timeLos[ilos][imultlos] > -660.) && (tFib - timeLos[ilos][imultlos] < -600.))) // one los
                //&& ((tFib-timeLos[ilos][imultlos] > 820.) && (tFib-timeLos[ilos][imultlos] < 870.)))  // two los
                {
                    fh_xlos_xfib[ilos]->Fill(dx, dy); // Fill(xfib,xToT_cm[ilos][imultlos]);

                    if (dx > -1 && dx < -0.4 && dy > -1.7 &&
                        dy < -0.7) // dx > -0.8 && dx < 0.6 && dy > -1.1 && dy < 0.4)
                    {
                        if (ilos > 0)
                            fh_ylos_yfib[ilos]->Fill(xToT_cm[ilos][imultlos] - xToT_cm[ilos - 1][imultlos],
                                                     yToT_cm[ilos][imultlos] - yToT_cm[ilos - 1][imultlos]);
                        fh_los_pos_ToT_cond[ilos]->Fill(xToT_cm[ilos][imultlos], yToT_cm[ilos][imultlos]);
                        fh_los_pos_MCFD_cond[ilos]->Fill(xV_cm[ilos][imultlos], yV_cm[ilos][imultlos]);
                        fh_los_tres_MCFD_cond[ilos]->Fill(LosTresV[ilos][imultlos]);
                        fh_los_tot_mean_cond[ilos]->Fill(totsum[ilos][imultlos]);
                        fh_los_pos_TAMEX[ilos]->Fill(xT_cm[ilos][imultlos], yT_cm[ilos][imultlos]);
                        fh_los_twin[ilos]->Fill(totsum[ilos][imultlos], dE_TWIN);

                        for (int ipm = 0; ipm < 8; ipm++)
                        {
                            fh_los_tot_cond[ilos]->Fill(ipm + 1, tot[ilos][imultlos][ipm]);
                        }
                    }
                }
            }
        }
    }

    if (fCalItems.at(DET_LOS) && LosFibCorr)
    {
        Int_t nevmem = 0;
        Double_t rpm = 4.75;
        Double_t lpm = 2. / 2;
        Double_t apm = 22.5 * 3.14159 / 180.;
        Double_t labs = 10.;
        Double_t anglepm[fNofLosDetectors][32][8];
        Double_t xpm[fNofLosDetectors][32][8];
        Double_t ypm[fNofLosDetectors][32][8];
        Double_t hpm[fNofLosDetectors][32][8];
        Double_t alpha[fNofLosDetectors][32][8];
        Double_t tot_corr[fNofLosDetectors][32][8];
        Double_t sum_alpha[fNofLosDetectors][32];
        Double_t totsum_corr[fNofLosDetectors][32];
        Double_t rtot[fNofLosDetectors][32];
        Double_t tot_lim[fNofLosDetectors] = { 0 };
        Double_t par0[8], par1[8], par2[8], par3[8], par4[8], par5[8];

        par0[0] = 1.61580e+02;  // 3.83810e+01;
        par1[0] = -1.17944e+00; //-1.09494e+00 ;
        par2[0] = 1.41921e+02;  // 1.28720e+02;
        par3[0] = -8.11592e+00; //-7.43137e+00;
        par4[0] = 0.;
        par5[0] = 1;

        par0[1] = 3.71160e+02;  // 1.02121e+02;
        par1[1] = -1.03041e+00; //-3.12171e+00;
        par2[1] = 1.32344e+02;  // 6.44474e+01;
        par3[1] = -9.86918e+00; //-1.87164e+01;
        par4[1] = 0.;
        par5[1] = 1.;

        par0[2] = 2.17270e+02;  // 1.24749e+02;
        par1[2] = -1.28556e+00; //-2.50991e+00;
        par2[2] = 1.51548e+02;  // 6.88595e+01;
        par3[2] = -8.29751e+00; //-1.43484e+01;
        par4[2] = 0.;
        par5[2] = 1.;

        par0[3] = 8.41271e+02;  // 6.55205e+01;
        par1[3] = -8.57171e-01; //-3.66726e+00;
        par2[3] = 1.79550e+02;  // 1.00290e+02;
        par3[3] = -7.47610e+00; //-7.36581e+00;
        par4[3] = 0.;
        par5[3] = 1.;

        par0[4] = 5.23367e+02;  // 5.30274e+01;
        par1[4] = -1.11625e+00; //-8.92102e+00;
        par2[4] = 1.58305e+02;  // 1.14336e+02;
        par3[4] = -7.60637e+00; //-4.33577e+00;
        par4[4] = 0.;
        par5[4] = 1.;

        par0[5] = 1.07023e+02;  // 5.15765e+01;
        par1[5] = -8.13576e-01; //-1.74148e+01;
        par2[5] = 2.27580e+02;  // 1.29832e+02;
        par3[5] = -5.51671e+00; //-3.40990e+00;
        par4[5] = 0.;
        par5[5] = 1.;

        par0[6] = 1.35075e+02;  // 4.20388e+01;
        par1[6] = -7.97772e+00; //-1.25505e+00;
        par2[6] = 2.10053e+02;  // 1.29497e+02;
        par3[6] = -1.13020e+00; //-6.92512e+00;
        par4[6] = 0.;
        par5[6] = 1.;

        par0[7] = 1.62558e+02;  // 2.02182e+01;
        par1[7] = -1.70696e+00; //-1.11835e+00;
        par2[7] = 1.22895e+02;  // 1.42994e+02;
        par3[7] = -1.10376e+01; //-6.83987e+00;
        par4[7] = 0.;
        par5[7] = 1.;

        for (Int_t i = 0; i < fNofLosDetectors; i++)
        {
            for (Int_t k = 0; k < 32; k++)
            {
                sum_alpha[i][k] = 0.;
                totsum_corr[i][k] = 0.;
                rtot[i][k] = 0. / 0.;
            }
        }
        Double_t rimpact = 0. / 0.;

        rimpact = sqrt(xfib * xfib + yfib * yfib);

        for (Int_t idet = 0; idet < fNofLosDetectors; idet++)
        {
            if (nPartc[idet] == 1)
            {
                for (Int_t imul = 0; imul < nPartc[idet]; imul++)
                {
                    if (iLOSType[idet][imul])
                    {
                        rtot[idet][imul] =
                            sqrt(xToT_cm[idet][imul] * xToT_cm[idet][imul] + yToT_cm[idet][imul] * yToT_cm[idet][imul]);
                        for (Int_t ipm = 0; ipm < 8; ipm++)
                        {
                            if (tot[idet][imul][ipm] > 0. && !(IS_NAN(tot[idet][imul][ipm])) && !(IS_NAN(xfib)) &&
                                !(IS_NAN(yfib)))
                            {
                                anglepm[idet][imul][ipm] = apm + 2. * (7. - ipm) * apm;
                                xpm[idet][imul][ipm] = rpm * sin(anglepm[idet][imul][ipm]);
                                ypm[idet][imul][ipm] = rpm * cos(anglepm[idet][imul][ipm]);
                                hpm[idet][imul][ipm] =
                                    sqrt((xpm[idet][imul][ipm] - xfib) * (xpm[idet][imul][ipm] - xfib) +
                                         (ypm[idet][imul][ipm] - yfib) * (ypm[idet][imul][ipm] - yfib));
                                // alpha[idet][imul][ipm] = 2. * atan(lpm/hpm[idet][imul][ipm]);
                                alpha[idet][imul][ipm] =
                                    2. * lpm * (1. - lpm / hpm[idet][imul][ipm]) / (hpm[idet][imul][ipm] - lpm);
                                sum_alpha[idet][imul] = sum_alpha[idet][imul] + alpha[idet][imul][ipm];
                                // if(idet == 0) tot_corr[idet][imul][ipm] =
                                // tot[idet][imul][ipm]/alpha[idet][imul][ipm]*exp(-hpm[idet][imul][ipm]/10.);
                                //  if(idet == 1){

                                tot_corr[idet][imul][ipm] = 100. * tot[idet][imul][ipm] /
                                                            (par0[ipm] * exp(hpm[idet][imul][ipm] / par1[ipm]) +
                                                             par2[ipm] * exp(hpm[idet][imul][ipm] / par3[ipm]) +
                                                             par4[ipm] * exp(hpm[idet][imul][ipm] / par5[ipm]));
                                if (hpm[idet][imul][ipm] < 2.2 || hpm[idet][imul][ipm] > 6.5)
                                    tot_corr[idet][imul][ipm] = 0. / 0.;

                                if (idet == 0)
                                    // fh_test_corr[ipm]->Fill(hpm[idet][imul][ipm], tot_corr[idet][imul][ipm]);
                                    // }

                                    // tot_corr[idet][imul][ipm] =
                                    // tot[idet][imul][ipm]/TMath::Power(alpha[idet][imul][ipm],0.75);

                                    totsum_corr[idet][imul] += tot_corr[idet][imul][ipm];
                                nevmem = fNEvents;

                                // cout<<idet<<", "<<ipm<<", "<<hpm[idet][ipm]<<endl;

                                if (idet == 0)
                                {
                                    //   fh_test[ipm]->Fill(hpm[idet][imul][ipm], tot[idet][imul][ipm]);
                                }
                            }
                        }

                        /*
                         totsum_corr[idet][imul] = (sqrt(tot[idet][imul][0]*tot[idet][imul][4])+
                                                    sqrt(tot[idet][imul][1]*tot[idet][imul][5])+
                                                    sqrt(tot[idet][imul][2]*tot[idet][imul][6])+
                                                    sqrt(tot[idet][imul][3]*tot[idet][imul][7]))/4.;
                        */
                        totsum_corr[idet][imul] = totsum_corr[idet][imul] / 8.;
                        sum_alpha[idet][imul] = sum_alpha[idet][imul] * 180. / 3.14159;

                        if (idet == 1)
                            tot_lim[idet] = -3.e-6 * dE_TWIN * dE_TWIN + 0.03 * dE_TWIN + 25.172;
                        if (idet == 0)
                            tot_lim[idet] = -8e-7 * dE_TWIN * dE_TWIN + 0.0169 * dE_TWIN + 27.399;

                        if (rfib < 3. && totsum[idet][imul] > 0. && twin)
                        {
                            fh_los_tot_mean_corr[idet]->Fill(totsum_corr[idet][imul]);
                            fh_los_tot_mean[idet]->Fill(totsum[idet][imul]);
                            fh_los_rw[idet]->Fill(sum_alpha[idet][imul]);
                        }

                        // Double_t tot_test = totsum_corr[idet][imul] * 360./sum_alpha[idet][imul];

                        // if(fNEvents == nevmem) cout<<"Output "<<totsum_corr[idet]<<", "<<totsum[idet][0]<<",
                        // "<<sum_alpha[idet]<<endl;
                    }
                }
            }
        }
    }

    //    myFile.close();

    fNEvents += 1;
}

void R3BOnlineSpectraLosEngRun2019::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {
        if (fMappedItems.at(det))
        {
            fMappedItems.at(det)->Clear();
        }
        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
        if (fHitItems.at(det))
        {
            fHitItems.at(det)->Clear();
        }
    }
}

void R3BOnlineSpectraLosEngRun2019::FinishTask()
{

    if (fMappedItems.at(DET_LOS))
    {
        if (fNofLosDetectors > 1)
        {
            fh_los_ToT_vs_ToT->Write();
            fh_los_nPart_vs_nPart->Write();
            fh_dt_los1_los2->Write();
        }
        for (Int_t iloscount = 0; iloscount < fNofLosDetectors; iloscount++)
        {
            fh_los_channels[iloscount]->Write();
            fh_los_tot[iloscount]->Write();
            fh_los_tot_cond[iloscount]->Write();
            fh_los_dt_hits_ToT[iloscount]->Write();
            fh_los_ihit_ToT[iloscount]->Write();
            fh_los_twin[iloscount]->Write();
            fh_los_tot_mean_corr[iloscount]->Write();
            fh_los_tot_mean_cond[iloscount]->Write();
            fh_los_tot_mean[iloscount]->Write();
            fh_los_rw[iloscount]->Write();
            fh_los_tres_MCFD[iloscount]->Write();
            fh_los_tres_MCFD_cond[iloscount]->Write();
            fh_los_tres_TAMEX[iloscount]->Write();
            fh_los_pos_MCFD[iloscount]->Write();
            fh_los_pos_MCFD_cond[iloscount]->Write();
            fh_los_pos_TAMEX[iloscount]->Write();
            fh_los_pos_ToT[iloscount]->Write();
            fh_los_pos_ToT_cond[iloscount]->Write();
            fh_los_tot_S8[iloscount]->Write();
            fh_los_vftx_tamex[iloscount]->Write();
            // fh_los_tres_MTDC->Write();
            fh_xlos_xfib[iloscount]->Write();
            fh_ylos_yfib[iloscount]->Write();
        }
        /*   for (Int_t ipm = 0; ipm < 8; ipm++)
           {
               fh_test[ipm]->Write();
               fh_test_corr[ipm]->Write();
           }*/
    }

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fMappedItems.at(ifibcount + DET_FI_FIRST))
        {
            fh_channels_Fib[ifibcount]->Write();
            fh_fibers_Fib[ifibcount]->Write();
            fh_mult_Fib[ifibcount]->Write();
            fh_time_Fib[ifibcount]->Write();
            fh_multihit_m_Fib[ifibcount]->Write();
            fh_multihit_s_Fib[ifibcount]->Write();
            fh_ToT_m_Fib[ifibcount]->Write();
            fh_ToT_s_Fib[ifibcount]->Write();
            fh_ToT_single_Fib[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_Fib_ToF[ifibcount]->Write();
            fh_channels_single_Fib[ifibcount]->Write();
            fh_Fib_pos[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_ToTmax_Fibmax[ifibcount]->Write();
        }
    }
    fh_Fib_xy->Write();
}

ClassImp(R3BOnlineSpectraLosEngRun2019)
