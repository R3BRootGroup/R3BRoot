//  ---------------------------------------------------------------------------
//
//   -- General Macro for R3B AMS Analysis results
//      Author: Jose Luis Rodriguez Sanchez <joseluis.rodriguez.sanchez@usc.es>
//      Last Update: 07/06/18
//      Comments:
//	         Hit level of AMS detectors, this means, positions in mm with 
//               respect to the first strip of each side (K and S)
//	
//  ---------------------------------------------------------------------------
//
//   Usage: 
//      > root -l results_ams_hit.C
//                         
//     BUT FIRST, select the number of AMS detectors in beam and the input file
//  ---------------------------------------------------------------------------

void results_ams_hit(){

    //DEBUG  (optional)   -----------------------------------------------------
    gDebug = 0;

    //STYLE   -----------------------------------------------------------------		
    //gROOT->SetStyle("Default");
    gStyle->SetOptStat(1);
    gStyle->SetOptFit(0);

    //DEFINE THE INPUT FILE  --------------------------------------------------
    TString filename = "data_0001.root";

    //DEFINE THE NUMBER OF AMS DETECTORS  -------------------------------------
    Int_t   nbdet = 4;//Experiment s444 

    //HISTOGRAMS   ------------------------------------------------------------
    TH2F * hams[160];
    char buf[126], buf2[126];
    for(Int_t i=0;i<nbdet;i++){
    sprintf(buf,"hams_%i",i+1);
    sprintf(buf2,"AMS detector %i",i+1);
    hams[i] = new TH2F(buf,buf2,125,0.,70.,100,0.,40.);
    }

    //OPEN ROOT FILE  ---------------------------------------------------------
    TFile *file1 = TFile::Open(filename);

    //OPEN TREE    ------------------------------------------------------------
    TTree* TAms = (TTree*)file1->Get("evt");

    //AMS Hits
    TClonesArray* amsHitCA;  
    R3BAmsHitData** amsHitData;
    amsHitCA = new TClonesArray("R3BAmsHitData",5);
    TBranch *branchamsHit = TAms->GetBranch("AmsHitData");
    branchamsHit->SetAddress(&amsHitCA);

    Int_t amsHitsPerEvent=0;
    Int_t nbdethit=0;

    //TREE ENTRIES  ---------------------------------------------------------------
    Long64_t nevents = TAms->GetEntries();

    //LOOP IN THE EVENTS  ---------------------------------------------------------
    for(Int_t i=0;i<nevents;i++){
     if(i%1500 == 0) printf("Event:%i\n",i);

     amsHitCA->Clear();
     TAms->GetEvent(i);
		
     amsHitsPerEvent = amsHitCA->GetEntries(); 

     if(amsHitsPerEvent>0) {
		amsHitData = new R3BAmsHitData*[amsHitsPerEvent];
		for(Int_t j=0;j<amsHitsPerEvent;j++){
				amsHitData[j] = new R3BAmsHitData;
				amsHitData[j] = (R3BAmsHitData*) amsHitCA->At(j);      
		}
     }

     //ANALYSIS of the RESULTS  ---------------------------------------------------
     for(Int_t h=0;h<amsHitsPerEvent;h++){

      if(gDebug){
      std::cout<<"Detector: "<<amsHitData[h]->GetDetId()<<" X= "<<amsHitData[h]->GetX()<<" Y= "<<amsHitData[h]->GetY()<<std::endl;
      }

      nbdethit = amsHitData[h]->GetDetId();
      if(nbdethit>=nbdet){std::cout << ">> ERROR in the number of detectors" << std::endl; continue;}

      hams[nbdethit]->Fill(amsHitData[h]->GetX(),amsHitData[h]->GetY());
     }

     if(amsHitsPerEvent>0)  delete[] amsHitData;
    }//for nevents


    TCanvas* c1 = new TCanvas("HIT_AMS","AMS Hits",0,0,720,900);

    if(nbdet==1){
     hams[0]->Draw();
    }else{
     if(nbdet==2)c1->Divide(2,1);
     if(nbdet==3 || nbdet==4)c1->Divide(2,2);
     if(nbdet==5 || nbdet==6)c1->Divide(2,3);
     if(nbdet==7 || nbdet==8)c1->Divide(2,4);
     for(Int_t i=0;i<nbdet;i++){
     c1->cd(1+i);
     hams[i]->Draw("col");
     }
    }

}
