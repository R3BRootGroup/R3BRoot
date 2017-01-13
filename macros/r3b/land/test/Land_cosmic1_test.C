#define nPlanes 12
#define minIonizing 1.15 * 5 // MeV/(5 cm)
#define cLight 29.9792458 * 5
#define lAtt 0.01
#define QDCsplitting 0.45

#define _x_ (entryPoint.X())
#define _y_ (entryPoint.Y())
#define _z_ (entryPoint.Z())
#define isHorizontal(x) ((x/50)%2 == 0)

using namespace std;

struct Filler : FairTask{

	TClonesArray* data = new TClonesArray("R3BNeulandCalData", 100);
	Double_t tOffset[nPlanes][50]; 		// ns
	Double_t vEff[nPlanes][50];		// cm/ns
	Double_t tSync[nPlanes][50];		// ns
	Double_t eGain[nPlanes][50][2];		// QDC/MEV
	TRandom1 r = TRandom1(0);
	Int_t i;

	TVector3 getDirection(){return TVector3(0, -1, 0.8 + r.Rndm()*0.4); }
	
	virtual InitStatus Init(){
		FairRootManager::Instance()->Register ("NeulandCalData", "NeuLAND", data, kTRUE);
		
		for(Int_t i = 0; i < nPlanes; i++){
		 for(Int_t j = 0; j < 50; j++){
		    tOffset[i][j] = r.Rndm()*200 - 100;
		    vEff[i][j] = (16 + r.Rndm()*0.5 - 0.25);
		    tSync[i][j] = r.Rndm()*200 - 100;
		    eGain[i][j][0] =  r.Rndm()*50 + 175;
		    eGain[i][j][1] =  r.Rndm()*50 + 175;
		 }
		}
		
		return kSUCCESS;
	}

	virtual void  Exec(Option_t *option){
		data->Clear();
		i = 0;

		Double_t scale = 0;
		Double_t scale_x = 0;
		Double_t scale_y = 0;
		Double_t scale_z = 0;
		
		Double_t pos, time = r.Rndm()*5;

		Int_t currentID;
		Int_t nextID;
		TVector3 from = TVector3(0,1,0);
		TVector3 entryPoint, exitPoint, direction;

		if(r.Rndm() < 0.6){ // Top
		  entryPoint = TVector3(r.Rndm()*50, 50, r.Rndm()*nPlanes);
		  
		  currentID = ((Int_t)_z_) * 50;
		  if(((Int_t)_z_)%2 == 0) //horizontal
		    currentID += 49;
		  else //vertical
		    currentID += (Int_t)_x_;
		  
		  direction = getDirection();
		  direction.RotateY((r.Rndm()*0.6*TMath::Pi() - 0.3*TMath::Pi() +(r.Rndm() < 0.5 ? 0 : TMath::Pi())));
		  
		  from = TVector3(0, 1, 0);
		}
		else{ //sides
		  Double_t temp = r.Rndm();
		  if(temp < 0.25){ //Front i.e. z = 0
		    entryPoint = TVector3(r.Rndm()*50, 50-r.Rndm()*40, 0);

		    currentID = (Int_t)_y_;
		    
		    direction  = getDirection();
		    direction.RotateY(r.Rndm()*TMath::Pi()*0.8 - TMath::Pi()*0.4);
		    
		    from = TVector3(0, 0, -1);
		  }
		  else if(temp < 0.5){ //Back z = nPlanes
		    entryPoint = TVector3(r.Rndm()*50, 50-r.Rndm()*40, nPlanes);

		    currentID = (nPlanes-1)*50 + (Int_t)_x_;
		    
		    direction  = getDirection();
		    direction.RotateY(r.Rndm()*TMath::Pi()*0.8 + TMath::Pi()*0.6);
		    
		    from = TVector3(0, 0, -1);
		  }
		  else if(temp < 0.75){ //R-Side i.e. x = 0
		    entryPoint = TVector3(0, 50-r.Rndm()*40, r.Rndm()*nPlanes);

		    currentID = ((Int_t)_z_) * 50;
		    if(((Int_t)_z_)%2 == 0) //horizontal
		      currentID += (Int_t)_y_;
		    else //vertical
		      currentID += 0;
		    
		    direction  = getDirection();
		    direction.RotateY(r.Rndm()*TMath::Pi()*0.8 + TMath::Pi()*0.1);
		    
		    from = TVector3(-1, 0, 0);
		  }
		  else{ //L-Side i.e. x = 50
		    entryPoint = TVector3(50, 50-r.Rndm()*40, r.Rndm()*nPlanes);

		    currentID = ((Int_t)_z_) * 50;
		    if(((Int_t)_z_)%2 == 0) //horizontal
		      currentID += (Int_t)_y_;
		    else //vertical
		      currentID += 49;
		    
		    direction  = getDirection();
		    direction.RotateY(-r.Rndm()*TMath::Pi()*0.8 - TMath::Pi()*0.1);
		    
		    from = TVector3(1, 0, 0);
		  }
		}

		while(kTRUE){
		  if(isHorizontal(currentID)){
		    if(from.Y() != 0)
		      scale_y = -1/direction.Y();
		    else
		      scale_y = (((Int_t)_y_) - _y_)/direction.Y();
		    
		    if(from.Z() != 0)
		      scale_z = fabs(1/direction.Z());
		    else if(direction.Z() > 0)
		      scale_z = (((Int_t)_z_ + 1) - _z_)/direction.Z();
		    else
		      scale_z = (((Int_t)_z_) - _z_)/direction.Z();
		    
		    if(scale_y < scale_z){
		      scale = scale_y;
		      exitPoint = entryPoint + scale * direction;
		      nextID = currentID - 1;
		      from = TVector3(0, 1, 0);
		    }
		    else{
		      scale = scale_z;
		      exitPoint = entryPoint + scale * direction;
		      if(from.Z() != 0)
			nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1.0001 : -1.0001))) * 50 + ((Int_t)exitPoint.X());
		      else
			nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1 : -1))) * 50 + ((Int_t)exitPoint.X());
		      from = TVector3(0, 0, (direction.Z() > 0 ? -1 : 1));
		    }
		  }
		  else{
		    if(from.X() != 0)
		      scale_x = fabs(1/direction.X());
		    else if(direction.X() > 0){
		      scale_x = (((Int_t)_x_ + 1) - _x_)/direction.X();
		    }
		    else
		      scale_x = (((Int_t)_x_) - _x_)/direction.X();
		    
		    if(from.Z() != 0)
		      scale_z = fabs(1/direction.Z());
		    else if(direction.Z() > 0){
		      scale_z = (((Int_t)_z_ + 1) - _z_)/direction.Z();
		    }
		    else
		      scale_z = (((Int_t)_z_) - _z_)/direction.Z();
		    
		    if(scale_x < scale_z){
		      scale = scale_x;
		      exitPoint = entryPoint + scale * direction;
		      nextID = currentID + (direction.X() > 0 ? 1 : -1);
		      from = TVector3((direction.X() > 0 ? -1 : 1), 0, 0);
		    }
		    else{
		      scale = scale_z;
		      exitPoint = entryPoint + scale * direction;
		      if(from.Z() != 0)
			nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1.0001 : -1.0001))) * 50 + ((Int_t)exitPoint.Y());
		      else
			nextID = ((Int_t)(_z_ + (direction.Z() > 0 ? 1 : -1))) * 50 + ((Int_t)exitPoint.Y());
		      from = TVector3(0, 0, (direction.Z() > 0 ? -1 : 1));
		    }
		  }
		  
		  if(scale == 0 || currentID < 0 || currentID >= nPlanes*50)
		    return;
		  
		  LOG(DEBUG) << "ID " << currentID << "   EP:(" <<  entryPoint.X() << ", " << entryPoint.Y() << ", " << entryPoint.Z() << ")" << FairLogger::endl;
		  
		  TVector3 centralPoint = (entryPoint + 0.5 * scale * direction);
		  if(isHorizontal(currentID)){
		    pos = centralPoint.X();
		  }
		  else{
		    pos = centralPoint.Y();
		  }
		  
		  Int_t barID = currentID % 50, plane = currentID / 50;
		  
		  Double_t t = time + tSync[plane][barID] + tOffset[plane][barID]/2 + (0.5 * scale * direction).Mag() / cLight + (50-pos)*5/ (vEff[plane][barID]);
		  Int_t qdc = QDCsplitting * (scale * direction).Mag() * minIonizing * eGain[plane][barID][0] * TMath::Exp(- lAtt * (50 - pos));
		  WriteData(currentID + 1, 1, t, qdc);

		  t = time + tSync[plane][barID] - tOffset[plane][barID]/2 + (0.5 * scale * direction).Mag() / cLight + pos*5/ (vEff[plane][barID]);
		  qdc = QDCsplitting * (scale * direction).Mag() * minIonizing * eGain[plane][barID][1] * TMath::Exp(- lAtt * pos);
		  WriteData(currentID + 1, 2, t, qdc);
		  
		  time += (scale * direction).Mag() / cLight;
		  
		  entryPoint = exitPoint;
		  currentID = nextID; 
		  
		  if(_x_ <= 0 || _x_ >= 50 || _y_ <= 0 || _z_ <= 0 || _z_ >= nPlanes)
		    return;
		    

		}
	}

	void WriteData(Int_t barID, Int_t side, Double_t time, Int_t qdc){
		new ((*data)[i++]) R3BNeulandCalData(barID, side, time, qdc);
	}
};

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
    Filler* cosmics = new Filler();
    run->AddTask(cosmics);
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    R3BNeulandCal2HitPar* cosmic1 = new R3BNeulandCal2HitPar();
    cosmic1->SetPlanes(nPlanes);
    run->AddTask(cosmic1);
    // ---------------------------------------------------------------------------
    
    // Initialize ----------------------------------------------------------------
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    run->Init();
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(0, 3000000);
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

      if(arbSync1 - arbSync2 < tSync1 - tSync2 - 0.1 && arbSync1 - arbSync2 > tSync1 - tSync2 + 0.1)
	failed = kTRUE;
      else
	conOffset = arbSync1 - tSync1;
    }
    
    if(par->GetNumModulePar() < nPlanes ){ // alle müssen kalibriert sein
      cout  << "Not enough PMTs calibrated!" << endl;
      failed = kTRUE;
    }  
    else{
      const Double_t t_tol = 0.1;
      const Double_t v_tol = 0.005;
      const Double_t e_tol = 0.01;
      
      TH1D* h_toff = new TH1D("toff", "toff", 100,-1, 1);
      TH1D* h_veff = new TH1D("veff", "veff", 100, 0.9, 1.1);
      TH1D* h_egain = new TH1D("egain", "egain", 100, 0.9, 1.1);
      
      for(Int_t i = 0; i < par->GetNumModulePar(); i++){
	R3BNeulandHitModulePar* mpar = par->GetModuleParAt(i);
	Int_t id = mpar->GetModuleId();
	Int_t plane = (id-1)/50, pdl = (id-1)%50, side = mpar->GetSide()-1;
	
	Double_t tOff = conOffset - (side == 0 ? 1 : -1) * cosmics->tOffset[plane][pdl]/2 - cosmics->tSync[plane][pdl];
	Double_t eGain = 1/(QDCsplitting * cosmics->eGain[plane][pdl][side] * TMath::Exp(-lAtt*25));
	
	h_toff->Fill(mpar->GetTimeOffset()-tOff);
	h_veff->Fill(2*mpar->GetEffectiveSpeed()/cosmics->vEff[plane][pdl]);
	h_egain->Fill(mpar->GetEnergieGain()/eGain);
      }
      
      if(h_toff->GetStdDev() > t_tol)
      {
	cout << "TSync failed!" << endl;
	failed = kTRUE;
      }
      
      if(h_veff->GetMean()-h_veff->GetStdDev() < 1-t_tol || h_veff->GetMean()+h_veff->GetStdDev() > 1+t_tol)
      {
	cout << "vEff failed!" << endl;
	failed = kTRUE;
      }
      
      if(h_egain->GetMean()-h_egain->GetStdDev() < 1-t_tol || h_egain->GetMean()+h_egain->GetStdDev() > 1+t_tol)
      {
	cout << "EGain failed!" << endl;
	failed = kTRUE;
      }
      
      /*
      TCanvas* canvas = new TCanvas("c","Deviations",1200,400);
      canvas->Divide(3,1,0,0);
      
      canvas->cd(1);
      h_toff->Draw();
      
      canvas->cd(2);
      h_veff->Draw();
      
      canvas->cd(3);
      h_egain->Draw();
      */
    }
    
    if(failed)
      cout << " Test failed! " << endl;
    else
      cout <<" Test successful! " << endl;
}
