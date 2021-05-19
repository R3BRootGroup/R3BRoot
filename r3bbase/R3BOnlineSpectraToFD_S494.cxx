// ------------------------------------------------------------
// -----                  R3BOnlineSpectraToFD_S494                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraToFD_S494.h"

#include "R3BTofdCalData.h"
#include "R3BTofdMappedData.h"

#include "R3BPaddleCalData.h"
#include "R3BPaddleTamexMappedData.h"

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"

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

R3BOnlineSpectraToFD_S494::R3BOnlineSpectraToFD_S494()
    : R3BOnlineSpectraToFD_S494("OnlineSpectra", 1)
{
}

R3BOnlineSpectraToFD_S494::R3BOnlineSpectraToFD_S494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fNofPlanes(N_PLANE_MAX_TOFD)
    , fPaddlesPerPlane(N_PADDLE_MAX_TOFD)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraToFD_S494::~R3BOnlineSpectraToFD_S494() {}

InitStatus R3BOnlineSpectraToFD_S494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraToFD_S494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels
    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    for (int det = 0; det < DET_MAX; det++)
    {
        fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
        if (NULL == fMappedItems.at(det))
        {
            printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        }
        fCalItems.push_back((TClonesArray*)mgr->GetObject(Form("%sCal", fDetectorNames[det])));
        fHitItems.push_back((TClonesArray*)mgr->GetObject(Form("%sHit", fDetectorNames[det])));
    }

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
    // TofD detector

    if (fMappedItems.at(DET_TOFD))
    {
        TCanvas* cTofd_planes = new TCanvas("TOFD_planes", "TOFD planes", 10, 10, 1100, 1000);
        cTofd_planes->Divide(5, 4);

        fh_TimePreviousEvent = new TH1F("TimePreviousEvent", "Time between 2 particles ", 300000, -3000, 3000);
        fh_TimePreviousEvent->GetXaxis()->SetTitle("time / µsec");
        fh_TimePreviousEvent->GetYaxis()->SetTitle("counts");

        for (Int_t j = 0; j < 4; j++)
        {
            char strName1[255];
            sprintf(strName1, "tofd_channels_plane_%d", j + 1);
            char strName2[255];
            sprintf(strName2, "Tofd channels plane %d", j + 1);
            fh_tofd_channels[j] = new TH1F(strName1, strName2, 90, -45., 45.);
            fh_tofd_channels[j]->GetXaxis()->SetTitle("Channel");
            fh_tofd_channels[j]->GetYaxis()->SetTitle("Counts");

            char strName3[255];
            sprintf(strName3, "tofd_ToT_plane_%d", j + 1);
            char strName4[255];
            sprintf(strName4, "Tofd ToT plane %d", j + 1);
            fh_tofd_TotPm[j] = new TH2F(strName3, strName4, 90, -45, 45, 3000, 0., 300.);
            fh_tofd_TotPm[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_TotPm[j]->GetYaxis()->SetTitle("ToT / ns");

            char strName9[255];
            sprintf(strName9, "tofd_multihit_plane_%d", j + 1);
            char strName10[255];
            sprintf(strName10, "Tofd multihit plane %d", j + 1);
            fh_tofd_multihit[j] = new TH2F(strName9, strName10, 45, 0., 45., 10, 0, 10);
            fh_tofd_multihit[j]->GetXaxis()->SetTitle("Bar number");
            fh_tofd_multihit[j]->GetYaxis()->SetTitle("Multihit");

            if (j < 3)
            {
                int jk = j + 1;
                char strName5[255];
                sprintf(strName5, "tofd_ToF_plane_%d_%d", jk, jk + 1);
                char strName6[255];
                sprintf(strName6, "Tofd ToF plane %d and %d ", jk, jk + 1);
                fh_tofd_dt[j] = new TH2F(strName5, strName6, 50, 0, 50, 800, -40., 40);
                fh_tofd_dt[j]->GetXaxis()->SetTitle("Bar number");
                fh_tofd_dt[j]->GetYaxis()->SetTitle("dt / ns");
            }
        }

        cTofd_planes->cd(1);
        fh_tofd_channels[0]->Draw();
        cTofd_planes->cd(2);
        gPad->SetLogz();
        fh_tofd_TotPm[0]->Draw("colz");
        cTofd_planes->cd(3);
        gPad->SetLogz();
        fh_tofd_multihit[0]->Draw("colz");
        cTofd_planes->cd(4);
        gPad->SetLogy();
        fh_TimePreviousEvent->Draw("hist");

        cTofd_planes->cd(6);
        fh_tofd_channels[1]->Draw();
        cTofd_planes->cd(7);
        gPad->SetLogz();
        fh_tofd_TotPm[1]->Draw("colz");
        cTofd_planes->cd(8);
        gPad->SetLogz();
        fh_tofd_multihit[1]->Draw("colz");
        cTofd_planes->cd(9);
        gPad->SetLogz();
        fh_tofd_dt[0]->Draw("colz");

        cTofd_planes->cd(11);
        fh_tofd_channels[2]->Draw();
        cTofd_planes->cd(12);
        gPad->SetLogz();
        fh_tofd_TotPm[2]->Draw("colz");
        cTofd_planes->cd(13);
        gPad->SetLogz();
        fh_tofd_multihit[2]->Draw("colz");
        cTofd_planes->cd(14);
        gPad->SetLogz();
        fh_tofd_dt[1]->Draw("colz");

        cTofd_planes->cd(16);
        fh_tofd_channels[3]->Draw();
        cTofd_planes->cd(17);
        gPad->SetLogz();
        fh_tofd_TotPm[3]->Draw("colz");
        cTofd_planes->cd(18);
        gPad->SetLogz();
        fh_tofd_multihit[3]->Draw("colz");
        cTofd_planes->cd(19);
        gPad->SetLogz();
        fh_tofd_dt[2]->Draw("colz");

        cTofd_planes->cd(0);
        run->AddObject(cTofd_planes);

        run->GetHttpServer()->RegisterCommand("Reset_TOFD", Form("/Tasks/%s/->Reset_TOFD_Histo()", GetName()));
    }

    // -------------------------------------------------------------------------

    return kSUCCESS;
}

void R3BOnlineSpectraToFD_S494::Reset_TOFD_Histo()
{
    for (int i = 0; i < N_PLANE_MAX_TOFD; i++)
    {
        fh_tofd_channels[i]->Reset();
        fh_tofd_multihit[i]->Reset();
        fh_tofd_ToF[i]->Reset();
        fh_tofd_TotPm[i]->Reset();
    }
    fh_tofd_dt[0]->Reset();
    fh_tofd_dt[1]->Reset();
    fh_tofd_dt[2]->Reset();
}

void R3BOnlineSpectraToFD_S494::Exec(Option_t* option)
{
    //  cout << "fNEvents " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(ERROR) << "FairRootManager not found";
        return;
    }

    time = header->GetTimeStamp();

    if (time_start == 0 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }
    Bool_t spectra_clear = false;
    Double_t xtime = double(time - time_start) / 1.e9;
    // for reseting spectra
    Int_t icount = iclear_count * reset_time; // reset after reset_time (sec)
    if (time_clear < 0. && int(xtime) % icount == 0 && xtime > 1.)
    {
        time_clear = xtime;
        spectra_clear = true;
    }

    if (header->GetTrigger() == 12)
        time_spill_start = header->GetTimeStamp(); // spill start in nsec
    if (header->GetTrigger() == 13)
        time_spill_end = header->GetTimeStamp(); // spill end  in nsec

    if (header->GetTrigger() == 12)
        cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " sec" << endl;
    if (header->GetTrigger() == 13)
        cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec" << endl;

    //   check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    // fTpat = 1-16; fTpat_bit = 0-15
    Int_t fTpat_bit = fTpat - 1;
    Int_t itpat;
    Int_t tpatvalue;
    if (fTpat_bit >= 0)
    {
        itpat = header->GetTpat();
        tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
        if ((tpatvalue == 0))
            return;
    }

    //----------------------------------------------------------------------
    // TOFD
    //----------------------------------------------------------------------

    Int_t NumPaddles[4] = { 0 };
    if (fMappedItems.at(DET_TOFD))
    {
        auto det = fMappedItems.at(DET_TOFD);
        Int_t nMapped = det->GetEntriesFast();
        Int_t iPlaneMem = 1, iBarMem = 0;
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BTofdMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen

            Int_t const iPlane = mapped->GetDetectorId(); // 1..n
            Int_t const iBar = mapped->GetBarId();        // 1..n
            Int_t const iSide = mapped->GetSideId();      // 1..n
            Int_t const iEdge = mapped->GetEdgeId();

            if (iPlaneMem != iPlane)
                iBarMem = 0;
            if (iPlane == 1 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[0] += 1;
            if (iPlane == 2 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[1] += 1;
            if (iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[2] += 1;
            if (iPlane == 3 && iBarMem != iBar && iSide == 1 && iEdge == 1)
                NumPaddles[3] += 1;

            //  cout<<imapped<<", "<<iPlane<<"; "<<iPlaneMem<<"; "<<iBar<<"; "<<iBarMem<<",
            //  "<<NumPaddles[iPlane-1]<<endl;

            iPlaneMem = iPlane;
            iBarMem = iBar;

            if (iPlane <= fNofPlanes)
            {
                if (iSide == 1)
                    fh_tofd_channels[iPlane - 1]->Fill(-iBar - 1);
                if (iSide == 2)
                    fh_tofd_channels[iPlane - 1]->Fill(iBar);
            }
        }
    }

    if (fCalItems.at(DET_TOFD))
    {
        auto det = fCalItems.at(DET_TOFD);
        Int_t nCals = det->GetEntriesFast();

        Double_t tot1[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t tot2[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t_paddle[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t1l[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t2l[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t1t[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t t2t[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };
        Double_t ToF[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0. / 0. };

        Bool_t Bar_present[10][N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { false };

        Int_t iBarMem = 0;
        Int_t jmult[N_PLANE_MAX_TOFD][N_PADDLE_MAX_TOFD] = { 0 };

        unsigned long long time0 = header->GetTimeStamp();
        double_t time1 = -1.;

        for (Int_t ical = 0; ical < nCals; ical++)
        {
            auto cal = (R3BTofdCalData const*)det->At(ical);
            if (!cal)
                continue; // should not happen

            Int_t const iPlane = cal->GetDetectorId(); // 1..n
            Int_t const iBar = cal->GetBarId();        // 1..n

            //      std::cout << iPlane << ' ' << iBar <<
            //          ',' << cal->GetTimeBL_ns() <<
            //          ' ' << cal->GetTimeBT_ns() <<
            //          ' ' << cal->GetTimeTL_ns() <<
            //          ' ' << cal->GetTimeTT_ns() << std::endl;

            // get all times of one bar
            /*
                         cout<<"TOFD Online: "<<fNEvents<<", "<<nCals<<"; "<<ical<<"; "<<iPlane<<", "<<iBar<<",
               "<<iBarMem<<", "<<", "<< cal->fTime1L_ns<<", "<<cal->fTime1T_ns<<", "<< cal->fTime2L_ns<<",
               "<<cal->fTime2T_ns<<endl;
            */
            /*
                  if(iBar != iBarMem )
                  {
                jmult[iPlane-1][iBar-1] = 0;
                if (!IS_NAN(cal->GetTimeBL_ns())) t1l[0][iPlane-1][iBar-1] = cal->GetTimeBL_ns();
                if (!IS_NAN(cal->GetTimeBT_ns())) t1t[0][iPlane-1][iBar-1] = cal->GetTimeBT_ns();
                if (!IS_NAN(cal->GetTimeTL_ns())) t2l[0][iPlane-1][iBar-1] = cal->GetTimeTL_ns();
                if (!IS_NAN(cal->GetTimeTT_ns())) t2t[0][iPlane-1][iBar-1] = cal->GetTimeTT_ns();
                Bar_present[0][iPlane-1][iBar-1] = true;
                  }
                  else
                  {
                jmult[iPlane-1][iBar-1] = jmult[iPlane-1][iBar-1] + 1;
                Int_t jm = jmult[iPlane-1][iBar-1];
                if (!IS_NAN(cal->GetTimeBL_ns())) t1l[jm][iPlane-1][iBar-1] = cal->GetTimeBL_ns();
                if (!IS_NAN(cal->GetTimeBT_ns())) t1t[jm][iPlane-1][iBar-1] = cal->GetTimeBT_ns();
                if (!IS_NAN(cal->GetTimeTL_ns())) t2l[jm][iPlane-1][iBar-1] = cal->GetTimeTL_ns();
                if (!IS_NAN(cal->GetTimeTT_ns())) t2t[jm][iPlane-1][iBar-1] = cal->GetTimeTT_ns();
                Bar_present[jm][iPlane-1][iBar-1] = true;
                  }
                  iBarMem = iBar;
            */

            Int_t jm = jmult[iPlane - 1][iBar - 1];
            /*		if (!IS_NAN(cal->GetTimeBL_ns())) t1l[jm][iPlane-1][iBar-1] = cal->GetTimeBL_ns();
                    if (!IS_NAN(cal->GetTimeBT_ns())) t1t[jm][iPlane-1][iBar-1] = cal->GetTimeBT_ns();
                    if (!IS_NAN(cal->GetTimeTL_ns())) t2l[jm][iPlane-1][iBar-1] = cal->GetTimeTL_ns();
                    if (!IS_NAN(cal->GetTimeTT_ns())) t2t[jm][iPlane-1][iBar-1] = cal->GetTimeTT_ns();*/
            Bar_present[jm][iPlane - 1][iBar - 1] = true;
            jmult[iPlane - 1][iBar - 1] = jmult[iPlane - 1][iBar - 1] + 1;

            double_t time2 = 0.;
            if (time1 < 0 && iPlane == 2)
                time1 = (t1l[jm][iPlane - 1][iBar - 1] + t2l[jm][iPlane - 1][iBar - 1]) / 2.;
            if (iPlane == 2)
                time2 = (t1l[jm][iPlane - 1][iBar - 1] + t2l[jm][iPlane - 1][iBar - 1]) / 2.;
            if (time1 > 0. && time2 > 0. && time2 > time1)
            {
                // cout<<"Time Test "<<time0<<"  "<<time1<< "   "<< time2 <<"  " <<time_previous_event <<endl;
                fh_TimePreviousEvent->Fill(time2 - time1);
                time2 = time1;
            }
        }

        /*
                for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
                for(Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++)
                {
                fh_tofd_multihit[ipl]->Fill(ibr+1,jmult[ipl][ibr]+1);
                }
         */
        for (Int_t ipl = 0; ipl < N_PLANE_MAX_TOFD; ipl++)
            for (Int_t ibr = 0; ibr < N_PADDLE_MAX_TOFD; ibr++)
                for (Int_t jm = 0; jm < jmult[ipl][ibr]; jm++)
                {

                    if (Bar_present[jm][ipl][ibr])
                    {
                        Int_t iPlane = ipl + 1; // 1..n
                        Int_t iBar = ibr + 1;   // 1..n

                        fh_tofd_multihit[ipl]->Fill(ibr + 1, jmult[ipl][ibr]);

                        // calculate time over threshold and check if clock counter went out of range

                        while (t1t[jm][iPlane - 1][iBar - 1] < t1l[jm][iPlane - 1][iBar - 1])
                        {
                            t1t[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                        }

                        while (t2t[jm][iPlane - 1][iBar - 1] < t2l[jm][iPlane - 1][iBar - 1])
                        {
                            t2t[jm][iPlane - 1][iBar - 1] += 2048. * fClockFreq;
                        }

                        // ToF
                        t_paddle[jm][iPlane - 1][iBar - 1] =
                            (t1l[jm][iPlane - 1][iBar - 1] + t2l[jm][iPlane - 1][iBar - 1]) / 2.;

                        //  between 2 bars in 2 planes
                        if (ipl > 0)
                        {
                            fh_tofd_dt[ipl - 1]->Fill(iBar,
                                                      t_paddle[jm][ipl][iBar - 1] - t_paddle[jm][ipl - 1][iBar - 1]);
                        }

                        // ToT
                        tot1[jm][iPlane - 1][iBar - 1] = t1t[jm][iPlane - 1][iBar - 1] - t1l[jm][iPlane - 1][iBar - 1];
                        if (tot1[jm][iPlane - 1][iBar - 1] < 0)
                        {
                            cout << "Negative ToT " << tot1[jm][iPlane - 1] << ", for hit= " << jm << endl;
                            cout << "Times1: " << t1t[jm][iPlane - 1][iBar - 1] << " " << t1l[jm][iPlane - 1][iBar - 1]
                                 << endl;
                        }

                        tot2[jm][iPlane - 1][iBar - 1] = t2t[jm][iPlane - 1][iBar - 1] - t2l[jm][iPlane - 1][iBar - 1];
                        if (tot2[jm][iPlane - 1][iBar - 1] < 0)
                        {
                            cout << "Negative ToT " << tot2[jm][iPlane - 1][iBar - 1] << ", for hit= " << jm << endl;
                            cout << "Times2: " << t2t[jm][iPlane - 1][iBar - 1] << " " << t2l[jm][iPlane - 1][iBar - 1]
                                 << endl;
                        }

                        fh_tofd_TotPm[iPlane - 1]->Fill(iBar, tot2[jm][iPlane - 1][iBar - 1]);
                        fh_tofd_TotPm[iPlane - 1]->Fill(-iBar - 1, tot1[jm][iPlane - 1][iBar - 1]);
                    }
                }
    }

    fNEvents += 1;
}

void R3BOnlineSpectraToFD_S494::FinishEvent()
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

void R3BOnlineSpectraToFD_S494::FinishTask()
{
    if (fCalItems.at(DET_TOFD))
    {
        fh_TimePreviousEvent->Write();

        for (Int_t i = 0; i < 4; i++)
        {
            fh_tofd_TotPm[i]->Write();
            fh_tofd_channels[i]->Write();
        }
        for (Int_t i = 0; i < 3; i++)
        {
            fh_tofd_dt[i]->Write();
        }
    }
}

ClassImp(R3BOnlineSpectraToFD_S494)
