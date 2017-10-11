///
/////////// OBSOLETE /////////////////////
///
//
//  Read the RAW output tree from StarTracker in R3Broot and order the data in time
//  to produce a ordered raw output tree.
//
//  root -l
//  .L startrackTimeSort.C
//  startrackTimeSort( "startrack_raw_data.root", "startrack_ordered_raw_data.root" )

#include <stdio.h>

#include <fstream>
#include "TFile.h"
#include "TGraph.h"
#include "TCanvas.h"
#include "TTree.h"
#include <vector>



// startrackTimeSort = the equivalent of DoSort() in Afredo's code
void startrackTimeSort(char* input, char* output)
{
  //TStopwatch timer;
  //timer.Start();

  // opening the Raw data File and Tree
  TFile *inFile = TFile::Open(input);
  // if only one input file:
  TTree* input_Tree = (TTree*)inFile->Get("cbmsim"); 

  // creating the time sorted File and Tree for the Si tracker


  //output File and Tree for the analysis result
  TFile* outFile = new TFile(output,"RECREATE");
  TTree* output_Tree = new TTree("cbmsim","cbmsim"); // cbmsim to keep same tree structure than raw tree.

  struct struct_entry_sort{
    //long long tm_stp; //reconstructed timestamp (MSB+LSB)
    long long tm_stp; //reconstructed timestamp (MSB+LSB)
    // not used *R3B* ->    long long info; //MBS info data (external timestamp), anything else(?)
    long long tm_stp_ext; //reconstructed external timestamp trigger
    int nhit;  // one hit is one strip hit (word 3)
     int type;
    int hit;
    int ModuleId;
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
  output_Tree->Branch("StarTrackRawHit", &s_entry.tm_stp,"tm_stp/L:tm_stp_ext/L:nhit/I:type/I:hit/I:ModuleId/I:Side/I:AsicId/I:StripId/I:ADCdata/I:sync_flag/O:pause_flag/O");

  outFile->cd();

  // Creating Histograms 
  TH1F *TS = new TH1F("TS","Time stamp (Reconstructed from hit strips) ",1000,1414.,1416.);
  TH2F *TS_TSext = new TH2F("TS_TSext"," TS vs TSext",1000,1414.,1416., 1000,1414.,1416.);
  TH2F *TS_event = new TH2F("TS_event"," TS vs hit#",4100,1.,410000.,1000,141480,141550);
  TH2F *TSext_event = new TH2F("TSext_event"," TSext vs hit#",20000,1.,20000.,1000,1414.315,1415.32);

  //  Information from input tree:

  TBranchElement *branchWordType = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fWordType");
  TBranchElement *branchHitValue = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fHitBit");

  TBranchElement *branchModuleId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fModuleId");
  TBranchElement *branchSideId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fSide");
  TBranchElement *branchAsicId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fAsicId");
  TBranchElement *branchStripId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fStripId");

  TBranchElement *branchEnergy = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fADCdata");
  TBranchElement *branchTimevhb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fTimevhb");
  TBranchElement *branchTimehb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fTimehb");
  TBranchElement *branchTimelb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fTimelb");

  TBranchElement *branchWRvhb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fWRvhb");
  TBranchElement *branchWRhb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fWRhb");
  TBranchElement *branchWRlb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fWRlb");

  TBranchElement *branchInfoCode = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fInfoCode");
  TBranchElement *branchInfoField = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fInfoField");

  TBranchElement *branchTimeExtvhb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fTimeExtvhb");
  TBranchElement *branchTimeExthb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fTimeExthb");
  TBranchElement *branchTimeExtlb = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fTimeExtlb");

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

  int my_info_code;
  unsigned long my_info_field;
  unsigned long my_tm_stp_hb;
  unsigned long my_tm_stp_vhb;
  unsigned long my_tm_stp_lb;


  // for extarnal timestamp trigger
  unsigned long my_tm_stp_Ext_hb;
  unsigned long my_tm_stp_Ext_vhb;
  unsigned long my_tm_stp_Ext_lb;

  Int_t TotalTrackMult=0;
  Int_t TotalHit = 0;
  Int_t TotalEvent = 0; // = Total hit after removing hit of word type 2

  //arrays to check synchronization is going fine (or not!)
  unsigned long tm_stp_msb_modules[30]={0};   // 30 = total number of ladders

  unsigned long my_tm_stp_msb; // most significant bits of time-stamp (in info code)

  //bool fill_flag;
  bool no_sync_flag;

  //
  //all things for ordering data by time stamp
  //	
  bool ts_sort_flag= false;

  vector<long long> ts_temp;
  vector<long long> index_temp;
  ts_temp.reserve(2048);
  index_temp.reserve(2048);

  const int MAXsort= 200; //do we need maximum number when we do not want to sort any more!?
  // const int MAXsort= 100000; //do we need maximum number when we do not want to sort any more!?

  long long n_entries = input_Tree->GetEntries();
  cout << "n_entries(=number of blocks) = " << n_entries << endl;

 
  for(long long i=0;i<n_entries;i++){  // n_entries is the number of blocks in the run
  //for(long long i=0;i<5;i++){  // n_entries is the number of blocks in the run
    //if(i%10 == 0) printf("Block:%d\n",i);
    if(i == 0) printf("Block:%d\n",i);

    Int_t nb = 0;
    Int_t Total_typ2 = 0;
 
 
    input_Tree->GetEntry(i); 
    //cout << "input Entry = " << input_Tree->GetEntry(i) << endl;
    //nb += input_Tree->GetEvent(i);
    //cout << "nb  = " << nb << endl;
 
    TotalTrackMult= branchEnergy->GetNdata();  // number of hit with adc_data
  
    cout << " number of <<hits>> in this block  = " << TotalTrackMult << endl;
    // 1 hit is a 3 (=11....) or a 2 (=10....) wordtype

    TotalEvent=TotalEvent+TotalHit+1;
       cout << "Total of hit of word type #3  = " << TotalEvent << endl;

     for(Int_t ii=0; ii<TotalTrackMult; ii++)
       //for(Int_t ii=0; ii<10; ii++)
      {
	TotalHit=(ii-Total_typ2);  // in the new tree we won't keep hit corresponding to type 2

	no_sync_flag=true;
    
	my_type= branchWordType->GetValue(ii,0,true);

	if(my_type==2)Total_typ2++;


        //cout << "########################"<< endl;
        //cout << "Total_typ2  = " << Total_typ2 << " ii= " << ii  << endl;
        //cout << "TotalEvent  = " << TotalEvent+TotalHit << "  my_type  = " << my_type << endl;
        //cout << "Totalhit  = " << TotalHit << "  my_type  = " << my_type << endl;


	  if(my_type==3){
	    my_det_id=branchModuleId->GetValue(ii,0,true);
	    my_hit=branchHitValue->GetValue(ii,0,true);
	    my_side_id=branchSideId->GetValue(ii,0,true);
	    my_asic_id=branchAsicId->GetValue(ii,0,true);
	    my_ch_id=branchStripId->GetValue(ii,0,true);
	    my_adc_data=branchEnergy->GetValue(ii,0,true);
	    
	    my_tm_stp_lb=branchTimelb->GetValue(ii,0,true);

	    // rec time stamp 47:0  To be used if time stamp already reconstructed in the unpacker
	    //my_tm_stp=branchTimevhb->GetValue(ii,0,true);
	    //cout << "my_tm_stp= " <<  my_tm_stp  << endl;
	    	    

	  }

	  
	  if(my_type==2){

	    my_info_code=branchInfoCode->GetValue(ii,0,true);
	    my_info_field=branchInfoField->GetValue(ii,0,true);
	    
	    //cout << "my_info_code=" << my_info_code << endl;
	    
	    // ts (47:28)
	    //if(my_info_code==7){
	    if(my_info_code==4 || my_info_code==7){
	    my_tm_stp_hb=branchTimehb->GetValue(ii,0,true);
	    //cout << "my_tm_stp_vhb   = " << my_tm_stp_vhb << endl;
	    //cout << "my_tm_stp_hb   = " << my_tm_stp_hb << " which should be equals to  Infofield =" << my_info_field <<  endl;
	    //cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;

   	    }

	    // ts (63:48)
	    //if(my_info_code==8){
	    if(my_info_code==5 || my_info_code==8){
	    my_tm_stp_vhb=branchTimevhb->GetValue(ii,0,true);
	    //cout << "my_tm_stp_vhb   = " << my_tm_stp_vhb << endl;
	    //cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
	    //cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;
	    }


	    // external ts (47:28)
	    if(my_info_code==14){

	      my_tm_stp_Ext_lb=branchTimeExtlb->GetValue(ii,0,true);
	      //if(my_tm_stp_Ext_lb==0){
	      //	cout << "Taking WR lb value in header" << endl;
	      //	my_tm_stp_Ext_lb=branchWRlb->GetValue(ii,0,true); // take WR time in block header instead
	      //}

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

	  }
	  
	  



	  /*
	  my_tm_stp_hb=branchTimehb->GetValue(ii,0,true); 
	  if(my_tm_stp_hb==0){
	    //cout << "Taking WR hb value in header" << endl;
	    my_tm_stp_hb=branchWRhb->GetValue(ii,0,true); // take WR time in block header instead
	  }

	  my_tm_stp_vhb=branchTimevhb->GetValue(ii,0,true);
	  if(my_tm_stp_vhb==0)
	  {
	    //cout << "Taking WR vhb value in header " << endl;
	    my_tm_stp_vhb=branchWRvhb->GetValue(ii,0,true); // take WR time in block header instead
	  }

	  //my_tm_stp=(my_tm_stp_vhb << 48) | (my_tm_stp_hb << 28) | (my_tm_stp_lb);
	  my_tm_stp= (my_tm_stp_hb << 28) | (my_tm_stp_lb);
	  //cout <<  "my_tm_stp="  << my_tm_stp << endl;
	  */

	  /*	 
	  my_tm_stp_Ext_lb=branchTimeExtlb->GetValue(ii,0,true);
	  if(my_tm_stp_Ext_lb==0){
	    //cout << "Takinbg WR lb value in header for ext" << endl;
	    my_tm_stp_Ext_lb=branchWRlb->GetValue(ii,0,true); // take WR time in block header instead
	  }
	  my_tm_stp_Ext_hb=branchTimeExthb->GetValue(ii,0,true);
	  if(my_tm_stp_Ext_hb==0){
	    //cout << "Takinbg WR hb value in header for ext" << endl;
	  my_tm_stp_Ext_hb=branchWRhb->GetValue(ii,0,true); // take WR time in block header instead
	  }
	  my_tm_stp_Ext_vhb=branchTimeExtvhb->GetValue(ii,0,true);
	  if(my_tm_stp_Ext_vhb==0){
	    //cout << "Takinbg WR vhb value in header for ext" << endl;
	  my_tm_stp_Ext_vhb=branchWRvhb->GetValue(ii,0,true); // take WR time in block header instead
	  }
	  */
	
	  //}

	  //cout << "my_tm_stp_vhb   = " << my_tm_stp_vhb << endl;
	  //cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
	  //cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;
	  //cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
	  //cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << endl;
	  //cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
	  //cout << "my_tm_stp_ext   = " << my_tm_stp_ext << endl;


	  // only feel tree for type 3 data
	  //	  if(my_type==3 && my_tm_stp_hb>0)
	  if(my_type==3 )
	    {
	      my_tm_stp= ( (my_tm_stp_hb<< 28 ) | (my_tm_stp_lb) ); // To be used if time stamp NOT already reconstructed in the unpacker
	      //cout << "my_tm_stp   = " << my_tm_stp << endl;

	      //cout << " filling event" << TotalEvent+TotalHit << endl; // 
	      //cout << " filling my_tm_stp=" << my_tm_stp << endl; // 
	      s_entry.tm_stp = my_tm_stp/1e7; //reconstructed timestamp (MSB+LSB)
	      // not used *R3B* ->    long long info; //MBS info data (external timestamp), anything else(?)
	      //cout << " filling my_tm_stp_ext=" << my_tm_stp_ext << endl; // 
	      s_entry.tm_stp_ext = my_tm_stp_ext/1e7; // 
	      s_entry.nhit     = TotalEvent+TotalHit; //j+n_first;
	      s_entry.type       = my_type;
	      s_entry.hit        = my_hit;
	      s_entry.ModuleId     = my_det_id;
	      s_entry.Side    = my_side_id;
	      s_entry.AsicId    = my_asic_id; // new *R3B*
	      s_entry.StripId      = my_ch_id;
	      // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only in type 0)
	      // type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
	      s_entry.ADCdata   = my_adc_data;
	      s_entry.sync_flag  = my_sync_flag; // check SYNC100 pulses received for this module
	      s_entry.pause_flag = my_pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...
	      
	      TS->Fill(my_tm_stp/1e11);
	      TS_TSext->Fill(my_tm_stp/1e9,my_tm_stp_ext/1e9);
	      TS_event->Fill(TotalEvent+TotalHit, my_tm_stp/1e9);
	      TSext_event->Fill(TotalEvent+TotalHit,my_tm_stp_ext/1e11);
	      output_Tree->Fill();
	      
	    }
	  /*
	    if(my_type==2){        // 10
	    
	    
	    my_info_code=branchInfoCode->GetValue(ii,0,true);
	    my_info_field=branchInfoField->GetValue(ii,0,true);
	    
	    cout << "my_info_code=" << my_info_code << endl;
	    
	    // external ts (47:28)
	    if(my_info_code==14){
	    my_tm_stp_Ext_hb=branchTimeExthb->GetValue(ii,0,true);
	    cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
	    cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << " which should be equals to  Infofield =" << my_info_field <<  endl;
	    cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
	    
	    my_tm_stp_ext= ( (my_tm_stp_Ext_hb<< 28 ) | (my_tm_stp_Ext_lb) );
	    cout << "my_tm_stp_ext   = " << my_tm_stp_ext << endl;
	    
	    }
	    // external ts (63:48)
	    if(my_info_code==15){
	    my_tm_stp_Ext_vhb=branchTimeExtvhb->GetValue(ii,0,true);
	    cout << "my_tm_stp_Ext_vhb   = " << my_tm_stp_Ext_vhb << endl;
	    cout << "my_tm_stp_Ext_hb   = " << my_tm_stp_Ext_hb << endl;
	    cout << "my_tm_stp_Ext_lb   = " << my_tm_stp_Ext_lb << endl;
	    
	    }
	    
	    // Data ts (47:28)
	    if(my_info_code==4 || my_info_code==7){
	    
	    no_sync_flag= false;
	    
	    if(my_info_code==4 || my_info_code==7) my_tm_stp_hb= my_info_field ;  //  47:28 timestamp
	    //if(my_info_code==5 || my_info_code==8) my_tm_stp_vhb= my_info_field;  //  63:48 
	    cout << "my_tm_stp_hb   = " << my_tm_stp_hb << endl;
	    branchTimehb->GetValue(ii,0,true);
	    cout << "which should be equals to   = " << branchTimehb->GetValue(ii,0,true) << endl;
	    
	    //my_tm_stp_msb= (my_tm_stp_vhb << 20) |  my_tm_stp_hb;	     
	    my_tm_stp_msb=  my_tm_stp_hb;	     
	    tm_stp_msb_modules[my_det_id]= my_tm_stp_msb; //update for use with other data types
	    
	    //reconstruct time stamp= MSB+LSB
	    cout << "my_tm_stp_lb   = " << my_tm_stp_lb << endl;
	    
	    my_tm_stp= (  (my_tm_stp_msb<< 28 ) | (my_tm_stp_lb) );
	    cout << "my_tm_stp   = " << my_tm_stp << endl;
	    
	    //vectors to order data by time-stamp
	    ts_temp.push_back(my_tm_stp);
	    index_temp.push_back(ii);
	    
	    ts_sort_flag= true;
	    
	    
	    }
	    cout << "ts_sort_flag=" << ts_sort_flag << endl;
	    }
	  */
	  
	  
	  //if any other data type
	  /*	if(no_sync_flag){
	  //reconstruct time stamp= MSB+LSB
	  //my_tm_stp= (  (tm_stp_msb_modules[my_det_id] << 28 ) | (my_tm_stp_lb) );
          cout << "my_tm_stp   = " << my_tm_stp << endl;
	  
	  //vectors to order data by time-stamp
	  ts_temp.push_back(my_tm_stp);
	  index_temp.push_back(ii);

	  cout << "ts_temp.size= "<<ts_temp.size() << endl;
	  cout << "index_temp.size= "<<index_temp.size() << endl;
	  
	  // if temp arrays too large, sort
	  //if(ts_temp.size()>MAXsort){
	    ts_sort_flag= true;
	    //  }
	  //if last, sort
	  //else if((ii+1)==n_entries){
	  //  ts_sort_flag= true;
	  //}
	}
	     */
	

	      /*
	if(ts_sort_flag){
	  
	  //long long n_first= index_temp.at(0); 
	    long long n_first=index_temp.at(0); 
	  //cout << n_first << endl;	  
	  long long n_loop= index_temp.size(); 
	  if(n_loop>1)cout << n_loop << endl;	  
	  
	  //InsertionSort(ts_temp,index_temp);
	  
	  ts_sort_flag= false;
	  
	  bool fill_flag= false;
	  bool my_sync_flag=false;
	  bool my_pause_flag=false;
	  
	  //transport sorted data to output_tree
	  for(long long j=0; j<n_loop; j++){	    //transport sorted data to output_tree
	    
	    //set address of new struct_entry_sort to temp_tree Branch? or is s_temp enough?
	    input_Tree->GetEntry(index_temp.at(j));
	    
	    
	    // Below is the equivalent of "if(SortEntry(s_entry_unpack, s_entry, j+n_first, ts_temp.at(j)) ) " in Alfredo's code:
	    fill_flag= false;
	    my_sync_flag=false;
	    my_pause_flag=false;
	    
	    
	    if(my_type==2){        // 10
	      
	      //if(branchInfoCode->GetValue(index_temp.at(j),0,true)==4 || branchInfoCode->GetValue(index_temp.at(j),0,true)==5 || branchInfoCode->GetValue(index_temp.at(j),0,true)==7){
	      if(branchInfoCode->GetValue(index_temp.at(j),0,true)==4 || branchInfoCode->GetValue(index_temp.at(j),0,true)==7){
		fill_flag= true;
	      }
	      else 
		{
		  fill_flag= false;
		}
	    }  
	    else if(my_type==3)  // 11
	    {
		my_hit=branchHitValue->GetValue(index_temp.at(j),0,true);
		my_adc_data=branchEnergy->GetValue(index_temp.at(j),0,true);
		my_asic_id=branchAsicId->GetValue(index_temp.at(j),0,true);
		my_ch_id=branchStripId->GetValue(index_temp.at(j),0,true);
		fill_flag=true;
	    }
	    
		//s_entry.tm_stp = ts_temp.at(j); //reconstructed timestamp (MSB+LSB)
	    cout << " filling my_tm_stp=" << my_tm_stp << endl; // 
	    s_entry.tm_stp = my_tm_stp; //reconstructed timestamp (MSB+LSB)
	    // not used *R3B* ->    long long info; //MBS info data (external timestamp), anything else(?)
	    s_entry.nevent     = j+n_first;
	    cout << " filling event" << j+n_first << endl; // 
	    cout << " filling my_tm_stp_ext=" << my_tm_stp_ext << endl; // 
	    s_entry.tm_stp_ext = my_tm_stp_ext; // 
	    s_entry.type       = my_type;
	    s_entry.hit        = my_hit;
	    s_entry.ModuleId     = my_det_id;
	    s_entry.Side    = my_side_id;
	    s_entry.AsicId    = my_asic_id; // new *R3B*
	    s_entry.StripId      = my_ch_id;
	    // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only in type 0)
	    // type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
	    s_entry.ADCdata   = my_adc_data;
	    s_entry.sync_flag  = my_sync_flag; // check SYNC100 pulses received for this module
	    s_entry.pause_flag = my_pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...
	    
	    if(fill_flag){
	      output_Tree->Fill();
	  

	    }else{    //if ts_sort_flag
	      std::cout << "*** ENTRY NOT FILLED:\n"
			<<     "    type= "<< my_type << std::endl
			<<     "    mod_id= "<< my_det_id << std::endl;
	      std::cout << " *!->NF<-!* " << endl;
	      std::cout << " " << endl;
	    } 
	  }

	}//if ts_sort_flag
	      */
	  index_temp.clear();
	  ts_temp.clear();
	       
      } // end of loop on the number of hit in the block
 
  } // End of main loop over TTree to process raw MIDAS data entries



  TS->Write();
  TS_TSext->Write();
  TS_event->Write();
  TSext_event->Write();

  // This ti to create and save a graph rather than a 2D histogrma
   output_Tree->Draw("StarTrackRawHit.nhit:StarTrackRawHit.tm_stp","","");
   cout << output_Tree->GetSelectedRows() << endl;;
   TGraph *gr = new TGraph(output_Tree->GetSelectedRows(),
                                   output_Tree->GetV1(), output_Tree->GetV2());
   gr->Draw("ap");
   gr->Write();




  outFile->Write();
  outFile->Print(); 
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



//void InsertionSort(std::vector<long long> & v_ts, std::vector<long long> & v_index){
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



