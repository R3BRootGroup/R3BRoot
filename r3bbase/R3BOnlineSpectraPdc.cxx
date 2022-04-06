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

R3BOnlineSpectraPdc::~R3BOnlineSpectraPdc()
{
    //	delete fhTpat ;
    //	delete fhTrigger;
    //	delete fh_SEETRAM;

    for (int i = 0; i < N_PLANE_MAX_PDC; i++)
    {
        delete fh_Pdc_Wire[i];
        delete fh_Pdc_Tot[i];
        delete fh_Pdc_ToF[i];
        delete fh_Pdc_xy[i];
        delete fh_Pdc_x[i];
        delete fh_Pdc_y[i];
        delete fh_Pdc_Time[i];
        delete fh_Pdc_Wire_vs_Events[i];
    }
}

InitStatus R3BOnlineSpectraPdc::Init()
{

    // Initialize random number:
    std::srand(std::time(0)); // use current time as seed for random generator

    LOG(INFO) << "R3BOnlineSpectraPdc::Init ";

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
        LOG(fatal) << "Branch PdcCal not found";

    fCalItems = (TClonesArray*)mgr->GetObject("PdcCal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch PdcCal not found";

    fHitItems = (TClonesArray*)mgr->GetObject("PdcHit");
    if (NULL == fHitItems)
        LOG(fatal) << "Branch PdcCal not found";

    //------------------------------------------------------------------------
    // create histograms of all detectors
    //------------------------------------------------------------------------

    // Trigger and Tpat
    TCanvas* cTrigg = new TCanvas("Trigg", "Triggers", 50, 50, 500, 500);
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

    //---------------------------------------------------------------------------------------------------
    // PDC detector

    if (fMappedItems)
    {
        TCanvas* cPdc_planes = new TCanvas("PDC_planes", "PDC planes", 50, 50, 500, 500);
        cPdc_planes->Divide(5, 4);

        TCanvas* cPdc = new TCanvas("PDC", "PDC", 50, 50, 500, 500);
        cPdc->Divide(1, 4);

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
            fh_Pdc_Tot[j] = new TH2F(strName3, strName4, 150, 0, 150, 100, 0., 100.);
            fh_Pdc_Tot[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_Tot[j]->GetYaxis()->SetTitle("ToT / ns");

            char strName5[255];
            sprintf(strName5, "ped_ToF_plane_%d", j + 1);
            char strName6[255];
            sprintf(strName6, "PDC ToF plane %d", j + 1);
            fh_Pdc_ToF[j] = new TH2F(strName5, strName6, 150, 0, 150, 1000, 0, 100);
            fh_Pdc_ToF[j]->GetXaxis()->SetTitle("Wire");
            fh_Pdc_ToF[j]->GetYaxis()->SetTitle("ToF / ns");

            char strName7[255];
            sprintf(strName7, "pdc_time_plane_%d", j + 1);
            char strName8[255];
            sprintf(strName8, "PDC Time-trigger time plane %d", j + 1);
            fh_Pdc_Time[j] = new TH2F(strName7, strName8, 150, 0, 150, 1000, -1000, 1000);
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
            fh_Pdc_Wire_vs_Events[j] = new TH2F(strName15, strName16, 1000, 0, 10000000, 150, 0, 150);
            fh_Pdc_Wire_vs_Events[j]->GetXaxis()->SetTitle("Event number");
            fh_Pdc_Wire_vs_Events[j]->GetYaxis()->SetTitle("Wire number");


            /*
                        char strName9[255];
                        sprintf(strName9, "tofd_multihit_plane_%d", j + 1);
                        char strName10[255];
                        sprintf(strName10, "Tofd multihit plane %d", j + 1);
                        fh_tofd_multihit[j] = new TH2F(strName9, strName10, 45, 0., 45., 10, 0, 10);
                        fh_tofd_multihit[j]->GetXaxis()->SetTitle("Bar number");
                        fh_tofd_multihit[j]->GetYaxis()->SetTitle("Multihit");
            */
        }

        for (Int_t j = 0; j < N_PLANE_MAX_PDC; j++)
        {
            cPdc_planes->cd(j * 5 + 1);
            fh_Pdc_Wire[j]->Draw();
            cPdc_planes->cd(j * 5 + 2);
            // gPad->SetLogz();
            fh_Pdc_Tot[j]->Draw("colz");
            cPdc_planes->cd(j * 5 + 3);
            // gPad->SetLogz();
            fh_Pdc_Time[j]->Draw("colz");
            cPdc_planes->cd(j * 5 + 4);
            if(j == 0 || j == 2)
            {
				fh_Pdc_x[j]->Draw();
			}
			else
			{
				fh_Pdc_y[j]->Draw();
			}
            cPdc_planes->cd(j * 5 + 5);
            // gPad->SetLogz();
            fh_Pdc_xy[j]->Draw("colz");
            
            cPdc->cd(j + 1);
            fh_Pdc_Wire_vs_Events[j]->Draw("colz");
            
        }

        cPdc_planes->cd(0);
        run->AddObject(cPdc_planes);
        cPdc->cd(0);
        run->AddObject(cPdc);

        run->GetHttpServer()->RegisterCommand("Reset_PDC", Form("/Tasks/%s/->Reset_PDC_Histo()", GetName()));


    }

    return kSUCCESS;
}

void R3BOnlineSpectraPdc::Reset_PDC_Histo()
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

void R3BOnlineSpectraPdc::Exec(Option_t* option)
{
	fNEvents++;
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
    {
        // FairLogger::GetLogger()->Fatal(MESSAGE_ORIGIN, "FairRootManager not found");
        LOG(ERROR) << "FairRootManager not found";
        return;
    }
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
        // cout << "MappedItems" << endl;
        auto det = fMappedItems;
        Int_t nMapped = det->GetEntriesFast();
        for (Int_t imapped = 0; imapped < nMapped; imapped++)
        {
            auto mapped = (R3BPdcMappedData const*)det->At(imapped);
            if (!mapped)
                continue; // should not happen

            Int_t const iPlane = mapped->GetPlaneId(); // 1..n
            Int_t const iWire = mapped->GetWireId();   // 1..n
            Int_t const iEdge = mapped->GetEdgeId();
            //cout << "Plane: " << iPlane << " Wire: " << iWire << endl;
            Int_t wire = iWire;
/*
			if(wire%8 == 1) wire=wire;
			else if(wire%8 == 2) wire=wire+1;
			else if(wire%8 == 3) wire=wire+2;
			else if(wire%8 == 4) wire=wire+3;
			else if(wire%8 == 5) wire=wire-3;
			else if(wire%8 == 6) wire=wire-2;
			else if(wire%8 == 7) wire=wire-1;
			else if(wire%8 == 0) wire=wire;
*/
            if (iPlane <= N_PLANE_MAX_PDC)
            {
                fh_Pdc_Wire[iPlane - 1]->Fill(wire);
                fh_Pdc_Wire_vs_Events[iPlane - 1]->Fill(fNEvents,wire);
            }
        }
    }

    if (fCalItems)
    {
        // cout << "CalItems" << endl;
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

            // cout << "Plane: " << plane << " Wire: " << wire << endl;
            // fh_Pdc_Time[plane-1]->Fill(wire,t);
        }
    }

    if (fHitItems)
    {
        auto det = fHitItems;
        Int_t nHits = det->GetEntriesFast();
        cout << "HitItems: " << nHits << endl;
        Double_t t0 = -10000.;
        Int_t plane = 0;
        Int_t wire = 0;
        Double_t x[4], y[4], eloss[4], t[4];
        for (Int_t i = 0; i < 4; i++)
        {
            x[i] = -10000.;
            y[i] = -10000;
            eloss[i] = 0.;
            t[i] = -10000.;
        }

		Double_t x_prev = -10.;
		Double_t y_prev = -10.;
		Double_t xPair = -10.;
		Double_t yPair = -10.;

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

			if (hit->GetX() > 0 )
			{				
				fh_Pdc_x[hit->GetDetId() - 1]->Fill(x[plane - 1]);			
				xPair = x[plane - 1];
			}
			
			if (hit->GetY() > 0 )
			{
				fh_Pdc_y[hit->GetDetId() - 1]->Fill(y[plane - 1]);	
				yPair = y[plane - 1];	
			}
        }
		if (xPair > 0 && yPair > 0)
		{
			fh_Pdc_xy[1]->Fill(xPair,yPair);   
		}
    }
    fNEvents += 1;
}

void R3BOnlineSpectraPdc::FinishEvent()
{
    fMappedItems->Clear();
    fCalItems->Clear();
    fHitItems->Clear();
}

void R3BOnlineSpectraPdc::FinishTask()
{
    if (fMappedItems)
    {
        for (Int_t i = 0; i < 4; i++)
        {
            // fh_Pdc_ToF[i]->Write();
            fh_Pdc_x[i]->Write();
            fh_Pdc_y[i]->Write();
            fh_Pdc_xy[i]->Write();
            fh_Pdc_Wire[i]->Write();
            fh_Pdc_Tot[i]->Write();
            fh_Pdc_Time[i]->Write();
            fh_Pdc_Wire_vs_Events[i]->Write();
        }
    }
}

ClassImp(R3BOnlineSpectraPdc)