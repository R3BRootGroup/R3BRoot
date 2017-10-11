Int_t  califa_test()
{
  // Create a Runtime Database singleton.
  FairRuntimeDb* db = FairRuntimeDb::instance();

   // Create in memory the relevant container
  R3BCalifaCrystalCalPar* par = (R3BCalifaCrystalCalPar*)(db->getContainer("CalifaCrystalCalPar"));  

  // Set the SQL IO as first input
  FairParAsciiFileIo* inp = new FairParAsciiFileIo();

  TString filename ="ducals.par";
  inp->open(filename.Data(),"in");

  db->setFirstInput(inp);

// Generate a unique RunID
  FairRunIdGenerator runID;
  UInt_t runId =  runID.generateId();
  db->initContainers(runId);


  // Get the container after initialisation
  // from the RuntimeDB
  R3BCalifaCrystalCalPar* par = (R3BCalifaCrystalCalPar*)(db->getContainer("CalifaCrystalCalPar"));

  // Dump the Parameters
  cout << endl;
  cout << "-I- Reading Parameter data from Ascii File: \n" << filename.Data() << endl;
  cout << endl;

  par->Print();
  cout << endl;

  // Convert in ROOT format 
  par->setChanged();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open("califa_cal_par.root");
  db->setOutput(parOut);
  db->saveOutput();
  db->print();
  // ------------------------------------------------------------------------

  if (db) delete db;
  return 0;
}
