extern "C" {
#include "/u/jtscheus/r3broot/R3BRoot/r3bsource/ext_h101_full.h"
}

void unpack_ucesb(){

  //needed input
  TString filename = "/u/jtscheus/r3broot/s43b_data/run222_0823.lmd.gz";
  TString outputFileName = "test.root";
  TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
  TString ucesb_dir = getenv("UCESB_DIR");
  TString ucesb_path = ucesb_dir + "/../upexps/s438b/s438b";

    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source =
        new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    //    source->SetMaxEvents(nev);
    source->AddReader(new R3BUnpackReader(&ucesb_struct));
    /*source->AddReader(new R3BNeulandTamexReader(&ucesb_struct));*/
    source->AddReader(new R3BFiberReader(&ucesb_struct));
    /* ------------------------------------------------------ */

    /* Create online run ------------------------------------ */
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile(outputFileName);
    /* ------------------------------------------------------ */

    /* Create ALADIN field map ------------------------------ */
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    Double_t fMeasCurrent = 2500.; // I_current [A]
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(1.);
    run->SetField(magField);
    /* ------------------------------------------------------ */

    /* Add analysis task ------------------------------------ */
    // R3BLandRawAna* ana = new R3BLandRawAna("LandRawAna", 1);
    //run->AddTask(ana);
    /* ------------------------------------------------------ */

    /* Initialize ------------------------------------------- */
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    /* ------------------------------------------------------ */

    /* Runtime data base ------------------------------------ */
    /*FairRuntimeDb* rtdb = run->GetRuntimeDb();
    R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(parFileName);
    rtdb->setOutput(parOut);
    rtdb->print();*/
    /* ------------------------------------------------------ */

    /* Run -------------------------------------------------- */
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
    /*rtdb->saveOutput();*/
    delete run;
    /* ------------------------------------------------------ */
    /*
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;
    */
}
