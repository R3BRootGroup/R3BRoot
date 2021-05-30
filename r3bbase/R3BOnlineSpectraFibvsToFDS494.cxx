// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraFibvsToFDS494.h"

#include "R3BTofdCalData.h"
#include "R3BTofdHitData.h"
#include "R3BTofdMappedData.h"

#include "R3BTofiCalData.h"
#include "R3BTofiHitData.h"
#include "R3BTofiMappedData.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

#include "R3BEventHeader.h"
#include "R3BSamplerMappedData.h"
#include "R3BTCalEngine.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRtdbRun.h"
#include "FairRunAna.h"
#include "FairRunIdGenerator.h"
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
#include <deque>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <vector>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

R3BOnlineSpectraFibvsToFDS494::R3BOnlineSpectraFibvsToFDS494()
    : FairTask("OnlineSpectraFibvsToFDS494", 1)
    , fTrigger(-1)
    , fTpat(-1)
    , fCuts(0)
    , fwindow_mv(10000)
    , fxmin(-1000)
    , fxmax(1000)
    , fNEvents(0)

{
}

R3BOnlineSpectraFibvsToFDS494::R3BOnlineSpectraFibvsToFDS494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fCuts(0)
    , fwindow_mv(10000)
    , fxmin(-1000)
    , fxmax(1000)
    , fNEvents(0)
{
}

R3BOnlineSpectraFibvsToFDS494::~R3BOnlineSpectraFibvsToFDS494()
{
    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        delete fh_ToT_Fib[i];
        delete fh_xy_Fib[i];
        delete fh_xy_Fib_ac[i];
        delete fh_Fib_ToF[i];
        delete fh_Fib_ToF_ac[i];
        delete fh_ToT_Fib[i];
        delete fh_ToT_Fib_ac[i];
        delete fh_Fibs_vs_Tofd[i];
        delete fh_Fibs_vs_Tofd_ac[i];
        delete fh_ToF_vs_Events[i];
    }

    delete fh_counter_fi30;
    delete fh_counter_fi31;
    delete fh_counter_fi32;
    delete fh_counter_fi33;
}

InitStatus R3BOnlineSpectraFibvsToFDS494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraFibvsToFDS494::Init ";

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
    cout << " I HAVE FOUND " << DET_MAX + 1 << " DETECTORS" << endl;
    printf("Have %d fiber detectors.\n", NOF_FIB_DET);
    for (int det = 0; det < DET_MAX; det++)
    {
        /*
                fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));
                if (NULL == fMappedItems.at(det))
                {
                    printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
                }
        */
        if (det == 7)
            maxevent = mgr->CheckMaxEventNo();
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

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    UInt_t Nmax = 1e7;
    TCanvas* cFib = new TCanvas("Fib", "Fib", 10, 10, 910, 910);
    cFib->Divide(7, 4);

    // ToF Tofd -> Fiber:
    fh_Tofi_ToF = new TH1F("Tofi_tof", "ToF Tofd to Tofi", 10000, -1100., 1100.);
    fh_Tofi_ToF->GetXaxis()->SetTitle("ToF / ns");
    fh_Tofi_ToF->GetYaxis()->SetTitle("counts");

    fh_Tofi_ToF_ac = new TH1F("Tofi_tof_ac", "ToF Tofd to Tofi after cuts", 10000, -1000., 1000.);
    fh_Tofi_ToF_ac->GetXaxis()->SetTitle("ToF / ns");
    fh_Tofi_ToF_ac->GetYaxis()->SetTitle("counts");

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        // cout<<"In the loop for ifibcout= "<<ifibcount<<endl;

        const char* detName;
        const char* detName2;
        detName = fDetectorNames[DET_FI_FIRST + ifibcount];

        // xy:
        fh_xy_Fib[ifibcount] = new TH2F(
            Form("%s_xToFDtotFib", detName), Form("%s ToTFib vs xToFD", detName), 1200, -60., 60., 400, 0., 100.);
        fh_xy_Fib[ifibcount]->GetXaxis()->SetTitle("xToFD / cm ");
        fh_xy_Fib[ifibcount]->GetYaxis()->SetTitle("ToT Fib / ns");

        fh_xy_Fib_ac[ifibcount] = new TH2F(Form("%s_xToFDtotFib_ac", detName),
                                           Form("%s ToTFib vs xToFD after cuts", detName),
                                           1200,
                                           -60.,
                                           60.,
                                           400,
                                           0.,
                                           100.);
        fh_xy_Fib_ac[ifibcount]->GetXaxis()->SetTitle("xToFD / cm ");
        fh_xy_Fib_ac[ifibcount]->GetYaxis()->SetTitle("ToT Fib / ns");

        // ToT vs xToFD:
        fh_ToT_Fib[ifibcount] = new TH2F(
            Form("%s_totFib_vs_totToFD", detName), Form("%s ToTFib vs totToFD", detName), 800, 0., 200., 400, 0., 100.);
        fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("ToFD ToT / ns");
        fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("Fib ToT / ns");

        fh_ToT_Fib_ac[ifibcount] = new TH2F(Form("%s_totFib_vs_totToFDD_ac", detName),
                                            Form("%s oTFib vs totToFD after cuts", detName),
                                            800,
                                            0.,
                                            200,
                                            400,
                                            0.,
                                            100.);
        fh_ToT_Fib_ac[ifibcount]->GetXaxis()->SetTitle("ToFD ToT / ns");
        fh_ToT_Fib_ac[ifibcount]->GetYaxis()->SetTitle("Fib ToT / ns");

        // ToF Tofd -> Fiber:
        fh_Fib_ToF[ifibcount] =
            new TH1F(Form("%s_tof", detName), Form("%s ToF Tofd to Fiber", detName), 10000, -1100., 1100.);
        fh_Fib_ToF[ifibcount]->GetXaxis()->SetTitle("ToF / ns");
        fh_Fib_ToF[ifibcount]->GetYaxis()->SetTitle("counts");

        fh_Fib_ToF_ac[ifibcount] = new TH1F(
            Form("%s_tof_ac", detName), Form("%s ToF Tofd to Fiber after cuts", detName), 10000, -1000., 1000.);
        fh_Fib_ToF_ac[ifibcount]->GetXaxis()->SetTitle("ToF / ns");
        fh_Fib_ToF_ac[ifibcount]->GetYaxis()->SetTitle("counts");

        // ToF Tofd -> Fiber vs. event number:
        fh_ToF_vs_Events[ifibcount] = new TH2F(Form("%s_tof_vs_events", detName),
                                               Form("%s ToF Tofd to Fiber vs event number", detName),
                                               10000,
                                               0,
                                               Nmax,
                                               2200,
                                               -5100,
                                               5100);
        fh_ToF_vs_Events[ifibcount]->GetYaxis()->SetTitle("ToF / ns");
        fh_ToF_vs_Events[ifibcount]->GetXaxis()->SetTitle("event number");

        // xfib vs. TofD position:
        fh_Fibs_vs_Tofd[ifibcount] = new TH2F(
            Form("%s_fib_vs_TofdX", detName), Form("%s Fiber # vs. Tofd x-pos", detName), 400, -60, 60, 600, -30., 30.);
        fh_Fibs_vs_Tofd[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fibs_vs_Tofd[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

        fh_Fibs_vs_Tofd_ac[ifibcount] = new TH2F(Form("%s_fib_vs_TofdX_ac", detName),
                                                 Form("%s Fiber # vs. Tofd x-pos after cuts", detName),
                                                 400,
                                                 -60,
                                                 60,
                                                 600,
                                                 -30.,
                                                 30.);
        fh_Fibs_vs_Tofd_ac[ifibcount]->GetYaxis()->SetTitle("Fiber x / cm");
        fh_Fibs_vs_Tofd_ac[ifibcount]->GetXaxis()->SetTitle("Tofd x / cm");

    } // end for(ifibcount)

    fh_counter_fi30 = new TH2F("Counts_Fi30_vs_ToFD", "Fib30 efficiency", 520, 0, 520, 105, 0., 105.);
    fh_counter_fi30->GetXaxis()->SetTitle("Fiber number");
    fh_counter_fi30->GetYaxis()->SetTitle("Efficiency / % ");
    fh_counter_fi31 = new TH2F("Counts_Fi31_vs_ToFD", "Fib31 efficiency", 520, 0, 520, 105, 0., 105.);
    fh_counter_fi31->GetXaxis()->SetTitle("Fiber number");
    fh_counter_fi31->GetYaxis()->SetTitle("Efficiency / % ");
    fh_counter_fi32 = new TH2F("Counts_Fi32_vs_ToFD", "Fib32 efficiency", 520, 0, 520, 105, 0., 105.);
    fh_counter_fi32->GetXaxis()->SetTitle("Fiber number");
    fh_counter_fi32->GetYaxis()->SetTitle("Efficiency / % ");
    fh_counter_fi33 = new TH2F("Counts_Fi33_vs_ToFD", "Fib33 efficiency", 520, 0, 520, 105, 0., 105.);
    fh_counter_fi33->GetXaxis()->SetTitle("Fiber number");
    fh_counter_fi33->GetYaxis()->SetTitle("Efficiency / % ");

    fh_test = new TH2F("Fi31_fibMax_vs_fNevent", "Fi31 fibMax vsfNevent", 10000, 0, 5e6, 520, 0., 526);
    fh_test1 = new TH2F("Fi31_fibavr_vs_fNevent", "Fi31 fibavr vsfNevent", 10000, 0, 5e6, 520, 0., 526);
    cFib->cd(1);
    // gPad->SetLogy();
    fh_Fib_ToF[0]->Draw();
    cFib->cd(8);
    gPad->SetLogz();
    fh_ToT_Fib[0]->Draw("colz");
    cFib->cd(15);
    gPad->SetLogz();
    fh_Fibs_vs_Tofd[0]->Draw("colz");

    cFib->cd(2);
    // gPad->SetLogy();
    fh_Fib_ToF[1]->Draw();
    cFib->cd(9);
    gPad->SetLogz();
    fh_ToT_Fib[1]->Draw("colz");
    cFib->cd(16);
    gPad->SetLogz();
    fh_Fibs_vs_Tofd[1]->Draw("colz");

    cFib->cd(3);
    //  gPad->SetLogy();
    fh_Fib_ToF[2]->Draw();
    cFib->cd(10);
    gPad->SetLogz();
    fh_ToT_Fib[2]->Draw("colz");
    cFib->cd(17);
    gPad->SetLogz();
    fh_Fibs_vs_Tofd[2]->Draw("colz");
    cFib->cd(24);
    fh_counter_fi30->Draw("colz");

    cFib->cd(4);
    // gPad->SetLogy();
    fh_Fib_ToF[3]->Draw();
    cFib->cd(11);
    gPad->SetLogz();
    fh_ToT_Fib[3]->Draw("colz");
    cFib->cd(18);
    gPad->SetLogz();
    fh_Fibs_vs_Tofd[3]->Draw("colz");
    cFib->cd(25);
    fh_counter_fi31->Draw("colz");

    cFib->cd(5);
    //  gPad->SetLogy();
    fh_Fib_ToF[4]->Draw();
    cFib->cd(12);
    gPad->SetLogz();
    fh_ToT_Fib[4]->Draw("colz");
    cFib->cd(19);
    gPad->SetLogz();
    fh_Fibs_vs_Tofd[4]->Draw("colz");
    cFib->cd(26);
    fh_test->Draw("colz"); // fh_counter_fi32->Draw("colz");

    cFib->cd(6);
    //  gPad->SetLogy();
    fh_Fib_ToF[5]->Draw();
    cFib->cd(13);
    gPad->SetLogz();
    fh_ToT_Fib[5]->Draw("colz");
    cFib->cd(20);
    gPad->SetLogz();
    fh_Fibs_vs_Tofd[5]->Draw("colz");
    cFib->cd(27);
    fh_test1->Draw("colz"); // fh_counter_fi33->Draw("colz");

    cFib->cd(7);
    //  gPad->SetLogy();
    if (fHitItems.at(DET_TOFI))
        fh_Tofi_ToF->Draw();
    cFib->cd(0);

    run->AddObject(cFib);
    run->GetHttpServer()->RegisterCommand("Reset_Fib", Form("/Tasks/%s/->Reset_All()", GetName()));

    return kSUCCESS;
}

void R3BOnlineSpectraFibvsToFDS494::Reset_All()
{
    for (Int_t i_FIB_DET = 0; i_FIB_DET < NOF_FIB_DET; i_FIB_DET++)
    {
        fh_xy_Fib[i_FIB_DET]->Reset();
        fh_xy_Fib_ac[i_FIB_DET]->Reset();
        fh_Fib_ToF[i_FIB_DET]->Reset();
        fh_Fib_ToF_ac[i_FIB_DET]->Reset();
        fh_ToT_Fib[i_FIB_DET]->Reset();
        fh_ToT_Fib_ac[i_FIB_DET]->Reset();
        fh_Fibs_vs_Tofd[i_FIB_DET]->Reset();
        fh_Fibs_vs_Tofd_ac[i_FIB_DET]->Reset();
        fh_ToF_vs_Events[i_FIB_DET]->Reset();
    }

    fh_counter_fi30->Reset();
    fh_counter_fi31->Reset();
    fh_counter_fi32->Reset();
    fh_counter_fi33->Reset();
    fh_test->Reset();
    fh_test1->Reset();
    if (fHitItems.at(DET_TOFI))
        fh_Tofi_ToF->Reset();
}

void R3BOnlineSpectraFibvsToFDS494::Exec(Option_t* option)
{

    Bool_t debug2 = false;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    if (header)
    {
        time = header->GetTimeStamp();
        if (time == 0)
            return;

        //		if (time > 0) cout << "header time: " << time << endl;
        if (time_start == 0 && time > 0)
        {
            time_start = time;
            fNEvents_start = fNEvents;
            // cout << "Start event number " << fNEvents_start << endl;
        }

        //   check for requested trigger (Todo: should be done globablly / somewhere else)
        if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        {
            counterWrongTrigger++;
            return;
        }

        // fTpat = 1-16; fTpat_bit = 0-15
        Int_t fTpat_bit = fTpat - 1;
        Int_t itpat;
        Int_t tpatvalue;
        if (fTpat_bit >= 0)
        {
            itpat = header->GetTpat();
            tpatvalue = (itpat && (1 << fTpat_bit)) >> fTpat_bit;
            if (tpatvalue == 0)
            {
                counterWrongTpat++;
                return;
            }
        }
    }

    fNEvents += 1;

    if (fNEvents / 10000. == (int)fNEvents / 10000)
        std::cout << "\rEvents: " << fNEvents << " / " << maxevent << " (" << (int)(fNEvents * 100. / maxevent)
                  << " %) " << std::flush;

    Int_t fibMaxFi30 = 0, fibMaxFi31 = 0, fibMaxFi32 = 0, fibMaxFi33 = 0;

    Int_t summ_tofd = 0;
    Int_t summ_tofdr = 0;
    Int_t summ_tofdl = 0;
    Int_t events30 = 0;
    Int_t events31 = 0;
    Int_t events32 = 0;
    Int_t events33 = 0;

    Double_t x1[n_det];
    Double_t y1[n_det];
    Double_t z1[n_det];
    Double_t q1[n_det];
    Double_t t1[n_det];
    Double_t x2[n_det];
    Double_t y2[n_det];
    Double_t z2[n_det];
    Double_t q2[n_det];
    Double_t t2[n_det];

    Int_t id, id1, id2;

    Int_t det = 0;
    Int_t det1 = 0;
    Int_t det2 = 0;

    for (int i = 0; i < n_det; i++)
    {

        x1[i] = 0.;
        y1[i] = 0.;
        z1[i] = 0.;
        q1[i] = 0.;
        t1[i] = -1000.;

        x2[i] = 0.;
        y2[i] = 0.;
        z2[i] = 0.;
        q2[i] = 0.;
        t2[i] = -1000.;
    }

    Double_t nHitsleft = 0.;
    Double_t nHitsright = 0.;

    // is also number of ifibcount
    Int_t fi23a = 0;
    Int_t fi23b = 1;
    Int_t fi30 = 2;
    Int_t fi31 = 3;
    Int_t fi32 = 4;
    Int_t fi33 = 5;
    Int_t tofi = 6;
    Int_t tofd1r = 7;
    Int_t tofd1l = 8;
    Int_t tofd2r = 9;
    Int_t tofd2l = 10;
    Int_t tofdgoodl = 11;
    Int_t tofdgoodr = 12;

    Double_t tof = 0.;
    Double_t tStart = 0.;
    Bool_t first = true;
    Double_t time_tofd_mem = 0. / 0.;

    Bool_t debug_in = false;

    nHitstemp = 0;

    auto detTofd = fHitItems.at(DET_TOFD);
    Int_t nHits = detTofd->GetEntriesFast();

    if (nHits > 0)
    {
        counterTofd++;
        if (debug_in)
        {
            cout << "********************************" << endl;
            cout << "ToFD hits: " << nHits << endl;
        }
    }

    if (nHits > 100)
        return;

    Int_t multTofd = 0;

    // loop over ToFD
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {

        R3BTofdHitData* hitTofd = (R3BTofdHitData*)detTofd->At(ihit);

        if (IS_NAN(hitTofd->GetTime()))
            continue;
        if (debug_in)
        {
            cout << "Hit " << ihit << " of " << nHits << " charge " << hitTofd->GetEloss() << " time "
                 << hitTofd->GetTime() << endl;
        }

        //     if (hitTofd->GetDetId() != 1)
        //      continue; // both planes 1&2 have hit

        Double_t randx;
        randx = (std::rand() / (float)RAND_MAX) - 0.5;
        Double_t xxx = hitTofd->GetX() + 2.7 * randx;
        Double_t yyy = hitTofd->GetY();
        Double_t ttt = hitTofd->GetTime();
        tStart = ttt;
        Double_t qqq = hitTofd->GetEloss();

        if (xxx >= fxmin || xxx <= fxmax)
            nHitstemp += 1;

        Bool_t tofd_left = false;
        Bool_t tofd_right = false;
        id2 = hitTofd->GetDetId();

        if (nHitstemp == 1)
        {
            summ_tofd += 1;
            time_tofd_mem = ttt;
        }
        if (hitTofd->GetX() <= 0.)
        {
            // tof rechts
            tofd_right = true;
            nHitsright += 1;
            if (nHitstemp == 1)
                summ_tofdr += 1;
            if (id2 == 1)
            {
                det = tofd1r;
            }
            else if (id2 == 2)
            {
                det = tofd2r;
            }
            else if (id2 == 12)
            {
                det = tofdgoodr;
            }
            x1[det] = hitTofd->GetX();
            y1[det] = hitTofd->GetY();
            q1[det] = hitTofd->GetEloss();
            t1[det] = hitTofd->GetTime();
        }
        else
        {
            // tof links
            nHitsleft += 1;
            if (nHitstemp == 1)
                summ_tofdl += 1;
            tofd_left = true;
            if (id2 == 1)
            {
                det = tofd1l;
            }
            else if (id2 == 2)
            {
                det = tofd2l;
            }
            else if (id2 == 2)
            {
                det = tofdgoodl;
            }

            x2[det] = hitTofd->GetX();
            y2[det] = hitTofd->GetY();
            q2[det] = hitTofd->GetEloss();
            t2[det] = hitTofd->GetTime();
        }

        if (fCuts && (qqq < 0 || qqq > 1000))
            continue;
        if (fCuts && (xxx < -10000 || xxx > 10000))
            continue;

        multTofd++;

        counterTofdMulti++;

        // cut in ToT for Fibers
        Double_t cutQ = 0.;

        // loop over TOFI
        if (fHitItems.at(DET_TOFI))
        {
            auto detHitTofi = fHitItems.at(DET_TOFI);
            Int_t nHitsTofi = detHitTofi->GetEntriesFast();
            LOG(DEBUG) << "Tofi hits: " << nHitsTofi << endl;

            for (Int_t ihitTofi = 0; ihitTofi < nHitsTofi; ihitTofi++)
            {
                det = tofi;
                R3BTofiHitData* hitTofi = (R3BTofiHitData*)detHitTofi->At(ihitTofi);
                x1[det] = hitTofi->GetX(); // cm
                y1[det] = hitTofi->GetY(); // cm
                z1[det] = 0.;
                q1[det] = hitTofi->GetEloss();

                t1[det] = hitTofi->GetTime();
                tof = tStart - t1[det];

                // Fill histograms before cuts
                fh_Tofi_ToF->Fill(tof);

                // Cuts on Tofi

                if (fCuts && x1[det] * 100. < -10000)
                    continue;
                if (fCuts && q1[det] < cutQ)
                    continue;
                if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                    continue;
                if (fCuts && (tof < -100000 || tof > 10000))
                    continue;

                // Fill histograms after cuts
                fh_Tofi_ToF_ac->Fill(tof);

                if (debug2)
                    cout << "FiTofi: " << ihitTofi << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                         << " t1: " << t1[det] << endl;
            }
        }

        // loop over fiber 33
        auto detHit33 = fHitItems.at(DET_FI33);
        Int_t nHits33 = detHit33->GetEntriesFast();
        Double_t tot_max33 = 0;
        fibMaxFi33 = 0;
        fibMaxFi33 = 0;
        LOG(DEBUG) << "Fi33 hits: " << nHits33 << endl;

        if (nHitstemp == 1 && nHits33 > 0) //&& tofd_right)
            events33++;

        for (Int_t ihit33 = 0; ihit33 < nHits33; ihit33++)
        {
            det = fi33;
            R3BFiberMAPMTHitData* hit33 = (R3BFiberMAPMTHitData*)detHit33->At(ihit33);
            x1[det] = hit33->GetX(); // cm
            y1[det] = hit33->GetY(); // cm
            z1[det] = 0.;
            q1[det] = hit33->GetEloss();

            t1[det] = hit33->GetTime();
            tof = tStart - t1[det];

            if (q1[det] > tot_max33) //&& tofd_right)
            {
                tot_max33 = q1[det];
                fibMaxFi33 = hit33->GetFiberId();
            }

            // Fill histograms before cuts
            if (1 == 1) // tofd_right)
            {
                fh_Fib_ToF[det]->Fill(tof);
                fh_xy_Fib[det]->Fill(xxx, q1[det]);
                fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            }
            fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
            hits33bc++;

            // Cuts on Fi33

            if (fCuts && x1[det] * 100. < -10000)
                continue;
            if (fCuts && q1[det] < cutQ)
                continue;
            if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                continue;
            if (fCuts && (tof < -100000 || tof > 10000))
                continue;

            hits33++;

            // Fill histograms after cuts
            fh_Fib_ToF_ac[det]->Fill(tof);
            fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);

            if (debug2)
                cout << "Fi33: " << ihit33 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;
        }

        // loop over fiber 31
        auto detHit31 = fHitItems.at(DET_FI31);
        Int_t nHits31 = detHit31->GetEntriesFast();
        Double_t tot_max31 = 0.;

        fibMaxFi31 = 0;
        LOG(DEBUG) << "Fi31 hits: " << nHits31 << endl;
        if (nHitstemp == 1 && nHits31 > 0) //&& tofd_right)
            events31++;
        for (Int_t ihit31 = 0; ihit31 < nHits31; ihit31++)
        {
            det = fi31;
            R3BFiberMAPMTHitData* hit31 = (R3BFiberMAPMTHitData*)detHit31->At(ihit31);
            x1[det] = hit31->GetX();
            y1[det] = hit31->GetY();
            z1[det] = 0.;
            q1[det] = hit31->GetEloss();
            t1[det] = hit31->GetTime();
            tof = tStart - t1[det];

            if (t1[det] < -20 || t1[det] > 100)
                continue;

            if (q1[det] > tot_max31) //&& tofd_right)
            {
                tot_max31 = q1[det];
                fibMaxFi31 = hit31->GetFiberId();
            }

            // Fill histograms before cuts
            if (1 == 1) // tofd_right)
            {
                fh_xy_Fib[det]->Fill(xxx, q1[det]);
                fh_Fib_ToF[det]->Fill(tof);
                fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            }
            fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
            hits31bc++;

            // Cuts on Fi31

            if (fCuts && x1[det] < -10000)
                continue;
            if (fCuts && q1[det] < cutQ)
                continue;
            if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                continue;
            if (fCuts && (tof < -100000 || tof > 10000))
                continue;

            hits31++;

            // Fill histograms
            fh_Fib_ToF_ac[det]->Fill(tof);
            fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);

            if (debug2)
                cout << "Fi31: " << ihit31 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;
        }

        // loop over fiber 30
        auto detHit30 = fHitItems.at(DET_FI30);
        Int_t nHits30 = detHit30->GetEntriesFast();
        Double_t tot_max30 = 0.;
        fibMaxFi30 = 0;
        LOG(DEBUG) << "Fi30 hits: " << nHits30 << endl;
        if (nHitstemp == 1 && nHits30 > 0) // && tofd_left)
            events30++;
        for (Int_t ihit30 = 0; ihit30 < nHits30; ihit30++)
        {
            det = fi30;
            R3BFiberMAPMTHitData* hit30 = (R3BFiberMAPMTHitData*)detHit30->At(ihit30);
            x1[det] = hit30->GetX();
            y1[det] = hit30->GetY();
            z1[det] = 0.;
            q1[det] = hit30->GetEloss();
            t1[det] = hit30->GetTime();
            tof = tStart - t1[det];

            if (q1[det] > tot_max30) //&& tofd_left)
            {
                tot_max30 = q1[det];
                fibMaxFi30 = hit30->GetFiberId();
            }

            // Fill histograms before cuts
            if (1 == 1) // tofd_left)
            {
                fh_Fib_ToF[det]->Fill(tof);
                fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
                fh_xy_Fib[det]->Fill(xxx, q1[det]);
            }
            fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
            hits30bc++;

            // Cuts on Fi30
            if (fCuts && q1[det] < cutQ)
                continue;
            if (fCuts && x1[det] < -10000)
                continue;
            if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                continue;
            if (tof < -10000 || tof > 10000)
                continue;

            hits30++;

            // Fill histograms
            fh_Fib_ToF_ac[det]->Fill(tof);
            fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);

            if (debug2)
                cout << "Fi30: " << ihit30 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;
        }

        // loop over fiber 32
        auto detHit32 = fHitItems.at(DET_FI32);
        Int_t nHits32 = detHit32->GetEntriesFast();
        Double_t tot_max32 = 0.;
        fibMaxFi32 = 0;
        LOG(DEBUG) << "Fi32 hits: " << nHits32 << endl;
        if (nHitstemp == 1 && nHits32) //> 0 && tofd_left)
            events32++;
        for (Int_t ihit32 = 0; ihit32 < nHits32; ihit32++)
        {
            det = fi32;
            R3BFiberMAPMTHitData* hit32 = (R3BFiberMAPMTHitData*)detHit32->At(ihit32);
            x1[det] = hit32->GetX();
            y1[det] = hit32->GetY();
            z1[det] = 0.;
            q1[det] = hit32->GetEloss();
            t1[det] = hit32->GetTime();
            tof = tStart - t1[det];

            if (q1[det] > tot_max32) //&& tofd_left)
            {
                tot_max32 = q1[det];
                fibMaxFi32 = hit32->GetFiberId();
            }

            // Fill histograms before cuts
            if (1 == 1) // tofd_left)
            {
                fh_Fib_ToF[det]->Fill(tof);
                fh_ToT_Fib[det]->Fill(qqq, q1[det]);
                fh_xy_Fib[det]->Fill(xxx, q1[det]);
                fh_ToF_vs_Events[det]->Fill(fNEvents, tof);
            }
            fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
            hits32bc++;

            // Cuts on Fi32
            if (q1[det] < cutQ)
                continue;
            if (fCuts && x1[det] < -10000)
                continue;
            if (fCuts && (y1[det] < -10000 || y1[det] > 10000))
                continue;
            if (tof < -10000 || tof > 10000)
                continue;

            hits32++;

            // Fill histograms
            fh_Fib_ToF_ac[det]->Fill(tof);
            fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);

            if (debug2)
                cout << "Fi32: " << ihit32 << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << t1[det] << endl;
        }

        // loop over fiber 23a
        auto detHit23a = fHitItems.at(DET_FI23A);
        Int_t nHits23a = detHit23a->GetEntriesFast();
        LOG(DEBUG) << "Fi23a hits: " << nHits23a << endl;
        for (Int_t ihit23a = 0; ihit23a < nHits23a; ihit23a++)
        {
            det = fi23a;
            R3BFiberMAPMTHitData* hit23a = (R3BFiberMAPMTHitData*)detHit23a->At(ihit23a);
            x1[det] = hit23a->GetX();
            y1[det] = hit23a->GetY();
            q1[det] = hit23a->GetEloss();
            t1[det] = hit23a->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(tof);
            fh_xy_Fib[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(xxx, x1[det]);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);

            if (q1[det] < cutQ)
                continue;
            if (x1[det] < -10000 || x1[det] > 10000)
                continue;
            if (tof < -10000 || tof > 100000)
                continue;

            // Fill histograms
            fh_Fib_ToF_ac[det]->Fill(tof);
            fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, x1[det]);

            if (debug2)
                cout << "Fi23a " << ihit23a << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << tof << endl;
        }

        // loop over fiber 23b
        auto detHit23b = fHitItems.at(DET_FI23B);
        Int_t nHits23b = detHit23b->GetEntriesFast();
        LOG(DEBUG) << "Fi23b hits: " << nHits23b << endl;
        for (Int_t ihit23b = 0; ihit23b < nHits23b; ihit23b++)
        {
            det = fi23b;
            R3BFiberMAPMTHitData* hit23b = (R3BFiberMAPMTHitData*)detHit23b->At(ihit23b);
            x1[det] = hit23b->GetX();
            y1[det] = hit23b->GetY();
            z1[det] = 0.;
            q1[det] = hit23b->GetEloss();
            t1[det] = hit23b->GetTime();
            tof = tStart - t1[det];

            // Fill histograms before cuts
            fh_Fib_ToF[det]->Fill(tof);
            fh_xy_Fib[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd[det]->Fill(xxx, y1[det]);
            fh_ToF_vs_Events[det]->Fill(fNEvents, tof);

            // Cuts on Fi23b
            if (q1[det] < cutQ)
                continue;
            if (y1[det] < -10000 || y1[det] > 10000)
                continue;
            if (tof < -10000 || tof > 10000)
                continue;

            // Fill histograms
            fh_Fib_ToF_ac[det]->Fill(tof);
            fh_xy_Fib_ac[det]->Fill(xxx, q1[det]);
            fh_ToT_Fib_ac[det]->Fill(qqq, q1[det]);
            fh_Fibs_vs_Tofd_ac[det]->Fill(xxx, y1[det]);

            if (debug2)
                cout << "Fi23b " << ihit23b << " x1: " << x1[det] << " y1: " << y1[det] << " q1: " << q1[det]
                     << " t1: " << tof << endl;
        }

    } // end ToFD loop

    while (windowToFD.size() < fwindow_mv)
    {
        windowToFD.push_back(summ_tofd);

        windowToFDl.push_back(summ_tofdl);
        windowFi30.push_back(events30);
        windowFi32.push_back(events32);
        fiberFi30.push_back(fibMaxFi30);
        fiberFi32.push_back(fibMaxFi32);

        totalToFD += summ_tofd;

        totalToFDl += summ_tofdl;
        totalFi30 += events30;
        totalFi32 += events32;
        totalfibFi30 += fibMaxFi30;
        totalfibFi32 += fibMaxFi32;
        //}
        // while (windowToFDr.size() < fwindow_mv)
        //{
        windowToFDr.push_back(summ_tofdr);
        windowFi31.push_back(events31);
        windowFi33.push_back(events33);
        fiberFi31.push_back(fibMaxFi31);
        fiberFi33.push_back(fibMaxFi33);

        totalToFDr += summ_tofdr;
        totalFi31 += events31;
        totalFi33 += events33;
        totalfibFi31 += fibMaxFi31;
        totalfibFi33 += fibMaxFi33;
    }
    totalToFDl -= windowToFDl.front();
    totalToFDl += summ_tofdl;
    windowToFDl.pop_front();
    windowToFDl.push_back(summ_tofdl);

    totalToFD -= windowToFD.front();
    totalToFD += summ_tofd;
    windowToFD.pop_front();
    windowToFD.push_back(summ_tofd);

    totalToFDr -= windowToFDr.front();
    totalToFDr += summ_tofdr;
    windowToFDr.pop_front();
    windowToFDr.push_back(summ_tofdr);

    totalFi30 -= windowFi30.front();
    totalFi30 += events30;
    windowFi30.pop_front();
    windowFi30.push_back(events30);

    totalFi31 -= windowFi31.front();
    totalFi31 += events31;
    windowFi31.pop_front();
    windowFi31.push_back(events31);

    totalFi32 -= windowFi32.front();
    totalFi32 += events32;
    windowFi32.pop_front();
    windowFi32.push_back(events32);

    totalFi33 -= windowFi33.front();
    totalFi33 += events33;
    windowFi33.pop_front();
    windowFi33.push_back(events33);

    totalfibFi30 -= fiberFi30.front();
    totalfibFi30 += fibMaxFi30;
    fiberFi30.pop_front();
    fiberFi30.push_back(fibMaxFi30);

    totalfibFi31 -= fiberFi31.front();
    totalfibFi31 += fibMaxFi31;
    fiberFi31.pop_front();
    fiberFi31.push_back(fibMaxFi31);

    totalfibFi32 -= fiberFi32.front();
    totalfibFi32 += fibMaxFi32;
    fiberFi32.pop_front();
    fiberFi32.push_back(fibMaxFi32);

    totalfibFi33 -= fiberFi33.front();
    totalfibFi33 += fibMaxFi33;
    fiberFi33.pop_front();
    fiberFi33.push_back(fibMaxFi33);

    auto effFi30 = double(totalFi30) / double(totalToFD);
    auto effFi31 = double(totalFi31) / double(totalToFD);
    auto effFi32 = double(totalFi32) / double(totalToFD);
    auto effFi33 = double(totalFi33) / double(totalToFD);

    auto avr_fib30 = double(totalfibFi30) / double(totalFi30);
    auto avr_fib31 = double(totalfibFi31) / double(totalFi31);
    auto avr_fib32 = double(totalfibFi32) / double(totalFi32);
    auto avr_fib33 = double(totalfibFi33) / double(totalFi33);

    /*
            cout<<"********* Fi31 window el:"<<endl;
            for (int i=0; i<windowFi31.size();++i) {
                cout << windowFi31[i] << ' ';
            }
            cout<<endl;
            for (int i=0; i<fiberFi31.size(); ++i) {
                cout << fiberFi31[i] << ' ';
            }
            cout<<endl;
        if(fNEvents > 5e5){
          cout<<"Fib31: "<<events31<<", "<<fibMaxFi31<<", "<<totalfibFi31<<"; "<<totalFi31<<", "<<avr_fib31<<", "
       <<double(totalfibFi31)/double(totalFi31)<<endl;
   }
    */

    //   cout<<"Eff: "<<fNEvents<<": "<<effFi30<<", "<<effFi31<<"; "<<effFi32<<", "<<effFi33<<endl;
    if (fNEvents > fwindow_mv)
    {
        fh_counter_fi30->Fill(avr_fib30, effFi30 * 100.);
        fh_counter_fi31->Fill(avr_fib31, effFi31 * 100.);
        fh_counter_fi32->Fill(avr_fib32, effFi32 * 100.);
        fh_counter_fi33->Fill(avr_fib33, effFi33 * 100.);
    }
    if (fibMaxFi31 > 0)
        fh_test->Fill(fNEvents, fibMaxFi31);
    fh_test1->Fill(fNEvents, avr_fib31);

    Nsumm_tofd += summ_tofd;
    Nsumm_tofdr += summ_tofdr;
    Nsumm_tofdl += summ_tofdl;
    Nevents30 += events30;
    Nevents31 += events31;
    Nevents32 += events32;
    Nevents33 += events33;
}

void R3BOnlineSpectraFibvsToFDS494::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {
        if (fHitItems.at(det))
        {
            fHitItems.at(det)->Clear();
        }
        if (fCalItems.at(det))
        {
            fCalItems.at(det)->Clear();
        }
    }
}

void R3BOnlineSpectraFibvsToFDS494::FinishTask()
{
    cout << "Statistics:" << endl;
    cout << "Events: " << fNEvents << endl;
    cout << "Wrong Trigger: " << counterWrongTrigger << endl;
    cout << "Wrong Tpat: " << counterWrongTpat << endl;
    cout << "TofD: " << counterTofd << endl;
    cout << "TofD multi: " << counterTofdMulti << endl;
    cout << "TofD multi - "
         << "total: " << Nsumm_tofd << ", left: " << Nsumm_tofdl << ", right: " << Nsumm_tofdr << endl;

    cout << "Eff. Fi30: " << Nevents30 << "  " << Nevents30 / Nsumm_tofd << endl;
    cout << "Eff. Fi31: " << Nevents31 << "  " << Nevents31 / Nsumm_tofd << endl;
    cout << "Eff. Fi32: " << Nevents32 << "  " << Nevents32 / Nsumm_tofd << endl;
    cout << "Eff. Fi33: " << Nevents33 << "  " << Nevents33 / Nsumm_tofd << endl;

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fHitItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_xy_Fib[ifibcount]->Write();
            fh_xy_Fib_ac[ifibcount]->Write();
            fh_ToT_Fib[ifibcount]->Write();
            fh_ToT_Fib_ac[ifibcount]->Write();
            fh_Fibs_vs_Tofd[ifibcount]->Write();
            fh_Fibs_vs_Tofd_ac[ifibcount]->Write();
            fh_Fib_ToF[ifibcount]->Write();
            fh_Fib_ToF_ac[ifibcount]->Write();
            fh_ToF_vs_Events[ifibcount]->Write();
        }
    }

    fh_counter_fi30->Write();
    fh_counter_fi31->Write();
    fh_counter_fi32->Write();
    fh_counter_fi33->Write();
    fh_test->Write();
    fh_test1->Write();
}

ClassImp(R3BOnlineSpectraFibvsToFDS494)
