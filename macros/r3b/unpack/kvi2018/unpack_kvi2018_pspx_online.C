
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
  EXT_STR_h101_LOS_onion_t los;
  EXT_STR_h101_unpack_t unpack;
  EXT_STR_h101_PSP_onion_t psp;
} EXT_STR_h101;

void unpack_kvi2018_pspx_online(Int_t RunId=181)
{
   TString runNumber=Form ("%03d", RunId);
   TStopwatch timer;
    timer.Start();

    const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */

    /* Create source using ucesb for input ------------------ */
 
    //TString filename = "stream://lxfs186:8000";
    TString filename = "stream://192.168.1.41:8000"; //ip of lipc-1
   // TString filename = "/home/land/PSPX/lmd/source_notraces_180221_0482.lmd"; //"+runNumber+"_*
    TString outputFileName = "/home/land/PSPX/rootfiles/source_notraces_180221_win160_pspx_online.root";
    TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW,PSPX";
    TString ucesb_dir = getenv("UCESB_DIR");
    TString ucesb_path = ucesb_dir + "/../upexps/kvi2018/kvi2018";
    //TString ucesb_path = "/home/land/upexps/kvi2018/kvi2018";

    TString pspxpar_dir = "/home/land/R3BRoot_PSPX/psp/par/";
    TString parPspxMappedFileName = "kvi2018_pspx_mapped.par";
    TString parPspxPrecalFileName = "kvi2018_pspx_precal_default.par";
    TString parPspxCalFileName = "kvi2018_pspx_cal_default.par";
    TString parPspxHitFileName = "kvi2018_pspx_hit_default.par";
    
    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);

    source->AddReader( new R3BUnpackReader ((EXT_STR_h101_unpack*)&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)) );
    source->AddReader( new R3BPspxReader((EXT_STR_h101_PSP*)&ucesb_struct.psp, offsetof(EXT_STR_h101, psp)) );


    const Int_t refresh = 2000;                 // refresh rate for saving     /* Create online run ------------------------------------ */
    FairRunOnline* run = new FairRunOnline(source);
    run->SetRunId(RunId);
    run->ActivateHttpServer(refresh);

    // Create analysis run ----------------------------------
    //FairRunAna* run = new FairRunAna();
    run->SetOutputFile(outputFileName.Data());


    /* Runtime data base ------------------------------------ */
/*    FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut1 = new FairParRootFileIo(kParameterMerged);
    TString parFileName1 = "tofd_params_25-05-16.root";
    parOut1->open(parFileName1);
    FairParRootFileIo* parOut2 = new FairParRootFileIo(kParameterMerged);
    TString parFileName2 = "los_params_25-05-16.root";
    parOut2->open(parFileName2);
    rtdb1->setFirstInput(parOut1);
    rtdb1->setSecondInput(parOut2);
    //rtdb1->setOutput(parOut1);
    rtdb1->print();*/
    /* ------------------------------------------------------ */


    FairRuntimeDb* rtdb1 = run->GetRuntimeDb();


    FairParAsciiFileIo* parInput = new  FairParAsciiFileIo();
    TList *parList = new TList();
    parList->Add(new TObjString(pspxpar_dir+parPspxMappedFileName));
    parList->Add(new TObjString(pspxpar_dir+parPspxPrecalFileName));
    parList->Add(new TObjString(pspxpar_dir+parPspxCalFileName));
    parList->Add(new TObjString(pspxpar_dir+parPspxHitFileName));
    parInput->open(parList);
    rtdb1->setFirstInput(parInput);
    rtdb1->print();





    /* Add analysis task ------------------------------------ */

    R3BPspxMapped2Precal* pspxMapped2Precal = new R3BPspxMapped2Precal("PspxMapped2Precal", 1);
    run->AddTask(pspxMapped2Precal);
    R3BPspxPrecal2Cal* pspxPrecal2Cal = new R3BPspxPrecal2Cal("PspxPrecal2Cal", 1);
    run->AddTask(pspxPrecal2Cal);
    R3BPspxCal2Hit* pspxCal2Hit = new R3BPspxCal2Hit("PspxCal2Hit", 1);
    run->AddTask(pspxCal2Hit);
    
    /* ------------------------------------------------------ */

    R3BOnlineSpectra* r3bOnlineSpectra=new R3BOnlineSpectra("OnlineSpectra", 1);

    run->AddTask( r3bOnlineSpectra );




    /* Initialize ------------------------------------------- */
    run->Init();
    rtdb1->print();
//    FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
//    FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
    FairLogger::GetLogger()->SetLogScreenLevel("ERROR");

    /* Run -------------------------------------------------- */
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
//    rtdb1->saveOutput();

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
         << endl << endl;
}

