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

#include "R3BTofDCal2HitPar.h"
#include "R3BEventHeader.h"
#include "R3BLogger.h"
#include "R3BCoarseTimeStitch.h"
#include "R3BTofDHitModulePar.h"
#include "R3BTofDHitPar.h"
#include "R3BTofDMappingPar.h"
#include "R3BTofdCalData.h"

#include "FairLogger.h"
#include <FairRootManager.h>
#include "FairRtdbRun.h"
#include "FairRunIdGenerator.h"
#include "FairRuntimeDb.h"

#include "TCanvas.h"
#include "TClonesArray.h"
#include "TF1.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TLine.h"
#include "TMath.h"
#include "TProfile.h"
#include "TFile.h"
#include "TSpectrum.h"
#include "TSystem.h"
#include "TVirtualFitter.h"

#include <iostream>
#include <stdlib.h>

#define IS_NAN(x) TMath::IsNaN(x)
using namespace std;

namespace
{
    double c_range_ns = 2048. * 5.;
    double c_bar_coincidence_ns = 20.; // nanoseconds.
} // namespace

R3BTofDCal2HitPar::R3BTofDCal2HitPar()
    : R3BTofDCal2HitPar("R3BTofDCal2HitPar", 1)
{
}

R3BTofDCal2HitPar::R3BTofDCal2HitPar(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalTriggerItems(NULL)
    , fMinStats(100000)
    , fTrigger(-1)
    , fTpat(-1)
    , fNofPlanes(4)
    , fPaddlesPerPlane(44)
    , fNofModules(fNofPlanes * fPaddlesPerPlane)
    , fNEvents(0)
    , fParameter(1)
    , fHitPar(NULL)
    , fTofdY(0.)
    , fTofdQ(0.)
	, fMinQ(180)
    , fMaxQ(300)
	, fToTMin(190.)
	, fToTMax(210.)
    , fNbZPeaks(2)
    , fZfitType("pol1")
    , fTofdTotLow(0.)
    , fTofdTotHigh(200.)
    , fMapPar(NULL)
    , fTofdSmiley(true)
    , fTofdZ(false)
    , fMeanTof(20.)
    , fHeader(nullptr)
    , maxevent(0)
    , fHistoFile("")
{
	if(fParameter != 5 && fParameter !=-3 && fParameter!=7){
    for (Int_t i = 0; i < fNofPlanes; i++)
    {
        fh_tofd_TotPm[i][0] = NULL;
		fh_tofd_TotPm[i][1]=NULL;
		fh_tofd_TotPm[i][2]=NULL;
		fh_tofd_TotPm1[i] = NULL;
		fh_tofd_TotPm2[i] = NULL;
        fhTdiff[i] = NULL;
        fhTsync[i] = NULL;
	fhTofsync[i] = NULL;
	//fhlos[i] = NULL;
        for (Int_t j = 0; j < fPaddlesPerPlane; j++)
        {
		//fhlost[i][j] = NULL;
		fhtott[i][j] = NULL;
            fhLogTot1vsLogTot2[i][j] = NULL;
            fhSqrtQvsPosToT[i][j] = NULL;
            fhQvsPos[i][j] = NULL;
            fhTot1vsPos[i][j] = NULL;
            fhTot2vsPos[i][j] = NULL;
            fh1_tofsync[i][j] = NULL;
             //fhTot1vsTot2[i][j] = NULL;
         fh1_walk_b[i][j] = NULL;
	 fh1_walk_t[i][j] = NULL;
		 //fhZoffset[i][j]=NULL;
	}
    }
	}
}

R3BTofDCal2HitPar::~R3BTofDCal2HitPar()
{
	if(fParameter != 5 && fParameter != -3 && fParameter!=7){
	for (Int_t i = 0; i < fNofPlanes; i++)
	{
		//if(fhlos[i])
			//delete fhlos[i];
		if(fhTofsync[i])
			delete fhTofsync[i];
		if (fh_tofd_TotPm[i][0])
			delete fh_tofd_TotPm[i][0];
		if (fh_tofd_TotPm[i][1])
			delete fh_tofd_TotPm[i][1];
		if (fh_tofd_TotPm[i][2])
			delete fh_tofd_TotPm[i][2];
		if (fhTdiff[i])
			delete fhTdiff[i];
		if (fhTsync[i])
			delete fhTsync[i];
		for (Int_t j = 0; j < fPaddlesPerPlane; j++)
		{
			//if(fhlost[i][j])
				//delete fhlost[i][j];
			if(fhtott[i][j])
				delete fhtott[i][j];
			if (fhLogTot1vsLogTot2[i][j])
				delete fhLogTot1vsLogTot2[i][j];
			if (fhSqrtQvsPosToT[i][j])
				delete fhSqrtQvsPosToT[i][j];
			if (fhQvsPos[i][j])
				delete fhQvsPos[i][j];
			if (fhTot1vsPos[i][j])
				delete fhTot1vsPos[i][j];
			if (fhTot2vsPos[i][j])
			delete fhTot2vsPos[i][j];
			if (fh1_tofsync[i][j])
				delete fh1_tofsync[i][j];
			if( fh1_walk_t[i][j])
				delete  fh1_walk_t[i][j];
			if( fh1_walk_b[i][j])
				delete  fh1_walk_b[i][j];
			
			 //  if (fhZoffset[i][j])
			  // delete fhZoffset[i][j];
			   
		}
	}
	if (fHitPar)
	{
		delete fHitPar;
	}
	}
}

InitStatus R3BTofDCal2HitPar::Init()
{
	R3BLOG(info, "");
	FairRootManager* rm = FairRootManager::Instance();
	if (!rm)
	{
		R3BLOG(fatal, "FairRootManager not found");
		return kFATAL;
	}

	fHeader = (R3BEventHeader*)rm->GetObject("EventHeader.");
	R3BLOG_IF(fatal, NULL == fHeader, "EventHeader. not found");

	fCalData = (TClonesArray*)rm->GetObject("TofdCal");
	R3BLOG_IF(fatal, NULL == fCalData, "TofdCal not found");

	fCalTriggerItems = (TClonesArray*)rm->GetObject("TofdTriggerCal");
	R3BLOG_IF(fatal, NULL == fCalTriggerItems, "TofdTriggerCal not found");

	fHitPar = (R3BTofDHitPar*)FairRuntimeDb::instance()->getContainer("tofdHitPar");
	if (!fHitPar)
	{
		R3BLOG(error, "Could not get access to tofdHitPar container");
		return kFATAL;
	}
	if(fParameter != 5 && fParameter != -3){
		for (Int_t i = 1; i <= fNofPlanes; i++){
			for (Int_t j = 1; j <= fPaddlesPerPlane; j++){
				SetHistogramsNULL(i, j);
		}
		}

		for (Int_t i = 1; i <= fNofPlanes; i++){
			for (Int_t j = 1; j <= fPaddlesPerPlane; j++){
				if(j==1) SetHistogramsNULL(i, j);
				CreateHistograms(i, j);
			}
		}
	}
	// Definition of a time stich object to correlate times coming from different systems
	fTimeStitch = new R3BCoarseTimeStitch();


	
	return kSUCCESS;
}

void R3BTofDCal2HitPar::SetParContainers()
{
	fMapPar = (R3BTofDMappingPar*)FairRuntimeDb::instance()->getContainer("tofdMappingPar");
	R3BLOG_IF(warning, !fMapPar, "Could not get access to tofdMappingPar container");
	}

namespace
{
	uint64_t n1, n2;
};

void R3BTofDCal2HitPar::Exec(Option_t* option)
{
	//auto parX = fHitPar->GetModuleParAt(1,1);
	if(fParameter == 5 || fParameter == -3 || fParameter==7) return;
	// test for requested trigger (if possible)
	if ((fTrigger >= 0) && (fHeader) && (fHeader->GetTrigger() != fTrigger))
		return;

	// fTpat = 1-16; fTpat_bit = 0-15
	Int_t fTpat_bit = fTpat - 1;
	if (fTpat_bit >= 0)
	{
		Int_t itpat = fHeader->GetTpat();
		Int_t tpatvalue = (itpat & (1 << fTpat_bit)) >> fTpat_bit;
		if ((fHeader) && (tpatvalue == 0))
			return;
	}


    // ToFD detector
    std::vector<std::vector<UInt_t>> multihits(fNofPlanes, std::vector<UInt_t>(fPaddlesPerPlane));
    for (Int_t i = 0; i < fNofPlanes; ++i)
        for (Int_t j = 0; j < fPaddlesPerPlane; ++j)
            multihits[i][j] = 0;

    UInt_t nHits = fCalData->GetEntriesFast();

    // Organize cals into bars.
    struct Entry
    {
        std::vector<R3BTofdCalData*> top;
        std::vector<R3BTofdCalData*> bot;
    };
    std::map<size_t, Entry> bar_map;
    for (Int_t ihit = 0; ihit < nHits; ihit++)
    {
        auto* hit = (R3BTofdCalData*)fCalData->At(ihit);
        size_t idx = (hit->GetDetectorId() - 1) * fPaddlesPerPlane + (hit->GetBarId() - 1);
        auto ret = bar_map.insert(std::pair<size_t, Entry>(idx, Entry()));
        auto& vec = 1 == hit->GetSideId() ? ret.first->second.top : ret.first->second.bot;
        vec.push_back(hit);
    }

    static bool s_was_trig_missing = false;
    auto trig_num = fCalTriggerItems->GetEntriesFast();
    // Find coincident PMT hits.
    for (auto it = bar_map.begin(); bar_map.end() != it; ++it)
    {
        auto const& top_vec = it->second.top;
        auto const& bot_vec = it->second.bot;
        size_t top_i = 0;
        size_t bot_i = 0;
        for (; top_i < top_vec.size() && bot_i < bot_vec.size();)
        {
            auto top = top_vec.at(top_i);
            auto bot = bot_vec.at(bot_i);

            Int_t top_trig_i = 0;
            Int_t bot_trig_i = 0;
            if (fMapPar)
            {
                top_trig_i = fMapPar->GetTrigMap(top->GetDetectorId(), top->GetBarId(), top->GetSideId());
                bot_trig_i = fMapPar->GetTrigMap(bot->GetDetectorId(), bot->GetBarId(), bot->GetSideId());
            }

            Double_t top_trig_ns = 0, bot_trig_ns = 0;
            if (top_trig_i < trig_num && bot_trig_i < trig_num)
            {
                auto top_trig = (R3BTofdCalData const*)fCalTriggerItems->At(top_trig_i);
                auto bot_trig = (R3BTofdCalData const*)fCalTriggerItems->At(bot_trig_i);
                top_trig_ns = top_trig->GetTimeLeading_ns();
                bot_trig_ns = bot_trig->GetTimeLeading_ns();
                ++n1;
            }
            else
            {
                if (!s_was_trig_missing)
                {
                    R3BLOG(error, "Missing trigger information!");
                    s_was_trig_missing = true;
                }
                ++n2;
            }

            // Shift the cyclic difference window by half a window-length and move it back,
            // this way the trigger time will be at 0.
            auto top_ns =
                fmod(top->GetTimeLeading_ns() - top_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;
            auto bot_ns =
                fmod(bot->GetTimeLeading_ns() - bot_trig_ns + c_range_ns + c_range_ns / 2, c_range_ns) - c_range_ns / 2;

            auto dt = top_ns - bot_ns;
            // Handle wrap-around.
            auto dt_mod = fmod(dt + c_range_ns, c_range_ns);
            if (dt < 0)
            {
                // We're only interested in the short time-differences, so we
                // want to move the upper part of the coarse counter range close
                // to the lower range, i.e. we cut the middle of the range and
                // glue zero and the largest values together.
                dt_mod -= c_range_ns;
            }
            if (std::abs(dt_mod) < c_bar_coincidence_ns)
            {
                // Hit
                Int_t iPlane = top->GetDetectorId(); // 1..n
                Int_t iBar = top->GetBarId();        // 1..n
                if (iPlane > fNofPlanes)             // this also errors for iDetector==0
                {
                    R3BLOG(error, "More detectors than expected! Det: " << iPlane << " allowed are 1.." << fNofPlanes);
                    continue;
                }
                if (iBar > fPaddlesPerPlane) // same here
                {
                    R3BLOG(error, "More bars then expected! Det: " << iBar << " allowed are 1.." << fPaddlesPerPlane);
                    continue;
                }
                auto top_tot = fmod(top->GetTimeTrailing_ns() - top->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                auto bot_tot = fmod(bot->GetTimeTrailing_ns() - bot->GetTimeLeading_ns() + c_range_ns, c_range_ns);
                // register multi hits
                multihits[iPlane - 1][iBar - 1] += 1;
				
				Double_t Ctop_tot = 0./0.;
				Double_t Cbot_tot = 0./0.;

				Bool_t ToTexeption = false;
                
				if (fParameter == 1)
                {
                    // calculate tdiff
                    auto tdiff = (bot_ns) - (top_ns);
/*
				if(iPlane == 2 && iBar == 7){
					if(sqrt(top_tot*bot_tot) > fToTMin && sqrt(top_tot*bot_tot) < 500.) ToTexeption = true;
				}
				if(iPlane == 2 && iBar == 1){
					if(sqrt(top_tot*bot_tot)> fToTMin && sqrt(top_tot*bot_tot) < 350.) ToTexeption = true;
				}
				if(iPlane == 2 && iBar == 9){
					if(sqrt(top_tot*bot_tot)>fToTMin && sqrt(top_tot*bot_tot) < 500.) ToTexeption = true;
				}
*/
                    // fill control histograms
                    fhLogTot1vsLogTot2[iPlane - 1][iBar - 1]->Fill(TMath::Log(top_tot), TMath::Log(bot_tot));
                if((sqrt(top_tot*bot_tot)>fToTMin && sqrt(top_tot*bot_tot)<fToTMax) || ToTexeption){    
					fh_tofd_TotPm[iPlane - 1][0]->Fill(iBar, top_tot);
                    fh_tofd_TotPm[iPlane - 1][0]->Fill(-iBar - 1, bot_tot);
					fh_tofd_TotPm1[iPlane - 1]->Fill(iBar, top_tot);
					fh_tofd_TotPm1[iPlane - 1]->Fill(-iBar - 1, bot_tot);
				}
				if(/*top_tot>10. && */top_tot<90.){
					fh_tofd_TotPm2[iPlane - 1]->Fill(iBar, top_tot);
					cout<<"Fill Top ToT "<<top_tot<<endl;
				}
				if(/*bot_tot>10. && */bot_tot<90.){
					fh_tofd_TotPm2[iPlane - 1]->Fill(-iBar - 1, bot_tot);
					cout<<"Fill Bot ToT "<<bot_tot<<endl;
				}

                    // Time differences of one paddle; offset  histo
                    if((sqrt(top_tot*bot_tot)>fToTMin &&sqrt(top_tot*bot_tot)<fToTMax) || ToTexeption){
		    fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
		    }
                    auto posToT = TMath::Log(top_tot / bot_tot);
                   if((sqrt(top_tot*bot_tot)>fToTMin &&sqrt(top_tot*bot_tot)<fToTMax) || ToTexeption){
		    fhSqrtQvsPosToT[iPlane - 1][iBar - 1]->Fill(posToT, sqrt(top_tot * bot_tot));
		   }
                    // Time of hit
                    auto THit = (top_ns + bot_ns) / 2.;
                    if((sqrt(top_tot*bot_tot)>fToTMin &&sqrt(top_tot*bot_tot)<fToTMax) || ToTexeption){
		    fhTsync[iPlane - 1]->Fill(iBar, THit);
		    }
		    // Tof with respect LOS detector
		    auto tof = fTimeStitch->GetTime((top_ns + bot_ns) / 2. - fHeader->GetTStart(), "tamex", "vftx");
		    if((sqrt(top_tot*bot_tot)>fToTMin &&sqrt(top_tot*bot_tot)<fToTMax) || ToTexeption){
			    fh1_tofsync[iPlane - 1][iBar - 1]->Fill(tof);
			    fhTofsync[iPlane-1]->Fill(iBar,tof);
			    //cout<<"top_ns--> "<<top_ns<<endl;
			    //	if(sqrt(top_tot*bot_tot)>fToTMin && sqrt(top_tot*bot_tot)<fToTMax){
			    fh1_walk_t[iPlane-1][iBar-1]->Fill(top_tot,top_ns - fHeader->GetTStart());
			    fh1_walk_b[iPlane-1][iBar-1]->Fill(bot_tot,bot_ns - fHeader->GetTStart());

		    }
		    //auto lost = fHeader->GetTStart();
		    // if(tof>-153.){
		    // cout<<"sqrtToT"<<sqrt(top_tot*bot_tot)<<endl; 
		    //}
		    //fhlos[iPlane-1]->Fill(iBar,lost);
		    // if(iPlane==1 && iBar==11){
		    //fhlost[1][11]->Fill(THit,lost);}
		    //fhtott[iPlane-1][iBar-1]->Fill(THit,top_tot);
		    }
			
			if(fTofdQ == /*0.0*/ 8. && fParameter == 10){
				auto par = fHitPar->GetModuleParAt(iPlane, iBar);
			    if (!par)
			    {
				    R3BLOG(error,
						    "Hit par not found, Plane: " << top->GetDetectorId() << ", Bar: " << top->GetBarId());
				    continue;
			    }
	
				//Ctop_tot = top_tot * par->GetToTOffset3() - par->GetToTOffset4();
				//Cbot_tot = bot_tot * par->GetToTOffset1() - par->GetToTOffset2();
				Ctop_tot = top_tot  * par->GetToTOffset2();
				Cbot_tot = bot_tot  * par->GetToTOffset1();
				
				if(Ctop_tot > 180.){    
					fh_tofd_TotPm[iPlane - 1][0]->Fill(iBar, Ctop_tot);
					fh_tofd_TotPm1[iPlane - 1]->Fill(iBar, Ctop_tot);
					cout<<"Fill Top ToT high"<<Ctop_tot<<endl;
				}
				if(Cbot_tot > 180.){ 
                    fh_tofd_TotPm[iPlane - 1][0]->Fill(-iBar - 1, Cbot_tot);
					fh_tofd_TotPm1[iPlane - 1]->Fill(-iBar - 1, Cbot_tot);
					cout<<"Fill Bot ToT high"<<Cbot_tot<<endl;
				}
				if(/*top_tot>10. && */Ctop_tot<70.){
					fh_tofd_TotPm2[iPlane - 1]->Fill(iBar, Ctop_tot);
					cout<<"Fill Top ToT low"<<Ctop_tot<<endl;
				}
				if(/*bot_tot>10. && */Cbot_tot<70.){
					fh_tofd_TotPm2[iPlane - 1]->Fill(-iBar - 1, Cbot_tot);
					cout<<"Fill Bot ToT low"<<Cbot_tot<<endl;
				}

			    fh_tofd_TotPm[iPlane - 1][1]->Fill(iBar, top_tot * par->GetToTOffset2());
			    fh_tofd_TotPm[iPlane - 1][1]->Fill(-iBar - 1, bot_tot * par->GetToTOffset1());			  
			}

		    // prepare offset and sync calculation
		    if (fTofdQ > 0. && fParameter > 1 && fParameter != 10)
		    {	
			    
				LOG(debug) << "Fill histo for offset and sync calculation plane " << iPlane << " bar " << iBar;
			    auto par = fHitPar->GetModuleParAt(iPlane, iBar);
			    if (!par)
			    {
				    R3BLOG(error,
						    "Hit par not found, Plane: " << top->GetDetectorId() << ", Bar: " << top->GetBarId());
				    continue;
			    }
			
			    // calculate tdiff
			    auto tdiff = (bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2());
				//Ctop_tot = top_tot  * par->GetToTOffset3() - par->GetToTOffset4();
				//Cbot_tot = bot_tot  * par->GetToTOffset1() - par->GetToTOffset2();
				Ctop_tot = top_tot  * par->GetToTOffset2();
				Cbot_tot = bot_tot  * par->GetToTOffset1();
			
			    // fill control histograms
			    // fhTot1vsTot2[iPlane - 1][iBar - 1]->Fill(top_tot, bot_tot);
			    fhLogTot1vsLogTot2[iPlane - 1][iBar - 1]->Fill(TMath::Log(top_tot), TMath::Log(bot_tot));
			    fh_tofd_TotPm[iPlane - 1][0]->Fill(iBar, top_tot);
			    fh_tofd_TotPm[iPlane - 1][0]->Fill(-iBar - 1, bot_tot);
			    fh_tofd_TotPm[iPlane - 1][1]->Fill(iBar, top_tot  * par->GetToTOffset2());
			    fh_tofd_TotPm[iPlane - 1][1]->Fill(-iBar - 1, bot_tot * par->GetToTOffset1());
			    fh_tofd_TotPm[iPlane - 1][2]->Fill(iBar, top_tot  / par->GetToTOffset2());
			    fh_tofd_TotPm[iPlane - 1][2]->Fill(-iBar - 1, bot_tot / par->GetToTOffset1());
				//cout<<"ToT_t "<<top_tot<<" par_a "<<par->GetToTOffset3()<<" par_b "<<par->GetToTOffset4()<<" | ToT_b "<<bot_tot<<" par_a "<<par->GetToTOffset1()<<" par_b "<<par->GetToTOffset2()<<endl;
			
			    // Time differences of one paddle; offset  histo

			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
				    fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
			    }
			    // offset histo via ToT
			    /*if (fTofdY == 0.)
			      {
			      R3BLOG(debug, "Will prepare for offset and sync calculation");
			      posToT = TMath::Log(top_tot / bot_tot);
			      }
			      else
			      {*/
			    auto posToT = TMath::Log((Ctop_tot) / (Cbot_tot));

			    //}
		
				cout<<sqrt(Ctop_tot*Cbot_tot)<<" "<<fToTMin<<" "<<fToTMax<<" "<<Ctop_tot<<" "<<Cbot_tot<<" "<<ToTexeption<<endl;
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
				    fhSqrtQvsPosToT[iPlane - 1][iBar - 1]->Fill(posToT, sqrt(Ctop_tot * Cbot_tot));
				    //fhTot1vsPos[iPlane - 1][iBar - 1]->Fill(posToT, Cbot_tot);
				    //fhTot2vsPos[iPlane - 1][iBar - 1]->Fill(posToT, Ctop_tot);
			    }
			    // Time of hit
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
				    auto THit = (top_ns + bot_ns) / 2. - par->GetSync();
				    fhTsync[iPlane - 1]->Fill(iBar, THit);
			    }
			    // Tof with respect LOS detector
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) /*|| ToTexeption*/){
				    auto tof = fTimeStitch->GetTime((top_ns + bot_ns) / 2. - fHeader->GetTStart(), "tamex", "vftx");
				    if(fParameter == 2){
						fhTofsync[iPlane-1]->Fill(iBar,tof);
				    	fh1_tofsync[iPlane - 1][iBar - 1]->Fill(tof);	
					}
					else{
						fhTofsync[iPlane-1]->Fill(iBar,tof - par->GetTofSyncOffset());
				    	fh1_tofsync[iPlane - 1][iBar - 1]->Fill(tof - par->GetTofSyncOffset());
					}
			    }
			}
		    if(fParameter==0){

			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
				    fh1_walk_t[iPlane-1][iBar-1]->Fill(Ctop_tot,top_ns);
				    fh1_walk_b[iPlane-1][iBar-1]->Fill(Cbot_tot,bot_ns);
			    }
		    }


		    else if ((fTofdQ > 0 && fParameter > 1 && fParameter != 10) || fParameter == 3)
		    {
			
			    //	Double_t qb = 0.;
			    // get parameter
			    auto para = fHitPar->GetModuleParAt(iPlane, iBar);
			    if (!para)
			    {
				    R3BLOG(error,
						    "Hit par not found, Plane: " << top->GetDetectorId() << ", Bar: " << top->GetBarId());
				    continue;
			    }

			    // calculate tdiff with offest
			    auto tdiff = (bot_ns + para->GetOffset1()) - (top_ns + para->GetOffset2());
				//Ctop_tot = top_tot  * para->GetToTOffset3() - para->GetToTOffset4();
				//Cbot_tot = bot_tot  * para->GetToTOffset1() - para->GetToTOffset2();
				Ctop_tot = top_tot  * para->GetToTOffset2();
				Cbot_tot = bot_tot  * para->GetToTOffset1();

			    // walk corrections
			    if (para->GetPar1Walk() == 0. || para->GetPar2Walk() == 0. ||
					    para->GetPar3Walk() == 0. || para->GetPar4Walk() == 0. || para->GetPar5Walk() == 0.){
				    R3BLOG(warning, "Walk correction not found!");
			    }else{

				    bot_ns = bot_ns - walk(Cbot_tot,
						    para->GetPar1Walk(),
						    para->GetPar2Walk(),
						    para->GetPar3Walk(),
						    para->GetPar4Walk(),
						    para->GetPar5Walk());
				    top_ns = top_ns - walk(Ctop_tot,
						    para->GetPar1Walk(),
						    para->GetPar2Walk(),
						    para->GetPar3Walk(),
						    para->GetPar4Walk(),
						    para->GetPar5Walk());
			    }
			    auto tof = fTimeStitch->GetTime((top_ns + bot_ns) / 2. - fHeader->GetTStart(), "tamex", "vftx");
			    if((sqrt(top_tot*bot_tot)>fToTMin && sqrt(top_tot*bot_tot)<fToTMax) || ToTexeption){
				    fhTofsync[iPlane-1]->Fill(iBar,tof - para->GetTofSyncOffset());
				    //	fh1_tofsync[iPlane - 1][iBar - 1]->Fill(tof - para->GetTofSyncOffset());
			    }

			    auto posToT = para->GetLambda() * log((Ctop_tot) / (Cbot_tot));
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
			    }
			    // fill control histograms
			    // fhTot1vsTot2[iPlane - 1][iBar - 1]->Fill(top_tot, bot_tot);
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){	
				    if (multihits[iPlane - 1][iBar - 1] < 2) fhSqrtQvsPosToT[iPlane - 1][iBar - 1]->Fill(posToT, sqrt(Ctop_tot * Cbot_tot));
				  //  std::cout<< " h0000000000: " << posToT << " , " << sqrt(Ctop_tot * Cbot_tot) << std::endl;
			    }
			    // Time differences of one paddle
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
				    fhTdiff[iPlane - 1]->Fill(iBar, tdiff);
			    }
			    // Time of hit
			    auto THit = (bot_ns + top_ns) / 2. - para->GetSync();
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
			    }
			    // Sync of one plane
			    if((sqrt(Ctop_tot*Cbot_tot)>fToTMin && sqrt(Ctop_tot*Cbot_tot)<fToTMax) || ToTexeption){
				    fhTsync[iPlane - 1]->Fill(iBar, THit);
			    }
			    /* if (fTofdSmiley)
			       {
			       qb = TMath::Sqrt(top_tot * bot_tot);
			    // /(parq[0] + parq[1] * posToT + parq[2] * pow(posToT, 2) + parq[3] * pow(posToT, 3));

			    qb = qb * fTofdQ;
			    }*/
		
			}
		    // prepare double exponential fit
		    if ((!fTofdSmiley && fTofdQ > 0.1 && fParameter > 1 && fParameter != 10))
		    {
				LOG(debug) << "Prepare histo for double exponential fit";
			    auto par = fHitPar->GetModuleParAt(iPlane, iBar);
			    if (!par)
			    {
				    R3BLOG(error,
						    "Hit par not found, Plane: " << top->GetDetectorId() << ", Bar: " << top->GetBarId());
				    continue;
			    }
				//Ctop_tot = top_tot  * par->GetToTOffset3() - par->GetToTOffset4();
				//Cbot_tot = bot_tot  * par->GetToTOffset1() - par->GetToTOffset2();
				Ctop_tot = top_tot  * par->GetToTOffset2();
				Cbot_tot = bot_tot  * par->GetToTOffset1();

			    // calculate y position
			    auto pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();

			    // fill fitting histograms and smiley histogram
			    fhTot1vsPos[iPlane - 1][iBar - 1]->Fill(pos, Cbot_tot);
			    fhTot2vsPos[iPlane - 1][iBar - 1]->Fill(pos, Ctop_tot);

			}

		    // std::cout<< " hola 10" << fTofdZ << std::endl;

		    // prepare charge fit / quench correction
		    if (fTofdZ == true && fParameter > 1 && fParameter != 10)
		    {
				
			    // std::cout<< " hola 20" << std::endl;
			    LOG(debug) << "Prepare histo for quenching correction";
			    // get parameter
			    auto par = fHitPar->GetModuleParAt(iPlane, iBar);
			    if (!par)
			    {
				    R3BLOG(error,
						    "Hit par not found, Plane: " << top->GetDetectorId() << ", Bar: " << top->GetBarId());
				    continue;
			    }

			    // Calculate y position
			    auto posToT =
				    par->GetLambda() * log((Ctop_tot) / (Cbot_tot));


			    Double_t parf[4];
			    parf[0] = par->GetPar1a();
			    parf[1] = par->GetPar1b();
			    parf[2] = par->GetPar1c();
			    parf[3] = par->GetPar1d();


			    Double_t parq[4];
			    parq[0] = par->GetPola();
			    parq[1] = par->GetPolb();
			    parq[2] = par->GetPolc();
			    parq[3] = par->GetPold();

			    // Calculate charge
			    Double_t qb = 0.;
			    Double_t qbx = 0.;
			    if (fTofdSmiley)
			    {	
					if(parq[0]==0. && parq[1]==0. && parq[2]==0. && parq[3]==0.) parq[0] = 1.;

				    qb = TMath::Sqrt(Ctop_tot * Cbot_tot);///(parq[0] + parq[1] * posToT + parq[2] * pow(posToT, 2) + parq[3] * pow(posToT, 3));
				    qbx = TMath::Sqrt(Ctop_tot * Cbot_tot)*(parq[0] + parq[1] * posToT + parq[2] * pow(posToT, 2) + parq[3] * pow(posToT, 3));
				    qb = qb *
					    fTofdQ / 200.; // theory says: dE ~ Z^2 but we see quenching -> just use linear and fit the rest
					qbx = qbx * fTofdQ / 200.;
				     std::cout<< " hola30 " << qb << " , " << fTofdQ << " , "<<qbx<<std::endl;
					 if(qbx > 70.){
						qbx = qbx / 200.;
						cout<<"XXXXXXX "<< qb << " , " << fTofdQ << " , "<<qbx<<std::endl;
					 }
			    }
			    else
			    {
				    // double exponential
				    auto pos = ((bot_ns + par->GetOffset1()) - (top_ns + par->GetOffset2())) * par->GetVeff();
				    auto q1 = Cbot_tot /
					    (parf[0] * (exp(-parf[1] * (pos + 100.)) + exp(-parf[2] * (pos + 100.))) + parf[3]*1.);
				    parq[0] = par->GetPar2a();
				    parq[1] = par->GetPar2b();
				    parq[2] = par->GetPar2c();
				    parq[3] = par->GetPar2d();
							for(int i=0;i<4;i++){
								cout<<"parf["<<i<<"] "<<parf[i]<<" parq["<<i<<"] "<<parq[i]<<endl;
							}
				    auto q2 = Ctop_tot /
					    (parq[0] * (exp(-parq[1] * (pos + 100.)) + exp(-parq[2] * (pos + 100.))) + parq[3]*1.);
				    q1 = q1 *
					    fTofdQ / 200.; // theory says: dE ~ Z^2 but we see quenching -> just use linear and fit the rest
				    q2 = q2 * fTofdQ / 200.;
					cout<<"q1 "<<q1<<" q2 "<<q2<<endl;
				    qb = (q1 + q2) / 2.;
			    }

				        std::cout<< " QB: " << posToT << " , " << qb << std::endl;
			    // Fill control histograms and Q vs Pos without multihits
			    
				if(fParameter == 6){
					Double_t parz[3];
                parz[0] = par->GetPar1za();
                parz[1] = par->GetPar1zb();
                parz[2] = par->GetPar1zc();

				qbx = parz[0] + parz[1]*qbx + parz[2]*qbx*qbx;
				}
				
				if (multihits[iPlane - 1][iBar - 1] < 2 /*&& qb>190. */ || 1)
			    {
				    fhQvsPos[iPlane - 1][iBar - 1]->Fill(posToT, qb);
					fhQXvsPos[iPlane - 1][iBar - 1]->Fill(posToT, qbx);
			    }
	
			}
	/*	    if(fParameter==5)
		    {
			    auto par = fHitPar->GetModuleParAt(iPlane, iBar);

			    Double_t parz[3];
			    parz[0] = par->GetPar1za();
			    parz[1] = par->GetPar1zb();
			    parz[2] = par->GetPar1zc();
			    //parq[3] = par->GetPold();



			    Double_t qb = 0.;
			    if (fTofdSmiley)
			    {
				    qb = TMath::Sqrt(top_tot * bot_tot);///(parq[0] + parq[1] * posToT + parq[2] * pow(posToT, 2) + parq[3] * pow(posToT, 3));
				    qb = qb *
					    fTofdQ; // theory says: dE ~ Z^2 but we see quenching -> just use linear and fit the rest
				    // std::cout<< " hola30 " << qb << " , " << fTofdQ << std::endl;
			    }

			    // Fill control histograms and Q vs Pos without multihits
			    if (multihits[iPlane - 1][iBar - 1] < 2 && (qb > 5.6 && qb < 6.7.))
			    {
				    //    std::cout<< " h: " << posToT << " , " << qb << std::endl;
				    fhZoffset[iPlane - 1][iBar - 1]->Fill(parz[0]+parz[1]*qb);
			    }



		    }
*/
		    ++top_i;
		    ++bot_i;

		    // Increment events
		    fNEvents += 1;
		}
		else if (dt < 0 && dt > -c_range_ns / 2)
		{
			++top_i;
		}
		else
		{
			++bot_i;
		}
	    }
	}
    }
	void R3BTofDCal2HitPar::SetHistogramsNULL(Int_t iPlane, Int_t iBar){
		fhTdiff[iPlane - 1] = NULL;
		fhTsync[iPlane - 1] = NULL;
		fhTofsync[iPlane - 1] = NULL;
		fh_tofd_TotPm[iPlane - 1][0] = NULL;
		fh_tofd_TotPm[iPlane - 1][1] = NULL;
		fh_tofd_TotPm[iPlane - 1][2] = NULL;
		fh_tofd_TotPm1[iPlane - 1] = NULL;
		fh_tofd_TotPm2[iPlane - 1] = NULL;
		fhLogTot1vsLogTot2[iPlane - 1][iBar - 1] = NULL;
		fhSqrtQvsPosToT[iPlane - 1][iBar - 1] = NULL;
		fhQvsPos[iPlane - 1][iBar - 1] = NULL;
		fhQXvsPos[iPlane - 1][iBar - 1] = NULL;
		fhTot1vsPos[iPlane - 1][iBar - 1] = NULL;
		fhTot2vsPos[iPlane - 1][iBar - 1] = NULL;
		fh1_tofsync[iPlane - 1][iBar - 1] = NULL;
		fh1_walk_t[iPlane - 1][iBar - 1] = NULL;
		fh1_walk_b[iPlane - 1][iBar - 1] = NULL;

	}
    void R3BTofDCal2HitPar::CreateHistograms(Int_t iPlane, Int_t iBar)
    {
	    Double_t max_charge = fMaxQ;

	    if (NULL == fhTdiff[iPlane - 1])
	    {
		    char *strName1;
			strName1 = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName1, "Time_Diff_Plane_%d", iPlane);
		    sprintf(strName2, "Time Diff Plane %d", iPlane);
		    fhTdiff[iPlane - 1] = new TH2F(strName1, strName2, 50, 0, 50, 4000, -20., 20.);
		    fhTdiff[iPlane - 1]->GetXaxis()->SetTitle("Bar ");
		    fhTdiff[iPlane - 1]->GetYaxis()->SetTitle("Time difference (PM1 - PM2) in ns");
			//cout<<"sprint1"<<endl;
			free(strName1);
			free(strName2);
	    }
	    if (NULL == fhTsync[iPlane - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Time_Sync_Plane_%d", iPlane);
		    sprintf(strName2, "Time Sync Plane %d", iPlane);
		    fhTsync[iPlane - 1] = new TH2F(strName, strName2, 45, 0, 45, 10000, -3500, 1000.);
		    fhTsync[iPlane - 1]->GetXaxis()->SetTitle("Bar ");
		    fhTsync[iPlane - 1]->GetYaxis()->SetTitle("THit in ns");
			//cout<<"sprint2"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (NULL == fhTofsync[iPlane - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Tof_Sync_Plane_%d", iPlane);
		    sprintf(strName2, "Tof Sync Plane %d", iPlane);
		    fhTofsync[iPlane - 1] = new TH2F(strName, strName2, 45, 0, 45, 10000, -500, 500.);
		    fhTofsync[iPlane - 1]->GetXaxis()->SetTitle("Bar ");
		    fhTofsync[iPlane - 1]->GetYaxis()->SetTitle("Tof in ns");
			//cout<<"sprint3"<<endl;
			free(strName);
			free(strName2);
	    }


	    /*  if (NULL == fhlos[iPlane - 1])
		{
		char strName[255];
		sprintf(strName, "los_Plane_%d", iPlane);
		fhlos[iPlane - 1] = new TH2F(strName, strName, 45, 0, 45, 50000, -5000., 5000.);
		fhlos[iPlane - 1]->GetXaxis()->SetTitle("Bar ");
		fhlos[iPlane - 1]->GetYaxis()->SetTitle("los t in ns");
		}
		*/
	    if (NULL == fh_tofd_TotPm[iPlane - 1][0])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Tofd_ToT_plane_%d_bc", iPlane);
		    sprintf(strName2, "Tofd ToT plane %d bc", iPlane);
		    fh_tofd_TotPm[iPlane - 1][0] = new TH2F(strName, strName2, 90, -45, 45, 500, 0., 500.);
		    fh_tofd_TotPm[iPlane - 1][0]->GetXaxis()->SetTitle("Bar ");
		    fh_tofd_TotPm[iPlane - 1][0]->GetYaxis()->SetTitle("ToT / ns");
			//cout<<"sprint4"<<endl;
			free(strName);
			free(strName2);
	    }
		if (NULL == fh_tofd_TotPm1[iPlane - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "1Tofd_ToT_plane_%d", iPlane);
		    sprintf(strName2, "1Tofd ToT plane %d", iPlane);
		    fh_tofd_TotPm1[iPlane - 1] = new TH2F(strName, strName2, 90, -45, 45, 500, 0., 500.);
		    fh_tofd_TotPm1[iPlane - 1]->GetXaxis()->SetTitle("Bar ");
		    fh_tofd_TotPm1[iPlane - 1]->GetYaxis()->SetTitle("ToT / ns");
			//cout<<"sprint4"<<endl;
			free(strName);
			free(strName2);
	    }
		if (NULL == fh_tofd_TotPm2[iPlane - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "2Tofd_ToT_plane_%d", iPlane);
		    sprintf(strName2, "2Tofd ToT plane %d", iPlane);
		    fh_tofd_TotPm2[iPlane - 1] = new TH2F(strName, strName2, 90, -45, 45, 500, 0., 500.);
		    fh_tofd_TotPm2[iPlane - 1]->GetXaxis()->SetTitle("Bar ");
		    fh_tofd_TotPm2[iPlane - 1]->GetYaxis()->SetTitle("ToT / ns");
			//cout<<"sprint4"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (NULL == fh_tofd_TotPm[iPlane - 1][1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Tofd_ToT_plane_%d_acN", iPlane);
		    sprintf(strName2, "Tofd ToT plane %d acN", iPlane);
		    fh_tofd_TotPm[iPlane - 1][1] = new TH2F(strName, strName2, 90, -45, 45, 500, 0., 500.);
		    fh_tofd_TotPm[iPlane - 1][1]->GetXaxis()->SetTitle("Bar ");
		    fh_tofd_TotPm[iPlane - 1][1]->GetYaxis()->SetTitle("ToT / ns");
			//cout<<"sprint4"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (NULL == fh_tofd_TotPm[iPlane - 1][2])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Tofd_ToT_plane_%d_acM", iPlane);
		    sprintf(strName2, "Tofd ToT plane %d acM", iPlane);
		    fh_tofd_TotPm[iPlane - 1][2] = new TH2F(strName, strName2, 90, -45, 45, 500, 0., 500.);
		    fh_tofd_TotPm[iPlane - 1][2]->GetXaxis()->SetTitle("Bar ");
		    fh_tofd_TotPm[iPlane - 1][2]->GetYaxis()->SetTitle("ToT / ns");
			//cout<<"sprint4"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (NULL == fhLogTot1vsLogTot2[iPlane - 1][iBar - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Plane_%d_Bar_%d_LogToT1vsLogToT2", iPlane, iBar);
		    sprintf(strName2, "Plane %d Bar %d LogToT1vsLogToT2", iPlane, iBar);
			//cout<<"sprint5 "<<strName<<" "<<strName2<<endl;
		    fhLogTot1vsLogTot2[iPlane - 1][iBar - 1] = new TH2F(strName, strName2, 400, 2., 6., 400, 2., 6.);
		    fhLogTot1vsLogTot2[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Log(ToT) of PM2");
		    fhLogTot1vsLogTot2[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Log(ToT) of PM1");
			free(strName);
			free(strName2);
	    }
	    if (NULL == fhSqrtQvsPosToT[iPlane - 1][iBar - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "SqrtQ_vs_PosToT_Plane_%d_Bar_%d", iPlane, iBar);
		    sprintf(strName2, "SqrtQ vs PosToT Plane %d Bar %d", iPlane, iBar);
			//cout<<"sprint6 "<<strName<<" "<<strName2<<endl;
		    fhSqrtQvsPosToT[iPlane - 1][iBar - 1] =
			    new TH2F(strName, strName2, 3000, -100, 100, max_charge * 4, 0., max_charge * 4);
		    fhSqrtQvsPosToT[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("sqrt(PM1*PM2)");
		    fhSqrtQvsPosToT[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position from ToT in cm");
			free(strName);
			free(strName2);
	    }
	    if (NULL == fhQvsPos[iPlane - 1][iBar - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Q_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
		    sprintf(strName2, "Q vs Pos Plane %d Bar %d", iPlane, iBar);
		    //cout<<"sprint7 "<<strName<<" "<<strName2<<endl;
			fhQvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, strName2, 2000, -100, 100, /*max_charge , 0., 2.*max_charge*/ 1600, 0., 16.);
		    fhQvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge");
		    fhQvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
			free(strName);
			free(strName2);
	    }
		if (NULL == fhQXvsPos[iPlane - 1][iBar - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "QX_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
		    sprintf(strName2, "QX vs Pos Plane %d Bar %d", iPlane, iBar);
		    //cout<<"sprint7 "<<strName<<" "<<strName2<<endl;
			fhQXvsPos[iPlane - 1][iBar - 1] = new TH2F(strName, strName2, 2000, -100, 100, /*max_charge , 0., 2.*max_charge*/ 1600, 0., 16.);
		    fhQXvsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("Charge");
		    fhQXvsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Position in cm");
			free(strName);
			free(strName2);
	    }
	   /* if (NULL == fhZoffset[iPlane - 1][iBar - 1])
	    {
		    char strName[255];
		    sprintf(strName, "Zoffset_Plane_%d_Bar_%d", iPlane, iBar);
		    fhZoffset[iPlane - 1][iBar - 1] = new TH2F(strName, "", 500,0,10);
		    fhZoffset[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("counts");
		    fhZoffset[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Z");
	    }

	      if (NULL == fhlost[i][j])
		{
		char strName[255];
		sprintf(strName, "los_vs_thit_Plane_%d_Bar_%d", 1, 11);
		fhlost[i][j] = new TH2F(strName, "", 20000, -4000, -3000, 50000, -5000, 5000);
		fhlost[i][j]->GetYaxis()->SetTitle("thit");
		fhlost[i][j]->GetXaxis()->SetTitle("los");
		}
		*/
	    /* if (NULL == fhtott[iPlane - 1][iBar - 1])
	       {
	       char strName[255];
	       sprintf(strName, "tot_vs_thit_Plane_%d_Bar_%d", iPlane, iBar);
	       fhtott[iPlane - 1][iBar - 1] = new TH2F(strName, "", 20000, -4000, -2000, 1000, 0., 500);
	       fhtott[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("thit");
	       fhtott[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("tot");
	       }*/
	    if (NULL == fhTot1vsPos[iPlane - 1][iBar - 1] /*&& !fTofdSmiley*/)
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Tot1_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
		    sprintf(strName2, "Tot1 vs Pos Plane %d Bar %d", iPlane, iBar);
		    fhTot1vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, strName2, 300, -100, 100, 400, 0., 300.);
		    fhTot1vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Pos in cm");
		    fhTot1vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("ToT of PM1 in ns");
			//cout<<"sprint8"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (NULL == fhTot2vsPos[iPlane - 1][iBar - 1] /*&& !fTofdSmiley*/)
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "Tot2_vs_Pos_Plane_%d_Bar_%d", iPlane, iBar);
		    sprintf(strName2, "Tot2 vs Pos Plane %d Bar %d", iPlane, iBar);
		    fhTot2vsPos[iPlane - 1][iBar - 1] = new TH2F(strName, strName2, 300, -100, 100, 400, 0., 300.);
		    fhTot2vsPos[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("Pos in cm");
		    fhTot2vsPos[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("ToT of PM2 in ns");
			//cout<<"sprint9"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (fh1_tofsync[iPlane - 1][iBar - 1] == NULL)
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "tofdiff_plane_%d_bar_%d", iPlane, iBar);
		    sprintf(strName2, "tofdiff plane %d bar %d", iPlane, iBar);
		    fh1_tofsync[iPlane - 1][iBar - 1] = new TH1F(strName, strName2, 25000, -1000, 200);
		    fh1_tofsync[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToF [ns]");
			//cout<<"sprint10"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (NULL == fh1_walk_t[iPlane - 1][iBar - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "PMTtime_vs_PMTToT_t_Plane_%d_Bar_%d", iPlane, iBar);
		    sprintf(strName2, "PMTtime vs PMTToT t Plane %d Bar %d", iPlane, iBar);
		    fh1_walk_t[iPlane - 1][iBar - 1] = new TH2F(strName, strName2, 400., 0., 200.,12000, -2000, 2000);
		    fh1_walk_t[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("PMT time ns");
		    fh1_walk_t[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PMT");
			//cout<<"sprint11"<<endl;
			free(strName);
			free(strName2);
	    }
	    if (NULL == fh1_walk_b[iPlane - 1][iBar - 1])
	    {
		    char *strName;
			strName = (char*)malloc(255);
		    char *strName2;
			strName2 = (char*)malloc(255);
		    sprintf(strName, "PMTtime_vs_PMTToT_b_Plane_%d_Bar_%d", iPlane, iBar);
		    sprintf(strName2, "PMTtime vs PMTToT b Plane %d Bar %d", iPlane, iBar);
		    fh1_walk_b[iPlane - 1][iBar - 1] = new TH2F(strName, strName2, 400., 0., 200.,12000, -2000, 2000);
		    fh1_walk_b[iPlane - 1][iBar - 1]->GetYaxis()->SetTitle("PMT time ns");
		    fh1_walk_b[iPlane - 1][iBar - 1]->GetXaxis()->SetTitle("ToT of PMT");
			//cout<<"sprint12"<<endl;
			free(strName);
			free(strName2);
	    }


    }

    void R3BTofDCal2HitPar::calcOffset()
    {
	    TCanvas* cOffset = new TCanvas("cOffset", "cOffset", 10, 10, 1000, 900);
	    cOffset->Divide(2, 2);
	    R3BTofDHitModulePar* mpar;
	    for (Int_t i = 0; i < fNofPlanes; i++)
	    {
		    if (fhTdiff[i])
		    {
			    LOG(warning) << "Found histo Time_Diff_Plane_" << i + 1;
			    // auto* h = (TH2F*)fhTdiff[i]->Clone();
			    for (Int_t j = 0; j < fPaddlesPerPlane; j++)
			    {
				    int s=0;
				    mpar = new R3BTofDHitModulePar();
				    Double_t offset = 0.;
				    cOffset->cd(i + 1);
				    fhTdiff[i]->Draw("colz");
				    TH1F* histo_py = (TH1F*)fhTdiff[i]->ProjectionY("histo_py", j + 2, j + 2, "");
				    histo_py->Rebin(4);
				    Int_t binmax = histo_py->GetMaximumBin();
				    Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				    TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.3, Max + 0.3);
				    histo_py->Fit("fgaus", "QR0");
				    offset = fgaus->GetParameter(1); // histo_py->GetXaxis()->GetBinCenter(binmax);
				    LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " Offset  " << offset;
				    mpar->SetPlane(i + 1);
				    mpar->SetPaddle(j + 1);
				    mpar->SetOffset1(-offset / 2.);
				    mpar->SetOffset2(offset / 2.);
				    fHitPar->AddModulePar(mpar);
				    //cin>>s;
			    }
		    fhTdiff[i]->Write();
			}
	    }
	    fHitPar->setChanged();
    }
    void R3BTofDCal2HitPar::calcToTOffset(Double_t totLow, Double_t totHigh)
    {
	    TCanvas* cToTOffset1 = new TCanvas("cToTOffset1", "cToTOffset1", 10, 10, 1000, 900);
	    cToTOffset1->Divide(1, 2);
	    TCanvas* cToTOffset2 = new TCanvas("cToTOffset2", "cToTOffset2", 10, 10, 1000, 900);
	    cToTOffset2->Divide(1, 2);
		Double_t fitrange = 25.;
		if(fParameter == -1) fitrange = 10.;
	    for (Int_t i = 0; i < fNofPlanes; i++)
	    {	
			R3BTofDHitModulePar* par;
		    for (Int_t j = 0; j < (fPaddlesPerPlane); j++)
		    {	
				if(j==45 || j == 46) continue;
			    Double_t offset_bot_high = 0.;
				Double_t offset_top_high = 0.;
			    Double_t offset_bot_low = 0.;
				Double_t offset_top_low = 0.;
				Double_t offset = 0.;	
				Double_t a_bot = 1.;	
				Double_t a_top = 1.;	
				Double_t b_bot = 0.;	
				Double_t b_top = 0.;
				Double_t Max1 = 1.;
				Double_t Max2 = 1.;
				Double_t Max3 = 0.;
				Double_t Max4 = 0.;

			    if(j<44) par = fHitPar->GetModuleParAt(i + 1, 1 + j);
			    else par = fHitPar->GetModuleParAt(i + 1, j - fPaddlesPerPlane - 2);
			    if (fhSqrtQvsPosToT[i][j])
			    {
				    LOG(info) << "Found histo SqrtQ_vs_PosToT_Plane_" << i + 1 << "_Bar_" << j + 1;
				    // auto* h = fhSqrtQvsPosToT[i][j]->Clone();
				    cToTOffset1->cd(1);
				    fhSqrtQvsPosToT[i][j]->Draw("colz");
				    auto histo_py = (TH2F*)fhSqrtQvsPosToT[i][j]->ProjectionX("histo_py", totLow, totHigh, "");
				    cToTOffset1->cd(2);
				    histo_py->Rebin(2);
				    histo_py->Draw();
				    Int_t binmax = histo_py->GetMaximumBin();
				    Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				    TF1* fgaus = new TF1(
						    "fgaus", "gaus(0)", Max - 0.2, Max + 0.2); // new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
				    histo_py->Fit("fgaus", "QR0");
				    offset = fgaus->GetParameter(1);
				    fgaus->Draw("SAME");
				    histo_py->SetAxisRange(Max - .5, Max + .5, "X");
				    fhSqrtQvsPosToT[i][j]->SetAxisRange(Max - .5, Max + .5, "X");
				    fhSqrtQvsPosToT[i][j]->SetAxisRange(totLow, totHigh, "Y");
				    cToTOffset1->Update();
				    delete fgaus;
			    fhSqrtQvsPosToT[i][j]->Write();
				}
			    LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " ToT Offset  " << offset << "\n";
				LOG(info) <<"TOP " << 1. / sqrt(exp(offset)) << " BOT " <<sqrt(exp(offset))<< "\n";
			    par->SetToTOffset1(sqrt(exp(offset)));
			    par->SetToTOffset2(1. / sqrt(exp(offset)));
				

/*				if (fh_tofd_TotPm1[i])
			    {
				cToTOffset1->cd(1);
				fh_tofd_TotPm1[i]->Draw("colz");
				    auto histo_py1 = (TH1F*)fh_tofd_TotPm1[i]->ProjectionY("histo_py1", fPaddlesPerPlane - j, fPaddlesPerPlane - j, "");//bot high max
				    auto histo_py2 = (TH1F*)fh_tofd_TotPm1[i]->ProjectionY("histo_py2", fPaddlesPerPlane+j+3,fPaddlesPerPlane + j+3, "");//top high max
				    cToTOffset1->cd(2);
				    histo_py1->Rebin(2);
				    histo_py1->Draw();
					Int_t binmax1 = histo_py1->GetMaximumBin();
				    Max1 = histo_py1->GetXaxis()->GetBinCenter(binmax1);
					Int_t binmax2 = histo_py2->GetMaximumBin();
				    Max2 = histo_py2->GetXaxis()->GetBinCenter(binmax2);
				    TF1* fgaus1 = new TF1(
						    "fgaus", "gaus(0)", Max1 - fitrange, Max1 + fitrange); // new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
				    histo_py1->Fit("fgaus", "QR0");
				    offset_bot_high = fgaus1->GetParameter(1);
				    fgaus1->Draw("SAME");
				    histo_py1->SetAxisRange(0., 350., "Y");
					cToTOffset1->Update();
				    delete fgaus1;
				    histo_py2->Rebin(2);
				    histo_py2->Draw();
				    TF1* fgaus2 = new TF1(
						    "fgaus", "gaus(0)", Max2 - fitrange, Max2 + fitrange); // new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
				    histo_py2->Fit("fgaus", "QR0");
				    offset_top_high = fgaus2->GetParameter(1);
				    fgaus2->Draw("SAME");
				    histo_py2->SetAxisRange(0., 350., "Y");
				    cToTOffset1->Update();
				    delete fgaus2;
				}
				if (fh_tofd_TotPm2[i])
			    {
				cToTOffset2->cd(1);
				fh_tofd_TotPm2[i]->Draw("colz");
				    auto histo_py3 = (TH1F*)fh_tofd_TotPm2[i]->ProjectionY("histo_py3", fPaddlesPerPlane - j, fPaddlesPerPlane - j, "");//bot low max
				    auto histo_py4 = (TH1F*)fh_tofd_TotPm2[i]->ProjectionY("histo_py4", fPaddlesPerPlane+j+3,fPaddlesPerPlane + j+3, "");//top low max
				    cToTOffset2->cd(2);
					Int_t binmax3 = histo_py3->GetMaximumBin();
				    Max3 = histo_py3->GetXaxis()->GetBinCenter(binmax3);
					Int_t binmax4 = histo_py4->GetMaximumBin();
				    Max4 = histo_py4->GetXaxis()->GetBinCenter(binmax4);
				    histo_py3->Rebin(2);
				    histo_py3->Draw();
				    TF1* fgaus3 = new TF1(
						    "fgaus", "gaus(0)", Max3 - 25., Max3 + 25.); // new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
				    histo_py3->Fit("fgaus", "QR0");
				    offset_bot_low = fgaus3->GetParameter(1);
				    fgaus3->Draw("SAME");
				    histo_py3->SetAxisRange(0., 100., "Y");
				    cToTOffset2->Update();
				    delete fgaus3;
				    histo_py4->Rebin(2);
				    histo_py4->Draw();
				    TF1* fgaus4 = new TF1(
						    "fgaus", "gaus(0)", Max4 - 25., Max4 + 25.); // new TF1("fgaus", "gaus(0)", Max - 0.06, Max + 0.06);
				    histo_py4->Fit("fgaus", "QR0");
				    offset_top_low = fgaus4->GetParameter(1);
				    fgaus4->Draw("SAME");
				    histo_py4->SetAxisRange(0., 100., "Y");
				    cToTOffset2->Update();
				    delete fgaus4;
				
				}


					if((offset_bot_high != 0)){
						//a_bot = 160. / (offset_bot_high - offset_bot_low);
						//b_bot = a_bot * offset_bot_high - 200.;
						a_bot = 200. / offset_bot_high;
					}
					if(offset_top_high != 0){
						//a_top = 160. / (offset_top_high - offset_top_low);
						//b_top = a_top * offset_top_high - 200.;
						a_top = 200. / offset_top_high;
					}
				if(fParameter == 10){
					a_bot = 160. / (Max1 - Max3);
					b_bot = a_bot * Max1 - 200.;
					a_top = 160. / (Max2 - Max4);
					b_top = a_top * Max2 - 200.;	
					
					//a_bot = a_bot * par->GetToTOffset1();
					//b_bot = b_bot + a_bot * par->GetToTOffset2();
					//a_top = a_top * par->GetToTOffset3();
					//b_top = b_top + a_top * par->GetToTOffset4();
				}
				cout<<"debugG "<<"P "<<i+1<<" B "<<j+1<<" BotH "<<offset_bot_high<<" BotL "<<offset_bot_low<<" a_bot "<<a_bot<<" b_bot "<<b_bot<<endl;
				cout<<"debugG "<<"P "<<i+1<<" B "<<j+1<<" TopH "<<offset_top_high<<" TopL "<<offset_top_low<<" a_top "<<a_top<<" b_top "<<b_top<<endl;

			   // if(j<45) LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " Bot High Max  " << offset_bot_high <<" Top High Max  " << offset_top_high << "\n";
			   // if(j<45) LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " Par Bot a " << a_bot <<" Bot b  " << b_bot << "\n";
			   // if(j<45) LOG(warning) << " Plane  " << i + 1 << " Bar " << j + 1 << " Par Top a  " << a_top <<" Top b  " << b_top << "\n";
			    par->SetToTOffset1(a_bot);//Bottom factor
			   // par->SetToTOffset2(b_bot);//Bottom offset
			    par->SetToTOffset2(a_top);//Top factor
			   // par->SetToTOffset4(b_top);//Top offset
			
			*/
			}
	    }
	    fHitPar->setChanged();
    }

    void R3BTofDCal2HitPar::calcSync()
    {
	    TCanvas* cSync = new TCanvas("cSync", "cSync", 10, 10, 1000, 900);
	    cSync->Divide(2, 2);
	    for (Int_t i = 0; i < fNofPlanes; i++)
	    {
		    if (fhTsync[i])
		    {
			    LOG(info) << "Found histo Time_Sync_Plane_" << i + 1;
			    // auto h = fhTsync[i]->Clone();
			    for (Int_t j = 0; j < fPaddlesPerPlane; j++)
			    {
				    cSync->cd(i + 1);
				    fhTsync[i]->Draw("colz");
				    auto* histo_py = (TH1F*)fhTsync[i]->ProjectionY("histo_py", j + 2, j + 2, "");
				    R3BTofDHitModulePar* par = fHitPar->GetModuleParAt(i + 1, j + 1);
				    Int_t binmax = histo_py->GetMaximumBin();
				    Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				    Double_t MaxEntry = histo_py->GetBinContent(binmax);
				    TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 10., Max + 10.);
				    fgaus->SetParameters(MaxEntry, Max, 20);
				    histo_py->Fit("fgaus", "QR0");
				    Double_t sync = fgaus->GetParameter(1); // histo_py->GetXaxis()->GetBinCenter(binmax);
				    par->SetSync(sync);
				    LOG(info) << " Plane  " << i + 1 << " Bar " << j + 1 << " Sync  " << sync;
			    }
		    fhTsync[i]->Write();
			}
	    }
	    fHitPar->setChanged();
    }

    int R3BTofDCal2HitPar::zcorr(TH2F* histo, Double_t min, Double_t max, Double_t* pars, Int_t pl, Int_t b, Int_t index)
    {
	    if (histo->GetEntries() < 10)
	    {
		    R3BLOG(warning, "Nb of events below 10 with "<< histo->GetEntries() << " entries for histo with index" << index);

		    //return;
	    }

	    Double_t par[3000] = { 0 };
	    Double_t x[3000] = { 0 };
	    char strName[255];
	    sprintf(strName, "canvas_%d", index);
	    TCanvas* c1 = new TCanvas(strName, "", 100, 100, 800, 800);
	    c1->Divide(1, 3);
	    c1->cd(1);
			auto* h = (TH2F*)histo->Clone();
	    	h->Draw("colz");
	    	h->SetAxisRange(min, max, "Y");
	    	// Projection of charge axis
	    	auto* h1 = h->ProjectionY("p_y", 1, 2000);
		
		c1->cd(2);
		//gPad->SetLogy();
	    h1->Draw();
	    // Use TSpectrum to find the peak candidates
		Double_t threshhold = 0.001;
		Int_t sigma = 10;
		Double_t sqrtf = 1.;
	    Int_t nPeaks = fNbZPeaks;
doagainfit:
		TSpectrum* s = new TSpectrum(nPeaks);
	    Int_t nfound = s->Search(h1, sigma, "nobackground", threshhold);
	    std::cout << "Found " << nfound << " candidate peaks to fit\n";

	    if (nfound == 0)
	    {
		    delete s;
		    delete c1;
		    delete h;
		    delete h1;
		    return -1;
	    }

	    c1->Update();
	    // Eliminate background peaks
	    Int_t mPeaks = 0;
	    Double_t* xpeaks = s->GetPositionX();
	    for (Int_t p = 0; p <= nfound; p++)
	    {
		    Float_t xp = xpeaks[p];
		    Int_t bin = h1->GetXaxis()->FindBin(xp);
		    Float_t yp = h1->GetBinContent(bin);
		    if (yp - sqrtf * TMath::Sqrt(yp) < 1.){
			    cout<<"jump "<<yp - sqrtf * TMath::Sqrt(yp)<<endl;
				continue;
			}
		    par[2 * mPeaks] = yp;
		    par[2 * mPeaks + 1] = xp;
		    mPeaks++;
	    }

	    Double_t peaks[mPeaks];
	    for (Int_t i = 0; i < mPeaks; i++)
	    {
		    // printf("Found peak @ %f\n",xpeaks[i]);
		    peaks[i] = par[2 * i + 1];
	    }
	    c1->Update();

	    if (mPeaks < 2 && 0)
	    {
		    pars[0] = 0.;
		    //pars[1] = fMaxQ / peaks[0];
		    pars[1] = 8. / peaks[0];
		    pars[2] = 0.;

		    delete s;
		    delete c1;
		    delete h;
		    delete h1;
		    return -1;
	    }
		cout<<"mPeaks "<<mPeaks<<endl;
	    // select useful peaks
	    sort(peaks, peaks + mPeaks);
	    Double_t zpeaks[3000] = { 0 };
	    string doagain = "y";
	    Int_t nfp;
//doagainfit:
		Int_t z = 0;
	    do
	    {
			string peakscheck = "x";
			std::cout << "Peaks ok? (y/n/c/goback) ";
		    std::cin >> peakscheck;
			if(peakscheck == "n"){
				delete s;
				std::cout << "New sigma: ";
		    	std::cin >> sigma;
				std::cout << "New threshhold: ";
		    	std::cin >> threshhold;
				std::cout << "New Sqrt factor: ";
		    	std::cin >> sqrtf;
				std::cout << "New # of peaks: ";
		    	std::cin >> nPeaks;
				goto doagainfit;
			}
			if(peakscheck == "goback"){
				int a=0;
				std::cout << "How many bars? ";
		    	std::cin >> a;
				return a;
			}
			while(peakscheck == 'c'){
				cout<< "We see "<<mPeaks<<" peaks. Continue? (y/n) ";
				cin >> peakscheck;
			}
		    
			nfp = 0;
		    for (Int_t i = 0; i < mPeaks; i++)
		    {
			    std::cout << "Peak @ " << peaks[i];
			    while ((std::cout << " corresponds to Z=") && !(std::cin >> z))
			    {
				    std::cout << "That's not a number;";
				    std::cin.clear();
				    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			    }
			    if (z == 0)
				    continue;
			    x[nfp] = (Double_t)z;
			    zpeaks[nfp] = peaks[i];
			    // std::cout<<"z real " << x[nfp] << " z found " << zpeaks[nfp] <<"\n";
			    nfp++;
		    }
			doagain = 'n';
//		    std::cout << "Do again? (y/n) ";
//		    std::cin >> doagain;
	    } while (doagain != "n");

	    if (mPeaks < 2)
	    {
		    pars[0] = 0.;
		    //pars[1] = fMaxQ / peaks[0];
		    pars[1] = z / peaks[0];
		    pars[2] = 0.;

		    delete s;
		    delete c1;
		    delete h;
		    delete h1;
		    return -1;
	    }

	    // fit charge axis
	    std::cout << "Selected " << nfp << " useful peaks to fit\nStart fitting...\n";
	    c1->cd(3)->Clear();
	    c1->Update();
	    c1->cd(3);
	    auto gr1 = new TGraph(nfp, zpeaks, x);
	    gr1->SetMarkerColor(4);
	    gr1->SetMarkerSize(1.5);
	    gr1->SetMarkerStyle(20);
	    gr1->Draw("AP");

		fZfitType = "pol2";

	    // TF1* fitz = new TF1("fitz", "[0]*TMath::Power(x,[2])+[1]", min, max);
	    if (fZfitType != "pol1" && fZfitType != "pol2")
	    {
		    R3BLOG(error, "Fit " << fZfitType << " is not allowed, use pol1 or pol2 ");
		    return -1;
	    }
	    auto fitz = new TF1("fitz", fZfitType, min, max);
	    fitz->SetLineColor(2);
	    fitz->SetLineWidth(2);
	    fitz->SetLineStyle(1);
	    // fitz->SetParameters(1.5, 2., .1);
	    gr1->Fit("fitz", "Q");
	    fitz->Draw("lsame");
	    c1->Update();
	    // write parameters
	    std::cout << "Is OK? (y/n) ";
	    std::cin >> doagain;
	    if (doagain == "n"){
		    std::cout << "New sigma: ";
		    std::cin >> sigma;
			std::cout << "New threshhold: ";
		    std::cin >> threshhold;
			delete s;
			goto doagainfit;
		}

	    int nbpars = 2;
	    if (fZfitType == "pol2")
		    nbpars = 3;

	    for (Int_t j = 0; j < nbpars; j++)
	    {
		    pars[j] = fitz->GetParameter(j);
		     std::cout<<Form("par%i= ",j)<<pars[j]<<"\n";
	    }
		c1->Write();
	    delete s;
	    delete h;
	    delete h1;
	    delete gr1;
	    delete c1;
	    delete fitz;

		return -1;
    }

    void R3BTofDCal2HitPar::calcVeff()
    {
	    TCanvas* cVeff = new TCanvas("cVeff", "cVeff", 10, 10, 1000, 900);
	    cVeff->Divide(2, 2);
	    for (Int_t i = 0; i < fNofPlanes; i++)
	    {
		    for (Int_t j = 0; j < fPaddlesPerPlane; j++)
		    {
			    Double_t max = 0.;
			    Double_t veff = 7.;
			    if (fhTdiff[i])
			    {
				    LOG(info) << "Found histo Time_Diff_Plane_" << i + 1;
				    auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
				    if (!par)
				    {
					    LOG(warning) << "Hit par not found, Plane: " << i + 1 << ", Bar: " << j + 1;
					    continue;
				    }
				    // auto* h = (TH2F*)histofilename->Get(Form("Time_Diff_Plane_%i", i + 1))->Clone();
				    cVeff->cd(i + 1);
				    // h->Draw("colz");
				    TH1F* histo_py = (TH1F*)fhTdiff[i]->ProjectionY("histo_py", j + 2, j + 2, "");
				    Int_t binmax = histo_py->GetMaximumBin();
				    max = histo_py->GetXaxis()->GetBinCenter(binmax);
				    Double_t maxEntry = histo_py->GetBinContent(binmax);
				    auto* fgaus = new TF1("fgaus", "gaus(0)", max - 0.3, max + 0.3); /// TODO: find best range
				    fgaus->SetParameters(maxEntry, max, 20);
				    histo_py->Fit("fgaus", "QR0");
				    Double_t offset1 = par->GetOffset1();
				    Double_t offset2 = par->GetOffset2();
				    max = fgaus->GetParameter(1) + offset1 - offset2; /// TODO: needs to be tested
				    // max = max+offset1-offset2;
				    veff = fTofdY / max; // effective speed of light in [cm/s]
				    LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " offset  " << par->GetOffset1();
				    LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " max  " << max;
				    LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " veff  " << veff;
				    par->SetVeff(veff);
			    }
		    }
	    }
	    fHitPar->setChanged();
    }

    void R3BTofDCal2HitPar::calcLambda(Double_t totLow, Double_t totHigh)
    {
	    TCanvas* cToTOffset = new TCanvas("cLambda", "cLambda", 10, 10, 1000, 900);
	    cToTOffset->Divide(1, 2);
	    for (Int_t i = 0; i < fNofPlanes; i++)
	    {
		    for (Int_t j = 0; j < fPaddlesPerPlane; j++)
		    {
			    Double_t offset = 0.;
			    auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
			    if (fhSqrtQvsPosToT[i][j])
			    {
				    LOG(info) << "Found histo SqrtQ_vs_PosToT_Plane_" << i + 1 << "_Bar_" << j + 1;
				    // auto* h = (TH2F*)histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1))->Clone();
				    cToTOffset->cd(1);
				    fhSqrtQvsPosToT[i][j]->Draw("colz");
				    auto* histo_py = (TH2F*)fhSqrtQvsPosToT[i][j]->ProjectionX("histo_py", totLow, totHigh, "");
				    cToTOffset->cd(2);
				    histo_py->Draw();
				    Int_t binmax = histo_py->GetMaximumBin();
				    Double_t Max = histo_py->GetXaxis()->GetBinCenter(binmax);
				    TF1* fgaus = new TF1("fgaus", "gaus(0)", Max - 0.6, Max + 0.6);
				    histo_py->Fit("fgaus", "QR0");
				    offset = fgaus->GetParameter(1);
				    fgaus->Draw("SAME");
				    histo_py->SetAxisRange(Max - .5, Max + .5, "X");
				    fhSqrtQvsPosToT[i][j]->SetAxisRange(Max - .5, Max + .5, "X");
				    fhSqrtQvsPosToT[i][j]->SetAxisRange(totLow, totHigh, "Y");
				    cToTOffset->Update();
				    delete fgaus;
				    delete histo_py;
			    }
			    else
				    LOG(error) << "Missing histo plane " << i + 1 << " bar " << j + 1;
			    Double_t lambda = fTofdY / offset;
			    LOG(info) << " Plane  " << i + 1 << " Bar " << j + 1 << " ToT Offset  " << offset << " Lambda " << lambda
				    << "\n";
			    par->SetLambda(lambda);
		    }
	    }
	    fHitPar->setChanged();
    }
    /*  void R3BTofDCal2HitPar::calcwalk(Double_t totLow, Double_t totHigh)
	{
	for (Int_t i = 0; i < fNofPlanes; i++)
	{
	for (Int_t j = 0; j < fPaddlesPerPlane; j++)
	{
	auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
	Double_t y[1000], x[1000];
	Int_t n = 0;

	TGraph* gr1 = new TGraph();
	TGraph* gr2 = new TGraph();
	TCanvas* cfit_walk = new TCanvas("cfit_walk", "fit walk", 100, 100, 800, 800);
	cfit_walk->Clear();
	cfit_walk->Divide(1, 4);
	cfit_walk->cd(1);
	TH2F* histowb1 = (TH2F*)histowb->Clone();
	histowb1->Draw("colz");
	TH2F* histowb2 = (TH2F*)histowb->Clone();
	histowb2->RebinX(50);
	histowb2->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
    // histo2->SetAxisRange(fTofdTotLow,fTofdTotHigh,"Y");
    cfit_walk->cd(2);
    histowb2->Draw("colz");
    std::cout << "Searching for points to fit...\n";
    for (Int_t i = 1; i < histowb2->GetNbinsX(); i++)
    {
    std::cout<<"Bin "<<i<<" of "<<histowb2->GetNbinsX()<<" with cut: "<<fTofdTotLow<<" < sqrt(q1*q2) <"<<fTofdTotHigh<<"\n";
    cfit_walk->cd(2);
    TLine* lw = new TLine(
    histowb2->GetXaxis()->GetBinCenter(i), fTofdTotLow, histowb2->GetXaxis()->GetBinCenter(i), fTofdTotHigh);
    lw->SetLineColor(kRed);
    lw->SetLineWidth(2.);
    lw->Draw();
    cfit_smiley->cd(3);
    TH1F* histo_wby = (TH1F*)histowb2->ProjectionY("histo_wby", i, i, "");
    histo_wby->Draw();
    // cfit_smiley->Update();
    x[n] = histowb2->GetXaxis()->GetBinCenter(i);
    Int_t binmaxwb = histo_wby->GetMaximumBin();
    y[n] = histo_wby->GetXaxis()->GetBinCenter(binmax);

    if ((x[n] < min || x[n] > max) || (y[n] < fTofdTotLow || y[n] > fTofdTotHigh))
    {
    delete histo_wby;
    continue;
    }
    if (histo_wby->GetMaximum() > 5)
    {
    n++;
    delete lw;
    }
    delete histo_wby;
    }
    gr1 = new TGraph(n, x, y);
    gr1->SetTitle("Points found for fitting; bot time in ns; Bot ToT");
    gr1->Draw("A*");
    std::cout << "Start fitting\n";
    //Double_t y = 0;
    //y = -30.2 + par1 * TMath::Power(Q, par2) + par3 / Q + par4 * Q + par5 * Q * Q;
    TF1* f1 = new TF1("f1", "", min, max);
    f1->SetLineColor(2);
    gr1->Fit("f1", "Q",  -30.2 + par[1] * TMath::Power(x, par[2]) + par[3] / x + par[4] * x + par[5] * x * x;
    , min, max);
    //for (Int_t j = 0; j <= 3; j++)

    para1 = f1->GetParameter(par[1]);
    para2 = f1->GetParameter(par[2]);
    para3 = f1->GetParameter(par[3]);
    para4 = f1->GetParameter(par[4]);
    para5 = f1->GetParameter(par[5]);




    std::cout << "Parameter: " << para << "\n";
    par->SetPar1Walk(para1);
    par->SetPar2Walk(para2);
    par->SetPar3Walk(para3);
    par->SetPar4Walk(para4);
    par->SetPar5Walk(para5);



}
}
fHitPar->setChanged();
}*/
void R3BTofDCal2HitPar::smiley(TH2F* histo, Double_t min, Double_t max, Double_t* para)
{
	cout<<"Enter smiley function"<<endl;
	// This fits the smiley: Sqrt(q1*q2) returns position dependent charge, we fit that via pol3 and try to correct
	//Double_t y[1000], x[1000];
	Int_t n = 0;
	Int_t rebin1 = 1;
	Int_t rebin2 = 1;
	Int_t coarse_threshold = 5;
	Int_t fine_threshold = 2;
Redo:	
	Double_t *y;
		y = (Double_t*)malloc(1000);
	Double_t *x;
		x = (Double_t*)malloc(1000);
	fTofdTotLow = 100.;
	fTofdTotHigh = 350.;

	for (Int_t j = 0; j <= 3; j++)
	{
		para[j] = 0;
	}
	TGraph* gr1 = new TGraph();
	TGraph* gr2 = new TGraph();
	TCanvas* cfit_smiley = new TCanvas("cfit_smiley", "fit smiley", 150, 150, 1200, 1200);
	cfit_smiley->Clear();
	cfit_smiley->Divide(1, 4);
	cfit_smiley->cd(1);
	TH2F* histo1 = (TH2F*)histo->Clone();
	histo1->Draw("colz");
	TH2F* histo2 = (TH2F*)histo->Clone();
	histo2->RebinX(50);
	histo2->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
	// histo2->SetAxisRange(fTofdTotLow,fTofdTotHigh,"Y");
	cfit_smiley->cd(2);
	histo2->Draw("colz");
	std::cout << "Searching for points to fit...\n";
	for (Int_t i = 1; i < histo2->GetNbinsX(); i++)
	{
		std::cout<<"Bin "<<i<<" of "<<histo2->GetNbinsX()<<" with cut: "<<fTofdTotLow<<" < sqrt(q1*q2) <"<<fTofdTotHigh<<"\n";
		cfit_smiley->cd(2);
		TLine* l = new TLine(
				histo2->GetXaxis()->GetBinCenter(i), fTofdTotLow, histo2->GetXaxis()->GetBinCenter(i), fTofdTotHigh);
		l->SetLineColor(kRed);
		l->SetLineWidth(2.);
		l->Draw();
		cfit_smiley->cd(3);
		TH1F* histo_py = (TH1F*)histo2->ProjectionY("histo_py", i, i, "");
		//histo_py->Rebin(rebin1);
		histo_py->Draw();
		cfit_smiley->Update();
		x[n] = histo2->GetXaxis()->GetBinCenter(i);
		Int_t binmax = histo_py->GetMaximumBin();
		Double_t y_check = histo_py->GetXaxis()->GetBinCenter(binmax);

		if ((x[n] < min || x[n] > max) || (y_check < fTofdTotLow || y_check > fTofdTotHigh))
		{
			delete histo_py;
			continue;
		}
		if(x[n] == 0.) cout<<"x=0 ";
		if (histo_py->GetMaximum() > coarse_threshold)
		{
			Double_t fitrange = 5.;
			TF1* fgaus = new TF1("fgausX", "gaus(0)", /*histo_py->GetXaxis()->GetBinCenter(binmax)*/y_check - fitrange, /*histo_py->GetXaxis()->GetBinCenter(binmax)*/y_check + fitrange);
			fgaus->SetParameter(1, y_check);
			fgaus->SetParLimits(1,y_check - (fitrange*1.25),y_check + (fitrange*1.25));
			histo_py->Fit(fgaus, "QR");
			Double_t fix = fgaus->GetParameter(1);
			//y[n] = y_check;
			y[n] = fix;
			if(x[n] == 0.) cout<<"y "<<y_check<<" "<<y[n]<<endl;
			fgaus->Draw();
			cout<<"y "<<y_check<<" y_new "<<fix<<endl;
			n++;
			delete l;
			delete fgaus;
		}
		delete histo_py;
	}
	for(int c = 0; c<=n;c++){
		cout<<"n "<<c<<" x "<<x[c]<<" y "<<y[c]<<endl;
	}
	gr1 = new TGraph(n, x, y);
	gr1->SetTitle("Points found for fitting; x position in cm; sqrt(tot1*tot2)");
	gr1->Draw("A*");
	std::cout << "Start fitting\n";
	TF1* f1 = new TF1("f1", "pol3", min, max);
	f1->SetLineColor(2);
	gr1->Fit("f1", "Q", "", min, max);
	for (Int_t j = 0; j <= 3; j++)
	{
		para[j] = f1->GetParameter(j);
		std::cout << "Parameter: " << para[j] << "\n";
	}
	// fit again but with more information and better cuts
	std::cout << "Fit again with more information\n";
	n = 0;
	cfit_smiley->cd(4);
	for (Int_t i = 1; i < histo2->GetNbinsX(); i++)
	{
		Double_t pos = histo2->GetXaxis()->GetBinCenter(i);
		Double_t ymean = f1->Eval(pos);
		histo2->SetAxisRange(ymean - 5., ymean + 5., "Y");
		histo2->Draw("colz");
		TH1F* histo_py = (TH1F*)histo2->ProjectionY("histo_py", i, i, "");
		//histo_py->Rebin(rebin2);
		histo_py->Draw();
		x[n] = histo2->GetXaxis()->GetBinCenter(i);
		Int_t binmax = histo_py->GetMaximumBin();
		Double_t y_check = histo_py->GetXaxis()->GetBinCenter(binmax);
		//y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);

		if ((x[n] < min || x[n] > max) || (y_check < fTofdTotLow || y_check > fTofdTotHigh))
		{
			delete histo_py;
			continue;
		}
		if (histo_py->GetMaximum() > fine_threshold){
			Double_t fitrange = 5.;
			TF1* fgaus2 = new TF1("fgaus2", "gaus(0)", y_check - (fitrange), y_check + (fitrange));
			fgaus2->SetParameter(1, y_check);
			fgaus2->SetParLimits(1,y_check - (fitrange*1.25),y_check + (fitrange*1.25));
			histo_py->Fit(fgaus2, "QR");
			y[n] = fgaus2->GetParameter(1);
			//y[n] = y_check;
			cout<<"fine y "<<y_check<<" y_new "<<y[n]<<endl;
			fgaus2->Draw();
			n++;
		}
		delete histo_py;
	}
	for(int c = 0; c<=n;c++){
		cout<<"n_fine "<<c<<" x "<<x[c]<<" y "<<y[c]<<endl;
	}
	gr2 = new TGraph(n, x, y);
	gr2->SetTitle("More information;x position in cm;sqrt(q1*q2)");
	gr2->Draw("A*");
	f1->DrawCopy("SAME");
	TF1* f2 = new TF1("f2", "pol3", min, max);
	f2->SetParameters(para[0], para[1], para[2], para[3]);
	f2->SetLineColor(3);
	gr2->Fit("f2", "0Q", "", min, max);
	f2->Draw("SAME");
	std::cout << "Will write:\n";
	for (Int_t j = 0; j <= 3; j++)
	{
		para[j] = f2->GetParameter(j);
		std::cout << "Parameter"<<j<<": " << para[j] << "\n";
	}
	histo2->GetYaxis()->SetRangeUser(fTofdTotLow, fTofdTotHigh);
	auto legend = new TLegend(.9, 0.7, .99, 0.9);
	legend->AddEntry("f1", "First Fit", "l");
	legend->AddEntry("f2", "Second Fit", "l");
	legend->Draw();
	cfit_smiley->Update();
	 //gPad->WaitPrimitive();
	// gSystem->Sleep(3000);
	char cCheck = 'y';
	while(!(cCheck == 'y' || cCheck == 'n')){
		cout<< "Continue? (y/n) ";
		cin >> cCheck;
	}
	cout<<"1"<<endl;
	delete histo1;
	delete histo2;
	delete gr1;
	delete gr2;
	delete f1;
	delete f2;
	delete cfit_smiley;
	free(x);
	free(y);
	n=0;
	if(cCheck == 'n'){
		cout<<"New Rebin1: ";
		cin>> rebin1;
		cout<<"New coarse threshold: ";
		cin>> coarse_threshold;
		cout<<"New Rebin2: ";
		cin>> rebin2;
		cout<<"New fine threshold: ";
		cin>> fine_threshold;
		goto Redo;
	}
	cout<<"2"<<endl;
}

void R3BTofDCal2HitPar::doubleExp(TH2F* histo, Double_t min, Double_t max, Double_t* para)
{
	// This fits the exponential decay of the light in a paddle. The 2 PMTs are fit with the same function but one
	// side will deliver negative attenuation parameters and the other positive.
	Double_t y[1000], x[1000];
	Int_t n = 0;
	for (Int_t j = 0; j <= 3; j++)
	{
		para[j] = 0;
	}
	TGraph* gr1 = new TGraph();
	TGraph* gr2 = new TGraph();
	TCanvas* cfit_exp = new TCanvas("cfit_exp", "fit exponential", 100, 100, 800, 800);
	cfit_exp->Clear();
	cfit_exp->Divide(1, 3);
	cfit_exp->cd(1);
	TH2F* histo1 = (TH2F*)histo->Clone();
	TH2F* histo2 = (TH2F*)histo->Clone();
	histo1->Draw("colz");
	cfit_exp->cd(2);
	for (Int_t i = 1; i < histo1->GetNbinsX() - 1; i++)
	{
		TH1F* histo_py = (TH1F*)histo1->ProjectionY("histo_py", i, i, "");
		histo_py->Draw();
		x[n] = histo1->GetXaxis()->GetBinCenter(i);
		Int_t binmax = histo_py->GetMaximumBin();
		y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
		if ((x[n] < -40. || x[n] > 40.) || y[n] < 50.)
		{
			delete histo_py;
			continue;
		}
		if (histo_py->GetMaximum() > 5)
			n++;
		delete histo_py;
	}
	gr1 = new TGraph(n, x, y);
	gr1->Draw("A*");
	TF1* f1 = new TF1("f1", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
	f1->SetParameters(520., 0.001, 17234, -485.);
	f1->SetLineColor(2);
	gr1->Fit("f1", "", "", min, max);
	for (Int_t j = 0; j <= 3; j++)
	{
		para[j] = f1->GetParameter(j);
		std::cout << "Parameter: " << para[j] << "\n";
	}
	// fit again but with more information and better cuts
	n = 0;
	cfit_exp->cd(3);
	for (Int_t i = 1; i < histo2->GetNbinsX(); i++)
	{
		Double_t pos = histo2->GetXaxis()->GetBinCenter(i);
		Double_t ymean = para[0] * (exp(-para[1] * (pos + 100.)) + exp(-para[2] * (pos + 100.))) + para[3];
		histo2->SetAxisRange(ymean - 5., ymean + 5., "Y");
		histo2->Draw("colz");
		TH1F* histo_py = (TH1F*)histo2->ProjectionY("histo_py", i, i, "");
		histo_py->Draw();
		x[n] = histo2->GetXaxis()->GetBinCenter(i);
		Int_t binmax = histo_py->GetMaximumBin();
		y[n] = histo_py->GetXaxis()->GetBinCenter(binmax);
		if (histo_py->GetMaximum() > 2)
			n++;
		delete histo_py;
	}
	gr2 = new TGraph(n, x, y);
	gr2->Draw("A*");
	TF1* f2 = new TF1("f2", "[0]*(exp(-[1]*(x+100.))+exp(-[2]*(x+100.)))+[3]", min, max);
	f2->SetParameters(para[0], para[1], para[2], para[3]);
	f2->SetLineColor(2);
	gr2->Fit("f2", "", "", min, max);
	for (Int_t j = 0; j <= 3; j++)
	{
		para[j] = f2->GetParameter(j);
		std::cout << "Parameter: " << para[j] << "\n";
	}
	cfit_exp->Update();
	// gPad->WaitPrimitive();
	// gSystem->Sleep(3000);
	delete gr1;
	delete gr2;
	delete f1;
	delete f2;
}
/*   void R3BTofDCal2HitPar::calcZoffset()
    {
	    TCanvas* cZoff = new TCanvas("cZoff", "cZoff", 10, 10, 1000, 900);
	    cZoff->Divide(2, 2);
	    for (Int_t i = 0; i < fNofPlanes; i++)
	    {
		    for (Int_t j = 0; j < fPaddlesPerPlane; j++)
		    {
			    //Double_t max = 0.;
			    //Double_t veff = 7.;
			   if (fZoffset[i][j])
			    {
				    LOG(info) << "Found histo Time_Diff_Plane_" << i + 1;
				    auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
				    if (!par)
				    {
					    LOG(warning) << "Hit par not found, Plane: " << i + 1 << ", Bar: " << j + 1;
					    continue;
				    }
				    // auto* h = (TH2F*)histofilename->Get(Form("Time_Diff_Plane_%i", i + 1))->Clone();
				    cZoff->cd(i + 1);
				    // h->Draw("colz");
				    TH1F* histo_zy = (TH1F*)fhZoffset[i][j]->ProjectionY("histo_zy", j + 2, j + 2, "");
				    Int_t binmax = histo_zy->GetMaximumBin();
				    max = histo_zy->GetXaxis()->GetBinCenter(binmax);
				    Double_t maxEntry = histo_zy->GetBinContent(binmax);
				    auto* fgaus = new TF1("fgaus", "gaus(0)", max - 0.4, max + 0.4); /// TODO: find best range
				    fgaus->SetParameters(maxEntry, max, 20);
				    histo_zy->Fit("fgaus", "QR0");
				    //Double_t offset1 = par->GetOffset1();
				    //Double_t offset2 = par->GetOffset2();
				    Double_t cent = fgaus->GetParameter(1) /// TODO: needs to be tested
				    // max = max+offset1-offset2;
				    Zoffset = fMaxQ - cent; // effective speed of light in [cm/s]
				    //LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " offset  " << par->GetOffset1();
				    //LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " max  " << max;
				    //LOG(info) << "Plane  " << i + 1 << " Bar " << j + 1 << " veff  " << veff;
				    par->SetZoffset(Zoffset);
			    }
		    }
	    }
	    fHitPar->setChanged();
    }
*/


void R3BTofDCal2HitPar::FinishTask()
{	

	if (fParameter == 10)
	{
		// Determine time offset of the 2 PMTs of one paddle. This procedure
		// assumes a sweep run in the middle of the ToF wall horizontally.
		// Since all paddles are mounted vertically one can determine the offset.
		// Half of the offset is added to PM1 and half to PM2.
		// Determine ToT offset between top and bottom PMT
		LOG(info) << "Calling function calcToTOffset";
		calcToTOffset(fTofdTotLow, fTofdTotHigh);
		// Determine sync offset between paddles

		for (Int_t i = 0; i < fNofPlanes; i++)
		{
		fhTofsync[i]->Write();
		fh_tofd_TotPm[i][0]->Write();
		fh_tofd_TotPm[i][1]->Write();
		fh_tofd_TotPm[i][2]->Write();
		}
	}
	if (fParameter == 1)
	{
		// Determine time offset of the 2 PMTs of one paddle. This procedure
		// assumes a sweep run in the middle of the ToF wall horizontally.
		// Since all paddles are mounted vertically one can determine the offset.
		// Half of the offset is added to PM1 and half to PM2.
		LOG(info) << "Calling function calcOffset";
		calcOffset();
		// Determine ToT offset between top and bottom PMT
		LOG(info) << "Calling function calcToTOffset";
		calcToTOffset(fTofdTotLow, fTofdTotHigh);
		// Determine sync offset between paddles
		LOG(info) << "Calling function calcSync";
		calcSync();
		LOG(error) << "Call walk correction before next step!";

	/*	for (Int_t i = 0; i < fNofPlanes; i++)
		{
			for (Int_t j = 0; j < fPaddlesPerPlane; j++)
			{
				auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
				Int_t binmax = fh1_tofsync[i][j]->GetMaximumBin();
				auto tofsync = fh1_tofsync[i][j]->GetXaxis()->GetBinCenter(binmax);
				par->SetTofSyncOffset(tofsync - fMeanTof);
				LOG(info) << " Plane  " << i + 1 << " Bar " << j + 1 << " Tof-Sync  " << tofsync;
				fh1_tofsync[i][j]->Write();
			}
		fhTofsync[i]->Write();
		fh_tofd_TotPm[i][0]->Write();
		fh_tofd_TotPm[i][1]->Write();
		fh_tofd_TotPm[i][2]->Write();
		}*/
	}
	else if(fParameter == 0)
	{
		// Calculate walk corrections
		LOG(info) << "Calling function";
		//calcwalk();


	}
	else if (fParameter == 2)
	{
		//Calculate ToF
		LOG(info) << "Calculate ToF";
		for (Int_t i = 0; i < fNofPlanes; i++)
		{
			for (Int_t j = 0; j < fPaddlesPerPlane; j++)
			{
				auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
				Int_t binmax = fh1_tofsync[i][j]->GetMaximumBin();
				auto tofsync = fh1_tofsync[i][j]->GetXaxis()->GetBinCenter(binmax);
				par->SetTofSyncOffset(tofsync - fMeanTof);
				LOG(info) << " Plane  " << i + 1 << " Bar " << j + 1 << " Tof-Sync  " << tofsync;
				fh1_tofsync[i][j]->Write();
			}
		fhTofsync[i]->Write();
		fh_tofd_TotPm[i][0]->Write();
		fh_tofd_TotPm[i][1]->Write();
		fh_tofd_TotPm[i][2]->Write();
		}
		// Determine effective speed of light in [cm/s] for each paddle
		LOG(info) << "Calling function";
		calcVeff();
		// Determine light attenuation lambda for each paddle
		LOG(info) << "Calling function calcLambda";
		calcLambda(fTofdTotLow, fTofdTotHigh);

		for(int i=0; i<4; i++){
			fhTofsync[i]->Write();
			fhTdiff[i]->Write();
		fh_tofd_TotPm[i][1]->Write();
		fh_tofd_TotPm[i][0]->Write();
		fh_tofd_TotPm[i][2]->Write();
			for(int j=0;j<44;j++){
				fhSqrtQvsPosToT[i][j]->Write();
			}
		}
	}
	else if (fParameter == 3)
	{
		// calculation of position dependend charge
		if (fTofdSmiley)
		{
			
		//	LOG(info) << "Calling function smiley";
		//	Double_t para2[4];
		//	for (Int_t i = 0; i < 4; i++)
		//		para2[i] = 0.;
		//	Double_t min2 = -40.; // -40 effective bar length
		//	Double_t max2 = 40.;  // 40 effective bar length = 80 cm
		//	// we will use 50 here for some fit safety margin
		//	for (Int_t i = 0; i < /*fNofPlanes*/1; i++)
		//	{
		//		for (Int_t j = 0; j < fPaddlesPerPlane; j++)
		//		{
		//			if (fhSqrtQvsPosToT[i][j])
		//			{
		//				LOG(info) << "Calling Plane " << i + 1 << " Bar " << j + 1;
		//				auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
		//				smiley(fhSqrtQvsPosToT[i][j], min2, max2, para2);
//
		//				cout<<"Plane "<<i<<" Paddle "<<j<<" ";
		//				for(int k = 0; k<4; k++){
		//					cout<<"para"<<k<<" "<<para2[k]<<" ";
		//				}
		//				cout<<endl;
		//				
		//				Double_t offset1 = par->GetToTOffset1();
		//				Double_t offset2 = par->GetToTOffset2();
		//				Double_t veff = par->GetVeff();
		//				Double_t sync = par->GetSync();
		//				par->SetPola(para2[0]);
		//				par->SetPolb(para2[1]);
		//				par->SetPolc(para2[2]);
		//				par->SetPold(para2[3]);
		//			
		//			fhSqrtQvsPosToT[i][j]->Write();
		//			}
		//		}
		//	}
			
			for(int i=0;i<fNofPlanes;i++){
				for(int j=0;j<fPaddlesPerPlane;j++){
					fhSqrtQvsPosToT[i][j]->Write();
					fhTot2vsPos[i][j]->Write();
					fhTot1vsPos[i][j]->Write();
				}
			}
			//fHitPar->setChanged();
		}
		else
		{
			//LOG(info) << "Calling function doubleExp";
			//Double_t para[4];
			//for (Int_t i = 0; i < 4; i++)
			//	para[i] = 0.;
			//Double_t min = -40.; // effective bar length
			//Double_t max = 40.;  // effective bar length = 80 cm
//
			//for (Int_t i = 0; i < fNofPlanes; i++)
			//{
			//	for (Int_t j = 0; j < fPaddlesPerPlane; j++)
			//	{
			//		if (fhTot1vsPos[i][j])
			//		{
			//			auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
			//			doubleExp(fhTot1vsPos[i][j], min, max, para);
			//			Double_t offset1 = par->GetOffset1();
			//			Double_t offset2 = par->GetOffset2();
			//			Double_t veff = par->GetVeff();
			//			Double_t sync = par->GetSync();
			//			par->SetPar1a(para[0]);
			//			par->SetPar1b(para[1]);
			//			par->SetPar1c(para[2]);
			//			par->SetPar1d(para[3]);
			//		}
			//		if (fhTot2vsPos[i][j])
			//		{
			//			auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
			//			doubleExp(fhTot2vsPos[i][j], min, max, para);
			//			Double_t offset1 = par->GetOffset1();
			//			Double_t offset2 = par->GetOffset2();
			//			Double_t veff = par->GetVeff();
			//			Double_t sync = par->GetSync();
			//			par->SetPar2a(para[0]);
			//			par->SetPar2b(para[1]);
			//			par->SetPar2c(para[2]);
			//			par->SetPar2d(para[3]);
			//		}
			//	}
			//}
			for(int i=0;i</*fNofPlanes*/1;i++){
				for(int j=0;j<fPaddlesPerPlane;j++){
					fhTot2vsPos[i][j]->Write();
					fhTot1vsPos[i][j]->Write();
				}
			}
			//fHitPar->setChanged();
		}
	}

	if(fParameter == -3){
		if (fTofdSmiley)
		{
			
			LOG(info) << "Calling function smiley";
			TFile* histofilename = TFile::Open(fHistoFile);
			Double_t para2[4];
			for (Int_t i = 0; i < 4; i++)
				para2[i] = 0.;
			Double_t min2 = -40.; // -40 effective bar length
			Double_t max2 = 40.;  // 40 effective bar length = 80 cm
			// we will use 50 here for some fit safety margin
			for (Int_t i = 0; i < fNofPlanes; i++)
			{
				for (Int_t j = 0; j < fPaddlesPerPlane; j++)
				{
					//if (fhSqrtQvsPosToT[i][j])
					if (histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)))
					{

						for (Int_t l = 0; l < 4; l++)
							para2[l] = 0.;
						LOG(info) << "Calling Plane " << i + 1 << " Bar " << j + 1;
						auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
						smiley(/*fhSqrtQvsPosToT[i][j]*/(TH2F*)histofilename->Get(Form("SqrtQ_vs_PosToT_Plane_%i_Bar_%i", i + 1, j + 1)), min2, max2, para2);

						cout<<"Plane "<<i+1<<" Paddle "<<j+1<<" ";
						for(int k = 0; k<4; k++){
							cout<<"para"<<k<<" "<<para2[k]<<" ";
						}
						cout<<endl;
						cout<<"3"<<endl;
						
						if(para2[0]== 0. && para2[1]== 0. && para2[2]== 0. && para2[3]== 0.){
							para2[0] = 1.;
							cout<<"Not enough statistics for smiley correction. Set par0 = 1, rest 0"<<endl;
						}

						Double_t offset1 = par->GetToTOffset1();
						Double_t offset2 = par->GetToTOffset2();
						Double_t veff = par->GetVeff();
						Double_t sync = par->GetSync();
						cout<<"3.5"<<endl;
						par->SetPola(para2[0]);
						cout<<"3.6"<<endl;
						par->SetPolb(para2[1]);
						cout<<"3.7"<<endl;
						par->SetPolc(para2[2]);
						cout<<"3.8"<<endl;
						par->SetPold(para2[3]);
						cout<<"4"<<endl;
					
					//fhSqrtQvsPosToT[i][j]->Write();
					}
				}
			}
			fHitPar->setChanged();

		}
		else
		{
			LOG(info) << "Calling function doubleExp";
			Double_t para[4];
			for (Int_t i = 0; i < 4; i++)
				para[i] = 0.;
			Double_t min = -40.; // effective bar length
			Double_t max = 40.;  // effective bar length = 80 cm

			for (Int_t i = 0; i < fNofPlanes; i++)
			{
				for (Int_t j = 0; j < fPaddlesPerPlane; j++)
				{
					if (fhTot1vsPos[i][j])
					{
						auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
						doubleExp(fhTot1vsPos[i][j], min, max, para);
						Double_t offset1 = par->GetOffset1();
						Double_t offset2 = par->GetOffset2();
						Double_t veff = par->GetVeff();
						Double_t sync = par->GetSync();
						par->SetPar1a(para[0]);
						par->SetPar1b(para[1]);
						par->SetPar1c(para[2]);
						par->SetPar1d(para[3]);
					fhTot1vsPos[i][j]->Write();
					}
					if (fhTot2vsPos[i][j])
					{
						auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
						doubleExp(fhTot2vsPos[i][j], min, max, para);
						Double_t offset1 = par->GetOffset1();
						Double_t offset2 = par->GetOffset2();
						Double_t veff = par->GetVeff();
						Double_t sync = par->GetSync();
						par->SetPar2a(para[0]);
						par->SetPar2b(para[1]);
						par->SetPar2c(para[2]);
						par->SetPar2d(para[3]);
					fhTot2vsPos[i][j]->Write();
					}
				}
			}
			fHitPar->setChanged();
		}
	}

	if (fParameter == 4)
	{
		//Write Histograms for Charge correction.
		//Call fParameter = 5 to read these Histogramms.
		for (Int_t i = 0; i < fNofPlanes; i++)
		{

			//if(i > 0) continue;


			for (Int_t j = 0; j < fPaddlesPerPlane; j++)
			{
				if (fhQvsPos[i][j])
				{
					/*auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
					std::cout << "Calling Plane: " << i + 1 << " Bar " << j + 1 << "\n";
					Int_t index = i * fPaddlesPerPlane + j;
					int a = zcorr(fhQvsPos[i][j], min, max, pars, index);
					std::cout << "Write parameter: " << pars[0] << " " << pars[1] << " " << pars[2] << "\n";
					
					if(a > 0) j = j - a;

					Double_t offset1 = par->GetOffset1();
					Double_t offset2 = par->GetOffset2();
					Double_t veff = par->GetVeff();
					Double_t sync = par->GetSync();
					par->SetPar1za(pars[0]);
					par->SetPar1zb(pars[1]);
					par->SetPar1zc(pars[2]);
			*/
					fhQvsPos[i][j]->Write();
				}
				if(fhQXvsPos[i][j]){
					fhQXvsPos[i][j]->Write();
				}
			}
		}
		fHitPar->setChanged();
	}

	if(fParameter == 5){
		// Z correction for each plane
		LOG(warning) << "Calling function zcorr";
		TFile* histofilename = TFile::Open(fHistoFile);
		Double_t para[8];
		Double_t pars[3];
		pars[0] = 0.;
		pars[1] = 0.;
		pars[2] = 0.;
		//Int_t min = fMinQ, max = fMaxQ; // select range for peak search
		Double_t min = 0., max = 16.; // select range for peak search
		cout<<"1"<<endl;
			for (Int_t i = 0; i < fNofPlanes; i++)
			{

			//	if(i > 0) continue;
			cout<<"2"<<endl;


				for (Int_t j = 0; j < fPaddlesPerPlane; j++)
				{
		cout<<"3 "<<j<<endl;
					//if (fhQvsPos[i][j])
					if (histofilename->Get(Form("QX_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
					{
		cout<<"4"<<endl;
						auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
						std::cout << "Calling Plane: " << i + 1 << " Bar " << j + 1 << "\n";
						Int_t index = i * fPaddlesPerPlane + j;
						//int a = zcorr(fhQvsPos[i][j], min, max, pars, index);
						int a = zcorr((TH2F*)histofilename->Get(Form("QX_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)), min, max, pars, i, j, index);
						std::cout << "Write parameter: " << pars[0] << " " << pars[1] << " " << pars[2] << "\n";
		cout<<"5"<<endl;

						if(a > 0) j = j - a;

						Double_t offset1 = par->GetOffset1();
						Double_t offset2 = par->GetOffset2();
						Double_t veff = par->GetVeff();
						Double_t sync = par->GetSync();
						par->SetPar1za(pars[0]);
						par->SetPar1zb(pars[1]);
						par->SetPar1zc(pars[2]);
						//fhQvsPos[i][j]->Write();
		cout<<"6"<<endl;
					}
				}
			}
	}
	if(fParameter == 6){
		//Use this if there are individual channels which need adjustment by hand


		Double_t para[8];
		Double_t pars[3];
		pars[0] = 0.;
		pars[1] = 1.;
		pars[2] = 0.;
		para[0] = 0.;
		para[1] = 0.;
		para[2] = 0.;
Double_t min = 0., max = 16.; // select range for peak search
		cout<<"1"<<endl;
			for (Int_t i = 0; i < fNofPlanes; i++)
			{

				if(i > 0) continue;
			cout<<"2"<<endl;


				for (Int_t j = 0; j < fPaddlesPerPlane; j++)
				{
		cout<<"3 "<<j<<endl;
					if (fhQXvsPos[i][j])
					//if (histofilename->Get(Form("QX_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)))
					{
		cout<<"4"<<endl;
						auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
						std::cout << "Calling Plane: " << i + 1 << " Bar " << j + 1 << "\n";
						Int_t index = i * fPaddlesPerPlane + j;
						int a = zcorr(fhQXvsPos[i][j], min, max, pars, i, j, index);
						//int a = zcorr((TH2F*)histofilename->Get(Form("QX_vs_Pos_Plane_%i_Bar_%i", i + 1, j + 1)), min, max, pars, i, j, index);
		cout<<"5"<<endl;

						if(a > 0) j = j - a;

						Double_t offset1 = par->GetOffset1();
						Double_t offset2 = par->GetOffset2();
						Double_t veff = par->GetVeff();
						Double_t sync = par->GetSync();
							para[0] = par->GetPar1za();
                			para[1] = par->GetPar1zb();

						std::cout << "Old parameter: " << para[0] << " " << para[1] << " " << pars[2] << "\n";
						std::cout << "New parameter: " << pars[0] << " " << pars[1] << " " << pars[2] << "\n";
						std::cout << "Write parameter: " << pars[0]*para[1]+para[0] << " " << pars[1]*para[1] << " " << pars[2] << "\n";

						par->SetPar1za(pars[0]*para[1]+para[0]);
						par->SetPar1zb(pars[1]*para[1]);
						par->SetPar1zc(pars[2]);
						//fhQvsPos[i][j]->Write();
		cout<<"6"<<endl;
					}
				}
			}
		/*for (Int_t i = 0; i < fNofPlanes; i++)
			{

				if(i > 0) continue;


				
					//if (fhQvsPos[i][j])
					if (histofilename->Get(Form("tofd_hit_Q_plane_%d", i + 1)))
					{
						pars[0] = 0.;
						pars[1] = 1.;
						pars[2] = 0.;
						para[0] = 0.;
						para[1] = 0.;
						para[2] = 0.;
						TH2F* histo = (TH2F*)histofilename->Get(Form("tofd_hit_Q_plane_%d", i + 1));
						for (Int_t j = 0; j < fPaddlesPerPlane; j++)
						{
							if(j>41) continue;
							TCanvas* c1 = new TCanvas("c1", "c1", 150, 150, 1200, 1200);
							c1->Clear();
							c1->Divide(1, 3);
							c1->cd(1);
							gPad->SetLogz();
							TH2F* histo1 = (TH2F*)histo->Clone();
							histo1->SetAxisRange(7.69, 8.15, "Y");
							TH2F* histo2 = (TH2F*)histo->Clone();
							histo2->SetAxisRange(1.8, 2.1, "Y");
							TH1F* histo_py = (TH1F*)histo->ProjectionY("histo_py", j+1, j+1, "");
							TH1F* histo_py8 = (TH1F*)histo1->ProjectionY("histo_py8", j+1, j+1, "");
							//histo_py8->GetXaxis()->SetRange(7.69, 8.15);
							TH1F* histo_py2 = (TH1F*)histo2->ProjectionY("histo_py2", j+1, j+1, "");
							//histo_py2->GetXaxis()->SetRange(1.8, 2.1);
							auto par = fHitPar->GetModuleParAt(i + 1, j + 1);
							std::cout << "Calling Plane: " << i + 1 << " Bar " << j + 1 << "\n";

							histo->Draw("colz");
							c1->cd(2);
							histo_py8->Draw();
							c1->cd(3);
							histo_py2->Draw();
							//histo_py8->SetAxisRange(7.69, 8.15, "X");
							//dhisto_py2->SetAxisRange(1.8, 2.1,"X");

							//Int_t binmax8 = histo_py8->GetMaximum();
				    		//Double_t Max8 = histo_py8->GetXaxis()->GetBinCenter(binmax8);
				    		//Int_t binmax2 = histo_py2->GetMaximum();
				    		//Double_t Max2 = histo_py2->GetXaxis()->GetBinCenter(binmax2);

							
							Int_t binmax8 = histo_py8->GetMaximumBin();
							
							//histo_py8->GetXaxis()->SetRange(7.69, 8.15);
				    		//Int_t binmax8 = histo->GetYaxis()->GetMaximum();
							//histo_py->SetAxisRange(1.8, 2.1, "X");
				    		Int_t binmax2 = histo_py2->GetMaximumBin();



							//histo_py->SetAxisRange(0., 12., "X");
				    		Double_t Max8 = histo_py8->GetBinCenter(binmax8);
				    		Double_t Max2 = histo_py2->GetBinCenter(binmax2);

							TF1* fgaus1 = new TF1("fgaus1", "gaus(0)", Max8 - 2., Max8 + 2.);
							histo_py8->Fit("fgaus1", "QR0");
							Double_t m8 = fgaus1->GetParameter(1);
							TF1* fgaus2 = new TF1("fgaus2", "gaus(0)", Max2 - 2., Max2 + 2.);
							histo_py2->Fit("fgaus2", "QR0");
							Double_t m2 = fgaus2->GetParameter(1);


							cout<<"binmax8 "<<binmax8<<" binmax2 "<<binmax2<<endl;
							cout<<"Max8 "<<Max8<<" Max2 "<<Max2<<endl;
							cout<<"M8 "<<m8<<" M2 "<<m2<<endl;

							Double_t f = 6. / (m8 - m2);
							Double_t d = m8 * f - 8.;

							cout<<"f "<<f<<" d "<<d<<endl;

							para[0] = par->GetPar1za();
                			para[1] = par->GetPar1zb();

							pars[0] = para[0] * f - d;
							pars[1] = para[1] * f;

							cout<<"Old pars: "<<para[0]<<" "<<para[1]<<endl;
							cout<<"New pars: "<<pars[0]<<" "<<pars[1]<<endl;

							par->SetPar1za(pars[0]);
							par->SetPar1zb(pars[1]);

							char check = 'x';

	 						if(j==-1){
								cout<<histo_py2->GetBinContent(0);
							gPad->WaitPrimitive();
	 						gSystem->Sleep(3000);
							}
							cout<<"Checking";
							//cin>>check;
							delete histo1;
							delete histo2;
							delete histo_py8;
							delete histo_py2;
						}

				}
			
			}
		fHitPar->setChanged();*/
	}
	if(fParameter==7){
		TFile* histofilename = TFile::Open(fHistoFile);
		Double_t para[8];
		Double_t pars[3];
		pars[0] = 0.;
		pars[1] = 0.;
		pars[2] = 0.;
		//Int_t min = fMinQ, max = fMaxQ; // select range for peak search
		Double_t min = 0., max = 12.; // select range for peak search
		cout<<"1"<<endl;
			for (Int_t i = 0; i < fNofPlanes; i++)
			{

			//	if(i > 0) continue;


				for (Int_t j = 0; j < fPaddlesPerPlane; j++)
				{
		
					//if (fhQvsPos[i][j])
					if (histofilename->Get(Form("tofd_hit_Q_plane_%d", i + 1)))
					{
		
						auto parY = fHitPar->GetModuleParAt(i + 1, j + 1);
						std::cout << "Calling Plane: " << i + 1 << " Bar " << j + 1 << "\n";
						Int_t index = i * fPaddlesPerPlane + j;
						//int a = zcorr(fhQvsPos[i][j], min, max, pars, index);
						auto* hist = (TH2F*)histofilename->Get(Form("tofd_hit_Q_plane_%d", i + 1));
						auto* histo = hist->ProjectionY("p_y", j+2, j+2,"");
						
						//begin zcorr
						if (histo->GetEntries() < 10)
	    				{
						    R3BLOG(warning, "Nb of events below 10 with "<< histo->GetEntries() << " entries for histo with index" << index);

						    continue;
	    				}

	    				Double_t par[3000] = { 0 };
	    				Double_t x[3000] = { 0 };
	    				char strName[255];
						int a = 0;
	    				sprintf(strName, "canvas_%d", index);
	    				TCanvas* c1 = new TCanvas(strName, "", 100, 100, 800, 800);
	    				c1->Divide(1, 3);
	    				c1->cd(1);
						gPad->SetLogz();
	    					hist->Draw("colz");
	    					// Projection of charge axis
	    					auto* h1 = (TH2F*)histo->Clone();

						c1->cd(2);
						gPad->SetLogy();
	    				h1->Draw();
	    				// Use TSpectrum to find the peak candidates
						Double_t threshhold = 0.001;
						Int_t sigma = 10;
						Double_t sqrtf = 1.;
	    				Int_t nPeaks = fNbZPeaks;
					doagainfit:
						TSpectrum* s = new TSpectrum(nPeaks);
	    				Int_t nfound = s->Search(h1, sigma, "nobackground", threshhold);
	    				std::cout << "Found " << nfound << " candidate peaks to fit\n";

	    				if (nfound == 0)
	    				{
						    delete s;
						    delete c1;
						    delete histo;
						    delete h1;
						    continue;
	    				}

	    				c1->Update();
	    				// Eliminate background peaks
	    				Int_t mPeaks = 0;
	    				Double_t* xpeaks = s->GetPositionX();
	    				for (Int_t p = 0; p <= nfound; p++)
	    				{
						    Float_t xp = xpeaks[p];
						    Int_t bin = h1->GetXaxis()->FindBin(xp);
						    Float_t yp = h1->GetBinContent(bin);
						    if (yp - sqrtf * TMath::Sqrt(yp) < 1.){
							    cout<<"jump "<<yp - sqrtf * TMath::Sqrt(yp)<<endl;
								continue;
							}
						    par[2 * mPeaks] = yp;
						    par[2 * mPeaks + 1] = xp;
						    mPeaks++;
	    				}

	    				Double_t peaks[mPeaks];
	    				for (Int_t ii = 0; ii < mPeaks; ii++)
	    				{
						    // printf("Found peak @ %f\n",xpeaks[i]);
						    peaks[ii] = par[2 * ii + 1];
	    				}
	    				c1->Update();

	    				if (mPeaks < 2 && 0)
	    				{
						    pars[0] = 0.;
						    //pars[1] = fMaxQ / peaks[0];
						    pars[1] = 8. / peaks[0];
						    pars[2] = 0.;

						    delete s;
						    delete c1;
						    delete histo;
						    delete h1;
						    continue;
	    				}
						cout<<"mPeaks "<<mPeaks<<endl;
	    				// select useful peaks
	    				sort(peaks, peaks + mPeaks);
	    				Double_t zpeaks[3000] = { 0 };
	    				string doagain = "y";
	    				Int_t nfp;
//doagai				nfit:
						Int_t z = 0;
	    				do
	    				{
							string peakscheck = "x";
							std::cout << "Peaks ok? (y/n/c/goback) ";
						    std::cin >> peakscheck;
							if(peakscheck == "n"){
								delete s;
								std::cout << "New sigma: ";
						    	std::cin >> sigma;
								std::cout << "New threshhold: ";
						    	std::cin >> threshhold;
								std::cout << "New Sqrt factor: ";
						    	std::cin >> sqrtf;
								std::cout << "New # of peaks: ";
						    	std::cin >> nPeaks;
								goto doagainfit;
							}
							if(peakscheck == "goback"){
								a=0;
								std::cout << "How many bars? ";
						    	std::cin >> a;
							}
							while(peakscheck == 'c'){
								cout<< "We see "<<mPeaks<<" peaks. Continue? (y/n) ";
								cin >> peakscheck;
							}

							nfp = 0;
						    for (Int_t ii = 0; ii < mPeaks; ii++)
						    {
							    std::cout << "Peak @ " << peaks[ii];
							    while ((std::cout << " corresponds to Z=") && !(std::cin >> z))
							    {
								    std::cout << "That's not a number;";
								    std::cin.clear();
								    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
							    }
							    if (z == 0)
								    continue;
							    x[nfp] = (Double_t)z;
							    zpeaks[nfp] = peaks[ii];
							    // std::cout<<"z real " << x[nfp] << " z found " << zpeaks[nfp] <<"\n";
							    nfp++;
						    }
							doagain = 'n';
//						    std::cout << "Do again? (y/n) ";
//						    std::cin >> doagain;
	    				} while (doagain != "n");

	    				if (mPeaks < 2)
	    				{
						    pars[0] = 0.;
						    //pars[1] = fMaxQ / peaks[0];
						    pars[1] = z / peaks[0];
						    pars[2] = 0.;

						    delete s;
						    delete c1;
						    delete histo;
						    delete h1;
						    continue;
	    				}

	    				// fit charge axis
	    				std::cout << "Selected " << nfp << " useful peaks to fit\nStart fitting...\n";
	    				c1->cd(3)->Clear();
	    				c1->Update();
	    				c1->cd(3);
	    				auto gr1 = new TGraph(nfp, zpeaks, x);
	    				gr1->SetMarkerColor(4);
	    				gr1->SetMarkerSize(1.5);
	    				gr1->SetMarkerStyle(20);
	    				gr1->Draw("AP");
						//if(fitz) delete fitz;
	    				//TF1* fitz = new TF1("fitz", "[0] + [1]*x + [2]*x*x", min, max);
	    				if (fZfitType != "pol1" && fZfitType != "pol2")
	    				{
						    R3BLOG(error, "Fit " << fZfitType << " is not allowed, use pol1 or pol2 ");
						    continue;
	    				}
	    				auto fitz = new TF1("fitz", /*fZfitType*/"pol1", 0., 12.);
	    				fitz->SetLineColor(2);
	    				fitz->SetLineWidth(2);
	    				fitz->SetLineStyle(1);
	    				// fitz->SetParameters(1.5, 2., .1);
	    				gr1->Fit(fitz);
	    				fitz->Draw("lsame");
	    				c1->Update();
						
	    				// write parameters
	    				std::cout << "Is OK? (y/n) ";
	    				std::cin >> doagain;
	    				if (doagain == "n"){
						    std::cout << "New sigma: ";
						    std::cin >> sigma;
							std::cout << "New threshhold: ";
						    std::cin >> threshhold;
							delete s;
							goto doagainfit;
						}

	    				int nbpars = 2;
	    				if (fZfitType == "pol2")
						    nbpars = 3;

	    				pars[0] = fitz->GetParameter(0);
	    				pars[1] = fitz->GetParameter(1);
						
						for (Int_t k = 0; k < nbpars; k++)
	    				{
						    pars[k] = fitz->GetParameter(k);
						    std::cout<<fitz->GetParameter(k)<<"\n";
						    //std::cout<<Form("par%i= ",jj)<<pars[jj]<<"\n";
	    				}
						if(doagain == 's'){
							pars[0] = 0.;
							pars[1] = 1.;
						}
						
						//c1->Write();
	    				delete s;
	    				delete histo;
	    				delete h1;
	    				delete gr1;
	    				delete c1;
	    				delete fitz;

						//end zcorr
						
						
						std::cout << "Write parameter: " << pars[0] << " " << pars[1] << " " << pars[2] << "\n";
	

						if(a > 0){ 
							j = j - a;
							continue;
						}


						Double_t offset1 = parY->GetOffset1();
						Double_t offset2 = parY->GetOffset2();
						Double_t veff = parY->GetVeff();
						Double_t sync = parY->GetSync();
							para[0] = parY->GetPar1za();
                			para[1] = parY->GetPar1zb();
                			para[2] = parY->GetPar1zc();

						std::cout << "Old parameter: " << para[0] << " " << para[1] << " " << para[2] << "\n";
						std::cout << "New parameter: " << pars[0] << " " << pars[1] << " " << pars[2] << "\n";
						std::cout << "Write parameter: " << para[0]*pars[1]+pars[0] << " " << pars[1]*para[1] << " " << para[2]*pars[1] << "\n";

						parY->SetPar1za(para[0]*pars[1]+pars[0]);
						parY->SetPar1zb(pars[1]*para[1]);
						parY->SetPar1zc(para[2]*pars[1]);
					}

				}

			}

	}

 /*if (fParameter == 5)
	{
		// Determine effective speed of light in [cm/s] for each paddle
		LOG(info) << "Calling function";
		calcZoffset();
		// Determine light attenuation lambda for each paddle
		//LOG(info) << "Calling function calcLambda";
		//calcLambda(fTofdTotLow, fTofdTotHigh);
	}
*/
//	for (Int_t i = 0; i < N_TOFD_HIT_PLANE_MAX; i++)
//	{ 
		//if(fhlos[i])
		//fhlos[i]->Write();
	//	if(fhTofsync[i])
	//		fhTofsync[i]->Write();
	//	if (fh_tofd_TotPm[i])
	//		fh_tofd_TotPm[i]->Write(); // control histogram for ToT
	//	if (fhTsync[i])
	//		fhTsync[i]->Write(); // histogram for sync calculation
	//	if (fhTdiff[i])
	//		fhTdiff[i]->Write(); // histogram for offset and veff calculation
	//	for (Int_t j = 0; j < N_TOFD_HIT_PADDLE_MAX; j++)
	//	{
			// if(fhlost[1][11])
			// fhlost[1][11]->Write();
	//		if(fhtott[i][j])
	//			fhtott[i][j]->Write();
	//		if (fh1_tofsync[i][j])
	//			fh1_tofsync[i][j]->Write(); // histogram for ToF sync calculation
	//		if (fhLogTot1vsLogTot2[i][j])
	//			fhLogTot1vsLogTot2[i][j]->Write(); // control histogram Log(ToT) Pm1 vs Log(ToT) Pm2
	//		if (fhSqrtQvsPosToT[i][j])
	//			fhSqrtQvsPosToT[i][j]->Write(); // histogram for ToT offset calculation
	//		if (fhQvsPos[i][j])
	//			fhQvsPos[i][j]->Write(); // histogram for charge fit
	//		if (fh1_walk_b[i][j])
	//			fh1_walk_b[i][j]->Write();
	//		if (fh1_walk_t[i][j])
	//			fh1_walk_t[i][j]->Write();
			//if (fZoffset[i][j])
			//	fZoffset[i][j]->Write();



		//	if (!fTofdSmiley)
		//	{
		//		if (fhTot1vsPos[i][j])
		//			fhTot1vsPos[i][j]->Write(); // histogram for position dependence of charge 1
		//		if (fhTot2vsPos[i][j])
		//			fhTot2vsPos[i][j]->Write(); // histogram for position dependence of charge 2
		//	}
			/*
			   if (fhTot1vsTot2[i][j])
			   fhTot1vsTot2[i][j]->Write(); // control histogram ToT Pm1 vs ToT Pm2
			   */
		//}
	//}
}

Double_t R3BTofDCal2HitPar::walk(Double_t Q, Double_t par1, Double_t par2, Double_t par3, Double_t par4, Double_t par5)
{
	Double_t y = 0;
	y = -30.2 + par1 * TMath::Power(Q, par2) + par3 / Q + par4 * Q + par5 * Q * Q;
	return y;
}

Double_t R3BTofDCal2HitPar::saturation(Double_t x)
{
	Double_t kor;
	Int_t voltage = 600;
	if (voltage == 600)
	{
		if (x < 173)
		{
			kor = 0.;
		}
		else if (x > 208)
		{
			kor = -1.73665e+03 + 2.82009e+01 * 208. - 1.53846e-01 * (208. * 208.) + 2.82425e-04 * (208. * 208. * 208.);
		}
		else
		{
			kor = -1.73665e+03 + 2.82009e+01 * x - 1.53846e-01 * (x * x) + 2.82425e-04 * (x * x * x);
		}
	}
	if (voltage == 500)
	{
		if (x < 95.5)
		{
			kor = 0.;
		}
		else if (x > 124)
		{
			kor = 1.08 * x - 112.44;
		}
		else
		{
			kor = 643.257 - 16.7823 * x + 0.139822 * (x * x) - 0.000362154 * (x * x * x);
		}
	}
	if (voltage == 700)
	{
		if (x < 198)
		{
			kor = 0.;
		}
		else if (x > 298)
		{
			kor = 0.21 * x - 45.54;
		}
		else
		{
			kor = -19067 + 383.93 * x - 3.05794 * (x * x) + 0.0120429 * (x * x * x) - 2.34619e-05 * (x * x * x * x) +
				1.81076e-08 * (x * x * x * x * x);
		}
	}
	return kor;
}

ClassImp(R3BTofDCal2HitPar);
