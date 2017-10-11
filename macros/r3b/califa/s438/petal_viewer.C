//
//   Usage: 
//      > root -l 
//      ROOT> .L petal_viewer.C
//      ROOT> petal_viewer("inputFile")
//     
//     where inputFile is RAW level file!!!!
//     
//  -------------------------------------------------------------------------
void petal_viewer(TString inputFile="") {

  gROOT->SetStyle("Default");
  //gStyle->SetOptTitle(0);
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(0);
  
  TFile *file1 = TFile::Open(inputFile);
  
  //HISTOGRAMS DEFINITION

  TH1F *E_p1[128];
  
  int cry=0;
  int fb[16]={6,5,4,3,2,1,0,7,8,15,14,13,12,11,10,9};
  //int pa[16]={15,13,7,5,14,12,6,4,11,9,3,1,10,8,2,0};
  // int pa[16]={0,2,8,10,1,3,9,11,4,6,12,14,5,7,13,15};
  int pa[16]={5,4,1,0,7,6,3,2,13,12,9,8,15,14,11,10};

  for(Int_t i=0;i<128;i++){
    //petal 1
    E_p1[i]= new TH1F(Form("E_p1_cr%d", i),Form("Crystal %d", i),500,0,300000);
    E_p1[i]->GetXaxis()->SetTitle("keV");
  }

  TTree* caloTree = (TTree*)file1->Get("cbmsim");
 
  //Raw Hits (input)
  TClonesArray* rawHitCA;  
  R3BCalifaMappedData** rawHit;
  rawHitCA = new TClonesArray("R3BCalifaMappedData",5);
  TBranch *branchRawHit = caloTree->GetBranch("CalifaMappedData");
  branchRawHit->SetAddress(&rawHitCA);
  
  Long64_t nevents = caloTree->GetEntries();

  Int_t rawHitsPerEvent =0;
    
  //for(Int_t i=0;i<nevents;i++){
  for(Int_t i=0;i<1000;i++){
    if(i%100000 == 0) printf("Event:%i\n",i);
    
    rawHitCA->Clear();
    caloTree->GetEvent(i);
    rawHitsPerEvent = rawHitCA->GetEntries(); 
    //cout<<"mul "<<rawHitsPerEvent<<endl;
    
    //    cout<<"Mul "<<rawHitsPerEvent<<endl;
    //loop in RAW Hits
    if(rawHitsPerEvent>0) {
      for(Int_t h=0;h<rawHitsPerEvent;h++){
	R3BCalifaMappedData *hit = (R3BCalifaMappedData*)rawHitCA->At(h);
	E_p1[hit->GetCrystalId()]->Fill(hit->GetEnergy());
	//	cout<<"E "<<rawHit[h]->GetEnergy()<<endl;
      }
    }    

  }

  char cname[8];
  char savecanv1[50];
  TCanvas *c[8];
  
  for (Int_t ii=0;ii<2;ii++) {
    sprintf(cname,"c%i",ii);
    c[ii] = new TCanvas(cname,cname,0,0,720,900);
    c[ii]->Divide(4,16);
    c[ii]->SetFillColor(0);
    c[ii]->SetFrameFillColor(0);
  }


  Int_t ii=0,jj=0;
 
  for(Int_t kk=0;kk<2;kk++){//Canvas
    c[kk]->cd();
    for (Int_t fact=0; fact<4; fact++){
      for(Int_t w=0;w<16;w++){ //Pad 
	c[kk]->cd(fact*16+w+1);
	gPad->SetLogy();
	gPad->SetFillColor(0);

      // if(w%16==0){
      // 	ii=0;
      // 	jj++;
      // }
	cry=kk*64+16*fact+fb[pa[w]];
	cout<<"W "<<w<<" Cr "<<cry<<endl;
	E_p1[cry]->Draw();
      // ii++;
    }
  }
  }
}	   
