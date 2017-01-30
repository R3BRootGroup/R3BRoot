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
//        > root -l pulserUnpack.C
//                         
//   But
//    select first the files for the pulser calibration, dividing in 30mev 
//    and 300mev (gain) for the later analysis with calPulser.C 
//  -------------------------------------------------------------------------
void pulserUnpack()
{
    TStopwatch timer;
    timer.Start();

    FairLmdSource* source = new FairLmdSource();
    /*source->AddFile("./data/140926/300mv/300mev_0159.lmd");
    source->AddFile("./data/140926/300mv/300mev_0160.lmd");
    source->AddFile("./data/140926/400mv/300mev_0161.lmd");
    source->AddFile("./data/140926/400mv/300mev_0162.lmd");
    source->AddFile("./data/140926/500mv/300mev_0168.lmd");
    source->AddFile("./data/140926/500mv/300mev_0169.lmd");
    source->AddFile("./data/140926/600mv/300mev_0170.lmd");
    source->AddFile("./data/140926/600mv/300mev_0171.lmd");
    */
    source->AddFile("./data/140926/300mv/30mev_0157.lmd");
    source->AddFile("./data/140926/300mv/30mev_0158.lmd");
    source->AddFile("./data/140926/400mv/30mev_0163.lmd");
    source->AddFile("./data/140926/400mv/30mev_0164.lmd");
    source->AddFile("./data/140926/500mv/30mev_0165.lmd");
    source->AddFile("./data/140926/500mv/30mev_0166.lmd");
    source->AddFile("./data/140926/500mv/30mev_0167.lmd");
    source->AddFile("./data/140926/600mv/30mev_0172.lmd");
    source->AddFile("./data/140926/600mv/30mev_0173.lmd");
    
    // CALIFA MBS parameters -------------------------------
    Short_t type = 10;
    Short_t subType = 1;
    Short_t procId = 1;
    Short_t subCrate = -1;
    Short_t control = 9;
    source->AddUnpacker(new R3BCaloUnpack("", type, subType, procId, subCrate, control));
    // ------------------------------------------------------


    // Create analysis run -------------------------------------------------------
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile("./data/140926/pul_30mev.root");
    // ---------------------------------------------------------------------------

    // Histograms ----------------------------------------------------------------
    R3BCaloRawAna* rawAna = new R3BCaloRawAna();
    run->AddTask(rawAna);
    // ---------------------------------------------------------------------------

    // Initialize ----------------------------------------------------------------
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    // ---------------------------------------------------------------------------

    // Run -----------------------------------------------------------------------
    run->Run(-1, 0);
    delete run;
    // ---------------------------------------------------------------------------

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
}

