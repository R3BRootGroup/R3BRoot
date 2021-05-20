// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */
#include "R3BOnlineSpectraFiber_s494.h"

#include "R3BEventHeader.h"
#include "R3BSamplerMappedData.h"
#include "R3BTCalEngine.h"

#include "R3BFiberMAPMTCalData.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BFiberMAPMTMappedData.h"

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
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

R3BOnlineSpectraFiber_s494::R3BOnlineSpectraFiber_s494()
    : FairTask("OnlineSpectraFiber_s494", 1)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraFiber_s494::R3BOnlineSpectraFiber_s494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraFiber_s494::~R3BOnlineSpectraFiber_s494()
{

    for (int i = 0; i < NOF_FIB_DET; i++)
    {
        if (fh_channels_Fib[i])
            delete fh_channels_Fib[i];
        if (fh_fibers_Fib[i])
            delete fh_fibers_Fib[i];
        if (fh_mult_Fib[i])
            delete fh_mult_Fib[i];
        if (fh_time_Fib[i])
            delete fh_time_Fib[i];
        if (fh_multihit_m_Fib[i])
            delete fh_multihit_m_Fib[i];
        if (fh_multihit_s_Fib[i])
            delete fh_multihit_s_Fib[i];
        if (fh_ToT_Fib[i])
            delete fh_ToT_Fib[i];
        if (fh_channels_single_Fib[i])
            delete fh_channels_single_Fib[i];
        if (fh_Fib_pos[i])
            delete fh_Fib_pos[i];
        if (fh_Fib_vs_Events[i])
            delete fh_Fib_vs_Events[i];
        if (fh_ToTmax_Fibmax[i])
            delete fh_ToTmax_Fibmax[i];
        if (fh_chan_corell[i])
            delete fh_chan_corell[i];
        if (fh_raw_tot_up[i])
            delete fh_raw_tot_up[i];
        if (fh_raw_tot_down[i])
            delete fh_raw_tot_down[i];
    }
}

InitStatus R3BOnlineSpectraFiber_s494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraFiber_s494::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    // uncomment lines below when ucesb avaliable
    FairRunOnline* run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels
    assert(DET_MAX + 1 == sizeof(fDetectorNames) / sizeof(fDetectorNames[0]));
    printf("**** Have %d fiber detectors ==>.\n", NOF_FIB_DET);
    for (int det = 0; det < DET_MAX; det++)
    {
        //       fMappedItems.push_back((TClonesArray*)mgr->GetObject(Form("%sMapped", fDetectorNames[det])));

        //       if (NULL == fMappedItems.at(det))
        //       {
        //           printf("Could not find mapped data for '%s'.\n", fDetectorNames[det]);
        //       }
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

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    //-----------------------------------------------------------------------
    // Fiber Detectors 1-NOF_FIB_DET

    char canvName[255];
    TCanvas* FibCanvas[NOF_FIB_DET];

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {

        if (fCalItems.at(DET_FI_FIRST + ifibcount))
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
            // Cal level
            // Channels:
            fh_channels_Fib[ifibcount] =
                new TH1F(Form("%sCal_channels_up", detName), Form("%sCal channels up", detName), 520, 0., 520.);
            fh_channels_Fib[ifibcount]->GetXaxis()->SetTitle("Channel number");
            fh_channels_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Channels:
            fh_channels_single_Fib[ifibcount] =
                new TH1F(Form("%sCal_channels_down", detName), Form("%sCal channels down", detName), 520, 0., 520.);
            fh_channels_single_Fib[ifibcount]->GetXaxis()->SetTitle("Channel number");
            fh_channels_single_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multihit MAPMT:
            fh_multihit_m_Fib[ifibcount] = new TH2F(
                Form("%sCal_multihit_up", detName), Form("%sCal multihits up", detName), 520, 0., 520., 20, 0., 20.);
            fh_multihit_m_Fib[ifibcount]->GetXaxis()->SetTitle("Channel");
            fh_multihit_m_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // Multihit SAPMT:
            fh_multihit_s_Fib[ifibcount] = new TH2F(
                Form("%sCal_multihit_down", detName), Form("%sCal multihits down", detName), 520, 0., 520, 20, 0., 20.);
            fh_multihit_s_Fib[ifibcount]->GetXaxis()->SetTitle("Channel");
            fh_multihit_s_Fib[ifibcount]->GetYaxis()->SetTitle("Multihit");

            // ToT raw up:
            fh_raw_tot_up[ifibcount] = new TH2F(Form("%sCal_totraw_up", detName),
                                                Form("%sCal ToT raw up", detName),
                                                N_FIBER_PLOT,
                                                0.,
                                                N_FIBER_PLOT,
                                                400,
                                                0.,
                                                100.);
            fh_raw_tot_up[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_raw_tot_up[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // ToT SAPMT:
            fh_raw_tot_down[ifibcount] = new TH2F(Form("%sCal_totraw_down", detName),
                                                  Form("%sCal ToT raw down", detName),
                                                  N_FIBER_PLOT,
                                                  0.,
                                                  N_FIBER_PLOT,
                                                  400,
                                                  0.,
                                                  100.);
            fh_raw_tot_down[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_raw_tot_down[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            fh_chan_corell[ifibcount] = new TH2F(Form("%sCal_chan_vs_chan", detName),
                                                 Form("%sCal ch_up vs ch_down", detName),
                                                 512,
                                                 0.,
                                                 512,
                                                 512,
                                                 0.,
                                                 512.);
            fh_chan_corell[ifibcount]->GetXaxis()->SetTitle("Channel number down");
            fh_chan_corell[ifibcount]->GetYaxis()->SetTitle("Channel number up");

            // Hit level
            // Fibers:
            fh_fibers_Fib[ifibcount] =
                new TH1F(Form("%sHit_fibers", detName), Form("%sHit fibers", detName), N_FIBER_PLOT, 0., N_FIBER_PLOT);
            fh_fibers_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_fibers_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // Multiplicity (number of hit fibers):
            fh_mult_Fib[ifibcount] = new TH1F(Form("%sHit_mult", detName), Form("%sHit mult", detName), 100, 0., 100.);
            fh_mult_Fib[ifibcount]->GetXaxis()->SetTitle("Multiplicity");
            fh_mult_Fib[ifibcount]->GetYaxis()->SetTitle("Counts");

            // ToT :
            fh_ToT_Fib[ifibcount] = new TH2F(
                Form("%sHit_tot", detName), Form("%sHit ToT", detName), N_FIBER_PLOT, 0., N_FIBER_PLOT, 400, 0., 100.);
            fh_ToT_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToT_Fib[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            // Time of fiber:
            fh_time_Fib[ifibcount] = new TH2F(Form("%sHit_TimevsFiber", detName),
                                              Form("%sHit Time vs Fiber", detName),
                                              N_FIBER_PLOT,
                                              0.,
                                              N_FIBER_PLOT,
                                              2048,
                                              -1024.,
                                              1024.);
            fh_time_Fib[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_time_Fib[ifibcount]->GetYaxis()->SetTitle("tMAPMT-tSPMT");

            // Not-calibrated position:
            fh_Fib_pos[ifibcount] = new TH2F(Form("%sHit_pos", detName),
                                             Form("%sHit Not-calibrated position", detName),
                                             2000,
                                             -100,
                                             100,
                                             2000,
                                             -100,
                                             100);
            fh_Fib_pos[ifibcount]->GetXaxis()->SetTitle("x position");
            fh_Fib_pos[ifibcount]->GetYaxis()->SetTitle("y position");

            // hit fiber number vs. event number:
            fh_Fib_vs_Events[ifibcount] = new TH2F(Form("%sHit_fib_vs_event", detName),
                                                   Form("%sHit Fiber # vs. Event #", detName),
                                                   10000,
                                                   0,
                                                   5e6,
                                                   N_FIBER_PLOT,
                                                   0.,
                                                   N_FIBER_PLOT);
            fh_Fib_vs_Events[ifibcount]->GetYaxis()->SetTitle("Fiber number");
            fh_Fib_vs_Events[ifibcount]->GetXaxis()->SetTitle("Event number");

            fh_ToTmax_Fibmax[ifibcount] = new TH2F(Form("%sHit_totMax_iFibMax", detName),
                                                   Form("%sHit ToTMax vs iFIbMax", detName),
                                                   N_FIBER_PLOT,
                                                   0.,
                                                   N_FIBER_PLOT,
                                                   400,
                                                   0.,
                                                   100.);
            fh_ToTmax_Fibmax[ifibcount]->GetXaxis()->SetTitle("Fiber number");
            fh_ToTmax_Fibmax[ifibcount]->GetYaxis()->SetTitle("ToT / ns");

            FibCanvas[ifibcount]->Divide(4, 4);
            if (fCalItems.at(DET_FI_FIRST + ifibcount))
            {
                FibCanvas[ifibcount]->cd(1);
                fh_channels_Fib[ifibcount]->Draw();
                FibCanvas[ifibcount]->cd(2);
                gPad->SetLogz();
                fh_multihit_m_Fib[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(3);
                fh_channels_single_Fib[ifibcount]->Draw();
                FibCanvas[ifibcount]->cd(4);
                gPad->SetLogz();
                fh_multihit_s_Fib[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(5);
                // gPad->SetLogz();
                // fh_chan_corell[ifibcount]->Draw("colz");
            }
            if (fHitItems.at(DET_FI_FIRST + ifibcount))
            {
                FibCanvas[ifibcount]->cd(6);
                gPad->SetLogz();
                fh_raw_tot_up[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(7);
                gPad->SetLogz();
                fh_raw_tot_down[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(9);
                fh_fibers_Fib[ifibcount]->Draw();
                FibCanvas[ifibcount]->cd(10);
                fh_mult_Fib[ifibcount]->Draw();
                FibCanvas[ifibcount]->cd(11);
                gPad->SetLogz();
                fh_ToT_Fib[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(12);
                gPad->SetLogz();
                fh_ToTmax_Fibmax[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(13);
                gPad->SetLogz();
                fh_time_Fib[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(14);
                gPad->SetLogz();
                fh_Fib_vs_Events[ifibcount]->Draw("colz");
                FibCanvas[ifibcount]->cd(15);
                gPad->SetLogz();
                fh_Fib_pos[ifibcount]->Draw("colz");
            }
            FibCanvas[ifibcount]->cd(0);
            // uncomment lines below when ucesb avaliable
            run->AddObject(FibCanvas[ifibcount]);
            run->GetHttpServer()->RegisterCommand("Reset_Fiber", Form("/Tasks/%s/->Reset_Fiber_Histo()", GetName()));
        } // end if(Mapped)

    } // end for(ifibcount)

    // -------------------------------------------------------------------------

    return kSUCCESS;
}
void R3BOnlineSpectraFiber_s494::Reset_Fiber()
{
    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fCalItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_channels_Fib[ifibcount]->Reset();
            fh_mult_Fib[ifibcount]->Reset();
            fh_multihit_m_Fib[ifibcount]->Reset();
            fh_multihit_s_Fib[ifibcount]->Reset();
            fh_channels_single_Fib[ifibcount]->Reset();
        }
        if (fHitItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_time_Fib[ifibcount]->Reset();
            fh_fibers_Fib[ifibcount]->Reset();
            fh_ToT_Fib[ifibcount]->Reset();
            fh_Fib_vs_Events[ifibcount]->Reset();
            fh_Fib_pos[ifibcount]->Reset();
            fh_Fib_vs_Events[ifibcount]->Reset();
            fh_ToTmax_Fibmax[ifibcount]->Reset();
            //  fh_chan_corell[ifibcount]->Reset();
            fh_raw_tot_up[ifibcount]->Reset();
            fh_raw_tot_down[ifibcount]->Reset();
        }
    }
}
void R3BOnlineSpectraFiber_s494::Exec(Option_t* option)
{
    fNEvents += 1;
    if (fNEvents / 10000. == (int)fNEvents / 10000)
        cout << "Events: " << fNEvents << flush << '\r';

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(ERROR) << "FairRootManager not found";
        return;
    }

    if (header)
    {
        if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
            return;

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
    }
    //----------------------------------------------------------------------
    // Fiber detectors
    //----------------------------------------------------------------------

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        Int_t iFib = 0;
        Double_t dtime = 0.0 / 0.0, dtimeMax;
        Double_t yfib = -100000., xfib = -100000., tfibMax = 0. / 0.;
        Double_t rfib = 0.0 / 0.0;
        Int_t iFibMax;
        Double_t totMax;
        Double_t tofMax;
        Double_t tot_up = 0., tot_down = 0.;
        Int_t iCha = 0;
        Int_t iCha_up = 0, iCha_down = 0;
        Double_t tFib = 0. / 0.;
        Int_t iSide;
        Double_t c_period = 4096. * (1000. / fClockFreq);

        auto detCal = fCalItems.at(DET_FI_FIRST + ifibcount);
        auto detHit = fHitItems.at(DET_FI_FIRST + ifibcount);

        if (detCal)
        {

            Int_t nCals = detCal->GetEntriesFast();
            //       if (nCals > 100)
            //          continue;

            std::vector<UInt_t> upmt_num(512); // up
            std::vector<UInt_t> dpmt_num(512); // down
            for (Int_t ical = 0; ical < nCals; ical++)
            {
                R3BFiberMAPMTCalData* hit = (R3BFiberMAPMTCalData*)detCal->At(ical);
                if (!hit)
                    continue;

                // channel numbers are stored 1-based (1..n)
                iCha = hit->GetChannel(); // 1..
                iSide = hit->GetSide();   // 0 = down, 1=up, 2=trigegr
                if (iSide == 1 && hit->IsLeading())
                {
                    iCha_up = iCha;
                    fh_channels_Fib[ifibcount]->Fill(iCha); // Fill which channel has events
                    ++upmt_num.at(iCha - 1);                // multihit of a given up killom channel
                }

                if (iSide == 0 && hit->IsLeading())
                {
                    iCha_down = iCha;
                    fh_channels_single_Fib[ifibcount]->Fill(iCha); // Fill which channel has events
                    ++dpmt_num.at(iCha - 1);                       // multihit of a given down killom channel
                }
            }

            //     fh_chan_corell[ifibcount]->Fill(iCha_down, iCha_up);

            for (int i = 0; i < 512; ++i)
            {
                auto m = upmt_num.at(i);
                if (m > 0)
                    fh_multihit_m_Fib[ifibcount]->Fill(i + 1, m); // multihit of a given up killom channel

                auto s = dpmt_num.at(i);
                if (s > 0)
                {
                    fh_multihit_s_Fib[ifibcount]->Fill(i + 1, s);
                } // multihit of a given down killom channel
            }
        }

        if (detHit)
        {
            Int_t nHits = detHit->GetEntriesFast();
            Double_t xposMax = 0. / 0.;
            Double_t yposMax = 0. / 0.;
            Double_t tfib = 0. / 0.;
            Double_t totMax_MA = 0.;
            Double_t spmtMax;
            Double_t mapmtMax;
            Double_t totMax_S = 0.;
            iFibMax = -1000;
            totMax = 0.;
            dtimeMax = 0. / 0.;

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                Double_t tDown = 0. / 0.;
                Double_t tUp = 0. / 0.;

                R3BFiberMAPMTHitData* hit = (R3BFiberMAPMTHitData*)detHit->At(ihit);
                if (!hit)
                    continue;

                iFib = hit->GetFiberId(); // 1..

                // times
                tUp = hit->GetTopTime_ns();
                tDown = hit->GetBottomTime_ns();
                dtime = tUp - tDown;

                // Not-calibrated ToF:
                tfib = hit->GetTime();

                // if not resonable y-position, go to next
                //  if (ypos < 70. || ypos > 100.)  continue;

                // non-calibrated ToT from up and down MAPMT:
                Double_t ToT_up = hit->GetTopToT_ns();
                Double_t ToT_down = hit->GetBottomToT_ns();
                fh_raw_tot_up[ifibcount]->Fill(iFib, ToT_up);
                fh_raw_tot_down[ifibcount]->Fill(iFib, ToT_down);

                Double_t ToT = hit->GetEloss();
                Double_t xpos = hit->GetX();
                Double_t ypos = hit->GetY();
                //  if(ypos>70. && ypos<100.){
                if (ToT > totMax)
                {
                    totMax = ToT;
                    iFibMax = iFib;
                    tfibMax = tfib;
                    dtimeMax = dtime;
                    xposMax = xpos;
                    yposMax = ypos;
                }
                //  }

                //    if(hit->GetSPMTToT_ns() > 0){
                fh_fibers_Fib[ifibcount]->Fill(iFib);
                fh_ToT_Fib[ifibcount]->Fill(iFib, ToT);
                fh_time_Fib[ifibcount]->Fill(iFib, tfib);
                fh_Fib_vs_Events[ifibcount]->Fill(fNEvents, iFib);

                // }

                // if not both times present, go to next
                //    if (!(tMAPMT > 0.) || !(tSPMT > 0.))  continue;
            } // end for(ihit)

            fh_ToTmax_Fibmax[ifibcount]->Fill(iFibMax, totMax);
            fh_Fib_pos[ifibcount]->Fill(xposMax, yposMax);

            if (nHits > 0)
                fh_mult_Fib[ifibcount]->Fill(nHits);

        } // end if(aHit[ifibcount])
    }     // end for(ifibcount)
}

void R3BOnlineSpectraFiber_s494::FinishEvent()
{

    for (Int_t det = 0; det < DET_MAX; det++)
    {

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

void R3BOnlineSpectraFiber_s494::FinishTask()
{

    for (Int_t ifibcount = 0; ifibcount < NOF_FIB_DET; ifibcount++)
    {
        if (fCalItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_channels_Fib[ifibcount]->Write();
            fh_mult_Fib[ifibcount]->Write();
            fh_multihit_m_Fib[ifibcount]->Write();
            fh_multihit_s_Fib[ifibcount]->Write();
            fh_channels_single_Fib[ifibcount]->Write();
        }
        if (fHitItems.at(DET_FI_FIRST + ifibcount))
        {
            fh_time_Fib[ifibcount]->Write();
            fh_fibers_Fib[ifibcount]->Write();
            fh_ToT_Fib[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_Fib_pos[ifibcount]->Write();
            fh_Fib_vs_Events[ifibcount]->Write();
            fh_ToTmax_Fibmax[ifibcount]->Write();
            fh_chan_corell[ifibcount]->Write();
            fh_raw_tot_up[ifibcount]->Write();
            fh_raw_tot_down[ifibcount]->Write();
        }
    }
}

ClassImp(R3BOnlineSpectraFiber_s494)
