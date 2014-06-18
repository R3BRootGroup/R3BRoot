
Int_t lmd_indexer(){


  TStopwatch timer;
  timer.Start();

 
  // Create source with unpackers ----------------------------------------------
  FairLmdSource* source  = new FairLmdSource();
  // adding the LMD file
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2983.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2984.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2985.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2986.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2987.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2988.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2989.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2990.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2991.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2992.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2993.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2994.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2995.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2996.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2997.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2998.lmd");
  source->AddFile("/Volumes/Data2/land/s406/lmd/r258_2999.lmd");

  // Add Unpacker to scan lmd files
  R3BDBLmdUnpack *lmd_unpacker = (new R3BDBLmdUnpack());
  source->AddUnpacker(lmd_unpacker);
 
  // Create online run ---------------------------------------------------------
  FairRunOnline* run = new FairRunOnline(source);
 
  // DB Setup 
  FairRunIdGenerator runID;
  UInt_t runId =  runID.generateId();
  Double_t s= (Double_t) runId;
  ValTimeStamp t1(s);
     
  cout << " RUNID generated ---> " << runId << " date:" << t1.AsString("s") <<  endl;
  run->SetRunId(runId); 
  run->SetOutputFile("output.root");
  //run->SetGenerateHtml(kFALSE);
  
  // ---------------------------------------------------------------------------
  
  
  // Create ALADIN field map ---------------------------------------------------
  R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
  Double_t fMeasCurrent = 2500.;// I_current [A]
  magField->SetCurrent(fMeasCurrent);
  magField->SetScale(1.);
  run->SetField(magField);
  
  // Runtime data base ---------------------------------------------------------
  FairRuntimeDb* rtdb = run->GetRuntimeDb();
  R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();

  // Set the SQL based IO as first input
  FairParTSQLIo* input_db = new FairParTSQLIo();  
  input_db->SetShutdown(kTRUE);
  input_db->open();  
  rtdb->setFirstInput(input_db);
  rtdb->setOutput(input_db);
  

   Bool_t kParameterMerged = kTRUE;
   FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
   parOut->open("parameter.root");
   rtdb->setSecondInput(parOut);
   rtdb->saveOutput();
   rtdb->print(); 
  // ---------------------------------------------------------------------------
  
 
 // Initialize ----------------------------------------------------------------
  run->Init();
  FairLogger::GetLogger()->SetLogScreenLevel("INFO");
  // ---------------------------------------------------------------------------

 
  // Run and Analyse LMD Files
  run->Run(-1);

  // Header report
  Int_t n_header = lmd_unpacker->GetNHeaderInfo();

  // Times report
  Int_t n_starts = lmd_unpacker->GetNStartTimes();
  Int_t n_stops = lmd_unpacker->GetNStopTimes();

 
  cout << " LMD_INDEXER: n_start, n_stop , n_header" << n_starts << " : " << n_stops << " : " << n_header <<endl;
 
  for (Int_t i=0; i<n_header;i++){     
	// Get File Start Time 
	ValTimeStamp* t_start = lmd_unpacker->GetStartTimesAt(i);
    cout << " LMD_INDEXER: START T: " << i << " : " <<  t_start->AsString("s") << endl;
	
    // Get File Stop Time
    ValTimeStamp* t_end = lmd_unpacker->GetStopTimesAt(i);
    cout << " LMD_INDEXER STOP T: " << i << " : " <<  t_end->AsString("s") << endl;
	
    // Get File Header Info
	LmdHeaderInfo* info= lmd_unpacker->GetHeaderInfoAt(i);
    if (info) cout << info->file << endl;

	R3BDBLmdFileInfo fLmdInfo;
	fLmdInfo.SetExpLabel("s406"); 
	fLmdInfo.SetExpPhase("PRE");
    fLmdInfo.SetRunType("DATA");
	fLmdInfo.SetFileId(t_start->GetSec());
		
	fLmdInfo.SetStartTime(*t_start);   
	fLmdInfo.SetEndTime(*t_end);
	// Servers Time Offset -> if synchronized Offset=0s    
	fLmdInfo.SetOffsetTime(0.);   
	
	fLmdInfo.SetType(info->type);   
	fLmdInfo.SetSubType(info->subtype);
   	fLmdInfo.SetLength(info->length);   
	fLmdInfo.SetFragmentation(info->frag);   
	fLmdInfo.SetFileLabel(info->label);   
	fLmdInfo.SetFileName(info->file);   
	fLmdInfo.SetFileComment(info->comment);   

	cout << " Commit Header Info# " << i  << endl;
	if (!fLmdInfo.Commit()) cout << "-E- LMD_INDEXER Error Writing Run Info " << endl;	
  } 
  

  // Define a Start Time ( taken from LMD Buffer Header)  
	ValTimeStamp* t_start = lmd_unpacker->GetStartTimesAt(1);

  // Read it back in 
  // make a extended query to get ConfigFileText
   ValTimeStamp tsStart(1970, 1, 1, 0, 0, 0);
   ValTimeStamp   tsEnd(2038, 1,18,19,14, 7);

   FairDbExtSqlContent extContextConfig(FairDbExtSqlContent::kStarts,tsStart,tsEnd,
                						Detector::kLand,   // any
									    DataType::kData);    // any
	 
                            
   string seqSelectConfig = Form("SEQNO=%d",t_start->GetSec());
   string tbl = fLmdInfo.GetTableName();
  
   FairDbReader<R3BDBLmdFileInfo> 
     rsConfig(tbl.c_str(),extContextConfig,FairDb::kDefaultVersion,
              seqSelectConfig.c_str());

   Int_t nFiles = rsConfig.GetNumRows();
   cout << "Found nFiles: = " << nFiles << endl;
   const R3BDBLmdFileInfo* rpInfo = rsConfig.GetRow(0); 
   
   if (rpInfo) cout << " file_id:run # " << rpInfo->GetFileId() << " has time: " << rpInfo->GetStartTime().AsString("s") << endl;


  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime<< "s" << endl << endl;

  return 0;
}

