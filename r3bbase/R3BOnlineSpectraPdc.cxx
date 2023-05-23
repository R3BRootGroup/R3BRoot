// ------------------------------------------------------------
// -----                  R3BOnlineSpectraPdc                -----
// -----          Created July  2020 by M.Heil       -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BOnlineSpectraPdc.h"

#include "R3BPdcCalData.h"
#include "R3BPdcHitData.h"
#include "R3BPdcMappedData.h"

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

R3BOnlineSpectraPdc::R3BOnlineSpectraPdc()
    : R3BOnlineSpectraPdc("OnlineSpectra", 1)
{
}

R3BOnlineSpectraPdc::R3BOnlineSpectraPdc(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fTrigger(-1)
    , fTpat(-1)
    , fSamp(-1)
    , fNEvents(0)
{
}

R3BOnlineSpectraPdc::~R3BOnlineSpectraPdc() {}

InitStatus R3BOnlineSpectraPdc::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(info) << "R3BOnlineSpectraPdc::Init ";

    // try to get a handle on the EventHeader. EventHeader may not be
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";

    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    FairRunOnline* run = FairRunOnline::Instance();

    run->GetHttpServer()->Register("/Tasks", this);

    // Get objects for detectors on all levels
    fMappedItems = (TClonesArray*)mgr->GetObject("PdcMapped");
    if (NULL == fMappedItems)
        LOG(fatal) << "Branch PdcMapped not found";

    fCalItems = (TClonesArray*)mgr->GetObject("PdcCal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch PdcCal not found";

    fHitItems = (TClonesArray*)mgr->GetObject("PdcHit");
    if (NULL == fHitItems)
        LOG(fatal) << "Branch PdcHit not found";

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
    // PDC detector
    if (fMappedItems)
    {
        Double_t bs = 1000. / 150. / 16.;
        TCanvas* cPdc_planes = new TCanvas("PDC_planes", "PDC planes", 50, 50, 800, 1200);
        cPdc_planes->Divide(6, 4);

        TCanvas* cPdc = new TCanvas("PDC", "PDC", 50, 50, 500, 500);
        cPdc->Divide(1, 4);

        fh_pdc_ebene4 = new TH2F("pdc_wire_vs_y_ebene4", "PDC wire vs y eben 4", 150, 0, 150, 1000, 0, 1000);
        fh_pdc_ebene4->GetXaxis()->SetTitle("wire number");
        fh_pdc_ebene4->GetYaxis()->SetTitle("y");

        for (Int_t j = 0; j < N_PLANE_MAX_PDC; j++)
        {
            char strName1[255];
            sprintf(strName1, "pdc_wire_plane_%d", j + 1);
            char strName2[255];
            sprintf(strName2, "PDC wires plane %d", j + 1);
            fh_Pdc_Wire[j] = new TH1F(strName1, strName2, 150, 0., 150.);
            fh_Pdc_Wire[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_Wire[j]->GetYaxis()->SetTitle("Counts");

            char strName11[255];
            sprintf(strName11, "pdc_x_plane_%d", j + 1);
            char strName12[255];
            sprintf(strName12, "PDC x plane %d", j + 1);
            fh_Pdc_x[j] = new TH1F(strName11, strName12, 1500, 0., 1500.);
            fh_Pdc_x[j]->GetXaxis()->SetTitle("x in mm");
            fh_Pdc_x[j]->GetYaxis()->SetTitle("Counts");

            char strName13[255];
            sprintf(strName13, "pdc_y_plane_%d", j + 1);
            char strName14[255];
            sprintf(strName14, "PDC y plane %d", j + 1);
            fh_Pdc_y[j] = new TH1F(strName13, strName14, 1500, 0., 1500.);
            fh_Pdc_y[j]->GetXaxis()->SetTitle("y in mm");
            fh_Pdc_y[j]->GetYaxis()->SetTitle("Counts");

            char strName3[255];
            sprintf(strName3, "pdc_ToT_plane_%d", j + 1);
            char strName4[255];
            sprintf(strName4, "PDC ToT plane %d", j + 1);
            fh_Pdc_Tot[j] = new TH2F(strName3, strName4, 150, 0, 150, (int)(800. / bs), 0., 800.);
            fh_Pdc_Tot[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_Tot[j]->GetYaxis()->SetTitle("ToT / ns");

            char strName23[255];
            sprintf(strName23, "pdc_mult_plane_%d", j + 1);
            char strName24[255];
            sprintf(strName24, "PDC multiplicity plane %d", j + 1);
            fh_Pdc_mult[j] = new TH2F(strName23, strName24, 150, 0, 150, 100, 0., 100.);
            fh_Pdc_mult[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_mult[j]->GetYaxis()->SetTitle("multiplicity");

            char strName5[255];
            sprintf(strName5, "pdc_ToF_plane_%d", j + 1);
            char strName6[255];
            sprintf(strName6, "PDC ToF plane %d", j + 1);
            fh_Pdc_ToF[j] = new TH2F(strName5, strName6, 150, 0, 150, (int)(100. / bs), 0, 100);
            fh_Pdc_ToF[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_ToF[j]->GetYaxis()->SetTitle("ToF / ns");

            char strName7[255];
            sprintf(strName7, "pdc_time_plane_%d", j + 1);
            char strName8[255];
            sprintf(strName8, "PDC Time-trigger time plane %d", j + 1);
            fh_Pdc_Time[j] = new TH2F(strName7, strName8, 150, 0, 150, (int)(2000. / bs), -1000, 1000);
            fh_Pdc_Time[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_Time[j]->GetYaxis()->SetTitle("Time / ns");

            char strName9[255];
            sprintf(strName9, "pdc_xy_plane_%d", j + 1);
            char strName10[255];
            sprintf(strName10, "PDC xy plane %d", j + 1);
            fh_Pdc_xy[j] = new TH2F(strName9, strName10, 1500, 0, 1500, 1000, 0, 1000);
            fh_Pdc_xy[j]->GetXaxis()->SetTitle("x in cm");
            fh_Pdc_xy[j]->GetYaxis()->SetTitle("y in cm");

            char strName15[255];
            sprintf(strName15, "pdc_wire_vs_Event_plane_%d", j + 1);
            char strName16[255];
            sprintf(strName16, "PDC wire vs. events plane %d", j + 1);
            fh_Pdc_Wire_vs_Events[j] = new TH2F(strName15, strName16, 1000, 0, 10000, 150, 0, 150);
            fh_Pdc_Wire_vs_Events[j]->GetXaxis()->SetTitle("Event number");
            fh_Pdc_Wire_vs_Events[j]->GetYaxis()->SetTitle("Wire number");
        }

        fh_Pdc_xvsx = new TH2F("PDC_x_vs_x", "PDC x vs x", 1500, 0, 1500, 1500, 0, 1500);
        fh_Pdc_xvsx->GetXaxis()->SetTitle("x1 in mm");
        fh_Pdc_xvsx->GetYaxis()->SetTitle("x3 in mm");

        fh_Pdc_xvsx_fi0 = new TH2F("PDC_x_vs_x_fi0", "PDC x vs x", 1500, 0, 1500, 1500, 0, 1500);
        fh_Pdc_xvsx_fi0->GetXaxis()->SetTitle("x1 in mm");
        fh_Pdc_xvsx_fi0->GetYaxis()->SetTitle("x3 in mm");

        fh_Pdc_yvsy = new TH2F("PDC_y_vs_y", "PDC y vs y", 1000, 0, 1000, 1000, 0, 1000);
        fh_Pdc_yvsy->GetXaxis()->SetTitle("y2 in mm");
        fh_Pdc_yvsy->GetYaxis()->SetTitle("y4 in mm");

        fh_Pdc_yvsy_fi0 = new TH2F("PDC_y_vs_y_fi0", "PDC y vs y", 1000, 0, 1000, 1000, 0, 1000);
        fh_Pdc_yvsy_fi0->GetXaxis()->SetTitle("y2 in mm");
        fh_Pdc_yvsy_fi0->GetYaxis()->SetTitle("y4 in mm");

        fh_Pdc_xvsy_fi0 = new TH2F("PDC_x_vs_y_fi0", "PDC x vs y", 1500, 0, 1500, 1500, 0, 1500);
        fh_Pdc_xvsy_fi0->GetXaxis()->SetTitle("x in mm");
        fh_Pdc_xvsy_fi0->GetYaxis()->SetTitle("y in mm");

        fh_Pdc_wvsw = new TH2F("PDC_w_vs_w", "PDC w vs w", 130, 0, 130, 130, 0, 130);
        fh_Pdc_wvsw->GetXaxis()->SetTitle("wire p2 ");
        fh_Pdc_wvsw->GetYaxis()->SetTitle("wire p4");

        /*
                for (Int_t j = 0; j < 144; j++)
                {

                    char strName17[255];
                    sprintf(strName17, "pdc_Tot_vs_Time_wire_%d", j + 1);
                    char strName18[255];
                    sprintf(strName18, "PDC ToT vs. Time wire %d", j + 1);
                    fh_Pdc_ToT_vs_Time[j] = new TH2F(strName17, strName18, 2000, -1000, 1000, 300, 0, 300);
                    fh_Pdc_ToT_vs_Time[j]->GetXaxis()->SetTitle("Time in ns");
                    fh_Pdc_ToT_vs_Time[j]->GetYaxis()->SetTitle("ToT in ns");

                    char strName19[255];
                    sprintf(strName19, "pdc_Tot_vs_Hit_wire_%d", j + 1);
                    char strName20[255];
                    sprintf(strName20, "PDC ToT vs. Hit number for wire %d", j + 1);
                    fh_Pdc_ToT_vs_Hit[j] = new TH2F(strName19, strName20, 20, 0, 20, 300, 0, 300);
                    fh_Pdc_ToT_vs_Hit[j]->GetXaxis()->SetTitle("Time in ns");
                    fh_Pdc_ToT_vs_Hit[j]->GetYaxis()->SetTitle("ToT in ns");
                }
        */
        for (Int_t j = 0; j < N_PLANE_MAX_PDC; j++)
        {
            cPdc_planes->cd(j * 6 + 1);
            fh_Pdc_Wire[j]->Draw();
            cPdc_planes->cd(j * 6 + 2);
            fh_Pdc_mult[j]->Draw("colz");
            cPdc_planes->cd(j * 6 + 3);
            fh_Pdc_Tot[j]->Draw("colz");
            cPdc_planes->cd(j * 6 + 4);
            // gPad->SetLogz();
            fh_Pdc_Time[j]->Draw("colz");
            cPdc_planes->cd(j * 6 + 5);
            if (j == 0 || j == 2)
            {
                fh_Pdc_x[j]->Draw();
            }
            else
            {
                fh_Pdc_y[j]->Draw();
                cPdc_planes->cd(j * 6 + 6);
                fh_Pdc_xy[j]->Draw("colz");
            }
            if (j == 0)
            {
                cPdc_planes->cd(j * 6 + 6);
                fh_Pdc_xvsx->Draw("colz");
            }
            if (j == 2)
            {
                cPdc_planes->cd(j * 6 + 6);
                fh_Pdc_yvsy->Draw("colz");
            }

            cPdc->cd(j + 1);
            fh_Pdc_Wire_vs_Events[j]->Draw("colz");
        }

        cPdc_planes->cd(0);
        run->AddObject(cPdc_planes);
        cPdc->cd(0);
        run->AddObject(cPdc);

        run->GetHttpServer()->RegisterCommand("Reset_PDC", Form("/Tasks/%s/->Reset_PDC_Histo()", GetName()));

        LOG(info) << "R3BOnlineSpectraPdc::Init End ";
    }
    return kSUCCESS;
}

void R3BOnlineSpectraPdc::Reset_PDC_Histo()
{

    if (fMappedItems)
    {
        for (int i = 0; i < N_PLANE_MAX_PDC; i++)
        {
            fh_Pdc_Wire[i]->Reset();
            fh_Pdc_Time[i]->Reset();
            fh_Pdc_ToF[i]->Reset();
            fh_Pdc_Tot[i]->Reset();
            fh_Pdc_xy[i]->Reset();
            fh_Pdc_x[i]->Reset();
            fh_Pdc_y[i]->Reset();
            fh_Pdc_Wire_vs_Events[i]->Reset();
        }
    }
}

void R3BOnlineSpectraPdc::Exec(Option_t* option)
{
    fNEvents++;

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
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
    // PDC
    //----------------------------------------------------------------------

    if (fMappedItems)
    {
        Int_t mult[5][145] = { 0 };

        auto det = fMappedItems;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BPdcMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen

            Int_t iPlane = mapped->GetPlaneId(); // 1..n
            Int_t iWire = mapped->GetWireId();   // 1..n
            Int_t iEdge = mapped->GetEdgeId();
            // cout << "Plane: " << iPlane << " Wire: " << iWire << " Edge: " << iEdge << endl;
            Int_t wire = iWire;

            // if(iEdge==1) cout << "Mapped Plane: " << iPlane << " wire: " << iWire << endl;
            mult[iPlane - 1][iWire - 1]++;
            if (iPlane <= N_PLANE_MAX_PDC)
            {
                fh_Pdc_Wire[iPlane - 1]->Fill(wire);
                fh_Pdc_Wire_vs_Events[iPlane - 1]->Fill(fNEvents, wire);
            }

            auto det1 = fMappedItems;
            Int_t nMapped1 = det1->GetEntriesFast();
            for (Int_t imapped1 = 0; imapped1 < nMapped1; imapped1++)
            {
                auto mapped1 = (R3BPdcMappedData const*)det1->At(imapped1);
                if (!mapped1)
                    continue; // should not happen

                Int_t iPlane1 = mapped1->GetPlaneId(); // 1..n
                Int_t iWire1 = mapped1->GetWireId();   // 1..n
                Int_t iEdge1 = mapped1->GetEdgeId();
                // cout << "Plane1: " << iPlane1 << " Wire1: " << iWire1 << " Edge1: " << iEdge1 << endl;

                if (iPlane == 2 && iPlane1 == 4)
                {
                    fh_Pdc_wvsw->Fill(iWire, iWire1);
                }
            }
        }

        for (Int_t p = 0; p < N_PLANE_MAX_PDC; p++)
        {
            for (Int_t w = 0; w < 144; w++)
            {
                // fh_Pdc_mult[p]->Fill(w, mult[p][w]);

                if (mult[p][w] > 0)
                {
                    fh_Pdc_mult[p]->Fill(w + 1, mult[p][w] / 2, 1); // divided by 2 for leading and trailing
                }
            }
        }
    }
    if (fCalItems)
    {
        auto det = fCalItems;
        Int_t nCals = det->GetEntriesFast();

        for (Int_t ical = 0; ical < nCals; ical++)
        {
            auto cal = (R3BPdcCalData const*)det->At(ical);
            if (!cal)
                continue; // should not happen

            Int_t plane = cal->GetPlaneId();
            Double_t wire = cal->GetWireId();
            Double_t edge = cal->GetEdgeId();
            Double_t t = cal->GetTime_ns();

            // cout << "Plane: " << plane << " Wire: " << wire << " edge: " << edge << endl;
            // fh_Pdc_Time[plane-1]->Fill(wire,t);
        }
    }

    Double_t yPdc2 = -1000;
    Double_t xPdc1 = -1000;
    Double_t yPdc4test = -1000;
    Double_t xPdc3test = -1000;
    Double_t yPdc4 = -1000;
    Double_t xPdc3 = -1000;
    Double_t t0 = -10000.;
    Double_t tPair[4];
    Double_t x[4], y[4], eloss[4], t[4];
    Int_t ipl[4];
    Int_t nmlt = 10;
    Double_t ppl[4][nmlt], tpl[4][nmlt], epl[4][nmlt];
    for (Int_t ip = 0; ip < 4; ip++)
    {
        for (Int_t ic = 0; ic < nmlt; ic++)
        {
            ppl[ip][ic] = -1000;
            tpl[ip][ic] = -1000;
            epl[ip][ic] = -1000;
        }
        ipl[ip] = -1;
    }

    Int_t mult_pdc4 = 0;
    Int_t mult_pdc3 = 0;

    if (fHitItems)
    {
        Int_t mult[5][145] = { 0 };
        auto det = fHitItems;
        Int_t nHits = det->GetEntriesFast();
        if (nHits > 0)
        {
            Int_t plane = 0;
            Int_t wire = 0;
            for (Int_t i = 0; i < 4; i++)
            {
                x[i] = -10000.;
                y[i] = -10000;
                eloss[i] = 0.;
                t[i] = -10000.;
            }

            Double_t x_prev = -10.;
            Double_t y_prev = -10.;
            Double_t xPair[4];
            Double_t yPair[4];
            Double_t TotMax[4];
            for (Int_t i = 0; i < 4; i++)
            {
                xPair[i] = -10.;
                yPair[i] = -10.;
                tPair[i] = -10000.;
            }

            for (Int_t ihit = 0; ihit < nHits; ihit++)
            {
                auto hit = (R3BPdcHitData const*)det->At(ihit);
                if (!hit)
                    continue; // should not happen
                if (t0 == -10000.)
                {
                    t0 = hit->GetTime();
                }

                plane = hit->GetDetId();
                wire = hit->GetWireId();
                x[plane - 1] = hit->GetX();
                y[plane - 1] = hit->GetY();
                eloss[plane - 1] = hit->GetEloss();
                t[plane - 1] = hit->GetTime();
                fh_Pdc_Tot[plane - 1]->Fill(wire, eloss[plane - 1]);
                fh_Pdc_Time[plane - 1]->Fill(wire, t[plane - 1]);

                // cout << "Hit Plane: " << plane << " wire: " << wire << endl;
                //" x: " << x[plane - 1]
                //	<< " y: " << y[plane - 1] << " eloss: " << eloss[plane - 1] << endl;

                if (plane == 3)
                {
                    mult_pdc3++;
                }
                if (plane == 4)
                {
                    mult_pdc4++;
                }

                mult[plane - 1][wire]++;

                // if (x[plane - 1] > 0 && eloss[plane - 1] > TotMax[plane - 1])
                if (x[plane - 1] > 0)
                {

                    // cout << "Plane: " << plane << " wire: " << wire << " x: " << x[plane - 1] << endl;

                    fh_Pdc_x[plane - 1]->Fill(x[plane - 1]);
                    xPair[plane - 1] = x[plane - 1];
                    TotMax[plane - 1] = eloss[plane - 1];
                }

                // if (y[plane - 1] > 0 && eloss[plane - 1] > TotMax[plane - 1])
                if (y[plane - 1] > 0)
                {

                    // cout << "Plane: " << plane << " wire: " << wire << " y: " << y[plane - 1] << endl;

                    fh_Pdc_y[plane - 1]->Fill(y[plane - 1]);
                    yPair[plane - 1] = y[plane - 1];
                    TotMax[plane - 1] = eloss[plane - 1];
                }

                if (plane == 4 && y[plane - 1] > 350. && y[plane - 1] < 390.)
                {
                    yPdc4test = y[plane - 1];
                }
            }

            if (xPdc1 > 0. && xPdc3 > 0. && yPdc2 > 0. && yPdc4 > 0.)
            {

                // cout << "Test: " << xPdc1 << "  " << yPdc2 << "  " << xPdc3 << "  " << yPdc4 << endl;
                fh_Pdc_xvsx->Fill(xPdc1, xPdc3);
                fh_Pdc_yvsy->Fill(yPdc2, yPdc4);
                fh_Pdc_xvsy_fi0->Fill((xPdc1 + xPdc3) / 2., (yPdc2 + yPdc4) / 2.);
            }
        }
    }
}

void R3BOnlineSpectraPdc::FinishEvent()
{

    if (fMappedItems)
        fMappedItems->Clear();
    if (fCalItems)
        fCalItems->Clear();
    if (fHitItems)
        fHitItems->Clear();
}

void R3BOnlineSpectraPdc::FinishTask()
{

    if (fMappedItems)
    {
        fh_Pdc_xvsx_fi0->Write();
        fh_Pdc_xvsx->Write();
        fh_Pdc_yvsy_fi0->Write();
        fh_Pdc_xvsy_fi0->Write();
        fh_Pdc_yvsy->Write();
        fh_Pdc_wvsw->Write();
        fh_pdc_ebene4->Write();
        for (Int_t i = 0; i < 4; i++)
        {
            fh_Pdc_x[i]->Write();
            fh_Pdc_y[i]->Write();
            fh_Pdc_xy[i]->Write();
            fh_Pdc_Wire[i]->Write();
            fh_Pdc_mult[i]->Write();
            fh_Pdc_Tot[i]->Write();
            fh_Pdc_Time[i]->Write();
            fh_Pdc_Wire_vs_Events[i]->Write();
        }
    }
}

ClassImp(R3BOnlineSpectraPdc)
