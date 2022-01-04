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
// -----                 R3BTofdCal2Hit                   -----
// -----            Created June 2020 by M.Heil            -----
// ------------------------------------------------------------

#include "R3BPdcCal2Hit.h"
#include "R3BEventHeader.h"
#include "R3BPdcCalData.h"
#include "R3BPdcHitData.h"
#include "R3BTCalEngine.h"
//#include "R3BPdcHitModulePar.h"
#include "R3BPdcHitPar.h" 

#include "FairLogger.h"
#include "FairRuntimeDb.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "THnSparse.h"
#include "TGraph.h"
#include "TCanvas.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TSystem.h"

#include <iostream>
#include <sstream>
#include <string>

#include "mapping_pdc_trig.hh"

extern "C"
{
//#include "ext_data_client.h"
#include "ext_h101_pdc.h"
}
#define LENGTH(x) (sizeof x / sizeof *x)

using namespace std;
#define IS_NAN(x) TMath::IsNaN(x)

R3BPdcCal2Hit::ToT::ToT(R3BPdcCalData const* a_lead,
                        R3BPdcCalData const* a_trail,
                        Double_t a_lead_ns,
                        Double_t a_tail_ns,
                        Double_t a_tot_ns)
    : lead(a_lead)
    , trail(a_trail)
    , lead_ns(a_lead_ns)
    , tail_ns(a_tail_ns)
    , tot_ns(a_tot_ns)
{
}


R3BPdcCal2Hit::R3BPdcCal2Hit(const char* name, Bool_t a_is_calibrator, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fCalTriggerItems(NULL)
    , fHitItems(new TClonesArray("R3BPdcHitData"))
    , fIsCalibrator(a_is_calibrator)
    , fNofHitItems(0)
    , fNofHitPars(0)
    , fHitPar(NULL)
    , fnEvents(0)
    , fClockFreq(1. / CTDC_16_CLOCK_MHZ * 1000.)
{
}

R3BPdcCal2Hit::~R3BPdcCal2Hit()
{
    if (fHitItems)
    {
        delete fHitItems;
        fHitItems = NULL;
    }
    delete fHitPar;  
}
 
InitStatus R3BPdcCal2Hit::Init()
{
    pdc_trig_map_setup();

    
	fHitPar = (R3BPdcHitPar*)FairRuntimeDb::instance()->getContainer("PdcHitPar");
	if (!fHitPar)
	{
		LOG(ERROR) << "Could not get access to PdcHitPar-Container.";
		fNofHitPars = 0;
		//return kFATAL;
	}
	else
	{
		fNofHitPars = fHitPar->GetNumModulePar();
		if (fNofHitPars == 0)
		{
			LOG(ERROR) << "There are no Hit parameters in container PdcHitPar";
			//return kFATAL;
		}
	}

    auto plane_num = LENGTH(EXT_STR_h101_PDC_onion::PDC_P);
    fPlaneArray.resize(plane_num);
    auto ch_num = LENGTH(EXT_STR_h101_PDC_onion::PDC_P[0].TLCMI);
    for (auto it = fPlaneArray.begin(); fPlaneArray.end() != it; ++it)
    {
        it->resize(ch_num);
    }

    // get access to Cal data
    FairRootManager* mgr = FairRootManager::Instance();
    if (NULL == mgr)
        LOG(fatal) << "FairRootManager not found";
    header = (R3BEventHeader*)mgr->GetObject("R3BEventHeader");
    fCalItems = (TClonesArray*)mgr->GetObject("PdcCal");
    if (NULL == fCalItems)
        LOG(fatal) << "Branch PdcCal not found";
    fCalTriggerItems = (TClonesArray*)mgr->GetObject("PdcTriggerCal");
    if (NULL == fCalTriggerItems)
        LOG(fatal) << "Branch PdcTriggerCal not found";
    maxevent = mgr->CheckMaxEventNo();

    // request storage of Hit data in output tree
    mgr->Register("PdcHit", "Land", fHitItems, kTRUE);

    char chistName[255];
    char chistTitle[255];
    for (Int_t j = 0; j < N_PLANE_MAX_PDC; j++)
    {
        sprintf(chistName, "pdc_time_raw_plane_%d", j + 1);
        sprintf(chistTitle, "PDC raw times plane %d", j + 1);
		fh_time_raw[j] = new TH2F(chistName, chistTitle, 150, 0, 150., 4000, -2000., 2000.);
		fh_time_raw[j]->GetXaxis()->SetTitle("wire");
		fh_time_raw[j]->GetYaxis()->SetTitle("time / ns");
		
	}
	sprintf(chistName, "pdc_running_sum");
	sprintf(chistTitle, "PDC running sum");
	fh_running_sum = new TH1F(chistName, chistTitle, 4000, -2000, 2000.);
	fh_running_sum->GetXaxis()->SetTitle("time");
	fh_running_sum->GetYaxis()->SetTitle("radius");

	sprintf(chistName, "pdc_radius");
	sprintf(chistTitle, "PDC r1+r2");
	fh_radius = new TH1F(chistName, chistTitle, 2000, 0., 20.);
	fh_radius->GetXaxis()->SetTitle("r1+r2 in mm");
	fh_radius->GetYaxis()->SetTitle("counts");

	sprintf(chistName, "pdc_wire_cor");
	sprintf(chistTitle, "PDC wire correlation");
	fh_wire_cor = new TH2F(chistName, chistTitle, 150, 0, 150., 150, 0., 150.);
	fh_wire_cor->GetXaxis()->SetTitle("wire");
	fh_wire_cor->GetYaxis()->SetTitle("wire");



    return kSUCCESS;
}

// Note that the container may still be empty at this point.
void R3BPdcCal2Hit::SetParContainers()
{
    
	fHitPar = (R3BPdcHitPar*)FairRuntimeDb::instance()->getContainer("PdcHitPar");
	if (!fHitPar)
	{
		LOG(ERROR) << "Could not get access to PdcHitPar-Container.";
		fNofHitPars = 0;
		return;
	}
    
}

InitStatus R3BPdcCal2Hit::ReInit()
{
    SetParContainers();
    return kSUCCESS;
}

void R3BPdcCal2Hit::Exec(Option_t* option)
{
    if (0 == fnEvents % 10000)
    {
        std::cout << "\rEvents: " << fnEvents << " / " << maxevent << " (" << (int)(fnEvents * 100. / maxevent)
                  << " %) " << std::flush;
    }
    for (auto it = fPlaneArray.begin(); fPlaneArray.end() != it; ++it)
    {
        auto& ch_arr = *it;
        for (auto it2 = ch_arr.begin(); ch_arr.end() != it2; ++it2)
        {
            it2->lead_list.clear();
            it2->tot_list.clear();
        }
    }

    // Make direct mapping tables for trigger items.
    size_t trig_num = fCalTriggerItems->GetEntries();
    std::vector<R3BPdcCalData const*> trig_table(LENGTH(EXT_STR_h101_PDC_onion::PDC_P));
    for (size_t j = 0; j < trig_num; ++j)
    {
        auto cal = (R3BPdcCalData const*)fCalTriggerItems->At(j);
        trig_table.at(cal->GetWireId() - 1) = cal;
    }

    // Read in data
    // Find multi-hit ToT for every channel.
    // The easiest safe way to survive ugly cases is to record all
    // leading edges per channel, and then pair up with whatever
    // trailing we have.
    // Not super efficient, but shouldn't crash if the data is not
    // perfect.
    size_t cal_num = fCalItems->GetEntriesFast();
    unsigned n_lead = 0;
    unsigned n_trail = 0;
    for (size_t j = 0; j < cal_num; ++j)
    {
        auto cur_cal = (R3BPdcCalData const*)fCalItems->At(j);
        if (cur_cal->GetEdgeId() == 1)
        {
            ++n_lead;
            auto plane_i = cur_cal->GetPlaneId() - 1;
            auto wire_i = cur_cal->GetWireId() - 1;
            auto& channel = fPlaneArray.at(plane_i).at(wire_i);
            channel.lead_list.push_back(cur_cal);
            //cout << "leading wire:" << wire_i <<  " time: " << cur_cal->GetTime_ns() << endl;
        }
        else
        {
            ++n_trail;
        }
    }
    if (n_lead != n_trail)
    {
        cout << "Number of leading edges not equal to number of trailing edges!" << endl;
        //    return;
    }

    for (size_t j = 0; j < cal_num; ++j)
    {
        auto cur_cal = (R3BPdcCalData const*)fCalItems->At(j);
        if (cur_cal->GetEdgeId() == 2)
        {
            Double_t c_period = 4096. * (1000. / fClockFreq);

            auto plane_i = cur_cal->GetPlaneId() - 1;
            auto wire_i = cur_cal->GetWireId() - 1;
            auto& channel = fPlaneArray.at(plane_i).at(wire_i);
            //cout << "trailing wire:" << wire_i <<  " time: " << cur_cal->GetTime_ns() << endl;
            if (channel.lead_list.empty())
            {
                cout << "lead list empty for plane, wire " << plane_i + 1 << "  " << wire_i + 1 << endl;
                continue;
            }
            auto lead = channel.lead_list.front();

            Double_t cur_cal_trig_ns = 0;
            Double_t lead_trig_ns = 0;

            assert(plane_i == lead->GetPlaneId() - 1);
            assert(wire_i == lead->GetWireId() - 1);

            // Trigger time is the same for leading and trailing,
            // but we still need to subtract it for when we want the
            // edge times.

            auto cur_cal_trig_i = g_pdc_trig_map[plane_i][wire_i];
            auto cur_cal_trig = trig_table.at(cur_cal_trig_i);
            cur_cal_trig_ns = cur_cal_trig->GetTime_ns();

            auto cur_cal_ns =
                fmod(cur_cal->GetTime_ns() - cur_cal_trig_ns + c_period + c_period / 2, c_period) - c_period / 2;
            auto lead_ns =
                fmod(lead->GetTime_ns() - cur_cal_trig_ns + c_period + c_period / 2, c_period) - c_period / 2;
            auto tot_ns = fmod(cur_cal_ns - lead_ns + c_period + c_period / 2, c_period) - c_period / 2;

            /*
            cout << plane_i << ' ' << wire_i << '\n';
            cout << "Lead: " << lead_ns << " = " << lead->GetTime_ns() << " - " << cur_cal_trig_ns << endl;
            cout << "Trail: " << cur_cal_ns << " = " << cur_cal->GetTime_ns() << " - " << cur_cal_trig_ns << endl; 
            cout << "ToT: " << tot_ns << " = " << cur_cal_ns << " - " << lead_ns << endl;
            */
            if (tot_ns > 0 && tot_ns < 1000)
            {
                channel.tot_list.push_back(ToT(lead, cur_cal, lead_ns, cur_cal_ns, tot_ns));
                channel.lead_list.pop_front();
            }
        }
    }
    cout << "***************** new event ****************" << endl;
	Int_t ID_old = 0;
	Int_t ID_new = 0;
	Double_t t_old = 0.;
	Double_t t_new = 0.;
	Double_t r_old = 0.;
	Double_t r_new = 0.;
	Double_t tot_old = 0.;
	Double_t tot_new = 0.;
	Int_t wire_old = 0;
	Int_t wire_new = 0;

    for (auto it = fPlaneArray.begin(); fPlaneArray.end() != it; ++it)
    {
        auto& ch_arr = *it;
        for (auto it2 = ch_arr.begin(); ch_arr.end() != it2; ++it2)
        {
            auto const& channel = *it2;
                       
            for (auto it_pdc_tot = channel.tot_list.begin(); channel.tot_list.end() != it_pdc_tot;
                 ++it_pdc_tot) // over ihit(fiber)            			
            {
                auto const& pdc_tot = *it_pdc_tot;
                auto plane = pdc_tot.lead->GetPlaneId();
                auto wire = pdc_tot.lead->GetWireId();
                cout << "wire before: " << wire << endl;
                if(wire%8 == 5) wire=wire+3;
                else if(wire%8 == 6) wire=wire+1;
                else if(wire%8 == 7) wire=wire-1;
                else if(wire%8 == 0) wire=wire-3;
                cout << "wire after: " << wire << endl;
                
                auto tot_pdc = pdc_tot.tot_ns;
                Double_t t_pdc = pdc_tot.lead_ns;





				// make wire correlation plot
				auto& ch_arr1 = *it;
				for (auto it3 = ch_arr1.begin(); ch_arr1.end() != it3; ++it3)
				{
					auto const& channel1 = *it3;
							   
					for (auto it_pdc_tot1 = channel1.tot_list.begin(); channel1.tot_list.end() != it_pdc_tot1;
						 ++it_pdc_tot1) // over ihit(fiber)            			
					{
						auto const& pdc_tot1 = *it_pdc_tot1;
						auto plane1 = pdc_tot1.lead->GetPlaneId();
						auto wire1 = pdc_tot1.lead->GetWireId();
						cout << "wire1 before: " << wire1 << endl;
						if(wire1%8 == 5) wire1=wire1+3;
						else if(wire1%8 == 6) wire1=wire1+1;
						else if(wire1%8 == 7) wire1=wire1-1;
						else if(wire1%8 == 0) wire1=wire1-3;
						cout << "wire1 after: " << wire1 << endl;

						auto tot_pdc1 = pdc_tot1.tot_ns;
						Double_t t_pdc1 = pdc_tot1.lead_ns;

						if(wire != wire1) fh_wire_cor->Fill(wire, wire1);


					}
				}

				fh_time_raw[plane-1] -> Fill(wire, t_pdc);
                cout << "BC plane: " << plane <<  " time: " << t_pdc << " wire: " << wire << " ToT: " << tot_pdc << endl;
				
				//cuts
				if(tot_pdc > 30.)
				{
					//continue;
				}
				
                cout << "plane: " << plane <<  " time: " << t_pdc << " wire: " << wire << " ToT: " << tot_pdc << endl;
				
				Double_t r = 0.;
				Int_t ID = 0;
				Double_t tmin = 0.;
				Double_t tmax = 0.;
				Double_t p1 = 0.;
				Double_t p2 = 0.;
				Double_t p3 = 0.;
				Double_t p4 = 0.;
				Double_t p5 = 0.;
				ID = plane;
				
				if (!fIsCalibrator && fHitPar)
				{
					R3BPdcHitModulePar* par = fHitPar->GetModuleParAt(wire);
					if (par)
					{
						tmin = par->GetTmin();
						tmax = par->GetTmax();
						p1 = par->GetPar1();
						p2 = par->GetPar2();
						p3 = par->GetPar3();
						p4 = par->GetPar4();
						p5 = par->GetPar5();
						cout << "Parameter: " << tmin << "  " << tmax << "  " << 
						p1 << "  " << p2 << "  " << p3 << "  " << p4 << "  " << p5 << endl;

					}
					// calculate radius 
					//Double_t wire_distance = 7.8;
					if (t_pdc < tmin)
					{
						r = 0.;
					}
					else if (t_pdc > tmax)
					{
						r = wire_distance;
					}
					else
					{
						Double_t m = wire_distance / (tmax - tmin);
						Double_t b = -m * tmin;
						//r = m * t_pdc + b;
						r = p1 + p2*t_pdc + p3*t_pdc*t_pdc + p4*t_pdc*t_pdc*t_pdc + p5*t_pdc*t_pdc*t_pdc*t_pdc;
					}
					cout << "Radius: " << r << endl;
					
				}
				
				// store first hit in wire
				ID_new = ID;
				t_new = t_pdc;
				r_new = r;
				tot_new = tot_pdc;
				wire_new = wire;
				
                // cout << "Hit level ID: " << ID << " x: " << x << " y: " << y << " ToT: " << tot_pdc << " t: " <<
                // t_pdc << endl;
            }
            if(ID_new == ID_old && wire_new - 1 == wire_old)
            {
				Double_t pos = 0.;
				Double_t x = 0.;
				Double_t y = 0.;
				cout << "found Pair!" << endl;
				if(ID_new == 1)
				{
					fh_radius->Fill(r_new + r_old);
				}
				if (!fIsCalibrator && fHitPar)
				{					
					pos = ((wire_new * wire_distance - r_new) + (r_old + wire_old * wire_distance)) / 2.;
				}
				else
				{
					pos = ((wire_new * wire_distance) + (wire_old * wire_distance)) / 2.;
				}

				if (ID_new & 1)
				{
					x = pos ;
					y = 0;
				}
				else
				{
					x = 0;
					y = pos;
				}


                new ((*fHitItems)[fNofHitItems++]) R3BPdcHitData((t_new + t_old) / 2., x, 
                y, sqrt(tot_new*tot_old), ID_new, wire_new);
				
                cout << "Hit level ID: " << ID_new << " x: " << x << " y: " << y << " ToT: " << 
                sqrt(tot_new*tot_old) << " t: " << (t_new + t_old) / 2. << " wire: " << wire_new << endl;
				
				
				
			}
			ID_old = ID_new;
			t_old = t_new;
			r_old = r_new;
			tot_old = tot_new;
			wire_old = wire_new;
        }
    }

    // Sort hits im time.

    fnEvents++;
}

void R3BPdcCal2Hit::FinishEvent()
{
    if (fHitItems)
    {
        fHitItems->Clear();
        fNofHitItems = 0;
    }
}

void R3BPdcCal2Hit::FinishTask() 
{
    for (Int_t j = 0; j < N_PLANE_MAX_PDC; j++)
    {
		fh_time_raw[j]->Write();
	}
	fh_radius->Write();
	fh_wire_cor->Write();
	
    if (fIsCalibrator)
    {
		TCanvas* cfit = new TCanvas("cfit", "fit", 50, 50, 500, 500);
		cfit->Clear();
		cfit->Divide(1,2);

        R3BPdcHitModulePar* mpar;

        UInt_t max = N_WIRE_MAX;
        if (fh_time_raw[0]->GetNbinsX() < N_WIRE_MAX)
        {
            max = fh_time_raw[0]->GetNbinsX();
		}
		
        for (UInt_t i = 1; i <= max; i++)
        {
            mpar = new R3BPdcHitModulePar();
            mpar->SetWire(i);
            fHitPar->AddModulePar(mpar);
        }
        for (UInt_t i = 1; i <= max; i++)
        {
            // find maximum value of drift times
            TH1D* proj = fh_time_raw[0]->ProjectionY("", i + 1, i + 1, 0);

			// make running sum of drift times
			if(i >= 0)
			{
				Double_t sum = 0;

				Double_t y[4000], x[4000];
				Double_t y1[4000], x1[4000];
				Int_t n = 0;
				Int_t n1 = 0;
				Double_t center = 0;
				//Double_t wire_distance = 7.8; // in mm
				TGraph* gr1 = new TGraph();
				TGraph* gr2 = new TGraph();
				Double_t tmin = -10000.;
				Double_t tmax = -10000.;
				sum = 0.;
				for (UInt_t j = 1; j < proj->GetNbinsX()-2; j++)
				{
					sum = sum + proj->GetBinContent(j)/proj->GetSum() * wire_distance;
					//cout << "sum: " << sum << "  " << proj->GetBinContent(j)/proj->GetSum() * 8. << endl;
					//cout << "Bin: " << proj->GetBinCenter(j) <<  "  " << proj->GetBinContent(j) << endl;
					fh_running_sum->Fill(proj->GetBinCenter(j), sum);
					if(sum > 0.1 && tmin < -9999) 
					{
						tmin = proj->GetBinCenter(j);
					}
					if(sum > wire_distance-0.1 && tmax < -9999) 
					{
						tmax = proj->GetBinCenter(j);
					}
					if(sum > 0.1 && sum < wire_distance-0.1)
					{
						x[n] = proj->GetBinCenter(j);
						y[n] = sum ;
						n++;
					}		
				}

				
				// Fitting pol
				cfit->cd(1);
				gr1 = new TGraph(n, x, y);
				gr1->SetTitle("Points found for fitting; drift times in ns; radius in mm");
				gr1->Draw("A*");
				TF1* f2 = new TF1("f2", "pol4", -400, 400);
				f2->SetLineColor(3);
				gr1->Fit("f2", "Q", "", -400, 400);
				Double_t p[5];
				std::cout << "Parameter pol: \n";
				for (Int_t j = 0; j < 5; j++)
				{
					p[j] = f2->GetParameter(j);
					std::cout << "Parameter: " << p[j] << "\n";
				}
				
				cfit->cd(2);

				n1 = n;
				for (Int_t j = 0; j < n; j++)
				{
					x1[j] = x[j];
					y1[j] = y[j] - (p[0] + p[1]*x[j] + p[2]*x[j]*x[j] + p[3]*x[j]*x[j]*x[j] + p[4]*x[j]*x[j]*x[j]*x[j]);
				}
				gr2 = new TGraph(n1, x1, y1);
				gr2->SetTitle("Residuals");
				gr2->Draw("A*");
				
				
				cfit->Update();
				gPad->WaitPrimitive();
				//gSystem->Sleep(3000);
				// time offset 
				R3BPdcHitModulePar* par2 = fHitPar->GetModuleParAt(i);
				par2->SetTmin(tmin);
				par2->SetTmax(tmax);
				par2->SetPar1(p[0]);
				par2->SetPar2(p[1]);
				par2->SetPar3(p[2]);
				par2->SetPar4(p[3]);
				par2->SetPar5(p[4]);

				cout << "Set parameter of wire: " << i << " tmin: " << tmin << " tmax: " << tmax << endl;
				cout << "Parameter: " << p[0] << "  " << p[1] << "  "<< p[2] << "  " 
				<< p[3] << "  "<< p[4] << endl;
			}



        }

        fHitPar->setChanged();
    }

	fh_running_sum->Write();
	
}

ClassImp(R3BPdcCal2Hit)
