//
//  Read the RAW output tree from StarTracker in R3Broot and order the data in time
//  to produce a ordered raw output tree.
//
//  root -l
//  .L startrackTimeSort
//  startrackTimeSort( "startrack_raw_data.root", "startrack_ordered_raw_data_DoUnpack2.root" )

#include <stdio.h>

#include <fstream>
#include "TFile.h"
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
    long long nevent;
    int type;
    int hit;
    int det_id;
    int side_id;
    int asic_id; // new *R3B*
    int ch_id;
    // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only in type 0)
              // type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
    int adc_data;
    bool sync_flag; // check SYNC100 pulses received for this module
    bool pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...
  };

  struct_entry_sort s_entry;

  // Defining outT branches
  output_Tree->Branch("STaRTrackRawHit", &s_entry.tm_stp,"tm_stp/L:nevent/L:type/I:hit/I:det_id/I:side_id/I:asic_id/I:ch_id/I:adc_data/I:sync_flag/O:pause_flag/O");

  outFile->cd();


  //  Information from input tree:

  TBranchElement *branchWordType = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fWordType");
  TBranchElement *branchHitValue = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fHitBit");

  TBranchElement *branchModuleId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fModuleId");
  TBranchElement *branchSideId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fSide");
  TBranchElement *branchAsicId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fAsicId");
  TBranchElement *branchStripId = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fStripId");

  TBranchElement *branchEnergy = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fEnergy");
  TBranchElement *branchTime = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fTime");

  TBranchElement *branchInfoCode = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fInfoCode");
  TBranchElement *branchInfoField = (TBranchElement*)input_Tree->GetBranch("StarTrackRawHit.fInfoField");

  //for structure struct_entry_sort
  long long my_tm_stp;
  int my_type= -1; //
  int my_hit= -1;
  int my_det_id;
  int my_side_id;
  int my_asic_id= -1;
  int my_ch_id= -1;
  int my_adc_data= -1;
  bool my_sync_flag=false;
  bool my_pause_flag=false;

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
  //vector<Long64_t> ts_temp;
  //vector<Long64_t> index_temp;
  //ts_temp.reserve(2048);
  //index_temp.reserve(2048);

  const int MAXsort= 200; //do we need maximum number when we do not want to sort any more!?
  // const int MAXsort= 100000; //do we need maximum number when we do not want to sort any more!?

  long long n_entries = input_Tree->GetEntries();
  cout << "n_entries = " << n_entries << endl;

  //Int_t nb = 0;
 
  //for(long long i=0;i<n_entries;i++){  
  for(long long i=0;i<MAXsort;i++){  
    if(i%10 == 0) printf("Event:%d\n",i);
    
    //nb += input_Tree->GetEvent(i);
    input_Tree->GetEntry(i);
 
    no_sync_flag=true;
    
    my_det_id=branchModuleId->GetValue(i,0,true);
    my_side_id=branchSideId->GetValue(i,0,true);
 
    if(branchWordType->GetValue(i,0,true)==2)
      {
	if(branchInfoCode->GetValue(i,0,true)==4 || branchInfoCode->GetValue(i,0,true)==5 || branchInfoCode->GetValue(i,0,true)==7)
	  {
	    no_sync_flag=false;

	    my_tm_stp_msb= (branchInfoField->GetValue(i,0,true) & 0x000FFFFF);
	    tm_stp_msb_modules[my_det_id]= my_tm_stp_msb; //update for use with other data types

	    //reconstruct time stamp= MSB+LSB
	    my_tm_stp= (  (tm_stp_msb_modules[my_det_id] << 28 ) | ( branchTime->GetValue(i,0,true) & 0x0FFFFFFF) );

	    //vectors to order data by time-stamp
	    ts_temp.push_back(my_tm_stp);
	    index_temp.push_back(i);
	    
	    ts_sort_flag= true;

	  }
      }

    //if any other data type
    if(no_sync_flag){
      //reconstruct time stamp= MSB+LSB
      my_tm_stp= (  (tm_stp_msb_modules[my_det_id] << 28 ) | ( branchTime->GetValue(i,0,true) & 0x0FFFFFFF) );

      //vectors to order data by time-stamp
      ts_temp.push_back(my_tm_stp);
      index_temp.push_back(i);
      
      //if temp arrays too large, sort
      if(ts_temp.size()>MAXsort){
	ts_sort_flag= true;
      }
      //if last, sort
      else if((i+1)==n_entries){
	ts_sort_flag= true;
      }
    }

    cout << "ts_sort_flag=" << ts_sort_flag << endl;
 
   if(ts_sort_flag){
	    
      long long n_first= index_temp.at(0); 
      long long n_loop= index_temp.size(); 

 
      InsertionSort(ts_temp,index_temp);

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

	my_type= branchWordType->GetValue(index_temp.at(j),0,true);

	if(my_type==2){        // 10

	  if(branchInfoCode->GetValue(index_temp.at(j),0,true)==4 || branchInfoCode->GetValue(index_temp.at(j),0,true)==5 || branchInfoCode->GetValue(index_temp.at(j),0,true)==7){
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

              s_entry.tm_stp = ts_temp.at(j); //reconstructed timestamp (MSB+LSB)
	      // not used *R3B* ->    long long info; //MBS info data (external timestamp), anything else(?)
	      s_entry.nevent     = j+n_first;
	      s_entry.type       = my_type;
	      s_entry.hit        = my_hit;
	      s_entry.det_id     = my_det_id;
	      s_entry.side_id    = my_side_id;
	      s_entry.asic_id    = my_asic_id; // new *R3B*
              s_entry.ch_id      = my_ch_id;
	      // not used *R3B* ->    int type; // QQQ: 0= 20 MeV or 1 GeV (decays), 1= 20 GeV (checked pulser data only in type 0)
              // type>=10: type = info_code+10 (i.e., PAUSE, RESUME, SYNC100, etc...)
	      s_entry.adc_data   = my_adc_data;
	      s_entry.sync_flag  = my_sync_flag; // check SYNC100 pulses received for this module
	      s_entry.pause_flag = my_pause_flag; // check Pause signals followed by proper Resume signal: true= SYNC100 paused...
	
	      if(fill_flag)output_Tree->Fill();
	  }
	else{
	  std::cout << "*** ENTRY NOT FILLED:\n"
		   <<     "    type= "<< my_type << std::endl
		   <<     "    mod_id= "<< my_det_id << std::endl;
	  std::cout << " *!->NF<-!* ";
	}
      
      
      index_temp.clear();
      ts_temp.clear();
      
   } //if ts_sort_flag
    

  } // End of main loop over TTree to process raw MIDAS data entries



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

  cout << "v_n size=" << v_n << endl;
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



