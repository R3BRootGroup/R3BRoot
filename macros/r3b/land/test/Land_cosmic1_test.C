#include <vector>

void Land_cosmic1_test(){
    TStopwatch timer;
    timer.Start();
     
    TString outputFileName = "data_cosmic1.root";             	 		// name of output file
  
    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------
    
    // ----- Runtime DataBase info -----------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    R3BNeulandHitFiller* cosmics = new R3BNeulandHitFiller();
    run->AddTask(cosmics);
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    R3BNeulandCal2HitPar* cosmic1 = new R3BNeulandCal2HitPar();
    cosmic1->SetPlanes(cosmics->Planes);
    run->AddTask(cosmic1);
    // ---------------------------------------------------------------------------
    
    // Initialize ----------------------------------------------------------------
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    run->Init();
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(0, 500000);
    rtdb->saveOutput();
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl; 
  
    Bool_t failed = kFALSE;
    Double_t conOffset = 0;
    
    R3BNeulandHitPar* par = (R3BNeulandHitPar*) rtdb->getContainer("NeulandHitPar");  
    {
      Int_t id1 = par->GetModuleParAt(0)->GetModuleId();
      Int_t id2 = par->GetModuleParAt(2)->GetModuleId();
      Int_t plane1 = (id1-1)/50, pdl1 = (id1-1)%50, side1 = par->GetModuleParAt(0)->GetSide();
      Int_t plane2 = (id2-1)/50, pdl2 = (id2-1)%50, side2 = par->GetModuleParAt(2)->GetSide();
      
      Double_t arbOffset = par->GetModuleParAt(0)->GetTimeOffset() - par->GetModuleParAt(1)->GetTimeOffset();
      Double_t tOff1  = -cosmics->tOffset[plane1][pdl1];
      
      Double_t arbSync1 = (par->GetModuleParAt(0)->GetTimeOffset() + par->GetModuleParAt(1)->GetTimeOffset())/2;
      Double_t tSync1  = -cosmics->tSync[plane1][pdl1];
      Double_t arbSync2 = (par->GetModuleParAt(2)->GetTimeOffset() + par->GetModuleParAt(3)->GetTimeOffset())/2;
      Double_t tSync2  = -cosmics->tSync[plane2][pdl2];

      if(arbSync1 - arbSync2 < tSync1 - tSync2 - 1 && arbSync1 - arbSync2 > tSync1 - tSync2 + 1)
	failed = kTRUE;
      else
	conOffset = arbSync1 - tSync1;
    }
    
    if(par->GetNumModulePar() < cosmics->Planes*50*2*0.9 ){ // more than 90% have to be calibrated
      cout  << "Not enough PMTs calibrated!" << endl;
      failed = kTRUE;
    }  
    else{
      const Double_t tcal_tol = 0.1;
      const Double_t tsync_tol = 0.4;
      const Double_t v_tol = 0.005;
      const Double_t e_tol = 0.015;
      
      TH1D* h_toff = new TH1D("toff", "toff", 200,-1, 1);
      TH1D* h_tsync = new TH1D("tsync", "tsync", 400,-2, 2);
      TH1D* h_veff = new TH1D("veff", "veff", 200, 0.9025, 1.1025);
      TH1D* h_egain = new TH1D("egain", "egain", 200, 0.9025, 1.1025);
      
      std::vector<std::vector<std::vector<Double_t>>> t_Off;
      std::vector<std::vector<std::vector<Double_t>>> t_Sync;
      std::vector<std::vector<std::vector<Double_t>>> v_Eff;
      std::vector<std::vector<std::vector<Double_t>>> e_Gain;
      
      for(Int_t pl = 0; pl < cosmics->Planes; pl++){
	std::vector<std::vector<Double_t>> vec_plane;
	for(Int_t pdl = 0; pdl < 50; pdl++){
	  std::vector<Double_t> vec_module;
	  vec_module.push_back(-100000);
	  vec_module.push_back(-100000);
	  vec_plane.push_back(vec_module);  
	}
	t_Off.push_back(vec_plane);
	t_Sync.push_back(vec_plane);
	v_Eff.push_back(vec_plane);
	e_Gain.push_back(vec_plane);
      }
      
      for(Int_t i = 0; i < par->GetNumModulePar(); i++){
	R3BNeulandHitModulePar* mpar = par->GetModuleParAt(i);
	Int_t plane = (mpar->GetModuleId()-1)/50, pdl = (mpar->GetModuleId()-1)%50, side = mpar->GetSide()-1;
	
	t_Sync[plane][pdl][side] = mpar->GetTimeOffset();
	v_Eff[plane][pdl][side] = mpar->GetEffectiveSpeed();
	e_Gain[plane][pdl][side] = mpar->GetEnergieGain();
      }
      
      for(Int_t pl = 0; pl < cosmics->Planes; pl++)
	for(Int_t pdl = 0; pdl < 50 && !failed; pdl++){
	  t_Off[pl][pdl][0] = t_Sync[pl][pdl][1] - t_Sync[pl][pdl][0];
	  t_Off[pl][pdl][1] = t_Sync[pl][pdl][1] - t_Sync[pl][pdl][0];
	  
	  t_Sync[pl][pdl][0] = -(t_Sync[pl][pdl][0] + t_Off[pl][pdl][0]/2);
	  t_Sync[pl][pdl][1] = -(t_Sync[pl][pdl][1] - t_Off[pl][pdl][1]/2);
	  
	  if(fabs(t_Off[pl][pdl][0] - t_Off[pl][pdl][1]) > 0.001  || fabs(t_Sync[pl][pdl][0] - t_Sync[pl][pdl][1]) > 0.001  || fabs(v_Eff[pl][pdl][0] - v_Eff[pl][pdl][1]) > 0.001 ){
	   cout << "Parameters of one module are not matching!" << endl;
	   failed = kTRUE;
	  }
	}
      
      if(!failed){
	for(Int_t pl = 0; pl < cosmics->Planes; pl++){
	  for(Int_t pdl = 0; pdl < 50; pdl++){
	    Double_t eGain1 = (cosmics->QDCsplitting * cosmics->eGain[pl][pdl][0] * TMath::Exp(-cosmics->lAtt*25));
	    Double_t eGain2 = (cosmics->QDCsplitting * cosmics->eGain[pl][pdl][1] * TMath::Exp(-cosmics->lAtt*25));
	    
	    h_toff->Fill(t_Off[pl][pdl][0] - cosmics->tOffset[pl][pdl]);
	    h_tsync->Fill(t_Sync[pl][pdl][0] - cosmics->tSync[pl][pdl] + conOffset);
	    h_veff->Fill(2*v_Eff[pl][pdl][0]/cosmics->vEff[pl][pdl]);
	    h_egain->Fill(e_Gain[pl][pdl][0]*eGain1);
	    h_egain->Fill(e_Gain[pl][pdl][1]*eGain2);
	  }
	}
	
	if(h_toff->GetStdDev() > tcal_tol){
	  cout << "TOff failed!" << endl;
	  failed = kTRUE;
	}
	
	if(h_tsync->GetStdDev() > tsync_tol){
	  cout << "TSync failed!" << endl;
	  failed = kTRUE;
	}
	
	if(h_veff->GetMean()-h_veff->GetStdDev() < 1-v_tol || h_veff->GetMean()+h_veff->GetStdDev() > 1+v_tol){
	  cout << "vEff failed!" << endl;
	  failed = kTRUE;
	}
	
	if(h_egain->GetMean()-h_egain->GetStdDev() < 1-e_tol || h_egain->GetMean()+h_egain->GetStdDev() > 1+e_tol){
	  cout << "EGain failed!" << endl;
	  failed = kTRUE;
	}
	
	TCanvas* canvas = new TCanvas("c","Deviations",1000,1000);
	canvas->Divide(2,2);
	
	canvas->cd(1);
	h_toff->Draw();
	
	canvas->cd(2);
	h_tsync->Draw();
	
	canvas->cd(3);
	h_veff->Draw();
	
	canvas->cd(4);
	h_egain->Draw();
      }
    }
    
    if(failed)
      cout << " Test failed! " << endl;
    else
      cout <<" Test successful! " << endl;
}
