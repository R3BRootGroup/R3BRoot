//
//
// This task order the Si tracker data in time according to the data TimeStamp, during the analysis of lmd files
// If this task is not performed, the data is not always ordered in time
//
// This task should become obsolete when the ASIC hdml programming is comppleted
//
//
//

#include "R3BStarTrackRecTS.h"
using namespace std;

#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TTree.h"
#include <vector>

#include "TString.h"

#include "R3BStarTrackRawHit.h"

#include "FairLogger.h"


//#include "R3BTofCalPar.h"
//#include "R3BTofTCalPar.h"
#include "FairRuntimeDb.h"

#include "FairRunOnline.h"

// Default constructor
R3BStarTrackRecTS::R3BStarTrackRecTS()
: FairTask("R3BStarTrackRecTS",1),
fRawData(NULL),
fNevents(0),  // This is actually the number of blocks
fTotalHits(0)//, This is the accumulater number of word of type 3 in the run
//fCal_Par(NULL)
{
}

// Standard constructor
R3BStarTrackRecTS::R3BStarTrackRecTS(const char* name, Int_t iVerbose)
: FairTask(name, iVerbose),
fRawData(NULL),
fNevents(0),
fTotalHits(0)//,
//fCal_Par(NULL)
{
}

// Destructor
R3BStarTrackRecTS::~R3BStarTrackRecTS()
{
}	


InitStatus R3BStarTrackRecTS::Init()
{
	
	FairRootManager *mgr = FairRootManager::Instance(); // singleton class
	FairRunOnline *run = FairRunOnline::Instance();  // Added for web-base event display

	
	fRawData = (TClonesArray*)mgr->GetObject("StarTrackRawHit"); // StarTrackRawHit is the name of the branch object in the tree to get the information from

	//fCal_Par = (R3BTofCalPar*)FairRuntimeDb::instance()->getContainer("StarTrackCalPar");
	//fCal_Par->setChanged();



	if(!fRawData) return kFATAL;
		
	// Creating Histograms 
	
	  // For Run 280-3364 (C target)
	  //TH1F *TS = new TH1F("TS","Time stamp ns (Reconstructed from hit strips) ",1000,707400,707750.);
        TS = new TH1F("TS","Time Stamp reconstructed from hit strips ",75,141480,141540.);
	TSext = new TH1F("TSext","Time Stamp reconstructed from external signal ",75,141480,141540.);
	TS_p = new TH1F("TS_p","Time Stamp reconstructed from hit strips ",75,141480,141540.);
	TSext_p = new TH1F("TSext_p","Time Stamp reconstructed from external signal ",75,141480,141540.);
	TS_n = new TH1F("TS_n","Time Stamp reconstructed from hit strips ",75,141480,141540.);
	TSext_n = new TH1F("TSext_n","Time Stamp reconstructed from external signal ",75,141480,141540.);
	ADC = new TH1F("ADC","ADC values ",400,0,4000.);

	//TH1F *TS_TSext_diff = new TH1F("TS_TSext_diff","Time stamp difference (Si - Ext trigger) ",5000,-0.4,.8);
	TS_TSext_diff = new TH1F("TS_TSext_diff","Time stamp difference (Si - Ext trigger) ",5000,-20.,20.);
	//TH2F *TS_TSext = new TH2F("TS_TSext"," TS vs TSext",1000,141480.,141550., 1000,141480.,141550.);
	//TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,1000,141480,141550);
	//TH2F *TSext_event = new TH2F("TSext_event"," TSext vs hit#",4100,1.,410000.,1000,141480,141550);
	TS_TSext = new TH2F("TS_TSext"," TS vs TSext",300,141480.,141540., 300,141480.,141540.);
	//TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",41000,1.,410000.,3000,141480,141540.);
	TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,300,141480,141540.);
	//  TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,300,141480,141540.);
	TSext_event = new TH2F("TSext_event"," TSext vs hit#",4100,1.,410000.,300,141480,141540);
	ADC_TS = new TH2F("ADC_TS"," ADC value vs TS",75,141480,141540,4000,1.,4000.);
	ADC_TS_p = new TH2F("ADC_TS_p"," ADC value vs TS ",75,141480,141540,4000,1.,4000.);
	ADC_TS_n = new TH2F("ADC_TS_n"," ADC value vs TS ",75,141480,141540,4000,1.,4000.);
	Asic_Side = new TH2F("Asic_Side"," Asic ID vs Det. Side ",5,0,5,15,0.,15.);

	run->AddObject(TS);
	run->AddObject(TSext);
	run->AddObject(TS_p);
	run->AddObject(TSext_p);
	run->AddObject(TS_n);
	run->AddObject(TSext_n);
	run->AddObject(ADC);
	run->AddObject(TS_TSext_diff);
	run->AddObject(TS_TSext);
	run->AddObject(TS_event);
	run->AddObject(TSext_event);
	run->AddObject(ADC_TS);
	run->AddObject(ADC_TS_p);
	run->AddObject(ADC_TS_n);
	
	// Define output tree 
	
	  // creating the time sorted File and Tree for the Si tracker

	//output File and Tree for the analysis result
	//outFile = new TFile("Out.root","RECREATE");

	output_Tree = new TTree("cbmsimTSsorted","cbmsimTSsorted"); // cbmsim to keep same tree structure than raw tree.

	// Defining outT branches
	output_Tree->Branch("StarTrackRecTS", &s_entry.tm_stp,"tm_stp/L:tm_stp_ext/L:nhit/I:type/I:hit/I:ModuleId/I:Side/I:AsicId/I:StripId/I:ADCdata/I:sync_flag/O:pause_flag/O");

	run->AddObject(output_Tree);

	//outFile->cd();

	
	
	return kSUCCESS;
}

// Add NULL Checks

// kSUCCESS if no error occure (during the initialisation for instance)
// kERROR  if error is not crucial
// kFATAL  if error is crucial




void R3BStarTrackRecTS::Exec(Option_t *option)
{
	// loop over input data
	R3BStarTrackRawHit *item;
	R3BStarTrackRawHit *item2;
	
	Int_t nItems = fRawData->GetEntriesFast(); // returns the number of hits in the block
	
	cout << "nItems="<< nItems << endl;
	
	//
	// Below is the equivalent of the root macros: macros/r3b/startrack/startrackTimeOrder.C :
	//
	
	unsigned long my_tm_stp;
	Int_t my_wordtype= -1; //
	Int_t my_hitbit= -1;
	Int_t my_det_id=-1;
	Int_t my_side_id=-1;
	Int_t my_asic_id= -1;
	Int_t my_ch_id= -1;
	Int_t my_adc_data= -1;
	bool my_sync_flag=false;
	bool my_pause_flag=false;
	unsigned long my_tm_stp_ext;

	Int_t my_new_wordtype= -1; //
	Int_t my_new_hitbit= -1;
	Int_t my_new_det_id=-1;
	Int_t my_new_side_id=-1;
	Int_t my_new_asic_id= -1;
	Int_t my_new_ch_id= -1;
	Int_t my_new_adc_data= -1;


	int my_info_code=0;
	unsigned long my_info_field=0;
	unsigned long my_tm_stp_hb=0;
	unsigned long my_tm_stp_vhb=0;
	unsigned long my_tm_stp_lb=0;

	// for extarnal timestamp trigger
	unsigned long my_tm_stp_Ext_hb=0;
	unsigned long my_tm_stp_Ext_vhb=0;
	unsigned long my_tm_stp_Ext_lb=0;

	//arrays to check synchronization is going fine (or not!)
	unsigned long tm_stp_msb_modules[30]={0};   // 30 = total number of ladders
	unsigned long my_tm_stp_msb; // most significant bits of time-stamp (in info code)


	bool Info4= false;



//	for(Int_t i=0; i<7791; i++)
	for(Int_t i=0; i<nItems; i++)  
	{
		item= (R3BStarTrackRawHit*) fRawData->At(i);

		if(NULL==item) { continue;}  // means just ignore the currnt i and move to the other

		my_wordtype=item->GetWordType();

		//cout << "my_wordtype=" << my_wordtype << endl;
    
    	  if(my_wordtype==3){
			my_det_id=item->GetModuleId();
			my_hitbit=item->GetHitBit();
			my_side_id=item->GetSide();
			my_asic_id=item->GetAsicId();
			my_ch_id=item->GetStripId();
			my_adc_data=item->GetADCdata();

			my_tm_stp_lb=(item->GetTimelb())*1e-6;

			// rec time stamp 47:0  To be used if time stamp already reconstructed in the unpacker
			//my_tm_stp=branchTimevhb->GetValue(ii,0,true);
			//cout << "my_tm_stp= " <<  my_tm_stp  << endl;
	    	    
			fTotalHits++;
			//cout << "Total hit of type 3= " << fTotalHits << endl;
		}	
	
		
		if(my_wordtype==2){        // 10
	    	    
			my_info_code=item->GetInfoCode();
			my_info_field=item->GetInfoField();
	    
			//cout << "my_info_code=" << my_info_code << endl;
	    
			// external ts (47:28)
			if(my_info_code==14){
				my_tm_stp_Ext_hb=item->GetTimeExthb(); //branchTimeExthb->GetValue(ii,0,true);
				//cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
				//cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << " which should be equals to  Infofield =" << my_info_field <<  endl;
				//cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
	    
				my_tm_stp_ext= ( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );
				//cout << "my_tm_stp_ext   = " << my_tm_stp_ext << endl;
	    
			}
			// external ts (63:48)
			if(my_info_code==15){
				my_tm_stp_Ext_vhb=item->GetTimeExtvhb();
				//cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
				//cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << endl;
				//cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
			}
			
			// Data ts (47:28)
			if(my_info_code==4 || my_info_code==5 || my_info_code==7 || my_info_code==8){
			//if(my_info_code==4 || my_info_code==7){
	    
				//if(my_info_code==4 || my_info_code==7)my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
				if(my_info_code==4 || my_info_code==5)
				{
					if(my_info_code==4)my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
					//if(my_info_code==5 || my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48 
					//cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
					//cout << "which should be equals to   = " << branchTimehb->GetValue(ii,0,true) << endl;	    
					//my_tm_stp_msb= (my_tm_stp_vhb << 20) |  my_tm_stp_hb;	     
		
					if(my_info_code==4)my_tm_stp_msb=  my_tm_stp_hb;	     
					cout << "my_det_id = " << my_det_id << endl;
		
					if(my_info_code==4)tm_stp_msb_modules[my_det_id]= my_tm_stp_msb; //update for use with other data types
					//cout << "tm_stp_msb_modules[my_det_id] = " << tm_stp_msb_modules[my_det_id] << endl;
		
					//reconstruct time stamp= MSB+LSB
					//cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;
		
					if(my_info_code==5) my_tm_stp_vhb= my_info_field;  //  63:48
					 
					my_tm_stp= (  (tm_stp_msb_modules[my_det_id]<< 28 ) | (my_tm_stp_lb) );
					//cout << "my_tm_stp   = " << my_tm_stp << endl;
		
					Info4= true; // set Info4 true
		


				}else if( (my_info_code==7 || my_info_code==8) && !Info4)
				{
					if(my_info_code==7) my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
					//if(my_info_code==5 || my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48 
					//cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
					//cout << "which should be equals to   = " << branchTimehb->GetValue(ii,0,true) << endl;	    
					//my_tm_stp_msb= (my_tm_stp_vhb << 20) |  my_tm_stp_hb;	     
		
					if(my_info_code==7) my_tm_stp_msb=  my_tm_stp_hb;	     
					//cout << "my_det_id = " << my_det_id << endl;
		
					if(my_info_code==7) tm_stp_msb_modules[my_det_id]= my_tm_stp_msb; //update for use with other data types
					//cout << "tm_stp_msb_modules[my_det_id] = " << tm_stp_msb_modules[my_det_id] << endl;
		
					//reconstruct time stamp= MSB+LSB
					//cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;
		
					if(my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48 
					if(my_info_code==8) my_tm_stp= (  (tm_stp_msb_modules[my_det_id]<< 28 ) | (my_tm_stp_lb) );
					//cout << "my_tm_stp   = " << my_tm_stp << endl;

				}
			}	
	
		}


		//if((my_wordtype==3))
	     if((my_wordtype==3) && (my_tm_stp/1e9 >1000))  // my_tm_stp/1e9 >1000 is to remove type=3 data that have been recorded before a type=2 word was)
	      {

		  //s_entry.tm_stp = ts_temp.at(j); //reconstructed timestamp (MSB+LSB)
		  //cout << " filling my_tm_stp=" << my_tm_stp << endl; // 
		  //s_entry.tm_stp = my_tm_stp/1e9; //reconstructed timestamp (MSB+LSB)
		  s_entry.tm_stp = my_tm_stp/1e9; //reconstructed timestamp (MSB+LSB) in sec
		  //cout << " filling my_tm_stp_ext=" << my_tm_stp_ext << endl; // 
		  s_entry.tm_stp_ext = my_tm_stp_ext/1e9; // in sec
		  s_entry.nhit        = fTotalHits; 
		  s_entry.type       = my_wordtype; //
		  s_entry.hit        = my_hitbit;
		  s_entry.ModuleId     = my_det_id;
		  s_entry.Side    = my_side_id;
		  s_entry.AsicId    = my_asic_id; // new *R3B*
		  s_entry.StripId      = my_ch_id;
		  // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only in type 0)
		  // type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
		  s_entry.ADCdata   = my_adc_data;
		  s_entry.sync_flag  = my_sync_flag; // check SYNC100 pulses received for this module
		  s_entry.pause_flag = my_pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...
		  
		  /*
		  std::cout << "*** ENTRY FILLED:\n"
			    <<     "    type= "<< my_wordtype << std::endl
			    <<     "    mod_id= "<< my_det_id << std::endl;
		  std::cout << " *!->NF<-!* " << endl;
		  std::cout << " " << endl;
		  */
		  
		  //Fill histograms
		  
		  TS->Fill(my_tm_stp/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TSext->Fill(my_tm_stp_ext/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  ADC_TS->Fill(my_tm_stp/1e9, my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
		  if(my_side_id==1)
		    {
		      TS_n->Fill(my_tm_stp/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		      TSext_n->Fill(my_tm_stp_ext/1e9);  // in sec !!!! if the 5ns already implemented in Vic's readout
		      ADC_TS_n->Fill(my_tm_stp/1e9, my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
		    }
		  if(my_side_id==2)
		    {
		      TSext_p->Fill(my_tm_stp_ext/1e9);  // in sec !!!! if the 5ns already implemented in Vic's readout
		      ADC_TS_p->Fill(my_tm_stp/1e9, my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
		    }
		  
		  
		  Asic_Side->Fill((my_side_id-1),(my_asic_id-1));
		  ADC->Fill(my_adc_data);
		  TS_TSext_diff->Fill( (my_tm_stp/1e9 - my_tm_stp_ext/1e9)); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TS_TSext->Fill(my_tm_stp/1e9,my_tm_stp_ext/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TS_event->Fill(fTotalHits, my_tm_stp/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TSext_event->Fill(fTotalHits, my_tm_stp_ext/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		
		  
		  output_Tree->Fill();
		  
		    
		}else{  
			std::cout << "*** ENTRY NOT FILLED:\n"
			<<     "    type= "<< my_wordtype << std::endl
			<<     "    mod_id= "<< my_det_id << std::endl;
			std::cout << " *!->NF<-!* " << endl;
			std::cout << " " << endl;
		 
		} // end of if(fill_flag && my_tm_stp/1e9>1000)


	}  // end of loop over the hit
}


void R3BStarTrackRecTS::FinishEvent()
{
	fNevents += 1;
	
	cout << "data blocks=" << fNevents << endl;
}


void R3BStarTrackRecTS::FinishTask()
{
	
	// Write histos
		  TS->Write();
		  TSext->Write(); 
		  ADC_TS->Write();
		  TS_n->Write();
		  TSext_n->Write();
		  ADC_TS_n->Write(); 
		  TSext_p->Write();
		  ADC_TS_p->Write();
		  Asic_Side->Write();
		  ADC->Write();
		  TS_TSext_diff->Write();
		  TS_TSext->Write(); // 
		  TS_event->Write(); // 
		  TSext_event->Write(); // 

	// Write Sorted tree
		output_Tree->Write();	
	
	
	// This ti to create and save a graph rather than a 2D histogrma
      output_Tree->Draw("StarTrackRecTS.nhit:StarTrackRecTS.tm_stp","","");
      cout << "SelectedRows=" << output_Tree->GetSelectedRows() << endl;;
      TGraph *gr = new TGraph(output_Tree->GetSelectedRows(),
                                   output_Tree->GetV1(), output_Tree->GetV2());
      gr->Draw("ap");
      gr->Write();

}




ClassImp(R3BStarTrackRecTS)

