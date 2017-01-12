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
//   Usage: 
//        > root -l 
//        ROOT> .L califaUnpack.C
//        ROOT>  run("inputFile", firstFileNumber, lastFileNumber, nevents) 
//        ????ROOT>  run("inputFile", firstFileNumber, lastFileNumber) in HOWTO without the nevents 
//   where 
//    inputFile is the input lmd file without the .lmd extension and the file number
//    firstFileNumber is the file number of the first lmd file
//    lastFileNumber is the file number of the last lmd file
//    nevents is the number of events to read, -1 - untill CTRL+C
//  -------------------------------------------------------------------------
void run(TString inputFile="", Int_t firstFileNumber=-999, Int_t lastFileNumber=-999, Int_t nevents=-1)
{
    TStopwatch timer;
    timer.Start();
    
    TString inDir = "lmd/";   // directory with lmd file
    TString outDir = "";  // output directory

    TString inputFileName = inDir + inputFile + ".lmd";                       // name of input file
    TString outputFileName = outDir + inputFile + "_raw.root";                // name of output file

    FairLmdSource* source = new FairLmdSource();
    char strName[1000];
    for(Int_t i = firstFileNumber; i < lastFileNumber+1; i++) {
        sprintf(strName, "%s%s_%04d.lmd", inDir.Data(), inputFile.Data(), i);
        cout << strName << endl;
        source->AddFile(strName);
    }
    
    // CALIFA MBS parameters ------------parameters -------------------
   Short_t type = 100; // CALIFA in included in the general day
   Short_t subType = 10000;
   Short_t procId = 2;
   Short_t subCrate = 2;
   Short_t control = 9;

    // Short_t type = 10; // CALIFA at the beginning of the experiment 
    // Short_t subType = 1;
    // Short_t procId = 1;
    // Short_t subCrate = -1;
    // Short_t control = 9;
    source->AddUnpacker(new R3BCaloUnpack("", type, subType, procId, subCrate, control));
    // ------------------------------------------------------


    // Create analysis run -------------------------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile(outputFileName.Data());
    // ---------------------------------------------------------------------------

    FairRootManager::Instance()->SetCompressData(true);

    // Histograms ----------------------------------------------------------------
//    R3BCaloRawAna* rawAna = new R3BCaloRawAna();
//    run->AddTask(rawAna);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
//    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(0, nEvents);
    delete run;
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

