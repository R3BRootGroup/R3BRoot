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

void run(TString inputFilePattern, TString outputFileName, Int_t nevents=-1)
{
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    TStopwatch timer;
    timer.Start();
    
    FILE *fd = fopen(inputFilePattern.Data(), "r");
    if(!fd)
    {
	cerr << "Could not open " << inputFilePattern << " for reading!\n";
	return;
    }

    FairLmdSource* source = new FairLmdSource();
    char fname[1000];
    int l;
    while(!feof(fd))
    {
        if(!(fgets(fname, 1000, fd)))
		break;
	l = strlen(fname);
        if(fname[l-1] == '\n')
		fname[l-1] = '\0';
        cout << fname << endl;
        source->AddFile(fname);
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

    // Set data compression (Only fill non-empty branches)
//    FairRootManager::Instance()->SetCompressData(true);

    // Histograms ----------------------------------------------------------------
    R3BCaloRawAna* rawAna = new R3BCaloRawAna();
    run->AddTask(rawAna);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
//    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
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
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

void califaUnpack(TString inputFilePattern, TString outputFileName, Int_t nevents=-1)
{
	run(inputFilePattern, outputFileName, nevents);
}

