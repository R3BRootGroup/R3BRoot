
/* In order to generate input for this, please go to $UCESB_DIR and run:
 *
 * Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * $UCESB_DIR/upexps/s438b/s438b --ntuple=UNPACK:TRIGGER,UNPACK:EVENTNO,RAW\
 *     STRUCT_HH,ext_h101.h
 *
 * Put this header file into the 'r3bsource' directory and recompile.
 * */

typedef struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_raw_nnp_onion_t nnp;
} EXT_STR_h101;

void unpack_ucesb_tacquila()
{
    TStopwatch timer;
    timer.Start();

    const Int_t nev = 10000; /* number of events to read, -1 - until CTRL+C */

    /* Create source using ucesb for input ------------------ */
    TString filename = "/Users/kresan/data/s438b/lmd/run269_2875.lmd.gz";
    TString outputFileName = "/data/r3b/r3broot_dev/root/unpack_ucesb.root";
    TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
    TString ucesb_dir = getenv("UCESB_DIR");
    TString ucesb_path = ucesb_dir + "/../upexps/s438b/s438b";

    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
                                                ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);
    source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(new R3BNeulandTacquilaReader((EXT_STR_h101_raw_nnp*)&ucesb_struct.nnp, offsetof(EXT_STR_h101, nnp)));
    /* ------------------------------------------------------ */

    /* Create online run ------------------------------------ */
    FairRunOnline* run = new FairRunOnline(source);
    run->SetOutputFile("output.root");
    run->SetRunId(269);
    run->ActivateHttpServer();
    run->SetAutoFinish(kFALSE);
    /* ------------------------------------------------------ */

    /* Create ALADIN field map ------------------------------ */
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    Double_t fMeasCurrent = 2500.; // I_current [A]
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(1.);
    run->SetField(magField);
    /* ------------------------------------------------------ */

    /* Add analysis task ------------------------------------ */
    R3BNeulandMappedHist* ana = new R3BNeulandMappedHist("LandRawAna", 1);
    run->AddTask(ana);
    /* ------------------------------------------------------ */

    /* Initialize ------------------------------------------- */
    run->Init();
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    /* ------------------------------------------------------ */

    /* Runtime data base ------------------------------------ */
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open("params.root");
    rtdb->setOutput(parOut);
    rtdb->print();
    /* ------------------------------------------------------ */

    /* Run -------------------------------------------------- */
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
    rtdb->saveOutput();
    delete run;
    /* ------------------------------------------------------ */

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;
}
