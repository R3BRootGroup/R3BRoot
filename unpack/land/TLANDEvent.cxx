///////////////////////////////////////////////////////////////
//    TLANDEvent IMPLEMENTATION 
//    Apr 2011
//    ver. 1.0 event unpacker
//    Taken from N. Kurs 05/06/2008
//
//    mod. version 1/2012   v. 1.1 light rev1.,  p.r., e.d.f. 
//    note: "light" revision rev1 maintains original data structures
//    just a tentative cleaning of the original source file 
//
//    v 1.2  rev2 v1.2   p.r., e.d.f. 
//    note: danger: this revision contains strong modifications
//                  of the original data structures. Anyway 
//                  the original design and LAND mapping is maintained 
////////////////////////////////////////////////////////////////
#include "TLANDEvent.h"
#include <fstream>
#include <iostream>
#include <cstring>
#include "Riostream.h"
#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TList.h"
#include "TGraph.h"
#include "TCanvas.h"
#include <stdio.h>
#include "TCanvas.h"
#include "TLANDstruct.h"
#define rot2 0.065
#include "TMath.h"

double lcalC1[200];
double lcalC2[200];
double lcalC5[200];
double lcalC6[200];
double lcalM1[200];
double lcalM2[200];
double lcalM5[200];
double lcalM6[200];
int  l_i;
int  l_check;
int  l_mult;
int  l_mult1;
int  l_mult2;
int  l_dat, l_da0, l_da1;
int  l_sam_id;
int  l_gtb_id;
int  l_da_siz;
int  l_tac_addr;
int  l_tac_ch;
int  l_cal;
double  l_tcal[480];     //calibtime
int     l_ch_tim;
int     l_charge;
int     l_17_ct;
double  l_tim[30][17];
double  l_q[30][17];
int     l_17[30][17];
double  l_17tim[30][17];
int     l_sum[30][17];



Sl_map_ta l_map_ta[20][2];
Sl_map_tc l_map_tc[20][2];
Sl_map_tim l_map_tim[20][2];
Sl_map_QDC l_map_QDC[20][2];
Sl_map_tusync l_map_tusync[20][2];
Sl_map_cl l_map_cl[20][2];
Sl_map_hit l_map_hit[20];


int l_once; 
static int  l_phy_evt_ct=0;


TLANDEvent *glandevent = 0;

//Constructor
TLANDEvent::TLANDEvent(string landcaldir) 
{
 flandcaldir=landcaldir;
 if(glandevent==0)glandevent = this;
}


//Destructor 
TLANDEvent::~TLANDEvent()
{
// cout<<"TLANDEvent destructor...done"<<endl;
 glandevent = 0;
}



int TLANDEvent::GetEvent(int *start)  
{
 int len = *start / 2 + 2;
 return len;
}

//************************************************
//
// LAND readout for LAND-MBS event structure 
//   
//************************************************
int TLANDEvent::ProcessCurrentEventMBS(int *pl_se_dat, int l_evt_siz)  //this should now be buffer
{
  //cout << "Data siz " << l_evt_siz << endl;
  // first we fill the arrays fCrate1,2 with data from MBS source
  //this is nick's comment and I don't follow it at all-fCrate1,2 do not exist in his code

  //buffer starts with sam id
  l_mult=0;
  l_once=0;
  l_check=0;
  //  int l_trig_ev=0;
  // cout << "event size is " << l_evt_siz << endl;  
  l_evt_siz = (l_evt_siz)/2;  //put to long words not words 
  l_evt_siz--;  

  //this will have 3 branches for the 3 gtbs used.  
  //cout << "Event size " << l_evt_siz << endl;
  l_i=0;
  
  //initialise time info
  memset(l_tim, 0, sizeof(l_tim));
  memset(l_q, 0, sizeof(l_q));
  memset(l_17, 0, sizeof(l_17));
  memset(l_17tim, 0, sizeof(l_17tim));


  while(l_i < l_evt_siz) {   
   l_dat =  *(pl_se_dat + l_i );
   
// cout << "l_dat is " << l_dat << endl;
// printf("l_dat (%d): 0x%08x\n",l_i,l_dat);
   l_sam_id = (l_dat & 0xf0000000) >> 28;   //identifies the sam  
   //  cout << "sam was " << l_sam_id << endl;
   l_gtb_id = (l_dat & 0x0f000000) >> 24;   //0 or 1, identifies which of the 2 cables of the sam 
   //cout << "gtb was " << l_gtb_id << endl;    
   l_da_siz = (l_dat & 0x000001ff);
    //Don't forget this includes the 17th channel
    
    //OK so now we have 2 sams: sam5 and sam6. Sam5 has GTB0 and sam6 has GTB0 and 1. 
    //Need to loop over these as well as the data size.    
    //in the case of second or third branch, find the size of the first branch subevt
    //generalise to all
    
    //expand this!
    //extra loop 
    l_i++;  
    for(int l_2_i = 1; l_2_i<=l_da_siz; l_2_i+=2) {   //incr by 2 to loop over data size.	
      //because we loop over subevent data size we don't need to loop over the sams.
     l_da0 = *(pl_se_dat + l_2_i    );  //tac
     l_da1 = *(pl_se_dat + l_2_i + 1);  //qdc
     l_tac_addr = (l_da0 & 0xf8000000) >> 27; //tac mod address
     l_tac_ch   = (l_da0 & 0x07c00000) >> 22; //tac channel no
     l_cal      = (l_da0 & 0x00100000) >> 20; //calibration bit, 0 for physics
     l_17_ct    = (l_da0 & 0x0003f000) >> 12; //cntrl data      
//	cout << l_17_ct <<" "<<" "<<l_cal<<" "<<  l_da0<< endl;
//      l_ch_tim   = (l_da0 & 0x00000fff);  //tac data
//      l_charge   = (l_da1 & 0x00000fff);  //qdc data
     l_ch_tim   = (l_da1 & 0x00000fff);  //tac data
     l_charge   = (l_da0 & 0x00000fff);  //qdc data
	
     //cout << "TAC ADDR IS " << l_tac_addr << endl;
     //cout << "TAC ch IS " << l_tac_ch << endl;
     //cout << "TAC info IS " << l_ch_tim << endl;
     //cout << "QDC INFO IS " << l_charge << endl;
     if ( (l_cal == 0) && (l_once == 0)) { // physics 
      l_once = 1;   
      l_phy_evt_ct++;    
     }
	
     //Find which tac and which channel fired (10 for test data)	
     for(int l_twoi=1; l_twoi<31; l_twoi++) {
      for(int l_twoj=0; l_twoj<17; l_twoj++) {   
        int new_l_tac_addr = l_tac_addr; 
	//reassign the tac address to count over all 30 addresses
	if((l_sam_id==6) && (l_gtb_id==1)) {
	 new_l_tac_addr=l_tac_addr+10;
	}
	if((l_sam_id==5) && (l_gtb_id==0)) {		
	 new_l_tac_addr=l_tac_addr+20;
	}
	//NOTE THAT AS OF 13/05/11 WE ARE REMOVING OBSCURE TAC VALUES (SEE LOGBOOK)
	if ((l_cal==0) && (new_l_tac_addr==l_twoi) && (l_tac_ch==l_twoj) && (l_ch_tim<3100)) {
	 l_check++;      
	 l_tim[new_l_tac_addr-1][l_tac_ch]=l_ch_tim;
	 l_17[new_l_tac_addr-1][l_tac_ch]=l_17_ct;
	 l_q[new_l_tac_addr-1][l_tac_ch]=l_charge;
	     
    //fill l_17tim with the 17th channel time info
    //paolo16122011
	  if((l_tac_ch==16) && (l_tim[new_l_tac_addr-1][l_tac_ch]>0)) {
           for(int lp17=0; lp17<17; lp17++) {
//            cout<<lp17<<" "<<new_l_tac_addr-1<<" "<<l_tac_ch<<" "                <<l_tim[new_l_tac_addr-1][l_tac_ch]<<endl; 
            l_17tim[new_l_tac_addr-1][lp17]=l_tim[new_l_tac_addr-1][l_tac_ch];
//	    getchar();
           }
           //l_tim[new_l_tac_addr-1][l_tac_ch]=-2000;
	  }
// somehow need to make l_17tim for all channels/boards...not sure what to do here.......
// I THINK it's OK to do l_17tim[new_l_tac_addr-1][17] and fill all 16 other chs 
// with l_17tim[blah][16]

	   if((l_tim[new_l_tac_addr-1][l_tac_ch]>0) && (l_tac_ch!=16)) {
	    l_mult++;
	   }
	   //if(l_tac_ch==16) l_tim[new_l_tac_addr-1][l_tac_ch]=-2000;	
	 }
	}
       } 
     
      l_i+=2;
     }
//Am I at the end of the data size? if no, keep going...
  }   //while {....}

  //not convert l_tim to real l_tim (counter and 17th ch)
  for(int l_loopaddrs=0; l_loopaddrs<30; l_loopaddrs++) {
   for(int l_loopchs=0; l_loopchs<16;l_loopchs++) {
    int l_clocktime = l_17[l_loopaddrs][l_loopchs]*25;
      // cout << "Clock time gives " << l_clocktime << endl;
      //when I have the calib file, use l_tcal[l_loopchs+(l_loopaddrs*10)] to get l_tim in ns, then do:
      //      l_tim[l_loopaddrs][l_loopchs]=l_tim[l_loopaddrs][l_loopchs]-l_17tim[l_loopaddrs]+l_clocktime;
      //for now I will just take it to be 10 ps/bin. I should take 17th ch from this (check file)
     double l_nstime=l_tim[l_loopaddrs][l_loopchs]*(0.01/25);
     //l_tim[l_loopaddrs][l_loopchs]=(l_nstime+l_clocktime);
    } 
   }

  int ftotfired=l_mult;

//   cout << "Mult is " << l_mult << endl;
//   cout << "The number of hit channels was " << l_check<< " but for some reason da_siz was " << l_da_siz <<endl;
 return ftotfired;
}


void TLANDEvent::GetTiMeStamp(int *tsl, int *tsm, int *tsh)
{
 int ftsl=1,ftsm=1,ftsh=1;   //timestamp for later
 *tsl = ftsl;
 *tsm = ftsm;
 *tsh = ftsh;
}



int TLANDEvent::DoLANDMapping()
{
//see channel_list-1.txt, read in values 
 int l_mapta[440];
 int l_maptc[440];
 //fill these with the tac add and channel of each line in order.
 
 FILE *landmap;
 int l_char_sam[440];
 int l_char_gtb[440];
 int l_char_addr[440];
 int l_char_ch[440];
 
 int lplaneno=0;
 int lpaddleno=0;
 int lpmno=0;
 char l_norv;  //is it the veto or normal plane? 
 
 string flandmapping = flandcaldir + "LANDMAPPING.txt";
 cout << flandmapping << endl;
// getchar();
// landmap=fopen("LANDMAPPING.txt","r");
 landmap=NULL;
 landmap=fopen(flandmapping.c_str(),"r");
 if(!landmap) return 1;

//cout << "giorgio_________________" << endl;
//getchar();
 if(landmap==NULL) {
  cout << "could not get file LANDMAPPING.txt" << endl;
 }
 else { 
  for(int l_linecount=0; l_linecount<440; l_linecount++) {     
   fscanf(landmap, "%c%d_%d_%d SAM%d_GTB%d_TAC%d_CH%d\n",
    &l_norv, &lplaneno, &lpaddleno, &lpmno, &l_char_sam[l_linecount], &l_char_gtb[l_linecount], 
    &l_char_addr[l_linecount], &l_char_ch[l_linecount]);     
//          cout << "got tac add as " << l_char_addr[l_linecount] << " and ch as " << l_char_ch[l_linecount]<< endl;
//cout << l_linecount << endl;
//getchar();
  } 
 fclose(landmap);
 }

 for(int l_lin=0; l_lin<440; l_lin++) {   
  int tempa=0;
  int tempc=0;
  //cout<<"tempa_ "<< l_char_addr[l_lin] <<"| __tempc__ "<< l_char_ch[l_lin] <<endl;
  //getchar();
  //set the tac addr and channel 
  tempa=l_char_addr[l_lin]-1;
  tempc = l_char_ch[l_lin]-1;
 
  //set addr depending on sam and gtb
  if((l_char_sam[l_lin]==6)&&(l_char_gtb[l_lin]==1))tempa=tempa+10;
  if((l_char_sam[l_lin]==5)&&(l_char_gtb[l_lin]==0))tempa=tempa+20;
  cout << "LAND Mapping:" << endl;   
  cout << "tac addr is "<<tempa << endl;
  cout << "tac ch is "<<tempc << endl;
  l_mapta[l_lin]=tempa;
  l_maptc[l_lin]=tempc;

 //getchar();
 }
 
 int paddlemapb = 0;
 for(int paddlemap=0; paddlemap<40; paddlemap+=2){
  int j=0;
  for(int i=0; i<=9; i++) {   
   l_map_ta[paddlemapb][0].ta[i] = l_mapta[paddlemap+j];
   l_map_ta[paddlemapb][1].ta[i] = l_mapta[paddlemap+j+1];

   l_map_tc[paddlemapb][0].tc[i] = l_maptc[paddlemap+j];
   l_map_tc[paddlemapb][1].tc[i] = l_maptc[paddlemap+j+1];

   j+=40; 
  }   
  l_map_ta[paddlemapb][0].taV = l_mapta[paddlemap+400];
  l_map_ta[paddlemapb][1].taV = l_mapta[paddlemap+401]; 
  l_map_tc[paddlemapb][0].tcV = l_maptc[paddlemap+400];
  l_map_tc[paddlemapb][1].tcV = l_maptc[paddlemap+401];

  paddlemapb++;
 }
 return 0;
}
//in theory can we do this mapping only once? Lets move it somewhere else!



int TLANDEvent::GetLANDRawData(TRootLANDEvent *landevt, bool corr)
{
//paolo 24022012
 double qdc1norm,qdc2norm,qposecm,qpost,qposecmrot,qpostcmrot;
// int tcutmin=-15,tcutmax=15;
//paolo 24022012

 //first add raw mult
 landevt->rawmulti = l_mult;

 //initialise tim etc 
 for(int l_init=0; l_init<20; l_init++) {
  for(int j=0; j<=9; j++) {
   l_map_tim[l_init][0].tim[j] = 0;
   l_map_tim[l_init][1].tim[j] = 0;
   l_map_QDC[l_init][0].QDC[j] = 0;
   l_map_QDC[l_init][1].QDC[j] = 0;
   l_map_tim[l_init][0].tim17[j] = 0;
   l_map_tim[l_init][1].tim17[j] = 0;
   l_map_cl[l_init][0].cl[j] = 0;
   l_map_cl[l_init][1].cl[j] = 0;
   l_map_hit[l_init].hit[j] = 0;
  }
  l_map_tim[l_init][0].timV = 0; 
  l_map_tim[l_init][1].timV = 0;
  l_map_QDC[l_init][0].QDCV = 0;
  l_map_QDC[l_init][1].QDCV = 0;
  l_map_tim[l_init][0].tim17V = 0;
  l_map_tim[l_init][1].tim17V = 0;
  l_map_cl[l_init][0].clV = 0;
  l_map_cl[l_init][1].clV = 0;
  l_map_hit[l_init].hitV = 0; 
 }

// cout << "initializing is ok...."<<endl;
 l_mult1=0;
 l_mult2=0;
 for(int paddlemap2=0; paddlemap2<20; paddlemap2++) {
  //fill l_map_timX with l_tim in order.
  //l_tim[the tac address of the Nth paddle][the tac channel of the nth paddle]

  for(int j=0; j<=9; j++) {
   int kta0 = l_map_ta[paddlemap2][0].ta[j];
   int ktc0 = l_map_tc[paddlemap2][0].tc[j];
   int kta1 = l_map_ta[paddlemap2][1].ta[j];
   int ktc1 = l_map_tc[paddlemap2][1].tc[j];

   l_map_tim[paddlemap2][0].tim[j] = l_tim[kta0][ktc0];
   if(l_map_tim[paddlemap2][0].tim[j]<0) l_map_tim[paddlemap2][0].tim[j]=0;
   l_map_tim[paddlemap2][1].tim[j] = l_tim[kta1][ktc1];
   if(l_map_tim[paddlemap2][1].tim[j]<0) l_map_tim[paddlemap2][1].tim[j]=0;

 
   //check to see if both PMs see hit, and if so count mult and record which paddle
   if((l_map_tim[paddlemap2][0].tim[j]>0) && (l_map_tim[paddlemap2][1].tim[j]>0)) {
    l_mult2++;
    l_map_hit[paddlemap2].hit[j] = 2;
   }else if ((l_map_tim[paddlemap2][0].tim[j]>0) || (l_map_tim[paddlemap2][1].tim[j]>0)){
    l_mult1++;
    l_map_hit[paddlemap2].hit[j] = 1;
   }

    

   //Do the same for clock info, ch17 (17tim), read calibs from file to make a TGraph and 
   //you can get real (unsynced) time. 
   //For now though I just do it in the data format and guess that each bin is X ns roughly.
   l_map_tim[paddlemap2][0].tim17[j] = l_17tim[kta0][ktc0];
   l_map_tim[paddlemap2][1].tim17[j] = l_17tim[kta1][ktc1];


   //same for clock cycle counts
   l_map_cl[paddlemap2][0].cl[j] =l_17[kta0][ktc0];
   l_map_cl[paddlemap2][1].cl[j] =l_17[kta1][ktc1];


   //same for QDC info
   l_map_QDC[paddlemap2][0].QDC[j]= l_q[kta0][ktc0];
   l_map_QDC[paddlemap2][1].QDC[j]= l_q[kta1][ktc1];

  }  //for j=0..9

  int ktaV0 = l_map_ta[paddlemap2][0].taV;
  int ktcV0 = l_map_tc[paddlemap2][0].tcV;
  int ktaV1 = l_map_ta[paddlemap2][1].taV;
  int ktcV1 = l_map_tc[paddlemap2][1].tcV;

  l_map_tim[paddlemap2][0].timV =l_tim[ktaV0][ktcV0];
  l_map_tim[paddlemap2][1].timV =l_tim[ktaV1][ktcV1];
  if((l_map_tim[paddlemap2][0].timV>0) && (l_map_tim[paddlemap2][1].timV>0)) {
   l_mult2++;
   l_map_hit[paddlemap2].hitV = 1;
  }
  l_map_tim[paddlemap2][0].tim17V = l_17tim[ktaV0][ktcV0];
  l_map_tim[paddlemap2][1].tim17V = l_17tim[ktaV1][ktcV1];	  
  l_map_cl[paddlemap2][0].clV     = l_17[ktaV0][ktcV0];
  l_map_cl[paddlemap2][1].clV     = l_17[ktaV1][ktcV1]; 
  l_map_QDC[paddlemap2][0].QDCV   = l_q[ktaV0][ktcV0];
  l_map_QDC[paddlemap2][1].QDCV   = l_q[ktaV1][ktcV1];

 }  //for{int paddlemap2=0; paddlemap2<20 .....}
 //
 //
 int nt=l_mult2;

// cout << " End of primary LAND unpacking section nt= "<<nt<<endl; 

 //here define new mult and all other raw data to save to landevt
//note for(int lrttree=0; lrttree<20; lrttree++) {
//note  for(int j=0; j<=9; j++) {
//note   landevt->out_l_map_QDC[lrttree][0].QDC[j] = l_map_QDC[lrttree][0].QDC[j]; //qdc info first plane
//note   landevt->out_l_map_QDC[lrttree][1].QDC[j] = l_map_QDC[lrttree][1].QDC[j]; //qdc info first plane
//note   landevt->out_l_map_TAC[lrttree][0].tim[j] = l_map_tim[lrttree][0].tim[j]; //tac info first plane
//note   landevt->out_l_map_TAC[lrttree][1].tim[j] = l_map_tim[lrttree][1].tim[j]; //tac info first plane
//note   landevt->out_l_map_TAC[lrttree][0].tim17[j] = l_map_tim[lrttree][0].tim17[j]; //t17 info first plane
//note   landevt->out_l_map_TAC[lrttree][1].tim17[j] = l_map_tim[lrttree][1].tim17[j]; //t17 info first plane
//note   landevt->out_l_map_cl[lrttree][0].cl[j] = l_map_cl[lrttree][0].cl[j]; //cl info first plane
//note   landevt->out_l_map_cl[lrttree][1].cl[j] = l_map_cl[lrttree][1].cl[j]; //cl info first plane
//note   landevt->out_l_map_hit[lrttree].hit[j] = l_map_hit[lrttree].hit[j]; //hits
//note  }

  // "V" section 
//note  landevt->out_l_map_QDC[lrttree][0].QDCV = l_map_QDC[lrttree][0].QDCV; //qdc info first plane
//note  landevt->out_l_map_QDC[lrttree][1].QDCV = l_map_QDC[lrttree][1].QDCV; //qdc info first plane
//note  landevt->out_l_map_TAC[lrttree][0].timV = l_map_tim[lrttree][0].timV; //tac info first plane
//note  landevt->out_l_map_TAC[lrttree][1].timV = l_map_tim[lrttree][1].timV; //tac info first plane
//note  landevt->out_l_map_TAC[lrttree][0].tim17V = l_map_tim[lrttree][0].tim17V; //t17 info first plane
//note  landevt->out_l_map_TAC[lrttree][1].tim17V = l_map_tim[lrttree][1].tim17V; //t17 info first plane
//note  landevt->out_l_map_cl[lrttree][0].clV = l_map_cl[lrttree][0].clV; //cl info first plane
//note  landevt->out_l_map_cl[lrttree][1].clV = l_map_cl[lrttree][1].clV; //cl info first plane
//note  landevt->out_l_map_hit[lrttree].hitV = l_map_hit[lrttree].hitV; //hits 
//note } //for(int lrttree=0....)
 //
 //can now try to define/use/save sync data
 //
 double taccal[10][2];
 double t17cal[10][2];
 double tclcy[10][2]; 
 //initialization to 0
 memset(taccal,0,sizeof(taccal));
 memset(t17cal,0,sizeof(t17cal));
 memset(tclcy, 0,sizeof(tclcy));

 //main loop
 int num=0;
 for(int hj=0; hj<20; hj++) {

 //what about TDiff, TSync? Need to apply these as well...
  for(int j=0; j<=9; j++) {
   int kindex = (j*20) + hj;
   //////// TAC PM, planes
   if(l_map_tim[hj][0].tim[j] !=0) {
    taccal[j][0] = (l_map_tim[hj][0].tim[j]*lcalM1[kindex]) + lcalC1[kindex];
   }
   if(l_map_tim[hj][1].tim[j]!=0) {
    taccal[j][1] = (l_map_tim[hj][1].tim[j]*lcalM2[kindex]) + lcalC2[kindex];
   }
 

   ///////// T17 PM, Planes.
   // Warning: hj=0 or hj>0 if j=0 have an inchoerent source in the original code 
   if(l_map_tim[hj][0].tim17[j]!=0) {
     t17cal[j][0] = (l_map_tim[hj][0].tim17[j]*lcalM5[kindex]) + lcalC5[kindex];
   }
   if(l_map_tim[hj][1].tim17[j]!=0) {
    t17cal[j][1] = (l_map_tim[hj][1].tim17[j]*lcalM6[kindex]) + lcalC6[kindex];
   }

   //clock cycle time 
   tclcy[j][0] = l_map_cl[hj][0].cl[j] *25;
   tclcy[j][1] = l_map_cl[hj][1].cl[j] *25;
  }  //for(int j=0; j<=9; j++) ...
    

 //set tusync val to 0
  for(int j=0; j<=9; j++) {
   l_map_tusync[hj][0].tusync[j] = 0;
   l_map_tusync[hj][1].tusync[j] = 0; 
   l_map_hit[hj].ut[j] = -1000; 
  }


 //final tsync value
  for(int j=0; j<=9; j++) {
   if(l_map_hit[hj].hit[j]!=0) {
    l_map_tusync[hj][0].tusync[j] = (taccal[j][0] + tclcy[j][0]) - t17cal[j][0];
    l_map_tusync[hj][1].tusync[j] = (taccal[j][1] + tclcy[j][1]) - t17cal[j][1];
    l_map_hit[hj].ut[j] = l_map_tusync[hj][0].tusync[j] - l_map_tusync[hj][1].tusync[j]; 
   }
  } //for(int j=0; j<=9; j++) ....


  // t1-t2 and usT (tusync); Root tree assignement
  for(int j=0; j<=9; j++) {
   if(l_map_hit[hj].hit[j]> 0){
    landevt->padID[num]=j*100+hj;
    qdc1norm=l_map_QDC[hj][0].QDC[j]*gain1[j][hj]/qdiff[j][hj];
    qdc2norm=l_map_QDC[hj][1].QDC[j]*gain2[j][hj]*qdiff[j][hj];
    qposecm=2*(log(qdc1norm)-log(qdc2norm))*qlarg[j][hj];
    qpost=l_map_tusync[hj][0].tusync[j] - l_map_tusync[hj][1].tusync[j];
    qposecmrot= qposecm*cos(rot2)+qpost*sin(rot2);
    qpostcmrot=-qposecm*sin(rot2)+qpost*cos(rot2);

    if (corr==true){
     if(qpostcmrot> tcutmax[j][hj] && qpostcmrot<(tcutmax[j][hj]+25)){
      l_map_cl[hj][0].cl[j]=l_map_cl[hj][0].cl[j]-1;
      l_map_tusync[hj][0].tusync[j]=l_map_tusync[hj][0].tusync[j]-25.;
     }
     if(qpostcmrot<tcutmin[j][hj] && qpostcmrot>(tcutmin[j][hj]-25)){
      l_map_cl[hj][1].cl[j]=l_map_cl[hj][1].cl[j]-1;
      l_map_tusync[hj][1].tusync[j]=l_map_tusync[hj][1].tusync[j]-25.;
     }
     if(qpostcmrot> (tcutmax[j][hj]+25) && qpostcmrot< (tcutmax[j][hj]+50)){
      l_map_cl[hj][0].cl[j]=l_map_cl[hj][0].cl[j]-2;
      l_map_tusync[hj][0].tusync[j]=l_map_tusync[hj][0].tusync[j]-50.;
     } 
     if(qpostcmrot<(tcutmin[j][hj]-25) && qpostcmrot> (tcutmin[j][hj]-50) ){
      l_map_cl[hj][1].cl[j]=l_map_cl[hj][1].cl[j]-2;
      l_map_tusync[hj][1].tusync[j]=l_map_tusync[hj][1].tusync[j]-50.;
     }
    }
    landevt->qdc1[num]=l_map_QDC[hj][0].QDC[j];
    landevt->qdc2[num]=l_map_QDC[hj][1].QDC[j];
    landevt->tac1[num]=l_map_tim[hj][0].tim[j];
    landevt->tac2[num]=l_map_tim[hj][1].tim[j];
    landevt->t171[num]=l_map_tim[hj][0].tim17[j];
    landevt->t172[num]=l_map_tim[hj][1].tim17[j];
    landevt->tcl1[num]=l_map_cl[hj][0].cl[j];
    landevt->tcl2[num]=l_map_cl[hj][1].cl[j];
    landevt->hit[num]=l_map_hit[hj].hit[j];
    landevt->tcal1[num]=l_map_tusync[hj][0].tusync[j];
    landevt->tcal2[num]=l_map_tusync[hj][1].tusync[j];
    num++;
   }
  }


 //reinitialise the variables here?
 } //for(int hj=0; hj<20; hj++) ..
 landevt->rawmulti=num;

 //need to put all the l_map_tusync vals into the trootevent as well as the l_map_ut (t1-t2) 
 //then try plotting as a histo
 return nt;
}


int TLANDEvent::TLANDTSync() 
{
 const char *name[] = {"landsynctac1int.txt", "landsynctac2int.txt", 
                       "landsynct171int.txt", "landsynct172int.txt",
                       "landsynctac1grad.txt","landsynctac2grad.txt",
                       "landsynct171grad.txt","landsynct172grad.txt"}; 

// string flandmapping = flandcaldir + "LANDMAPPING.txt";

 FILE *infile[8];

 //reset all to 0
 memset(lcalC1, 0, sizeof(lcalC1));
 memset(lcalC2, 0, sizeof(lcalC2));
 memset(lcalC5, 0, sizeof(lcalC5));
 memset(lcalC6, 0, sizeof(lcalC6));
 memset(lcalM1, 0, sizeof(lcalM1));
 memset(lcalM2, 0, sizeof(lcalM2));
 memset(lcalM5, 0, sizeof(lcalM5));
 memset(lcalM6, 0, sizeof(lcalM6));

 int ok = 8;
 for(int i=0; i<8; i++) {
  string flandcalfile = flandcaldir + name[i];
  cout << flandcalfile.c_str() << endl;
//  getchar();
//  infile[i] = fopen(name[i], "r"); 
  infile[i] = fopen(flandcalfile.c_str(), "r"); 

  if(infile[i]==0) {
   cout << "TLANDTSync>> Could not get calib file:" <<name<< endl;
   ok--;
  }
 }

 //all files are ok
 if(ok == 8) { 
  for(int iw=0;iw<200; iw++) { 
 
   float mk1=0;
   float mk2=0;
   float mk3=0;
   float mk4=0;
   float mk5=0;
   float mk6=0;
   float mk7=0;
   float mk8=0;
 
   float mj1=0;
   float mj2=0;
   float mj3=0;
   float mj4=0;
   float mj5=0;
   float mj6=0;
   float mj7=0;
   float mj8=0;

   fscanf(infile[0], "%f.%f\n",&mk1,&mj1); //tac
   fscanf(infile[1], "%f.%f\n",&mk2,&mj2); //tac
   fscanf(infile[2], "%f.%f\n",&mk3,&mj3); //t17
   fscanf(infile[3], "%f.%f\n",&mk4,&mj4); //t17
   fscanf(infile[4], "%f.%f\n",&mk5,&mj5); //tac
   fscanf(infile[5], "%f.%f\n",&mk6,&mj6); //tac
   fscanf(infile[6], "%f.%f\n",&mk7,&mj7); //t17
   fscanf(infile[7], "%f.%f\n",&mk8,&mj8); //t17

   lcalC1[iw]=(mk1+(mj1*0.00001));
   lcalC2[iw]=(mk2+(mj2*0.00001));
   lcalC5[iw]=(mk3+(mj3*0.00001));
   lcalC6[iw]=(mk4+(mj4*0.00001));
   lcalM1[iw]=(mk5+(mj5*0.0000001));
   lcalM2[iw]=(mk6+(mj6*0.0000001));
   lcalM5[iw]=(mk7+(mj7*0.0000001));
   lcalM6[iw]=(mk8+(mj8*0.0000001));

/*   cout << "lcalC1[iw]:" <<lcalC1[iw] << endl;
   cout << "lcalC2[iw]:" <<lcalC2[iw] << endl;
   cout << "lcalC5[iw]:" <<lcalC5[iw] << endl;
   cout << "lcalC6[iw]:" <<lcalC6[iw] << endl;
   cout << "lcalM1[iw]:" <<lcalM1[iw] << endl;
   cout << "lcalM2[iw]:" <<lcalM2[iw] << endl;
   cout << "lcalM5[iw]:" <<lcalM5[iw] << endl;
   cout << "lcalM6[iw]:" <<lcalM6[iw] << endl;*/
  }
 }
 for(int i=0; i<8; i++) {
  if(infile[i] !=0)fclose(infile[i]);
 }
 return 0;
}



void TLANDEvent::TLANDReadPar() 
{
  double g1,o1,g2,o2,qd,ql,td,tl;
  int nplane,pad;
  int tmin,tmax;

  for(int iw=0;iw<9; iw++) { 
   for(int jw=0;jw<9; jw++) {
    gain1[iw][jw]=1;
    gain2[iw][jw]=1;
    off1[iw][jw]=0;
    off2[iw][jw]=0;
    qdiff[iw][jw]=1;
    qlarg[iw][jw]=0;
    tdiff[iw][jw]=0;
    tlarg[iw][jw]=0;
  }
 }
 FILE *infile1;
 FILE *infile2;
 FILE *infile3;
 string flandcorrfile1 = flandcaldir+"fout_all.dat";
 string flandcorrfile2 = flandcaldir+"fouttime_all.dat";
 string flandcorrfile3 = flandcaldir+"tcut_all.dat";
 cout << flandcorrfile1.c_str() << endl;
 cout << flandcorrfile2.c_str() << endl;
 cout << flandcorrfile3.c_str() << endl;
 infile1 = fopen(flandcorrfile1.c_str(), "r"); 
  if(infile1==0) {
   cout << "TLANDReadPar>> Could not get par file:" <<flandcorrfile1.c_str()<< endl;
 }
 infile2 = fopen(flandcorrfile2.c_str(), "r"); 
  if(infile2==0) {
   cout << "TLANDReadPar>> Could not get par file:" <<flandcorrfile2.c_str()<< endl;
 }
 infile3 = fopen(flandcorrfile3.c_str(), "r"); 
  if(infile3==0) {
   cout << "TLANDReadPar>> Could not get par file:" <<flandcorrfile3.c_str()<< endl;
 }


  for(int iw=0;iw<200; iw++) { 
   fscanf(infile1, "%d %d %lf %lf %lf %lf %lf %lf\n",&nplane,&pad,&g1,&o1,&g2,&o2,&qd,&ql); //tac  cout << iw << endl;
   gain1[nplane][pad]=g1;
   off1[nplane][pad]=o1;
   gain2[nplane][pad]=g2;
   off2[nplane][pad]=o2;
   qdiff[nplane][pad]=qd;
   qlarg[nplane][pad]=ql;
   printf("%d %d %lf %lf %lf %lf %lf %lf\n",nplane,pad,gain1[nplane][pad],off1[nplane][pad],gain2[nplane][pad],off2[nplane][pad],qdiff[nplane][pad],qlarg[nplane][pad]); 
   fscanf(infile2, "%d %d %lf %lf\n",&nplane,&pad,&td,&tl);
   tdiff[nplane][pad]=td;
   tlarg[nplane][pad]=tl;
   printf("%d %d %lf %lf\n",nplane,pad,tdiff[nplane][pad],tlarg[nplane][pad]);
   fscanf(infile3, "%d %d %d %d\n",&nplane,&pad,&tmin,&tmax);
   tcutmin[nplane][pad]=tmin;
   tcutmax[nplane][pad]=tmax;
   printf("%d %d %lf %lf\n",nplane,pad,tcutmin[nplane][pad],tcutmax[nplane][pad]);
   if(iw%20==19){
    cout << "press return to continue! "<< iw<<"/200"<<endl;
    getchar();
   }
  }
  fclose(infile1);
  fclose(infile2);
  fclose(infile3);
}
