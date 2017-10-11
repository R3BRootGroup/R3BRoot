

///////////////////////////////////////////////////////////////////////
// ToDo: transfer this macros into a FairTask and include this task in 
// the macro startrack_ucesb.C
///////////////////////////////////////////////////////////////////////

//
//  Read the RAW output tree from Startrack in R3Broot and order the data in time
//  to produce a ordered raw output tree.
//
//  root -l
//  .L startrackTimeOrder.C
//  startrackTimeOrder( "startrack_raw_data.root", "startrack_ordered_raw_data.root" ).root

#include <stdio.h>

#include <fstream>
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TTree.h"
#include <vector>


void InsertionSort(vector<long long> & v_ts, vector<long long> & v_index);
void InsertionSort2(vector<long long> & v_ts, vector<long long> & v_block_index, vector<long long> & v_hit_index);


// startrackTimeSort = the equivalent of DoSort() in Afredo's code
void startrackTimeOrder_ucesb2(const char* input, const char* output)
{
  //TStopwatch timer;
  //timer.Start();


  // opening the Raw data File and Tree
  TFile *inFile = TFile::Open(input);
  // if only one input file:
  TTree* input_Tree = (TTree*)inFile->Get("evt"); 

  // creating the time sorted File and Tree for the Si tracker


  //output File and Tree for the analysis result
  TFile* outFile = new TFile(output,"RECREATE");

  TTree* output_Tree = new TTree("evt","evt"); // cbmsim to keep same tree structure than raw tree.

  struct struct_entry_sort{
    //long long tm_stp; //reconstructed timestamp (MSB+LSB)
    long long tm_stp; //reconstructed timestamp (MSB+LSB)
    // not used *R3B* ->    long long info; //MBS info data (external timestamp), anything else(?)
    long long tm_stp_ext; //reconstructed external timestamp trigger
    int nhit;  // one hit is one strip hit (word 3)
    int type;
    int hit;
    int LadderId;
    int Side;
    int AsicId; // new *R3B*
    int StripId;
    // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only in type 0)
              // type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
    int ADCdata;
    bool sync_flag; // check SYNC100 pulses received for this module
    bool pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...
  };

  struct_entry_sort s_entry;

  // Defining outT branches
  output_Tree->Branch("StartrackMapped", &s_entry.tm_stp,"tm_stp/L:tm_stp_ext/L:nhit/I:type/I:hit/I:LadderId/I:Side/I:AsicId/I:StripId/I:ADCdata/I:sync_flag/O:pause_flag/O");

  outFile->cd();




  // Creating Histograms 

// For Run 280-3364 (C target)
  //TH1F *TS = new TH1F("TS","Time stamp ns (Reconstructed from hit strips) ",1000,707400,707750.);
  TH1F *TS = new TH1F("TS","Time Stamp reconstructed from hit strips ",75,141480,141540.);
  TH1F *TSext = new TH1F("TSext","Time Stamp reconstructed from external signal ",75,141480,141540.);
  TH1F *TS_p = new TH1F("TS_p","Time Stamp reconstructed from hit strips ",75,141480,141540.);
  TH1F *TSext_p = new TH1F("TSext_p","Time Stamp reconstructed from external signal ",75,141480,141540.);
  TH1F *TS_n = new TH1F("TS_n","Time Stamp reconstructed from hit strips ",75,141480,141540.);
  TH1F *TSext_n = new TH1F("TSext_n","Time Stamp reconstructed from external signal ",75,141480,141540.);
  TH1F *ADC = new TH1F("ADC","ADC values ",400,0,4000.);

  //TH1F *TS_TSext_diff = new TH1F("TS_TSext_diff","Time stamp difference (Si - Ext trigger) ",5000,-0.4,.8);
  TH1F *TS_TSext_diff = new TH1F("TS_TSext_diff","Time stamp difference (Si - Ext trigger) ",5000,-20.,20.);
  //TH2F *TS_TSext = new TH2F("TS_TSext"," TS vs TSext",1000,141480.,141550., 1000,141480.,141550.);
  //TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,1000,141480,141550);
  //TH2F *TSext_event = new TH2F("TSext_event"," TSext vs hit#",4100,1.,410000.,1000,141480,141550);
  TH2F *TS_TSext = new TH2F("TS_TSext"," TS vs TSext",300,141480.,141540., 300,141480.,141540.);
  //TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",41000,1.,410000.,3000,141480,141540.);
  TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",1600,1.,16000.,300,141484,141487.);
  //  TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,300,141480,141540.);
  TH2F *TSext_event = new TH2F("TSext_event"," TSext vs hit#",4100,1.,410000.,300,141480,141540);
  TH2F *ADC_TS = new TH2F("ADC_TS"," ADC value vs TS",75,141480,141540,4000,1.,4000.);
  TH2F *ADC_TS_p = new TH2F("ADC_TS_p"," ADC value vs TS ",75,141480,141540,4000,1.,4000.);
  TH2F *ADC_TS_n = new TH2F("ADC_TS_n"," ADC value vs TS ",75,141480,141540,4000,1.,4000.);


  TH2F *Asic_Side = new TH2F("Asic_Side"," Asic ID vs Det. Side ",5,0,5,15,0.,15.);
 
 

// For Run 290- (CH2 target)
/*
  TH1F *TS = new TH1F("TS","Time Stamp reconstructed from hit strips ",300,209700,209760.);
  TH1F *TSext = new TH1F("TSext","Time Stamp reconstructed from external signal ",300,209700,209760.);
  TH1F *TS_p = new TH1F("TS_p","Time Stamp reconstructed from hit strips ",300,209700,209760.);
  TH1F *TSext_p = new TH1F("TSext_p","Time Stamp reconstructed from external signal ",300,209700,209760.);
  TH1F *TS_n = new TH1F("TS_n","Time Stamp reconstructed from hit strips ",300,209700,209760.);
  TH1F *TSext_n = new TH1F("TSext_n","Time Stamp reconstructed from external signal ",300,209700,209760.);
  TH1F *ADC = new TH1F("ADC","ADC values ",400,0,4000.);

  TH1F *TS_TSext_diff = new TH1F("TS_TSext_diff","Time stamp difference (Si - Ext trigger) ",5000,-20.,20.);
  TH2F *TS_TSext = new TH2F("TS_TSext"," TS vs TSext",300,209700,209760., 300,209700,209760.);
  TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",5000,1.,1800000.,300,209700,209760.);
  TH2F *TSext_event = new TH2F("TSext_event"," TSext vs hit#",5000,1.,1800000.,300,209700,209760.);
  TH2F *ADC_TS = new TH2F("ADC_TS"," ADC value vs TS",300,209700,209760.,4000,1.,4000.);
  TH2F *ADC_TS_p = new TH2F("ADC_TS_p"," ADC value vs TS ",300,209700,209760.,4000,1.,4000.);
  TH2F *ADC_TS_n = new TH2F("ADC_TS_n"," ADC value vs TS ",300,209700,209760.,4000,1.,4000.);

  TH2F *Asic_Side = new TH2F("Asic_Side"," Asic ID vs Det. Side ",5,0,5,15,0.,15.);
*/

  //  Information from input tree:

  TBranchElement *branchWordType = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fWordtype");
  TBranchElement *branchHitValue = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fHitbit");

  TBranchElement *branchLadderId = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fLadderId");
  TBranchElement *branchSideId = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fSide");
  TBranchElement *branchAsicId = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fAsicId");
  TBranchElement *branchStripId = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fStripId");

  TBranchElement *branchEnergy = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fADCdata");
  TBranchElement *branchTimevhb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTimevhb");
  TBranchElement *branchTimehb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTimehb");
  TBranchElement *branchTimelb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTimelb");

  //  TBranchElement *branchWRvhb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fWRvhb");
  //  TBranchElement *branchWRhb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fWRhb");
  //  TBranchElement *branchWRlb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fWRlb");
  TBranchElement *branchTS = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTS");
  TBranchElement *branchTSExt = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTSExt");

  TBranchElement *branchInfoCode = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fInfoCode");
  TBranchElement *branchInfoField = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fInfoField");

  TBranchElement *branchTimeExtvhb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTimeExtvhb");
  TBranchElement *branchTimeExthb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTimeExthb");
  TBranchElement *branchTimeExtlb = (TBranchElement*)input_Tree->GetBranch("StartrackMapped.fTimeExtlb");

  //for structure struct_entry_sort
  unsigned long my_tm_stp;
  int my_type= -1; //
  int my_hit= -1;
  int my_det_id;
  int my_side_id;
  int my_asic_id= -1;
  int my_ch_id= -1;
  int my_adc_data= -1;
  bool my_sync_flag=false;
  bool my_pause_flag=false;
  unsigned long my_tm_stp_ext;

  int my_new_type= -1; //
  int my_new_hit= -1;
  int my_new_det_id;
  int my_new_side_id;
  int my_new_asic_id= -1;
  int my_new_ch_id= -1;
  int my_new_adc_data= -1;

  int my_info_code;
  unsigned long my_info_field;
  unsigned long my_tm_stp_hb;
  unsigned long my_tm_stp_vhb;
  unsigned long my_tm_stp_lb;
  unsigned long my_rec_tm_stp;


  // for extarnal timestamp trigger
  unsigned long my_tm_stp_Ext_hb;
  unsigned long my_tm_stp_Ext_vhb;
  unsigned long my_tm_stp_Ext_lb;
  unsigned long my_rec_tm_stp_ext;

  Int_t TotalTrackMult=0;
  Int_t TotalHit = 0;
  Int_t TotalEvent = 1; // = Total hit after removing hit of word type 2

  //arrays to check synchronization is going fine (or not!)
  unsigned long tm_stp_msb_modules[30]={0};   // 30 = total number of ladders

  unsigned long my_tm_stp_msb; // most significant bits of time-stamp (in info code)

  //bool fill_flag;
  bool no_sync_flag;

  long long n_first=0;

  //
  //all things for ordering data by time stamp
  //	
  bool ts_sort_flag= false;
  bool Info4= false;

  vector<long long> ts_temp;
  //vector<long long> index_temp;
  vector<long long> index_block_temp, index_hit_temp;
  ts_temp.reserve(2048);
  //index_temp.reserve(2048);
  index_block_temp.reserve(2048);
  index_hit_temp.reserve(2048);

  const int MAXsort= 10000; //do we need maximum number when we do not want to sort any more!?

  Int_t n_entries = input_Tree->GetEntries();
  cout << "n_entries = total number of number of blocks (with or without startrack data) " << n_entries << endl;

// Loop over all the blocks: 
  for(Int_t i=0;i<n_entries;i++){  // n_entries is the number of blocks in the run
  //for(Int_t i=0;i<1040;i++){  // n_entries is the number of blocks in the run
  //for(Int_t i=0;i<3040;i++){  // n_entries is the number of blocks in the run
   //  for(Int_t i=0;i<3;i++){  // n_entries is the number of blocks in the run
    //if(i%10 == 0) printf("Block:%d\n",i);
    //if(i == 0) printf("Block:%d\n",i);

    Int_t nb = 0;
    Int_t Total_typ2 = 0; 
    input_Tree->GetEntry(i); 
    //cout << "input Entry = " << input_Tree->GetEntry(i) << endl;
    //nb += input_Tree->GetEvent(i);
    //cout << "nb  = " << nb << endl;


// Look for startrack data:  
    TotalTrackMult= branchEnergy->GetNdata();  // number of hits (ies nb of words of type 2 or 3) in the block

// if there is startrack data in this block, then TotalTrackMult will be non null and:
    if(TotalTrackMult!=0){

     cout << " ########################## New block ################################## " << endl;
     cout << " This is block #" << i << endl;
     cout << " number of <<hits>> in this block  = " << TotalTrackMult << endl;
    // 1 hit is a 3 (=11....) or a 2 (=10....) wordtype

    TotalEvent=TotalEvent+TotalHit;
       cout << "Total of hit of word type #3  = " << TotalEvent << endl;

  
// Loop over the hits in the block
       //TotalTrackMult=380;
       //TotalTrackMult=2590;
       //TotalTrackMult=500;
       //TotalTrackMult=460;
       //TotalTrackMult=380;
      for(Int_t ii=0; ii<TotalTrackMult; ii++)
      {
	TotalHit=(ii-Total_typ2);  // in the new tree we won't keep hits corresponding to word type 2
        //cout << "hit number inside the block  = " << TotalHit << endl;

	no_sync_flag=true;
    
	my_type= branchWordType->GetValue(ii,0,true);
        //cout << " #### New Hit in the block " << endl;
        //cout << "Word Type  = " << my_type << endl;

	if(my_type==2)Total_typ2++;


        //cout << "########################"<< endl;
        //cout << "Total_typ2  = " << Total_typ2 << " ii= " << ii  << endl;
        //cout << "TotalEvent  = " << TotalEvent+TotalHit << "  my_type  = " << my_type << endl;
        //cout << "Totalhit  = " << TotalHit << "  my_type  = " << my_type << endl;


	  if(my_type==3){
	    my_det_id=branchLadderId->GetValue(ii,0,true);
	    my_hit=branchHitValue->GetValue(ii,0,true);
	    my_side_id=branchSideId->GetValue(ii,0,true);
	    my_asic_id=branchAsicId->GetValue(ii,0,true);
	    my_ch_id=branchStripId->GetValue(ii,0,true);
	    my_adc_data=branchEnergy->GetValue(ii,0,true);
	    

	    my_tm_stp_lb=branchTimelb->GetValue(ii,0,true);

	    my_rec_tm_stp=branchTS->GetValue(ii,0,true);  // reconstructed full time stamp
	    my_rec_tm_stp_ext=branchTSExt->GetValue(ii,0,true);  // reconstructed full time stamp


	    // rec time stamp 47:0  To be used if time stamp already reconstructed in the unpacker
	    //my_tm_stp=branchTimevhb->GetValue(ii,0,true);
	    // cout << "my_tm_stp= " <<  my_tm_stp  << endl;
	    	    
	    //cout << "hit number=" <<  TotalEvent+TotalHit<< endl;

	    //cout << "my_rec_tm_stp= " <<  my_rec_tm_stp  << endl;

	    ts_sort_flag= false;

	  }


	  /*
	  if(my_type==2){        // 10
	    	    
	    no_sync_flag= false;  //

	    my_info_code=branchInfoCode->GetValue(ii,0,true);
	    my_info_field=branchInfoField->GetValue(ii,0,true);
	    
	    //cout << "my_info_code=" << my_info_code << endl;
	    
	    // external ts (47:28)
	    if(my_info_code==14){
	    my_tm_stp_Ext_hb=branchTimeExthb->GetValue(ii,0,true);
	    //cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
	    //cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << " which should be equals to  Infofield =" << my_info_field <<  endl;
	    //cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
	    
	    my_tm_stp_ext= ( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );
	    //cout << "my_tm_stp_ext   = " << my_tm_stp_ext << endl;
	    
	    }
	    // external ts (63:48)
	    if(my_info_code==15){
	    my_tm_stp_Ext_vhb=branchTimeExtvhb->GetValue(ii,0,true);
	    //cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
	    //cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << endl;
	    //cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
	    
	    }
	    
	    // Data ts (47:28)
	    if(my_info_code==4 || my_info_code==5 || my_info_code==7 || my_info_code==8){
	      //if(my_info_code==4 || my_info_code==7){
	    
	    
	      //if(my_info_code==4 || my_info_code==7)my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
	    if(my_info_code==4)
	      {
		my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
		//if(my_info_code==5 || my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48 
		//cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
		//cout << "which should be equals to   = " << branchTimehb->GetValue(ii,0,true) << endl;	    
		//my_tm_stp_msb= (my_tm_stp_vhb << 20) |  my_tm_stp_hb;	     
		
		my_tm_stp_msb=  my_tm_stp_hb;	     
		//cout << "my_det_id = " << my_det_id << endl;
		
		tm_stp_msb_modules[my_det_id]= my_tm_stp_msb; //update for use with other data types
		//cout << "tm_stp_msb_modules[my_det_id] = " << tm_stp_msb_modules[my_det_id] << endl;
		
		//reconstruct time stamp= MSB+LSB
		//cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;
		
		my_tm_stp= (  (tm_stp_msb_modules[my_det_id]<< 28 ) | (my_tm_stp_lb) );
		//cout << "my_tm_stp   = " << my_tm_stp << endl;
		
	    //vectors to order data by time-stamp
	    //ts_temp.push_back(my_tm_stp);
	    //index_temp.push_back(ii+TotalEvent);
	    //index_block_temp.push_back(i);
	    //index_hit_temp.push_back(ii);
	    
		ts_sort_flag= true;
		Info4= true; // set Info4 true
      
	      }

	    if(my_info_code==7 && !Info4)
	      {
		my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
		//if(my_info_code==5 || my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48 
		//cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
		//cout << "which should be equals to   = " << branchTimehb->GetValue(ii,0,true) << endl;	    
		//my_tm_stp_msb= (my_tm_stp_vhb << 20) |  my_tm_stp_hb;	     
		
		my_tm_stp_msb=  my_tm_stp_hb;	     
		//cout << "my_det_id = " << my_det_id << endl;
		
		tm_stp_msb_modules[my_det_id]= my_tm_stp_msb; //update for use with other data types
		//cout << "tm_stp_msb_modules[my_det_id] = " << tm_stp_msb_modules[my_det_id] << endl;
		
		//reconstruct time stamp= MSB+LSB
		//cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;
		
		my_tm_stp= (  (tm_stp_msb_modules[my_det_id]<< 28 ) | (my_tm_stp_lb) );
		//cout << "my_tm_stp   = " << my_tm_stp << endl;


		ts_sort_flag= true;

		Info4= false;

	      }else
	      {
		ts_sort_flag= false;
	      }

	    
	    
	    //  else if(my_info_code==7 && !Info4) // if there was no my_info_code=4 before my_info_code=7 do nothing
	    //  {
	    //	ts_sort_flag= false;
	    //  }
	    

	    //cout << "ts_temp.size= "<<ts_temp.size() << endl;
	    //cout << "index_block_temp.size= "<<index_block_temp.size() << endl;
	    //cout << "index_hit_temp.size= "<<index_hit_temp.size() << endl;
	    	    
	    }
	    //cout << "ts_sort_flag=" << ts_sort_flag << endl;
	    //cout << "hit number=" <<  TotalEvent+TotalHit<< endl;
	  }
	  
	  */	 
	  	  
	  //if any other data type
	  if(no_sync_flag){  // let's fill the vectors ts_temp, index_block, index_hit_temp for word of type 3
	  //reconstruct time stamp= MSB+LSB
	      my_tm_stp= (  (tm_stp_msb_modules[my_det_id] << 28 ) | (my_tm_stp_lb) );
	      //cout << "my_tm_stp   = " << my_tm_stp << endl;
  
	      //vectors to order data by time-stamp
	      ts_temp.push_back(my_rec_tm_stp);
	      //index_temp.push_back(ii+TotalEvent);
	      index_block_temp.push_back(i);
	      index_hit_temp.push_back(ii);

	      cout << "ts_temp.size= "<<ts_temp.size() << endl;
	      cout << "index_block_temp.size= "<<index_block_temp.size() << endl;
	      cout << "index_hit_temp.size= "<<index_hit_temp.size() << endl;
	  
	      // if temp arrays too large, sort
	      if(ts_temp.size()>MAXsort){
		ts_sort_flag= true;
	      }
	      //if last, sort
	      //else if((ii+1)==n_entries){
	      else if((ii+1)==TotalTrackMult){
	       ts_sort_flag= true;
	      }

	      //cout << "no_sync_flag= "<< no_sync_flag << endl;
	      //cout << "ts_sort_flag= "<< ts_sort_flag << endl;

	      n_first++;  // counter of word type 3
	      cout << "n_first= " << n_first << endl;	  
    	  }
     	    


	     
	  if(ts_sort_flag){
	  //long long n_first=index_temp.at(0); 
	  //long long n_loop= index_temp.size(); 
	  //long long n_first=index_hit_temp.at(0); 

	  long long n_loop= index_hit_temp.size(); 

	  cout << "n_first=" << n_first << endl;	  
	  cout << "n_loop=" << n_loop << endl;	  
	  
	  //InsertionSort(ts_temp,index_temp);                      // when calling InsertionSort
	  InsertionSort2(ts_temp,index_block_temp, index_hit_temp); // when calling InsertionSort2
	  ts_sort_flag= false;
	  
	  
	  bool fill_flag= false;
	  bool my_sync_flag=false;
	  bool my_pause_flag=false;
	  
	  //transport sorted data to output_tree
	  //n_loop=10;
	  for(long long j=0; j<n_loop; j++){	    //transport sorted data to output_tree
  
	    //cout << "j= " << j << endl;
	     //set address of new struct_entry_sort to temp_tree Branch? or is s_temp enough?
	     //input_Tree->GetEntry(index_temp.at(j));
	    input_Tree->GetEntry(index_block_temp.at(j));  // get the input tree content at the block number = index_block_temp.at(j)

	    cout << "Treating data block #" << i << " as it contain startrack data" <<  endl;
	    //cout << "index_block_temp.at(j)= " << index_block_temp.at(j) << endl;
	    //cout << "index_hit_temp.at(j)= " << index_hit_temp.at(j) << endl;
		  
	    my_rec_tm_stp=ts_temp.at(j);
	    //cout << "my_rec_tm_stp= " << ts_temp.at(j) << endl;
	    //cout << "my_new Type=" <<  branchWordType->GetValue(index_hit_temp.at(j),0,true) << endl;     // when calling InsertionSort2
	    

	    my_new_type=branchWordType->GetValue(index_hit_temp.at(j),0,true);
	    my_new_hit=branchHitValue->GetValue(index_hit_temp.at(j),0,true);     // when calling InsertionSort2
	    my_new_det_id=branchLadderId->GetValue(index_hit_temp.at(j),0,true);     // when calling InsertionSort2
	    my_new_adc_data=branchEnergy->GetValue(index_hit_temp.at(j),0,true);  // when calling InsertionSort2
	    my_new_asic_id=branchAsicId->GetValue(index_hit_temp.at(j),0,true);   // when calling InsertionSort2
	    my_new_ch_id=branchStripId->GetValue(index_hit_temp.at(j),0,true);    // when calling InsertionSort2
	    my_new_adc_data=branchEnergy->GetValue(index_hit_temp.at(j),0,true);    // when calling InsertionSort2

	    fill_flag=true;

	      // Below is the equivalent of "if(SortEntry(s_entry_unpack, s_entry, j+n_first, ts_temp.at(j)) ) " in Alfredo's code:
	    	    

		  /*
	      if(my_type==2){        // 10
		  //if(branchInfoCode->GetValue(index_temp.at(j),0,true)==4 || branchInfoCode->GetValue(index_temp.at(j),0,true)==7){
		 // 4 or 7 may be sufficient but lte's include 7 and 8 as well
		  if(branchInfoCode->GetValue(index_hit_temp.at(j),0,true)==4 || branchInfoCode->GetValue(index_hit_temp.at(j),0,true)==5 || branchInfoCode->GetValue(index_hit_temp.at(j),0,true)==7 || branchInfoCode->GetValue(index_hit_temp.at(j),0,true)==8 ){
		    fill_flag= true;
		  }
		  else 
		    {
		      //fill_flag= false;
		    }
	      }  
	      else if(my_type==3)  // 11
		{
		  //my_hit=branchHitValue->GetValue(index_temp.at(j),0,true);       // when calling InsertionSort
		  //my_adc_data=branchEnergy->GetValue(index_temp.at(j),0,true);    // when calling InsertionSort
		  //my_asic_id=branchAsicId->GetValue(index_temp.at(j),0,true);     // when calling InsertionSort
		  //my_ch_id=branchStripId->GetValue(index_temp.at(j),0,true);      // when calling InsertionSort
		  cout << "my_new Type=" <<  branchWordType->GetValue(index_hit_temp.at(j),0,true) << endl;     // when calling InsertionSort2
		  my_hit=branchHitValue->GetValue(index_hit_temp.at(j),0,true);     // when calling InsertionSort2
		  my_adc_data=branchEnergy->GetValue(index_hit_temp.at(j),0,true);  // when calling InsertionSort2
		  my_asic_id=branchAsicId->GetValue(index_hit_temp.at(j),0,true);   // when calling InsertionSort2
		  my_ch_id=branchStripId->GetValue(index_hit_temp.at(j),0,true);    // when calling InsertionSort2

		  fill_flag=true;
		}
		  */
	     
	      // Only fill if a word type 2 has appeared first followed by words of type 3 ()
	      // In that way one gets rid off the first words of type 3 at the beginning of a run (first block of the run) that have no high bit timestamp information.  
	      if(fill_flag && (my_rec_tm_stp/1.e9 >1000))
	      {

		  //s_entry.tm_stp = ts_temp.at(j); //reconstructed timestamp (MSB+LSB)
		  cout << " filling my_tm_stp=" << my_rec_tm_stp << endl; // 
		  //s_entry.tm_stp = my_tm_stp/1.e9; //reconstructed timestamp (MSB+LSB)
		  s_entry.tm_stp = my_rec_tm_stp; //reconstructed timestamp (MSB+LSB)
		  cout << " filling my_rec_tm_stp_ext=" << my_rec_tm_stp_ext << endl; // 
	           cout << "n_loop=" << n_loop << endl;	  
	           cout << "n_first=" << n_first << endl;	  
		  s_entry.tm_stp_ext = my_rec_tm_stp_ext/1.e9; // 
		  s_entry.nhit       = (n_first-n_loop)+j;
		  //s_entry.nhit        = TotalEvent+TotalHit+j; // j+n_first;
		  //s_entry.nhit        = TotalEvent+j;
		  //s_entry.nhit        = TotalEvent+TotalHit;
		  s_entry.type        = my_type;
		  s_entry.hit         = my_hit;
		  s_entry.LadderId    = my_det_id;
		  s_entry.Side        = my_side_id;
		  s_entry.AsicId      = my_asic_id; // new *R3B*
		  s_entry.StripId     = my_ch_id;
		  // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only in type 0)
		  // type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
		  s_entry.ADCdata    = my_adc_data;
		  s_entry.sync_flag  = my_sync_flag; // check SYNC100 pulses received for this module
		  s_entry.pause_flag = my_pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...
		  
		  /*
		  std::cout << "*** ENTRY FILLED:\n"
			    <<     "    type= "<< my_type << std::endl
			    <<     "    mod_id= "<< my_det_id << std::endl;
		  std::cout << " *!->NF<-!* " << endl;
		  std::cout << " " << endl;
		  */

		  //Fill histograms
		  TS->Fill(my_rec_tm_stp/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TSext->Fill(my_rec_tm_stp_ext/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  ADC_TS->Fill(my_rec_tm_stp/1e9, my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
		  if(my_side_id==1)
		    {
		      TS_n->Fill(my_rec_tm_stp/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		      TSext_n->Fill(my_rec_tm_stp_ext/1e9);  // in sec !!!! if the 5ns already implemented in Vic's readout
		      ADC_TS_n->Fill(my_rec_tm_stp/1e9, my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
		    }
		  if(my_side_id==2)
		    {
		      TSext_p->Fill(my_rec_tm_stp_ext/1e9);  // in sec !!!! if the 5ns already implemented in Vic's readout
		      ADC_TS_p->Fill(my_rec_tm_stp/1e9, my_adc_data); // in sec !!!! if the 5ns already implemented in Vic's readout
		    }
		  
		  Asic_Side->Fill((my_side_id-1),(my_asic_id-1));
		  ADC->Fill(my_adc_data);
		  TS_TSext_diff->Fill( (my_tm_stp/1e9 - my_tm_stp_ext/1e9)); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TS_TSext->Fill(my_tm_stp/1e9,my_tm_stp_ext/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TS_event->Fill(TotalEvent+TotalHit, my_tm_stp/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout
		  TSext_event->Fill(TotalEvent+TotalHit,my_tm_stp_ext/1e9); // in sec !!!! if the 5ns already implemented in Vic's readout

		  
		  output_Tree->Fill();
		  
		  /*  
		}else{    //if ts_sort_flag
		std::cout << "*** ENTRY NOT FILLED:\n"
			  <<     "    type= "<< my_type << std::endl
			  <<     "    mod_id= "<< my_det_id << std::endl;
		std::cout << " *!->NF<-!* " << endl;
		std::cout << " " << endl;
		  */
	      } // end of if(fill_flag && my_tm_stp/1e9>1000)



	  }
	

	  //if ts_sort_flag	      
	  //index_temp.clear();
	  index_block_temp.clear();
	  index_hit_temp.clear();
	  ts_temp.clear();

	  } 

	       
      } // end of loop on the number of hit in the block

    } // end of if test on TotalTrackMult
 
  } // End of main loop over TTree to process raw MIDAS data entries



  TS->Write();
  TSext->Write();
  TS_p->Write();
  TSext_p->Write();
  TS_n->Write();
  TSext_n->Write();


  TS_TSext_diff->Write();
  TS_TSext->Write();
  TS_event->Write();
  TSext_event->Write();
  ADC_TS->Write();
  ADC_TS_p->Write();
  ADC_TS_n->Write();



  ADC->Write();

  Asic_Side->Write();

  outFile->Print(); 


  // This ti to create and save a graph rather than a 2D histogrma
   output_Tree->Draw("StartrackMapped.nhit:StartrackMapped.tm_stp","","");
   cout << output_Tree->GetSelectedRows() << endl;;
   TGraph *gr = new TGraph(output_Tree->GetSelectedRows(),
                                   output_Tree->GetV1(), output_Tree->GetV2());
   gr->Draw("ap");
   gr->Write();


  outFile->Write();

  outFile->Close();

  inFile->Close(); // Close TFile



  /*
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime
  << "s" << endl << endl;
  */
}



// The following ordering function only works if the number of entries of the tree (input_Tree->GetEntries()) is the total number of hits in the data stream
// and not the number of data blocks
void InsertionSort(vector<long long> & v_ts, vector<long long> & v_index){

  int v_n = v_ts.size();

  //cout << "v_n size=" << v_n << endl;
  for(int i=1; i<v_n; i++){

    long long next_ts, next_index;
    next_ts= v_ts.at(i);
    next_index= v_index.at(i);

    //cout << "i=" <<  i << endl;
    //cout << "next_ts=" <<  v_ts.at(i) << endl;
    //cout << "next_ts=" <<  v_ts.at(i-1) << endl;
    //cout << "next_index=" << v_index.at(i)  << endl;
    //cout << "next_index=" << v_index.at(i-1)  << endl;

    int j;
    for(j= i; j>0 && v_ts.at(j-1) > next_ts; j--){

      //v_ts[j]= v_ts.at(j-1);
      v_ts.at(j)= v_ts.at(j-1);
       //v_index[j]= v_index.at(j-1);
      v_index.at(j)= v_index.at(j-1);
    }


    //v_ts[j]= next_ts;
    v_ts.at(j)= next_ts;
    //v_index[j]= next_index;
    v_index.at(j)= next_index;



  }

  return;

}


// The following ordering function only works if the number of entries of the tree (input_Tree->GetEntries()) is the the number of data blocks and not 
// total number of hits in the data stream
void InsertionSort2(vector<long long> & v_ts, vector<long long> & v_block_index, vector<long long> & v_hit_index){

  int v_n = v_ts.size();

  cout << "v_n size=" << v_n << endl;
  for(int i=1; i<v_n; i++){

    long long next_ts, next_block_index, next_hit_index;
    next_ts= v_ts.at(i);
    next_block_index= v_block_index.at(i);
    next_hit_index= v_hit_index.at(i);

    //cout << "i=" <<  i << endl;
    //cout << "next_ts=" <<  v_ts.at(i) << endl;
    //cout << "next_ts=" <<  v_ts.at(i-1) << endl;
    //cout << "next_index=" << v_index.at(i)  << endl;
    //cout << "next_index=" << v_index.at(i-1)  << endl;

    int j;
    for(j= i; j>0 && v_ts.at(j-1) > next_ts; j--){

      //v_ts[j]= v_ts.at(j-1);
      v_ts.at(j)= v_ts.at(j-1);
       //v_index[j]= v_index.at(j-1);
      //v_index.at(j)= v_index.at(j-1);
      v_block_index.at(j)= v_block_index.at(j-1);
      v_hit_index.at(j)= v_hit_index.at(j-1);
    }

    //v_ts[j]= next_ts;
    v_ts.at(j)= next_ts;
    //v_index[j]= next_index;
    v_block_index.at(j)= next_block_index;
    v_hit_index.at(j)= next_hit_index;

  }

  return;

}

