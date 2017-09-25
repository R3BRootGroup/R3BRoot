//
// Procedure example:
//
//  root -l
//  .L OrderTreeInspect.cpp
//  OrderTreeInspect("../../../../../startrack_0001.root")
//
//


#include "Riostream.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBrowser.h"
#include "TH1.h"
#include "TH2.h"
#include "TRandom.h"

void OrderTreeInspect(const char* input)
{

 gStyle->SetPalette(1);

  gROOT->SetStyle("Default");
  gStyle->SetOptTitle(1);
  gStyle->SetOptFit(0);

  //TFile *f = new TFile("startrack_raw_data_order_12Crun.root");
  TFile *f = TFile::Open(input);
  cout << "List of root file content:" << endl;
   f->ls();
   cout << " " << endl;


  TTree *t1 =(TTree*)f->Get("evt");
  cout << "List of branches:" << endl;
  t1->GetListOfBranches()->Print();
  cout << " " << endl;
  cout << "Please be patient ...Plots are being produced ..." << endl;
  cout << "Then, you'll be to enter the time stamp range to produce additional histograms" << endl;
  cout << " " << endl;

  //    TH1F *h1 = new TH1F();


  //new TBrowser();

  // Drawing tree 2D spectrum 
   
  c1 = new TCanvas("c1","S438b",200,10,900,700);
  c1->SetFillColor(0);
  c1->SetFrameFillColor(0);
  //c1->Divide(1,2);

  c1->cd(1);
  t1->Draw("StartrackCalData.fTS/100:StartrackCalData.fNHits", "","" );  // n hit means n strip has a signal over threshold 
  TGraph *gr= new TGraph(t1->GetSelectedRows(), t1->GetV2(), t1->GetV1());
  gr->Draw("ap");

  //c1->cd(2);
  //t1->Draw("StartrackCalData.fTS/100:StartrackCalData.fNHits", "","" );  // n hit means n strip has a signal over threshold 
  //t1->Draw("StartrackCalData.fTS/100:StartrackCalData.fNHits >> h1(2000, 0, 20000, 50000,,)", "","" );  // n hit means n strip has a signal over threshold 
  //h1->GetXaxis()->SetTitle("hit number");
  //h1->GetYaxis()->SetTitle("x 10^2 ns");
  //h1->Draw();

   /* c1->cd(2);
   t1->Draw("StarTrackOrderTS.tm_stp/100:StarTrackOrderTS.AsicId >> h2(20, 0, 20, 50000,,)", "","" );
    h2->GetXaxis()->SetTitle("Asic Id");
    h2->GetYaxis()->SetTitle("x 10^2 ns");
    h2->Draw("colz");
  c1->cd(3);
   t1->Draw("StarTrackOrderTS.tm_stp/100:StarTrackOrderTS.ADCdata >> h3(4000, 0, 4000, 50000,,)", "","" );
   h3->GetXaxis()->SetTitle("Asic Id");
   h3->GetYaxis()->SetTitle("x 10^2 ns");
   h3->Draw();
  c1->cd(4);
   t1->Draw("StarTrackOrderTS.tm_stp_ext/100:StarTrackOrderTS.ADCdata >> h4(4000, 0, 4000, 50000,,)", "","" );
   h4->GetXaxis()->SetTitle("Energy [u.a.]");
   h4->GetYaxis()->SetTitle("x 10^2 ns");
   h4->Draw();
  
 
   // Plotting existing histogram
   c2 = new TCanvas("c2","S438b",200,10,900,700);
   c2->SetFillColor(0);
   c2->SetFrameFillColor(0);
   c2->cd();
   TS_event->Draw(); // here "event" means a block
    
    
 

   // Reading the tree, creating and filling new histograms

     TBranch *branch = 0;
     branch = t1->GetBranch("StarTrackOrderTS");
     TLeaf *tm_stp = branch->GetLeaf("tm_stp");
     TLeaf *tm_stp_ext = branch->GetLeaf("tm_stp_ext");

     //Create Histograms

     Double_t TsLow, TsUp;
     Int_t Nbins;

     // Some interactive inputs from thwe user:
     cout << "Please, enter the lower limit of timestamp (in sec)  (or Enter for default) ):" << endl;
     if(cin.peek()=='\n'){
       TsLow= 1266e3;
       TsUp= 1268e3;
       cout << "current default for lower limit is " << TsLow  << endl;       
       cout << "current default for upper limit is " << TsUp  << endl;       
     }else // default
       {  cin >> TsLow; 
	 cout << "Enter the upper limit of timestamp (in sec):" << endl;
	 cin >> TsUp; 
	 if(TsUp<=TsLow)
	   { 
	     cout << "error upper limit smaller than lower limit" << endl;
	     break;
	   }
       }

     Nbins= int((TsUp-TsLow));

     TH1F *TSb   = new TH1F("TSb","Full time stamp Si tracker (in sec)",Nbins,TsLow,TsUp);
     TH1F *TSextb   = new TH1F("TSextb","Full time stamp Ext Signal (in sec)",Nbins,TsLow,TsUp);
     TH1F *TS_TSext_diffb   = new TH1F("TS_TSext_diffb","Time stamp difference between Si and Ext Signal",500,-2.,2.);
     TH2F *TS_TSextb   = new TH2F("TS_TSextb","Time stamp in Si vs Ext Signal", Nbins,TsLow,TsUp, Nbins,TsLow,TsUp);

   // Fill histograms
   Long64_t nentries = t1->GetEntries();

   Double_t diff=1.;

   cout << "nentries=" << nentries <<  endl; 

   for(Long64_t i=0; i<nentries; i++){

     branch->GetEntry(i);

     //cout << "tm_stp= " << tm_stp->GetValue()  << " i=" << i <<  endl; 

     diff= (tm_stp->GetValue()/1.e9 - tm_stp_ext->GetValue()/1.e9); // in sec

     TSb->Fill(tm_stp->GetValue()/1.e9);
     TSextb->Fill(tm_stp_ext->GetValue()/1.e9);
     TS_TSext_diffb->Fill(diff);
     TS_TSextb->Fill(tm_stp_ext->GetValue()/1.e9 , tm_stp->GetValue()/1.e9);

   }

    cout << "looping finished ... now please wait while the histograms are plotted." <<  endl; 

   // Add Stat information to the hitorgrams:
  gStyle->SetOptStat(01001101);


  c3 = new TCanvas("c4","S438b",200,10,900,700);
  c3->SetFillColor(0);
  c3->SetFrameFillColor(0);
  c3->Divide(2,2);

  c3->cd(1);
   TSb->Draw();
  c3->cd(2);
   TSextb->Draw();
  c3->cd(3);
   TS_TSext_diff->Draw();
   TS_TSext_diffb->SetLineStyle(2);
   TS_TSext_diffb->SetLineColor(3);
   TS_TSext_diffb->Draw("same");
  c3->cd(4);
   TS_TSextb->Draw();

  //TS_TSext_diffb->Draw();
  //TS_TSext_diff->Draw("same");
  
  */
}
