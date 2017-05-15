//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B CALIFA Unpacking
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//         Last Update: 22/09/14
//         Comments:
//			Runs the CALIFA Unpacker. Outputs a root file with 
//			a collection (TClonesArray) of R3BCrystalHits
//
//  -------------------------------------------------------------------------
//
//   Usage: e.g.
//
// root -l -b -q 'califaUnpack_batch.C("1000mv_30mev_",362,364)';
//  -------------------------------------------------------------------------
void califaUnpack_batch(TString inputFile="", Int_t firstFileNumber=-999, Int_t lastFileNumber=-999, Int_t nevents=-1)
{
    TStopwatch timer;
    timer.Start();
    
    TString inDir = "./data/run336/";   // directory with lmd files
    TString outDir = "./data/run336/";  // output directory

    TString inputFileName = inDir + inputFile + ".lmd";                       // name of input file
    TString outputFileName = outDir + inputFile + "_raw.root";                // name of output file
    TString parFileName = outDir + "params_" + inputFile + "_raw.root";       // name of parameter file

    FairLmdSource* source = new FairLmdSource();
    char strName[1000];
    for(Int_t i = firstFileNumber; i < lastFileNumber+1; i++) {
        sprintf(strName, "%s%s%4d.lmd", inDir.Data(), inputFile.Data(), i);
        for(Int_t j = 0; j < 1000; j++) if(' ' == strName[j]) strName[j] = '0';
        cout << strName << endl;
        source->AddFile(strName);
    }
    
    // CALIFA MBS parameters ------------parameters -------------------

   Short_t type = 100; // CALIFA in included in the general day
   Short_t subType = 10000;
   Short_t procId = 2;
   Short_t subCrate = 2;
   Short_t control = 9;
    /*
    Short_t type = 10; // CALIFA at the beginning of the experiment 
    Short_t subType = 1;
    Short_t procId = 1;
    Short_t subCrate = -1;
    Short_t control = 9;
    */
    source->AddUnpacker(new R3BCalifaUnpack("", type, subType, procId, subCrate, control));
    // ------------------------------------------------------


    // Create analysis run -------------------------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    // Histograms ----------------------------------------------------------------
    R3BCalifaMappedDataAnalysis* rawAna = new R3BCalifaMappedDataAnalysis();
    run->AddTask(rawAna);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(nevents, 0);
    delete run;
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Parameter file is " << parFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

