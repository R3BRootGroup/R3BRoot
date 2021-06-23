
// ------------------------------------------------------------
// -----                  R3BOnlineSpectra                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */
#include "R3BOnlineSpectraFiber23.h"

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
#include <map>
#include <sstream>
#include <vector>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;
namespace
{
    double c_period = 4096 * 1000. / 150;
    double c_fiber_coincidence_ns = 20; // nanoseconds.
} // namespace
R3BOnlineSpectraFiber23::R3BOnlineSpectraFiber23()
    : FairTask("OnlineSpectraFiber23", 1)
    , fTrigger(1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraFiber23::R3BOnlineSpectraFiber23(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraFiber23::~R3BOnlineSpectraFiber23()
{
}

InitStatus R3BOnlineSpectraFiber23::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraFiber23::Init ";

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
        fCalTriggerItems.push_back((TClonesArray*)mgr->GetObject(Form("%sTriggerCal", fDetectorNames[det])));
        if (NULL == fCalTriggerItems.at(det))
        {
            printf("Could not find CalTrigger data for '%s'.\n", fDetectorNames[det]);
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

    TCanvas* cXY = new TCanvas("XY_Fib23", "XvsY FIb23", 10, 10, 1100, 1100);
    cXY->Divide(2,2);
        
    if (fHitItems.at(DET_FI23A) && fHitItems.at(DET_FI23B))
    {
        fh_xy_global = new TH2F("xy_Fib23", "X vs Y from Fib23", 300, -6, 6, 300, -6, 6);
        fh_xy_global->GetXaxis()->SetTitle("x position / cm");
        fh_xy_global->GetYaxis()->SetTitle("y position / cm");

        fh_dtime_Fib23 = new TH2F("dt_Fib23", "dTime from Fib23", 500, -10, 10, 4096, -2048, 2048);
        fh_dtime_Fib23->GetXaxis()->SetTitle("Y Fib23b / cm");
        fh_dtime_Fib23->GetYaxis()->SetTitle("dTime / ns");
        
        fh_x = new TH1F("x_Fib23", "X from Fib23", 300, -6, 6);
        fh_x->GetXaxis()->SetTitle("x position / cm");
        fh_x->GetYaxis()->SetTitle("counts");
        
        fh_y = new TH1F("y_Fib23", "Y from Fib23", 300, -6, 6);
        fh_y->GetXaxis()->SetTitle("y position / cm");
        fh_y->GetYaxis()->SetTitle("counts");
        
        cXY->cd(1);
        gPad->SetLogz();
        fh_xy_global->Draw("colz");
        cXY->cd(2);
        fh_y->Draw();
        cXY->cd(3);
        fh_x->Draw();
        
        cXY->cd(0);
        run->AddObject(cXY);
        run->GetHttpServer()->RegisterCommand("Reset_Fiber", Form("/Tasks/%s/->Reset_Fiber_Histo()", GetName()));

    }

    // -------------------------------------------------------------------------

    return kSUCCESS;
}
void R3BOnlineSpectraFiber23::Reset_Fiber_Histo()
{
    if (fHitItems.at(DET_FI23A) && fHitItems.at(DET_FI23B))
    {
        fh_xy_global->Reset();
        fh_dtime_Fib23->Reset();
        fh_x->Reset();
        fh_y->Reset();
    }
}
void R3BOnlineSpectraFiber23::Exec(Option_t* option)
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

    //   if (header)
    //   {
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
    // }
    //----------------------------------------------------------------------
    // Fiber detectors
    //----------------------------------------------------------------------
    Double_t xpos_global = 0. / 0.;
    Double_t ypos_global = 0. / 0.;

    auto detFib23a = fHitItems.at(DET_FI23A);
    Int_t nHitsa = detFib23a->GetEntriesFast();
    for (Int_t ihita = 0; ihita < nHitsa; ihita++)
    {
        if (ihita < 100)
        {
            xpos_global = 0. / 0.;
            R3BFiberMAPMTHitData* hitFi23a = (R3BFiberMAPMTHitData*)detFib23a->At(ihita);
            xpos_global = hitFi23a->GetX();

            auto detFib23b = fHitItems.at(DET_FI23B);
            Int_t nHitsb = detFib23b->GetEntriesFast();
            for (Int_t ihitb = 0; ihitb < nHitsb; ihitb++) // just first hit
            {
                ypos_global = 0. / 0.;
                R3BFiberMAPMTHitData* hitFi23b = (R3BFiberMAPMTHitData*)detFib23b->At(ihitb);
                ypos_global = hitFi23b->GetY();
                auto dtime =
                    fmod(hitFi23a->GetTime() - hitFi23b->GetTime() + c_period + c_period / 2, c_period) - c_period / 2;
                fh_dtime_Fib23->Fill(hitFi23b->GetY(), dtime);

                if (std::abs(dtime) < c_fiber_coincidence_ns)
                {
                    fh_xy_global->Fill(xpos_global, ypos_global);
                    fh_x->Fill(xpos_global);
                    fh_y->Fill(ypos_global);
                }
            }
        }
    }
}

void R3BOnlineSpectraFiber23::FinishEvent()
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

void R3BOnlineSpectraFiber23::FinishTask()
{
    if (fHitItems.at(DET_FI23A) && fHitItems.at(DET_FI23B))
    {
        fh_xy_global->Write();
        fh_dtime_Fib23->Write();
        fh_x->Write();
        fh_y->Write();
    }
}

ClassImp(R3BOnlineSpectraFiber23)
