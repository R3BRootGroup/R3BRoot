// ------------------------------------------------------------
// -----                  R3BPspxOnlineSpectra            -----
// -----          Created Feb 23th 2019 by J.L. Rodriguez -----
// ------------------------------------------------------------

/*
 * This task should fill histograms with detector variables which allow
 * to test the detectors online
 *
 */

#include "R3BPspxOnlineSpectra.h"
#include "R3BPspxCalData.h"
#include "R3BPspxHitData.h"
#include "R3BPspxMappedData.h"
#include "R3BPspxPrecalData.h"

#include "R3BEventHeader.h"

#include "FairLogger.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRunOnline.h"
#include "FairRuntimeDb.h"
#include "TCanvas.h"
#include "TGaxis.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TFolder.h"

#include "THttpServer.h"

#include "TClonesArray.h"
#include "TMath.h"
#include <TRandom3.h>
#include <TRandomGen.h>
#include <array>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

R3BPspxOnlineSpectra::R3BPspxOnlineSpectra()
    : FairTask("PspOnlineSpectra", 1)
    , fPrecalItemsPspx(NULL)
    , fHitItemsPspx(NULL)
    , fMappedItemsPspx(NULL)
    , fTrigger(-1)
    , fNEvents(0)
{
}

R3BPspxOnlineSpectra::R3BPspxOnlineSpectra(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItemsPspx(NULL)
    , fPrecalItemsPspx(NULL)
    , fHitItemsPspx(NULL)
    , fTrigger(-1)
    , fNEvents(0)
{
}

R3BPspxOnlineSpectra::~R3BPspxOnlineSpectra()
{
}

InitStatus R3BPspxOnlineSpectra::Init()
{
    LOG(INFO) << "R3BPspxOnlineSpectra::Init " << FairLogger::endl;
    // try to get a handle on the EventHeader. EventHeader may not be 
    // present though and hence may be null. Take care when using.

    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
      LOG(fatal)<<"FairRootManager not found"<< FairLogger::endl;
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");

    FairRunOnline *run = FairRunOnline::Instance();
    run->GetHttpServer()->Register("", this);

    // create histograms of all detectors
    //
    // PSPX detector
    // get access to data
    fMappedItemsPspx = (TClonesArray*)mgr->GetObject("PspxMappedData");
    fPrecalItemsPspx = (TClonesArray*)mgr->GetObject("PspxPrecalData");
    fCalItemsPspx = (TClonesArray*)mgr->GetObject("PspxCalData");
    fHitItemsPspx = (TClonesArray*)mgr->GetObject("PspxHitData");


    //MAIN FOLDER-AMS
    TFolder* mainfolPspx = new TFolder("PSPX","PSPX info");  


    TCanvas* cPspx_comp = new TCanvas("Pspx_comp", "Pspx Comparison", 10, 10, 1100, 1000);
    cPspx_comp->Divide(2, 3);

    Int_t Emax = 500000;

    if (fMappedItemsPspx)
    {
        // LOG(INFO) << "Init MappedPspx" << FairLogger::endl;

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            if (i % 2 == 0)
            { // even numbers = read out with energy filter
                fh_pspx_channel_x[i] =
                    new TH1F(Form("pspx_%d_energyreadout_channel_x", i / 2),
                             Form("Pspx %d Energy Readout: x Channel;x Position / Channel;Counts", i / 2 + 1),
                             2 * N_STRIPS_PSPX,
                             1,
                             2 * N_STRIPS_PSPX + 1);
                fh_pspx_channel_y[i] =
                    new TH1F(Form("pspx_%d_energyreadout_channel_y", i / 2),
                             Form("Pspx %d Energy Readout: y Channel;y Position / Channel;Counts", i / 2 + 1),
                             2 * N_STRIPS_PSPX,
                             2 * N_STRIPS_PSPX + 1,
                             4 * N_STRIPS_PSPX + 1);

                fh_pspx_multiplicity_x[i] =
                    new TH1F(Form("pspx_%d_energyreadout_multiplicity_x", i / 2),
                             Form("Pspx %d Energy Readout: x Multiplicity;Multiplicity;Counts", i / 2 + 1),
                             10,
                             0,
                             10);
                fh_pspx_multiplicity_y[i] =
                    new TH1F(Form("pspx_%d_energyreadout_multiplicity_y", i / 2),
                             Form("Pspx %d Energy Readout: y Multiplicity;Multiplicity;Counts", i / 2 + 1),
                             10,
                             0,
                             10);
                fh_pspx_strips_position[i] = new TH2F(
                    Form("pspx_%d_energyreadout_strips", i),
                    Form("Pspx %d Energy Readout Position;x Position / Strips; y Position / Strips", i / 2 + 1),
                    N_STRIPS_PSPX,
                    1,
                    N_STRIPS_PSPX + 1,
                    N_STRIPS_PSPX,
                    N_STRIPS_PSPX + 1,
                    2 * N_STRIPS_PSPX + 1);
            }
            else
            { // odd numbers = read out with position filter
                fh_pspx_channel_x[i] =
                    new TH1F(Form("pspx_%d_positionreadout_channel_x", i / 2),
                             Form("Pspx %d Position Readout: x Channel;x Position / Channel;Counts", i / 2 + 1),
                             2 * N_STRIPS_PSPX,
                             1,
                             2 * N_STRIPS_PSPX + 1);
                fh_pspx_channel_y[i] =
                    new TH1F(Form("pspx_%d_positionreadout_channel_y", i / 2),
                             Form("Pspx %d Position Readout: y Channel;y Position / Channel;Counts", i / 2 + 1),
                             2 * N_STRIPS_PSPX,
                             2 * N_STRIPS_PSPX + 1,
                             4 * N_STRIPS_PSPX + 1);

                fh_pspx_multiplicity_x[i] =
                    new TH1F(Form("pspx_%d_positionreadout_multiplicity_x", i / 2),
                             Form("Pspx %d Position Readout: x Multiplicity;Multiplicity;Counts", i / 2 + 1),
                             10,
                             0,
                             10);
                fh_pspx_multiplicity_y[i] =
                    new TH1F(Form("pspx_%d_positionreadout_multiplicity_y", i / 2),
                             Form("Pspx %d Position Readout: y Multiplicity;Multiplicity;Counts", i / 2 + 1),
                             10,
                             0,
                             10);
                fh_pspx_strips_position[i] = new TH2F(
                    Form("pspx_%d_positionreadout_strips", i),
                    Form("Pspx %d Position Readout Position;x Position / Strips; y Position / Strips", i / 2 + 1),
                    N_STRIPS_PSPX,
                    1,
                    N_STRIPS_PSPX + 1,
                    N_STRIPS_PSPX,
                    N_STRIPS_PSPX + 1,
                    2 * N_STRIPS_PSPX + 1);
            }
        }

        TCanvas* cPspx_strips = new TCanvas("Pspx_strips", "Pspx Strips", 10, 10, 1100, 1000);
        cPspx_strips->Divide(N_PSPX, 2);

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            cPspx_strips->cd(i + 1);
            fh_pspx_channel_x[i]->Draw();

            cPspx_strips->cd(i + 1 + N_PSPX);
            fh_pspx_channel_y[i]->Draw();
        }

        mainfolPspx->Add(cPspx_strips);

        TCanvas* cPspx_multiplicity = new TCanvas("Pspx_multiplicity", "Pspx Multiplicity", 10, 10, 1100, 1000);
        cPspx_multiplicity->Divide(N_PSPX, 2);

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            cPspx_multiplicity->cd(i + 1);
            fh_pspx_multiplicity_x[i]->Draw();

            cPspx_multiplicity->cd(i + 1 + N_PSPX);
            fh_pspx_multiplicity_y[i]->Draw();
        }

        mainfolPspx->Add(cPspx_multiplicity);

        // Fill cPspx_comp with Mapped level data
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            cPspx_comp->cd(i + 1);
            fh_pspx_strips_position[i]->Draw("colz");

            // Remove the current axis
            fh_pspx_strips_position[i]->GetYaxis()->SetLabelOffset(999);
            fh_pspx_strips_position[i]->GetYaxis()->SetTickLength(0);

            // Redraw the new inverted axis
            gPad->Update();
            TGaxis* newaxis = new TGaxis(gPad->GetUxmin(),
                                         gPad->GetUymax(),
                                         gPad->GetUxmin() - 0.001,
                                         gPad->GetUymin(),
                                         fh_pspx_strips_position[i]->GetYaxis()->GetXmin(),
                                         fh_pspx_strips_position[i]->GetYaxis()->GetXmax(),
                                         510,
                                         "+");
            newaxis->SetLabelOffset(0.003);
            newaxis->SetLabelSize(0.03);
            newaxis->SetTickLength(0.025);
            newaxis->Draw();
        }
    }

    if (fCalItemsPspx)
    {
        UInt_t nbins = 200;

        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            fh_pspx_cal_energy_frontback[i] =
                new TH2F(Form("pspx_%d_energy_frontback", i),
                         Form("Pspx %d Energy Back vs Front;Front Energy / arb.u.;Back Energy / arb.u.", i + 1),
                         nbins,
                         0,
                         Emax,
                         nbins,
                         -Emax,
                         0);
        }

        // Fill cPspx_comp with Cal level data
        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            cPspx_comp->cd(i + 1 + N_PSPX); //i*2
            fh_pspx_cal_energy_frontback[i]->Draw("colz");
        }
    }

    if (fHitItemsPspx)
    {
        UInt_t nbins = 100;
        UInt_t length = 10; // detector length, range of histogram

        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            fh_pspx_hit_position[i] = new TH2F(Form("pspx_%d_position_cm", i),
                                               Form("Pspx %d Position;x Position / cm; y Position / cm", i + 1),
                                               nbins,
                                               -length / 2.,
                                               length / 2.,
                                               nbins,
                                               -length / 2.,
                                               length / 2.);

            fh_pspx_hit_energy[i] = new TH1F(
                Form("pspx_%d_energy", i), Form("Pspx %d Energy;Energy / arb.u.; Counts", i + 1), nbins, 0, Emax);
        }

        TCanvas* cPspx_hit = new TCanvas("Pspx_hit", "Pspx Hit", 10, 10, 1100, 1000);
        cPspx_hit->Divide((N_PSPX + 1) / 2, 2);

        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            cPspx_hit->cd(i + 1);
            fh_pspx_hit_position[i]->Draw("colz");

            cPspx_hit->cd(i + 1 + (N_PSPX + 1) / 2);
            fh_pspx_hit_energy[i]->Draw();
        }

        mainfolPspx->Add(cPspx_hit);

        // Fill cPspx_comp with Hit level data-((channel_y[i][0] + 1) / 2) + 3 * N_STRIPS_PSPX + 1)
        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            cPspx_comp->cd(i * 2 + 2 * N_PSPX + 1); // supposed to be +2 if
                                                    // energy and position
                                                    // readout is used
            fh_pspx_hit_position[i]->Draw("colz");
        }
    }


    mainfolPspx->Add(cPspx_comp);
    run->AddObject(mainfolPspx);

    run->GetHttpServer()->RegisterCommand("Reset_PSPX", Form("/Objects/%s/->Reset_PSPX_Histo()", GetName()));
    
    // -------------------------------------------------------------------------

    return kSUCCESS;
}


void R3BPspxOnlineSpectra::Reset_PSPX_Histo()
{
    if (fMappedItemsPspx)
    {
        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            fh_pspx_channel_x[i]->Reset();
            fh_pspx_channel_y[i]->Reset();
            fh_pspx_multiplicity_x[i]->Reset();
            fh_pspx_multiplicity_y[i]->Reset();
            fh_pspx_strips_position[i]->Reset();
        }
    }
    if (fCalItemsPspx)
    {
        for (UInt_t i = 0; i < N_PSPX / 2; i++)
        {
            fh_pspx_cal_energy_frontback[i]->Reset();
        }
    }
    if (fHitItemsPspx)
    {
        for (UInt_t i = 0; i < N_PSPX / 2; i++)
        {
            fh_pspx_hit_position[i]->Reset();
            fh_pspx_hit_energy[i]->Reset();
        }
    }
}

void R3BPspxOnlineSpectra::Exec(Option_t* option)
{
  FairRootManager* mgr = FairRootManager::Instance();
  if (NULL == mgr)
    LOG(FATAL) << "R3BPspxOnlineSpectra::Exec FairRootManager not found" << FairLogger::endl;
  // check for requested trigger (Todo: should be done globablly / somewhere else)
  if ((fTrigger >= 0) && (header) && (header->GetTrigger() != fTrigger))
    return;


    //----------------------------------------------------------------------
    // PSPX
    //----------------------------------------------------------------------
    if (fMappedItemsPspx)
    {
        UInt_t mult_x[N_PSPX];
        UInt_t mult_y[N_PSPX];

        UInt_t channel_x[N_PSPX][N_STRIPS_PSPX * 2];
        UInt_t channel_y[N_PSPX][N_STRIPS_PSPX * 2];

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            mult_x[i] = 0;
            mult_y[i] = 0;

            for (UInt_t j = 0; j < N_STRIPS_PSPX * 2; j++)
            {
                channel_x[i][j] = 0;
                channel_y[i][j] = 0;
            }
        }

        Int_t nHits = fMappedItemsPspx->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            R3BPspxMappedData* mappedData = (R3BPspxMappedData*)fMappedItemsPspx->At(ihit);
            UInt_t i = mappedData->GetDetector() - 1;
                if (mappedData->GetChannel() > N_STRIPS_PSPX * 2 &&
                    mappedData->GetChannel() < N_STRIPS_PSPX * 4 + 1)
                {
                    // LOG(INFO) << "Test1 " << i << " " << mappedData->GetDetector() << " " <<
                    // mappedData->GetChannel()<< FairLogger::endl;
                    channel_y[i][mult_y[i]] = mappedData->GetChannel();
                    mult_y[i]++;
                }
                else if (mappedData->GetChannel() > 0 &&
                         mappedData->GetChannel() < N_STRIPS_PSPX * 2 + 1)
                {
                    // LOG(INFO) << "Test2 " << i << " " << mappedData->GetDetector() << " " <<
                    // mappedData->GetChannel()<< FairLogger::endl;
                    channel_x[i][mult_x[i]] = mappedData->GetChannel();
                    mult_x[i]++;
                }
            
        }

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            // LOG(INFO) << "Test3 " << i << " " << mult_x[i] << " " << mult_y[i] << FairLogger::endl;
            fh_pspx_multiplicity_x[i]->Fill(mult_x[i]);
            fh_pspx_multiplicity_y[i]->Fill(mult_y[i]);
            // LOG(INFO) << "Test4 " << fh_pspx_multiplicity_x[i]->GetBinContent(1) << FairLogger::endl;

	std:: vector<int> v_ch_x, v_ch_y;
          for(Int_t j = 0; j < mult_x[i]; j++){
            if ((channel_x[i][j] + 1)/2 == (channel_x[i][j+1] + 1)/2)
            {
			v_ch_x.push_back((channel_x[i][j]+1)/2);
            }
	   }
	  for(Int_t j = 0; j < mult_y[i]; j++){
            if ((channel_y[i][j] + 1)/2 == (channel_y[i][j+1] + 1)/2)
            {
			v_ch_y.push_back((channel_y[i][j]+1)/2);
            }
	   }
	   for (auto it_x = v_ch_x.begin(); v_ch_x.end() != it_x; ++it_x)
	   {
	     for (auto it_y = v_ch_y.begin(); v_ch_y.end() != it_y; ++it_y)
	     {
	                   fh_pspx_strips_position[i]->Fill(*it_x,
                                                 -*it_y + 3 * N_STRIPS_PSPX +
                                                     1); // with inverted axis to account for orientation
                // fh_pspx_strips_position[i]->Fill((channel_x[i][0] + 1) / 2, ((channel_y[i][0] + 1) / 2)); //without
                // inverted axis => wrong orientation y axis
                }
                }
                
                //std::cout << "x: " << mult_x[i] << ", " << (channel_x[i][0]+1)/2 << endl;
                //std::cout << "y: " << mult_y[i] << ", " << -((int)channel_y[i][0]+1)/2 + 3 * N_STRIPS_PSPX + 1<< endl;
                
            for (Int_t j = 0; j < mult_x[i]; j++)
            {
                fh_pspx_channel_x[i]->Fill(channel_x[i][j]);
            }

            for (Int_t j = 0; j < mult_y[i]; j++)
            {
                fh_pspx_channel_y[i]->Fill(channel_y[i][j]);
            }
        }
    }

    if (fCalItemsPspx)
    {
        Int_t energy_front[N_PSPX];
        Int_t energy_back[N_PSPX];

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            energy_front[i] = 0;
            energy_back[i] = 0;
        }

        Int_t nHits = fCalItemsPspx->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            for (UInt_t i = 0; i < N_PSPX; i++)
            {
                R3BPspxCalData* calData = (R3BPspxCalData*)fCalItemsPspx->At(ihit);
                if (calData->GetDetector() == i + 1 && calData->GetStrip() > N_STRIPS_PSPX &&
                    calData->GetStrip() < N_STRIPS_PSPX * 2 + 1)
                {
                    energy_back[i] += (calData->GetEnergy1() + calData->GetEnergy2());
                }
                else if (calData->GetDetector() == i + 1 && calData->GetStrip() > 0 &&
                         calData->GetStrip() < N_STRIPS_PSPX + 1)
                {
                    energy_front[i] += (calData->GetEnergy1() + calData->GetEnergy2());
                }
            }
        }

        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            // LOG(INFO) << "Test " << i << " " << energy_front[i] << " " << energy_back[i] << FairLogger::endl;
            fh_pspx_cal_energy_frontback[i]->Fill(energy_front[i], energy_back[i]);
        }
    }

    if (fHitItemsPspx)
    {
        Int_t nHits = fHitItemsPspx->GetEntriesFast();

        for (Int_t ihit = 0; ihit < nHits; ihit++)
        {
            for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
            {
                R3BPspxHitData* hitData = (R3BPspxHitData*)fHitItemsPspx->At(ihit);

                if (hitData->GetDetector() == i * 2 + 1)
                {
                    fh_pspx_hit_energy[i]->Fill(hitData->GetEnergy());
                    fh_pspx_hit_position[i]->Fill(hitData->GetX(), hitData->GetY());
                }
            }
        }
    }
    fNEvents += 1;
}

void R3BPspxOnlineSpectra::FinishEvent()
{
    if (fMappedItemsPspx)
    {
        fMappedItemsPspx->Clear();
    }
    if (fPrecalItemsPspx)
    {
        fPrecalItemsPspx->Clear();
    }
    if (fCalItemsPspx)
    {
        fCalItemsPspx->Clear();
    }
    if (fHitItemsPspx)
    {
        fHitItemsPspx->Clear();
    }
}

void R3BPspxOnlineSpectra::FinishTask()
{

    if (fMappedItemsPspx)
    {
        // LOG(INFO) << "Finish MappedPspx" << FairLogger::endl;

        for (UInt_t i = 0; i < N_PSPX; i++)
        {
            fh_pspx_channel_x[i]->Write();
            fh_pspx_channel_y[i]->Write();
            fh_pspx_multiplicity_x[i]->Write();
            fh_pspx_multiplicity_y[i]->Write();
            fh_pspx_strips_position[i]->Write();
        }
    }
    if (fCalItemsPspx)
    {
        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            fh_pspx_cal_energy_frontback[i]->Write();
        }
    }
    if (fHitItemsPspx)
    {
        for (UInt_t i = 0; i < (N_PSPX + 1) / 2; i++)
        {
            fh_pspx_hit_energy[i]->Write();
            fh_pspx_hit_position[i]->Write();
        }
    }
}

ClassImp(R3BPspxOnlineSpectra)
