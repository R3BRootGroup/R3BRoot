
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
// -----                  R3BOnlineSpectraBMON_S494                -----
// -----          Created April 13th 2016 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */
#include "R3BOnlineSpectraBMON_S494.h"

#include "R3BRoluCalData.h"
#include "R3BRoluMappedData.h"

#include "R3BBeamMonitorMappedData.h"

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

#include <boost/multi_array.hpp>
#include <cassert>
#include <cmath> // NAN

template <class T>
static void init_array(T& array, double init)
{
    // boost::multi_array prefers cascaded for loops over index sets
    // (e.g. python's dict().keys())
    // thus we can not really use a range based for loop
    // and instead wrangle pointers like it is 1980 again.
    for (double* e = array.origin(); e < array.origin() + array.num_elements(); ++e)
        *e = init;
}

using namespace std;

R3BOnlineSpectraBMON_S494::R3BOnlineSpectraBMON_S494()
    : R3BOnlineSpectraBMON_S494("OnlineSpectra", 1)
{
}

R3BOnlineSpectraBMON_S494::R3BOnlineSpectraBMON_S494(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fClockFreq(1. / VFTX_CLOCK_MHZ * 1000.)
    , fNEvents(0)
{
}

R3BOnlineSpectraBMON_S494::~R3BOnlineSpectraBMON_S494()
{
    //	delete fhTpat ;
    //	delete fhTrigger;
    //	delete fh_SEETRAM;
}

InitStatus R3BOnlineSpectraBMON_S494::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraBMON_S494::Init ";

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

    // Trigger and Tpat
    TCanvas* cTrigg = new TCanvas("Trigg", "Triggers", 10, 10, 650, 350);
    fhTpat = new TH1F("Tpat", "Tpat", 20, 0, 20);
    fhTpat->GetXaxis()->SetTitle("Tpat value");

    fhTrigger = new TH1F("Trigger", "Trigger all", 20, 0, 20);
    fhTrigger->GetXaxis()->SetTitle("Trigger value");

    cTrigg->Divide(2, 1);
    cTrigg->cd(1);
    gPad->SetLogy();
    fhTrigger->Draw();
    cTrigg->cd(2);
    gPad->SetLogy();
    fhTpat->Draw();
    cTrigg->cd(0);

    //------------------------------------------------------------------------
    // Rolu detector

    if (fMappedItems.at(DET_ROLU))
    {
        TCanvas* cRolu = new TCanvas("Rolu", "Rolu", 10, 10, 800, 400);

        fh_rolu_tot = new TH2F("Rolu_tot", "ROLU ToT", 10, 0, 10, 3000, 0, 300);
        fh_rolu_tot->GetXaxis()->SetTitle("Channel number");
        fh_rolu_tot->GetYaxis()->SetTitle("ToT / ns");

        fh_rolu_channels = new TH1F("Rolu_channels", "ROLU channels", 10, 0, 10);
        fh_rolu_channels->GetXaxis()->SetTitle("Channel number");
        fh_rolu_channels->GetYaxis()->SetTitle("Counts");

        cRolu->Divide(2, 1);
        cRolu->cd(1);
        gPad->SetLogy();
        fh_rolu_channels->Draw();
        cRolu->cd(2);
        gPad->SetLogz();
        fh_rolu_tot->Draw("colz");
        cRolu->cd(0);
        run->AddObject(cRolu);

        run->GetHttpServer()->RegisterCommand("Reset_ROLU", Form("/Tasks/%s/->Reset_ROLU_Histo()", GetName()));
    }

    if (fMappedItems.at(DET_BMON))
    {

        // get the theoretical calib factors for SEETRAM
        Double_t fexp = float(fsens_SEE + 9);
        Double_t fpow = float(pow(10., fexp));
        calib_SEE = 104457.9 * fpow;
        cout << "SEETRAM: " << fsens_SEE << ", " << fexp << ", " << fpow << ", " << calib_SEE << endl;
        // get the theoretical calib factors for IC
        Double_t fexp_ic = float(fsens_IC + 7);
        Double_t fpow_ic = float(pow(10., fexp_ic));
        calib_IC = 5551. * fpow_ic;
        cout << "IC     : " << fsens_IC << ", " << fexp_ic << ", " << fpow_ic << ", " << calib_IC << endl;

        TCanvas* cbmon = new TCanvas("Beam_Monitor", "Beam Monitors", 820, 10, 900, 900);
        Int_t Nbin_bmon = reset_time / read_time;

        fh_spill_length = new TH1F("spill_length", "Spill ", Nbin_bmon, 0, reset_time);
        fh_spill_length->GetXaxis()->SetTitle("time / sec");

        fh_IC = new TH1F("IC", "IC ", Nbin_bmon, 0, reset_time);
        fh_IC->GetXaxis()->SetTitle("time / sec");
        fh_IC->GetYaxis()->SetTitle("IC counts");

        fh_SEE = new TH1F("SEETRAM", "SEETRAM ", Nbin_bmon, 0, reset_time);
        fh_SEE->GetXaxis()->SetTitle("time / sec");
        fh_SEE->GetYaxis()->SetTitle("SEETRAM counts");

        fh_TOFDOR = new TH1F("TOFDOR", "TOFDOR ", Nbin_bmon, 0, reset_time);
        fh_TOFDOR->GetXaxis()->SetTitle("time / sec");
        fh_TOFDOR->GetYaxis()->SetTitle("TOFDOR counts");

        fh_SROLU1 = new TH1F("SROLU1", "SROLU1 ", Nbin_bmon, 0, reset_time);
        fh_SROLU1->GetXaxis()->SetTitle("time / sec");
        fh_SROLU1->GetYaxis()->SetTitle("SROLU1 counts");

        fh_SROLU2 = new TH1F("SROLU2", "SROLU2 ", Nbin_bmon, 0, reset_time);
        fh_SROLU2->GetXaxis()->SetTitle("time / sec");
        fh_SROLU2->GetYaxis()->SetTitle("SROLU2 counts");

        fh_IC_spill = new TH1F("IC_spill", "IC particles rate in kHz ", Nbin_bmon, 0, reset_time);
        fh_IC_spill->GetXaxis()->SetTitle("time / sec");
        fh_IC_spill->GetYaxis()->SetTitle("Particles / kHz");

        fh_SEE_spill = new TH1F("SEE_spill", "SEE particles rate in kHz", Nbin_bmon, 0, reset_time);
        fh_SEE_spill->GetXaxis()->SetTitle("time / sec");
        fh_SEE_spill->GetYaxis()->SetTitle("Particles / kHz");

        fh_SEE_spill_raw = new TH1F("SEE_spill_raw", "SEE count rate in kHz", Nbin_bmon, 0, reset_time);
        fh_SEE_spill_raw->GetXaxis()->SetTitle("time / sec");
        fh_SEE_spill_raw->GetYaxis()->SetTitle("Counts / kHz");

        fh_TOFDOR_spill = new TH1F("TOFDOR_spill", "TOFDOR rate in kHz ", Nbin_bmon, 0, reset_time);
        fh_TOFDOR_spill->GetXaxis()->SetTitle("time / sec");
        fh_TOFDOR_spill->GetYaxis()->SetTitle("TOFDOR rate / kHz");

        fh_SROLU1_spill = new TH1F("SROLU1_spill", "SROLU1 rate in kHz ", Nbin_bmon, 0, reset_time);
        fh_SROLU1_spill->GetXaxis()->SetTitle("time / sec");
        fh_SROLU1_spill->GetYaxis()->SetTitle("SROLU1 rate / kHz");

        fh_SROLU2_spill = new TH1F("SROLU2_spill", "SROLU2 rate in kHz ", Nbin_bmon, 0, reset_time);
        fh_SROLU2_spill->GetXaxis()->SetTitle("time / sec");
        fh_SROLU2_spill->GetYaxis()->SetTitle("SROLU2 rate / kHz");

        fh_SEE_TOFDOR = new TH1F("SEE_vs_TOFDOR", "SEETRAM vs TOFDOR ", 20000, 0, 20000);
        fh_SEE_TOFDOR->GetYaxis()->SetTitle("SEETRAM counts/spill");
        fh_SEE_TOFDOR->GetXaxis()->SetTitle("TOFDOR counts/spill");

        fh_IC_TOFDOR = new TH1F("IC_vs_TOFDOR", "IC vs TOFDOR ", 20000, 0, 20000);
        fh_IC_TOFDOR->GetYaxis()->SetTitle("IC counts/spill");
        fh_IC_TOFDOR->GetXaxis()->SetTitle("TOFDOR counts/spill");

        cbmon->Divide(6, 3);
        cbmon->cd(1);
        fh_spill_length->Draw();
        cbmon->cd(2);
        fh_IC->Draw("hist");
        cbmon->cd(3);
        fh_SEE->Draw("hist");
        cbmon->cd(4);
        fh_TOFDOR->Draw("hist");
        cbmon->cd(5);
        fh_SROLU1->Draw("hist");
        cbmon->cd(6);
        fh_SROLU2->Draw("hist");
        cbmon->cd(8);
        fh_IC_spill->Draw("hist");
        cbmon->cd(9);
        fh_SEE_spill->Draw("hist");
        cbmon->cd(10);
        fh_TOFDOR_spill->Draw("hist");
        cbmon->cd(11);
        fh_SROLU1_spill->Draw("hist");
        cbmon->cd(12);
        fh_SROLU2_spill->Draw("hist");
        cbmon->cd(14);
        fh_IC_TOFDOR->SetMarkerStyle(21);
        fh_IC_TOFDOR->Draw("hist p");
        cbmon->cd(15);
        fh_SEE_TOFDOR->SetMarkerStyle(21);
        fh_SEE_TOFDOR->Draw("hist p");
        cbmon->cd(17);
        fh_SEE_spill_raw->Draw("hist");

        cbmon->cd(0);

        run->AddObject(cbmon);

        run->GetHttpServer()->RegisterCommand("Reset_BMON", Form("/Tasks/%s/->Reset_BMON_Histo()", GetName()));
    }

    return kSUCCESS;
}

void R3BOnlineSpectraBMON_S494::Reset_ROLU_Histo()
{
    fh_rolu_channels->Reset();
    fh_rolu_tot->Reset();
    fhTrigger->Reset();
    fhTpat->Reset();
}

void R3BOnlineSpectraBMON_S494::Reset_BMON_Histo()
{

    fh_spill_length->Reset();
    fh_IC->Reset();
    fh_IC_spill->Reset();
    fh_SEE->Reset();
    fh_SEE_spill->Reset();
    fh_SEE_spill_raw->Reset();
    fh_TOFDOR->Reset();
    fh_TOFDOR_spill->Reset();
    fh_SROLU1->Reset();
    fh_SROLU1_spill->Reset();
    fh_SROLU2->Reset();
    fh_SROLU2_spill->Reset();
    fh_SEE_TOFDOR->Reset();
    fh_IC_TOFDOR->Reset();
}

void R3BOnlineSpectraBMON_S494::Exec(Option_t* option)
{
    fNEvents += 1;
    //  cout << "fNEvents " << fNEvents << endl;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(ERROR) << "FairRootManager not found";
        return;
    }

    time = header->GetTimeStamp();
    // time = 0;

    if (time_start == -1 && time > 0)
    {
        time_start = time;
        fNEvents_start = fNEvents;
    }

    if (header->GetTrigger() == 12)
    {
        time_spill_start = time; // header->GetTimeStamp();    // spill start in nsec
        cout << "Spill start: " << double(time_spill_start - time_start) / 1.e9 << " sec " << endl;
        // reset counters:
        see_spill = 0;
        ic_spill = 0;
        tofdor_spill = 0;
        spill_on = true;
        in_spill_off = 0;
    }
    if (header->GetTrigger() == 13)
    {
        time_spill_end = time; // header->GetTimeStamp();    // spill end  in nsec
        cout << "Spill stop: " << double(time_spill_end - time_start) / 1.e9 << " sec " << endl;
        spill_on = false;
    }

    fhTrigger->Fill(header->GetTrigger());

    Int_t tpatbin;
    for (int i = 0; i < 16; i++)
    {
        tpatbin = (header->GetTpat() & (1 << i));
        if (tpatbin != 0)
            fhTpat->Fill(i + 1);
    }

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

    if (fMappedItems.at(DET_BMON) && fMappedItems.at(DET_BMON)->GetEntriesFast() > 0)
    {

        Bool_t spectra_clear = false;
        Double_t xtime = double(time - time_start) / 1.e9;
        // for reseting spectra
        Int_t icount = iclear_count * reset_time; // reset after reset_time (sec)
        if (time_clear < 0. && int(xtime) % icount == 0 && xtime > 1.)
        {
            time_clear = xtime;
            spectra_clear = true;
        }
        unsigned long int IC;
        unsigned long int SEETRAM;
        unsigned long int TOFDOR;
        unsigned long int SROLU1;
        unsigned long int SROLU2;

        Bool_t bmon_read = false;

        auto det = fMappedItems.at(DET_BMON);
        Int_t nHitsbm = det->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHitsbm; ihit++)
        {
            R3BBeamMonitorMappedData* hit = (R3BBeamMonitorMappedData*)det->At(ihit);
            if (!hit)
                continue;

            IC = hit->GetIC();           // negative values if offset not high enough
            SEETRAM = hit->GetSEETRAM(); // negative values if offset not high enough
            TOFDOR = hit->GetTOFDOR();   // only positive values possible
            SROLU1 = hit->GetSROLU1();
            SROLU2 = hit->GetSROLU2();

            //     unsigned long long time = header->GetTimeStamp();

            if (fNEvents == fNEvents_start)
            {
                see_mem = SEETRAM;
                ic_mem = IC;
                tofdor_mem = TOFDOR;
                srolu1_mem = SROLU1;
                srolu2_mem = SROLU2;
                time_mem = time_start;
                see_first = SEETRAM;
                see_start = SEETRAM;
                ic_start = IC;
                tofdor_start = TOFDOR;
                srolu1_start = SROLU1;
                srolu2_start = SROLU2;
                time_prev_read = time_start;
            }

            if (time > 0)
            {

                fh_spill_length->Fill(double(time - time_mem) / 1e9);

                // Spectra below are filled every read_time (secs)
                if (time_to_read == 0 && (time - time_prev_read) >= read_time * 1000000000) // in nsec
                {
                    time_to_read = time;
                    bmon_read = true;
                }

                if (bmon_read)
                {
                    tdiff = double(time - time_mem) / 1.e9;
                    fNorm = 1.e-3 / (double(time - time_prev_read) / 1.e9); // kHz

                    // IC:
                    int yIC = (IC - ic_start);
                    int yIC_mem = (IC - ic_mem);
                    if (yIC > 0)
                        fh_IC->Fill(tdiff, yIC);
                    Double_t yIC_part = ((double)yIC_mem * fNorm) * calib_IC;
                    if (yIC_mem > 0 && yIC_mem_mem > 0)
                        fh_IC_spill->Fill(tdiff, yIC_part);
                    ic_mem = IC;

                    // SEETRAM:SEETRAM
                    int ySEE = (SEETRAM - see_start);
                    int ySEE_mem = (SEETRAM - see_mem);
                    if (ySEE > 0)
                        fh_SEE->Fill(tdiff, ySEE);
                    Double_t ySEE_part = ((double)ySEE_mem * fNorm) * calib_SEE;
                    if (ySEE_mem > 0 && ySEE_mem_mem > 0)
                        fh_SEE_spill->Fill(tdiff, ySEE_part);
                    if (ySEE_mem > 0 && ySEE_mem_mem > 0)
                        fh_SEE_spill_raw->Fill(tdiff, ySEE_part / calib_SEE);
                    see_mem = SEETRAM;

                    // TOFDOR: here bewusst ySEE in if!
                    int yTOFDOR = (TOFDOR - tofdor_start);
                    int yTOFDOR_mem = (TOFDOR - tofdor_mem);
                    if (ySEE > 0)
                        fh_TOFDOR->Fill(tdiff, yTOFDOR);
                    Double_t yTOFDOR_part = (double)yTOFDOR_mem * fNorm;
                    if (ySEE_mem > 0 && ySEE_mem_mem > 0)
                        fh_TOFDOR_spill->Fill(tdiff, yTOFDOR_part);
                    tofdor_mem = TOFDOR;

                    // correlations:
                    if (spill_on)
                    {
                        if (ySEE_mem > 0 && ySEE_mem_mem > 0 && yIC_mem > 0 && yIC_mem_mem > 0)
                        {
                            see_spill += ySEE_part;
                            ic_spill += yIC_part;
                            tofdor_spill += yTOFDOR_part;
                        }
                    }
                    if (!spill_on && time_spill_start > 0 && time_spill_end > 0)
                    {
                        in_spill_off += 1;
                        if (in_spill_off == 1)
                        {
                            Double_t spill_length = double(time_spill_end - time_spill_start) / 1.e9;
                            tofdor_spill = tofdor_spill / spill_length;
                            ic_spill = ic_spill / spill_length;
                            see_spill = see_spill / spill_length;
                            fh_IC_TOFDOR->Fill(tofdor_spill, ic_spill);
                            fh_SEE_TOFDOR->Fill(tofdor_spill, see_spill);
                        }
                    }
                    yTOFDOR_mem_mem = yTOFDOR_mem;
                    ySEE_mem_mem = ySEE_mem;
                    yIC_mem_mem = yIC_mem;

                    // SROLU1:
                    int ySROLU1 = (SROLU1 - srolu1_start);
                    int ySROLU1_mem = (SROLU1 - srolu1_mem);
                    if (ySROLU1 > 0)
                        fh_SROLU1->Fill(tdiff, ySROLU1);
                    if (ySROLU1_mem > 0 && ySROLU1_mem_mem > 0)
                        fh_SROLU1_spill->Fill(tdiff, (double)ySROLU1_mem * fNorm);
                    srolu1_mem = SROLU1;
                    ySROLU1_mem_mem = ySROLU1_mem;

                    // SROLU2:
                    int ySROLU2 = (SROLU2 - srolu2_start);
                    int ySROLU2_mem = (SROLU2 - srolu2_mem);
                    if (ySROLU2 > 0)
                        fh_SROLU2->Fill(tdiff, ySROLU2);
                    if ((double)ySROLU2_mem > 0 && ySROLU2_mem_mem > 0)
                        fh_SROLU2_spill->Fill(tdiff, (double)ySROLU2_mem * fNorm);
                    srolu2_mem = SROLU2;
                    ySROLU2_mem_mem = ySROLU2_mem;

                    time_to_read = 0;
                    time_prev_read = time;
                    bmon_read = false;
                }

                if (spectra_clear)
                {
                    fh_spill_length->Reset();
                    fh_IC_spill->Reset("ICESM");
                    fh_SEE_spill->Reset("ICESM");
                    fh_TOFDOR_spill->Reset("ICESM");
                    fh_SROLU1_spill->Reset("ICESM");
                    fh_SROLU2_spill->Reset("ICESM");
                    fh_IC->Reset("ICESM");
                    fh_SEE->Reset("ICESM");
                    fh_TOFDOR->Reset("ICESM");
                    fh_SROLU1->Reset("ICESM");
                    fh_SROLU2->Reset("ICESM");
                    fh_SEE_spill_raw->Reset("ICESM");
                    time_mem = time;
                    time_clear = -1.;
                    iclear_count = iclear_count + 1;
                    spectra_clear = false;
                    see_start = SEETRAM;
                    ic_start = IC;
                    tofdor_start = TOFDOR;
                    srolu1_start = SROLU1;
                    srolu2_start = SROLU2;
                }
            }
        }
    }

    // **************   ROLU *************************************

    if (fMappedItems.at(DET_ROLU))
    {
        auto det = fMappedItems.at(DET_ROLU);
        Int_t nHits = det->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BRoluMappedData* hit = (R3BRoluMappedData*)det->At(ihit);
            if (!hit)
                continue;

            // channel numbers are stored 1-based (1..n)
            Int_t iDet = hit->GetDetector(); // 1..
            Int_t iCha = hit->GetChannel();  // 1..

            if (iDet < 2)
                fh_rolu_channels->Fill(iCha); // ROLU 1
            if (iDet > 1)
                fh_rolu_channels->Fill(iCha + 4); // ROLU 2
        }
    }

    Int_t nParts;
    if (fCalItems.at(DET_ROLU))
    {

        auto det = fCalItems.at(DET_ROLU);
        nParts = det->GetEntriesFast();

        if (nParts < 1)
            return;

        Int_t iDet = 0;
        /*
         * Note: double x[nParts][2][4]={NAN};
         * will initialize everything other than x[0][0][0] to 0.0.
         * (and also not compile with our ancient gcc 4.8.5.)
         */
        using A = boost::multi_array<double, 3>;
        auto dims = boost::extents[nParts][2][4];
        A timeRolu_L(dims);
        init_array(timeRolu_L, NAN);
        A timeRolu_T(dims);
        init_array(timeRolu_T, NAN);
        A totRolu(dims);
        init_array(totRolu, NAN);

        for (Int_t iPart = 0; iPart < nParts; iPart++)
        {
            /*
             * nParts is the number of particle passing through detector in one event
             */
            R3BRoluCalData* calData = (R3BRoluCalData*)det->At(iPart);
            iDet = calData->GetDetector();

            for (Int_t iCha = 0; iCha < 4; iCha++)
            {

                if (!(IS_NAN(calData->GetTimeL_ns(iCha))))
                { // TAMEX leading
                    timeRolu_L[iPart][iDet - 1][iCha] = calData->GetTimeL_ns(iCha);
                }
                if (!(IS_NAN(calData->GetTimeT_ns(iCha))))
                { // TAMEX trailing
                    timeRolu_T[iPart][iDet - 1][iCha] = calData->GetTimeT_ns(iCha);
                }

                if (timeRolu_T[iPart][iDet - 1][iCha] > 0. && timeRolu_L[iPart][iDet - 1][iCha] > 0. &&
                    !(IS_NAN(timeRolu_T[iPart][iDet - 1][iCha])) && !(IS_NAN(timeRolu_L[iPart][iDet - 1][iCha])))
                {
                    while (timeRolu_T[iPart][iDet - 1][iCha] - timeRolu_L[iPart][iDet - 1][iCha] <= 0.)
                    {
                        timeRolu_T[iPart][iDet - 1][iCha] = timeRolu_T[iPart][iDet - 1][iCha] + 2048. * fClockFreq;
                    }

                    totRolu[iPart][iDet - 1][iCha] =
                        timeRolu_T[iPart][iDet - 1][iCha] - timeRolu_L[iPart][iDet - 1][iCha];
                }

                if (iDet < 2)
                    fh_rolu_tot->Fill(iCha + 1, totRolu[iPart][iDet - 1][iCha]);
                if (iDet > 1)
                    fh_rolu_tot->Fill(iCha + 5, totRolu[iPart][iDet - 1][iCha]);
            }

            if (!calData)
            {
                cout << "Rolu !calData" << endl;
                continue; // can this happen?
            }
        }
    }
}

void R3BOnlineSpectraBMON_S494::FinishEvent()
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

void R3BOnlineSpectraBMON_S494::FinishTask()
{

    if (fMappedItems.at(DET_ROLU))
    {
        fh_rolu_channels->Write();
        fh_rolu_tot->Write();
    }

    if (fMappedItems.at(DET_BMON))
    {
        fhTpat->Write();
        fh_spill_length->Write();
        fhTrigger->Write();
        fh_IC->Write();
        fh_SEE->Write();
        fh_TOFDOR->Write();
        fh_IC_spill->Write();
        fh_SEE_spill->Write();
        fh_SEE_spill_raw->Write();
        fh_TOFDOR_spill->Write();
        fh_SEE_TOFDOR->Write();
        fh_IC_TOFDOR->Write();
    }
}
