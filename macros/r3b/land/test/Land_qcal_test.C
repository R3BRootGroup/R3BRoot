struct Filler : FairTask{

	TClonesArray* data = new TClonesArray("R3BNeulandMappedData", 100);
	R3BEventHeader* header = new R3BEventHeader();
	Int_t i = 0;

	virtual InitStatus Init(){
		FairRootManager::Instance()->Register ("NeulandMappedData", "NeuLAND", data, kTRUE);
		FairRootManager::Instance()->Register ("R3BEventHeader", "", header, kTRUE);
		return kSUCCESS;
	}

	virtual void  Exec(Option_t *option){
		Int_t value;
		i++;
		for(Int_t j = 0; j < 100; j++){	
			value = j + 100;	
			if(i > 5000)
				value += std::rand()%10 - 5;			
			new ((*data)[j]) R3BNeulandMappedData(0, 0, 0, value, 1, j/2 + 1, j%2 + 1);
		}			
		header = new (header) R3BEventHeader();
		header->SetTrigger(2);
	}

};

void Land_qcal_test(){
    TStopwatch timer;
    timer.Start();
     
    TString outputFileName = "data_qcal.root";             	 		// name of output file
  
    // Create analysis run -------------------------------------------------------
    FairRunAna* run = new FairRunAna();
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------
    
    // ----- Runtime DataBase info -----------------------------------------------
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    run->AddTask(new Filler());
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    R3BNeulandMapped2QCalPar* pedpar = new R3BNeulandMapped2QCalPar();
    pedpar->SetPlanes(1);
    run->AddTask(pedpar);
    // ---------------------------------------------------------------------------
    
    // Initialize ----------------------------------------------------------------
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    run->Init();
    // ---------------------------------------------------------------------------
    
    // Run -----------------------------------------------------------------------
    run->Run(0,10000);
    rtdb->saveOutput();
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl; 
  
    R3BNeulandQCalPar* par = (R3BNeulandQCalPar*) rtdb->getContainer("NeulandQCalPar");
    Bool_t failed = false;
    
    for(Int_t i = 0; i < 100; i++)
	if(par->GetParAt(1, i/2 + 1, i%2 +1) != i + 100){
	  failed = true;    
	  cout << "[" << 1 << ", " << i/50 + 1 << ", " << i%2 + 1 << "] Expected: " << i+100 << " but value is " << par->GetParAt(1, i/2 + 1, i%2 +1) << endl;
	}
    
    if(failed)
      cout << " Test failed! " << endl;
    else
      cout <<" Test successful! " << endl;
}
