Int_t  calgeo_read()
{
  // Create a Runtime Database singleton.
  FairRuntimeDb* db = FairRuntimeDb::instance();

  // Set the SQL IO as first input
  FairParTSQLIo* inp = new FairParTSQLIo();
  
  // Verbosity level
  //inp->SetVerbosity(1);

  inp->open();
  db->setFirstInput(inp);

   // Create in memory the relevant container
  R3BCaloGeometryPar* par = (R3BCaloGeometryPar*)(db->getContainer("CaloGeometryPar"));

  // Create a dummy runID using date in UTC from which
  // corresponding  parameters will be initialised

  ValTimeStamp tStamp(2014,03,21,18,00,00);
  UInt_t runId = tStamp.GetSec();
  cout << "-I- looking for parameters at runID# " << runId << endl;
  cout << "-I- corresponding  time in runID (UTC) " << tStamp.Format("iso") << endl;

  // Use the generated RunID to initialised the parameter
  // using the SQL-based IO input
  db->initContainers(runId);


  // Get the container after initialisation
  // from the RuntimeDB
  R3BCaloGeometryPar* par = (R3BCaloGeometryPar*)(db->getContainer("CaloGeometryPar"));


  cout << endl;
  cout << "-I- Reading Parameter data from SQL Database: \n" << endl;
  cout << endl;

  par->Print();

  cout << endl;
 


  if (db) delete db;
  return 0;
}
