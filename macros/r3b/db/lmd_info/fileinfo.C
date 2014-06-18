Int_t  fileinfo()
{

  //gDebug = 5;

  // Generate a unique RunID
  FairRunIdGenerator runID;
  UInt_t runId =  runID.generateId();

  // Create the Runtime Database ( parameter manager class )
  FairRuntimeDb* db = FairRuntimeDb::instance();

  // Set the SQL based IO as second input
  FairParTSQLIo* input_db = new FairParTSQLIo();  
  input_db->SetVerbosity(1);

  // Set Global SeqNo ( Replication Global Index Base )
  //inp2->SetGlobalSeqNoIn();

  // Shutdown Mode ( True, False )
  input_db->SetShutdown(kTRUE);


  // Open first input
  input_db->open();
  db->setFirstInput(input_db);

  // Set the output=input
  db->setOutput(input_db); 

  
  // Write a Run
  R3BDBLmdFileInfo fLmdInfo;
  fLmdInfo.SetExpLabel("s406");
  fLmdInfo.SetFileId(runId);
  ValTimeStamp start(runId);
  fLmdInfo.SetStartTime(start);
  
  cout << " Commit " << endl;
  if (!fLmdInfo.Commit()) cout << "-E- Error Writing Run Info " << endl;


  // Read it back in 
  // make a extended query to get ConfigFileText
   ValTimeStamp tsStart(1970, 1, 1, 0, 0, 0);
   ValTimeStamp   tsEnd(2038, 1,18,19,14, 7);

   FairDbExtSqlContent extContextConfig(FairDbExtSqlContent::kStarts,tsStart,tsEnd,
									 Detector::kLand,   // any
									 DataType::kData);    // any
	 
                            
   string seqSelectConfig = Form("SEQNO=%d",fLmdInfo.GetFileId());
   string tbl = fLmdInfo.GetTableName();
  
   FairDbReader<R3BDBLmdFileInfo> 
     rsConfig(tbl.c_str(),extContextConfig,FairDb::kDefaultVersion,
              seqSelectConfig.c_str());

   Int_t nConfig = rsConfig.GetNumRows();
   cout << "nConfig: = " << nConfig << endl;
   const R3BDBLmdFileInfo* rpInfo = rsConfig.GetRow(0); 
   
   if (rpInfo) cout << " file_id:run # " << rpInfo->GetFileId() << " has time: " << rpInfo->GetStartTime().AsString("s") << endl;

  // FairRuntimeDB deletion
  if (db) delete db;
  return 0;
}
