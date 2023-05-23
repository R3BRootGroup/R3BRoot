// ------------------------------------------------------------
// -----                  R3BOnlineFib60                -----
// -----          Created April  2023 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineFib60.h"

#include "R3BBunchedFiberCalData.h"
#include "R3BBunchedFiberHitData.h"
#include "R3BBunchedFiberMappedData.h"

#include "R3BEventHeader.h"
#include "R3BTCalEngine.h"

#include "R3BSamplerMappedData.h"

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
#include "TLegend.h"

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

#include <assert.h>
#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

R3BOnlineFib60::R3BOnlineFib60()
    : R3BOnlineFib60("OnlineSpectra", 1)
{
}

R3BOnlineFib60::R3BOnlineFib60(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fSamp(-1)
    , fNEvents(0)
{
}

R3BOnlineFib60::~R3BOnlineFib60()
{
}

InitStatus R3BOnlineFib60::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BOnlineFib60::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels
    fMappedItems_fi60 = (TClonesArray*)mgr->GetObject("Fi60Mapped");
    if (NULL == fMappedItems_fi60)
        LOG(fatal) << "Branch Fi60Mapped not found";

    fCalItems_fi60 = (TClonesArray*)mgr->GetObject("Fi60Cal");
    if (NULL == fCalItems_fi60)
        LOG(fatal) << "Branch Fi60Cal not found";

    fHitItems_fi60 = (TClonesArray*)mgr->GetObject("Fi60Hit");
    if (NULL == fHitItems_fi60)
        LOG(fatal) << "Branch Fi60Hit not found";

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------
    // Trigger and Tpat
    //  TCanvas* cTrigg = new TCanvas("Trigg", "Triggers", 50, 50, 500, 500);
    fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fhTpat->GetXaxis()->SetTitle("Tpat value");

    fhTrigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fhTrigger->GetXaxis()->SetTitle("Trigger value");
    /*
        cTrigg->Divide(2, 1);
        cTrigg->cd(1);
        gPad->SetLogy();
        fhTrigger->Draw();
        cTrigg->cd(2);
        gPad->SetLogy();
        fhTpat->Draw();
        cTrigg->cd(0);
    */

    //---------------------------------------------------------------------------------------------------
    // Fiber60 detector
    if (fMappedItems_fi60)
    {
        Int_t ch = 1050;
        Double_t ds = 1000. / 150. / 16.;

        fh_fi60_fiber = new TH1F("Fi60_fibers", "Fi60 fibers", ch, 0., ch);
        fh_fi60_fiber->GetXaxis()->SetTitle("Fiber");
        fh_fi60_fiber->GetYaxis()->SetTitle("Counts");

        fh_fi60_mult = new TH1F("Fi60_mult", "Fi60 mult", ch, 0., ch);
        fh_fi60_mult->GetXaxis()->SetTitle("mult");
        fh_fi60_mult->GetYaxis()->SetTitle("Counts");

        fh_fi60_Tot = new TH2F("Fi60_ToT", "Fi60 ToT", ch, 0., ch, (int)(500./ds), 0, 500);
        fh_fi60_Tot->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Tot->GetYaxis()->SetTitle("ToT");

        fh_fi60_Time = new TH2F("Fi60_Time", "Fi60 time", ch, 0., ch, 2500, -1500, 1000);
        fh_fi60_Time->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Time->GetYaxis()->SetTitle("time");

        // with cuts
        fh_fi60_fiber_ave = new TH1F("Fi60_fibers_ave", "Fi60 fibers average", ch, 0., ch);
        fh_fi60_fiber_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi60_fiber_ave->GetYaxis()->SetTitle("Counts");

        fh_fi60_fiber_mc = new TH1F("Fi60_fibers_mc", "Fi60 fibers with cuts", ch, 0., ch);
        fh_fi60_fiber_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi60_fiber_mc->GetYaxis()->SetTitle("Counts");

        fh_fi60_mult_mc = new TH1F("Fi60_mult_mc", "Fi60 mult with cuts", ch, 0., ch);
        fh_fi60_mult_mc->GetXaxis()->SetTitle("mult");
        fh_fi60_mult_mc->GetYaxis()->SetTitle("Counts");

        fh_fi60_Tot_mc = new TH2F("Fi60_ToT_mc", "Fi60 ToT with cuts", ch, 0., ch, (int)(500./ds), 0, 500);
        fh_fi60_Tot_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Tot_mc->GetYaxis()->SetTitle("ToT");

        fh_fi60_Tot_ave = new TH2F("Fi60_ToT_ave", "Fi60 ToT sum vs. fiber average", ch, 0., ch, (int)(500./ds), 0, 500);
        fh_fi60_Tot_ave->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Tot_ave->GetYaxis()->SetTitle("ToT");

        fh_fi60_Time_mc = new TH2F("Fi60_Time_mc", "Fi60 time with cuts", ch, 0., ch, (int)(1000./ds), -500, 500);
        fh_fi60_Time_mc->GetXaxis()->SetTitle("Fiber");
        fh_fi60_Time_mc->GetYaxis()->SetTitle("time");

        fh_fi60_cor = new TH2F("Fi60_cor", "Fi60 correlation plot", ch, 0, ch, ch, 0, ch);
        fh_fi60_cor->GetXaxis()->SetTitle("Fiber");
        fh_fi60_cor->GetYaxis()->SetTitle("Fiber");

        fh_fi60_tot2_tot1 =
            new TH2F("Fi60_ToT1_vs_ToT2", "Fi60 ToT1 vs. ToT2", (int)(60. / ds), 0, 60, (int)(60. / ds), 0, 60);
        fh_fi60_tot2_tot1->GetXaxis()->SetTitle("ToT1");
        fh_fi60_tot2_tot1->GetYaxis()->SetTitle("ToT2");

        fh_fi60_eff = new TH2F("fi60_eff", "Fi60 hits for PDC x vs y", 1500, 0, 1500, 1500, 0, 1500);
        fh_fi60_eff->GetXaxis()->SetTitle("x in mm");
        fh_fi60_eff->GetYaxis()->SetTitle("y in mm");

        fh_fi60_eff_vs_time = new TH2F("fi60_eff_vs_time", "Fi60 eff vs. time", 10000, 0, 10000, 100, 0, 100);
        fh_fi60_eff_vs_time->GetXaxis()->SetTitle("number of hits");
        fh_fi60_eff_vs_time->GetYaxis()->SetTitle("eff");

        TCanvas* cFib60 = new TCanvas("Fib60", "Fiber60 detectors", 50, 50, 500, 500);
        cFib60->Divide(4, 2);

        cFib60->cd(1);
        gPad->SetLogy();
        fh_fi60_fiber->Draw();
        cFib60->cd(2);
        gPad->SetLogy();
        fh_fi60_mult->Draw();
        cFib60->cd(3);
        gPad->SetLogz();
        fh_fi60_Tot->Draw("colz");
        cFib60->cd(4);
        gPad->SetLogz();
        fh_fi60_Time->Draw("colz");

        cFib60->cd(5);
        gPad->SetLogy();
        fh_fi60_fiber_mc->Draw();
        cFib60->cd(6);
        gPad->SetLogy();
        fh_fi60_mult_mc->Draw();
        cFib60->cd(7);
        gPad->SetLogz();
        fh_fi60_Tot_mc->Draw("colz");
        cFib60->cd(8);
        gPad->SetLogz();
        fh_fi60_Time_mc->Draw("colz");

        cFib60->cd(0);
        run->AddObject(cFib60);
        run->GetHttpServer()->RegisterCommand("Reset_Fi60", Form("/Tasks/%s/->Reset_Fi60_Histo()", GetName()));
    }

    return kSUCCESS;
}

void R3BOnlineFib60::Reset_Fi60_Histo()
{
    fh_fi60_fiber->Reset();
    fh_fi60_mult->Reset();
    fh_fi60_Tot->Reset();
    fh_fi60_Time->Reset();
}

void R3BOnlineFib60::Exec(Option_t* option)
{
    fNEvents++;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        LOG(error) << "FairRootManager not found";
        return;
    }

    //   check for requested trigger (Todo: should be done globablly / somewhere else)
    if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
        return;

    /*
        if (fMappedItems)
        {
            fhTrigger->Fill(header->GetTrigger());
        }

        //   check for requested trigger (Todo: should be done globablly / somewhere else)
        if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
            return;

        if (fMappedItems)
        {
            Int_t tpatbin;
            for (int i = 0; i < 16; i++)
            {
                tpatbin = (header->GetTpat() & (1 << i));
                if (tpatbin != 0)
                    fhTpat->Fill(i + 1);
            }
        }

        // fTpat = 1-16; fTpat_bit = 0-15
        Int_t fTpat_bit = fTpat - 1;
        Int_t itpat;
        Int_t tpatvalue;
        if (fTpat_bit >= 0)
        {
            itpat = header->GetTpat();
            tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
            if (tpatvalue == 0)
                return;
        }
    */

    //----------------------------------------------------------------------
    // Fiber60 detectors
    //----------------------------------------------------------------------
    if (fMappedItems_fi60)
    {
        Int_t mult_fi60 = 0;
        Int_t iFib = 0;
        Int_t iFib2 = 0;

        auto det = fMappedItems_fi60;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BBunchedFiberMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen
            if (mapped->IsMAPMTTrigger())
            {
                triggerCountsFi60++;
            }
            if (!mapped->IsMAPMT())
            {
                continue;
            }
            if (mapped->IsLeading())
            {
                iFib = mapped->GetChannel(); // 1..n
                mult_fi60++;
                fh_fi60_fiber->Fill(iFib);
            }
        }
        // if (mult_fi0_0 > 0)
        {
            totalFi60 += mult_fi60;
            fh_fi60_mult->Fill(mult_fi60);
        }
        // Cal Items
        if (fCalItems_fi60)
        {
        }

        // Hit Items
        Double_t tmin = -15;
        Double_t tmax =  15;
        if (fHitItems_fi60)
        {
            Int_t multi[1024] = { 0 };
            Int_t nHits = fHitItems_fi60->GetEntriesFast();
            Double_t xpos = 0. / 0.;
            Double_t ypos = 0. / 0.;
            Double_t tfib = 0. / 0., tof_fib = 0. / 0.;
            Double_t randx;
            Double_t fib_ave = 0.;
            Double_t tot_sum = 0.;
            Double_t m = 0.;
            Int_t fib_old = 0;
            
            //cout << "new event: " << nHits << "  " << endl;

            Double_t tMAPMT = 0. / 0.;
            Double_t tMAPMT2 = 0. / 0.;
            Double_t tSPMT1 = 0. / 0.;
            Double_t tSPMT2 = 0. / 0.;
            Double_t tot = 0. / 0.;
            Double_t tot2 = 0. / 0.;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                tMAPMT = 0. / 0.;
                tot = 0. / 0.;
                Double_t dtime = 0. / 0.;
                R3BBunchedFiberHitData* hit = (R3BBunchedFiberHitData*)fHitItems_fi60->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..
                tMAPMT = hit->GetTime();  //+ 5444.;
                tot = hit->GetEloss() - 200. / 15.;
                fh_fi60_Tot->Fill(iFib, tot);
                fh_fi60_Time->Fill(iFib, tMAPMT);
                // fiber correlation plot. Loop again over all fibers
                Int_t mult = 0;
                for (Int_t jhit = 0; jhit < nHits; jhit++)
                {
                    R3BBunchedFiberHitData* hit2 = (R3BBunchedFiberHitData*)fHitItems_fi60->At(jhit);
                    if (!hit2)
                        continue;

                    iFib2 = hit2->GetFiberId(); // 1..
                    tot2 = hit2->GetEloss();
                    tMAPMT2 = hit2->GetTime(); //+ 5444.;

                    if (iFib != iFib2)
                    {
                        fh_fi60_cor->Fill(iFib, iFib2);
                        fh_fi60_tot2_tot1->Fill(tot, tot2);
                    }
                    if (tMAPMT2 < tmin || tMAPMT2 > tmax)
                    {
                        mult++;
                    }
                }
                // cuts ***********************************
                // if not in the right time window, go to next
                if (tMAPMT < tmin || tMAPMT > tmax)
                {
                    continue;
                }

                if (tot < 0 || tot > 2500)
                {
                    continue;
                }

                if (mult < 1)
                {
                    //continue;
                }
				//cout << "Fiber: " << iFib << " tot: " << tot << endl;
				if(ihit == 0)
				{
					fib_ave = (float)iFib;
					tot_sum = tot;
					m = 1.;
					fib_old = iFib;
				}
				Double_t diff = 3.1;
				if(iFib - fib_old < diff && ihit> 0 )
				{
					fib_ave += (float)iFib;
					tot_sum += tot;
					m += 1.;
					fib_old = iFib;
				}
				
				if(iFib - fib_old > diff) 
				{
					// plot average and reset it
					//cout << "fill: " << fib_ave << " tot sum: " << tot_sum << endl;
					fh_fi60_fiber_ave->Fill(fib_ave / m);
					fh_fi60_Tot_ave->Fill(fib_ave / m, tot_sum);
					fib_ave = (float)iFib;
					tot_sum = tot;
					m = 1.;
					fib_old = iFib;
				}
				if(ihit + 1 == nHits)
				{
					fh_fi60_fiber_ave->Fill(fib_ave / m);
					fh_fi60_Tot_ave->Fill(fib_ave / m, tot_sum);
				}
				//cout << "average: " << fib_ave / m << " tot sum: " << tot_sum << endl;
                // cout << "Test: " << tSPMT1 << IS_NAN(tSPMT1) << endl;
                xpos = hit->GetX();
                ypos = dtime;
                multi[iFib - 1]++;
                fh_fi60_Tot_mc->Fill(iFib, tot);
                fh_fi60_Time_mc->Fill(iFib, tMAPMT);
                fh_fi60_fiber_mc->Fill(iFib);
				
                if (nHits > 0 && nHits < 3)
                {
                    // cout << "Fib0: fiber = " << iFib << endl;
                }

            } // end for(ihit)

            Int_t multi_fi60 = 0;
            for (Int_t i = 0; i < 1024; i++)
            {
                multi_fi60 += multi[i];
            }
            if (multi_fi60 > 0)
            {
                fh_fi60_mult_mc->Fill(multi_fi60);
                detectedFi60++;
            }
            fh_fi60_eff_vs_time->Fill(triggerCountsFi60, (float)detectedFi60 / (float)triggerCountsFi60 * 100.);
        }
        
    }

}

void R3BOnlineFib60::FinishEvent()
{    
	
    if (fMappedItems_fi60)
        fMappedItems_fi60->Clear();
    if (fCalItems_fi60)
        fCalItems_fi60->Clear();
    if (fHitItems_fi60)
        fHitItems_fi60->Clear();
        
}

void R3BOnlineFib60::FinishTask()
{
    cout << "Total Fi60: " << totalFi60 << endl;
    cout << "trigger Fi60: " << triggerCountsFi60 << endl;

    if (fMappedItems_fi60)
    {
        fh_fi60_fiber->Write();
        fh_fi60_mult->Write();
        fh_fi60_Tot->Write();
        fh_fi60_Time->Write();
        fh_fi60_fiber_mc->Write();
        fh_fi60_fiber_ave->Write();
        fh_fi60_mult_mc->Write();
        fh_fi60_Tot_mc->Write();
        fh_fi60_Tot_ave->Write();
        fh_fi60_Time_mc->Write();
        fh_fi60_tot2_tot1->Write();
        fh_fi60_cor->Write();
        fh_fi60_eff->Write();
        fh_fi60_eff_vs_time->Write();
    }
}

ClassImp(R3BOnlineFib60)
