Int_t  calgeo_prime()
{

  // Generate a unique RunID
  FairRunIdGenerator runID;
  UInt_t runId =  runID.generateId();

  // Create the Runtime Database ( parameter manager class )
  FairRuntimeDb* db = FairRuntimeDb::instance();
  // Create in memory the relevant container
  R3BCaloGeometryPar* par = (R3BCaloGeometryPar*)(db->getContainer("CaloGeometryPar"));

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

  cout << "-I- CALIFA GEO PRIMING:  Opening database + Meta Data done via TSQLIO .. " << endl;

  R3BGeomAnalyzer g_ana;
  // Import Barrel Only Geo Definition
  TFile *file = new TFile("../../../../geometry/califa_10_v8.11.geo.root");
  g_ana.Import(file);
  g_ana.RecReadMedia();
  g_ana.RecReadShapes();
  g_ana.RecReadNodes();    

  // Init the Parameter Container
  g_ana.FillGeometryPar(par);

  cout << " Analyzer: Media# " <<  g_ana.GetNumMedia() 
       << " MediaPar: Media# " << par->GetNumMedia() << endl; 

  cout << " Analyzer: Unique Crystals# " <<  g_ana.GetNumCrystals() 
       << " CrysGeoPar: Crystal stored# " << par->GetNumCrystalGeom() << endl;
 
  cout << " Analyzer: Unique Alveoli# " <<  g_ana.GetNumAlveoli() 
       << " AlveolusGeoPar: Alveolus stored# " << par->GetNumAlveolusGeom() << endl; 

  cout << " Analyzer: Node Crystals# " <<  g_ana.GetAllCrystals() 
       << " CrysGeoPar: Crystal stored# " << par->GetNumCrystalNodes() << endl; 

  cout << " Analyzer: Node Alveolus# " <<  g_ana.GetAllAlveoli() 
       << " AlveolusPar: Nodes stored# "   << par->GetNumAlveolusNodes() << endl;



  // Write just added container
  db->addRun(runId,-1);
  db->writeContainer(par, db->getCurrentRun());


  // FairRuntimeDB deletion
  if (db) delete db;
  return 0;
}
