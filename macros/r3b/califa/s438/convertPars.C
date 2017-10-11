//  -------------------------------------------------------------------------
//
//  -----  Macro for R3B CALIFA parameters conversion
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         based on the macros/db/calibfa/califa_test.C macro by D. Bertini
//         Last Update: 30/09/14
//         Comments:
//  -------------------------------------------------------------------------
//
//   Usage: 
//      > root -l 
//      ROOT> .L convertPars.C
//      ROOT> convert(inputFile, outputFile) // with ""!
//                             
//     where inputFile is the file name of the ascii parameter file (input)
//           outputFile is the file name of the root parameter file (output)
//       
//  -------------------------------------------------------------------------
#include <unistd.h>

Int_t  convertPars(TString inputfilename="",TString outputfilename="")
{
  // Create a Runtime Database singleton.
  FairRuntimeDb* db = FairRuntimeDb::instance();

   // Create in memory the relevant container
  R3BCalifaCrystalCalPar* par = (R3BCalifaCrystalCalPar*)(db->getContainer("CalifaCrystalCalPar"));  

  // Set the SQL IO as first input
  FairParAsciiFileIo* inp = new FairParAsciiFileIo();

  inp->open(inputfilename.Data(),"in");

  db->setFirstInput(inp);

// Generate a unique RunID
  FairRunIdGenerator runID;
  UInt_t runId = 0;//  runID.generateId();
  db->initContainers(runId);


  // Get the container after initialisation
  // from the RuntimeDB
  R3BCalifaCrystalCalPar* par = (R3BCalifaCrystalCalPar*)(db->getContainer("CalifaCrystalCalPar"));

  // Dump the Parameters
  cout << endl;
  cout << "-I- Reading Parameter data from Ascii File: \n" << inputfilename.Data() << endl;
  cout << endl;

  par->Print();
  cout << endl;

  // Convert in ROOT format 
  par->setChanged();
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  unlink(outputfilename);
  parOut->open(outputfilename.Data());
  db->setOutput(parOut);
  db->print();
  // ------------------------------------------------------------------------

  if (db) delete db;
  return 0;
}
