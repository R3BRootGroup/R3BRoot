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

/*
extern "C" {
//#include "/home/bloeher/git/R3BRoot/r3bsource/ext_h101_full.h"
//#include "ext_data_client.h"
#include "/home/huetchen/R3BRoot/r3bsource/ext_h101_los.h"
#include "/home/huetchen/R3BRoot/r3bsource/ext_h101_psp.h"
}
*/

typedef struct EXT_STR_h101_t
{
    // EXT_STR_h101_LOS_onion_t los;
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_PSP_onion_t psp;
} EXT_STR_h101;

//#define RUN_ONLINE

void unpack_jun16_Xe_pspx(Int_t RunId = 111)
{
    TString runNumber = Form("%03d", RunId);
    TStopwatch timer;
    timer.Start();

    //const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
     const Int_t nev = 1000;

    /* Create source using ucesb for input ------------------ */
#ifdef RUN_ONLINE
    TString filename = "/home/huetchen/Desktop/sonja_x5/lmd/run" + runNumber + "_*.lmd";
#else
    TString filename = "/home/huetchen/Desktop/sonja_x5/root/run" + runNumber + "_pspx_mapped.root";
#endif
    TString outputFileName = "/home/huetchen/Desktop/sonja_x5/root/run" + runNumber + "_pspx_all_test.root";
    TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW,PSPX";
    // TString ucesb_dir = getenv("UCESB_DIR");
    // TString ucesb_path = ucesb_dir + "/../upexps/jun16Xe/jun16Xe";
    TString ucesb_path = "/home/huetchen/Programs/ucesb/upexps/jun16/jun16Xe";

    TString pspxpar_dir = "/home/huetchen/R3BRoot/psp/par/";
    TString parPspxMappedFileName = "sDET_Xe_pspx_mapped.par";
    TString parPspxPrecalFileName = "sDET_Xe_pspx_precal.par";
    TString parPspxCalFileName = "sDET_Xe_pspx_cal.par";
    TString parPspxHitFileName = "sDET_Xe_pspx_hit.par";

#ifdef RUN_ONLINE
    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source =
        new R3BUcesbSource(filename, ntuple_options, ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);

    source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct.unpack, offsetof(EXT_STR_h101, unpack)));
    //  source->AddReader( new R3BLosReader ((EXT_STR_h101_LOS*)&ucesb_struct.los, offsetof(EXT_STR_h101, los)) );
    source->AddReader(new R3BPspxReader((EXT_STR_h101_PSP*)&ucesb_struct.psp, offsetof(EXT_STR_h101, psp)));
#endif

    /* Create run ------------------------------------------- */
#ifdef RUN_ONLINE
    FairRunOnline* run = new FairRunOnline(source);
    run->SetRunId(RunId);
#else
    FairRunAna* run = new FairRunAna();
    run->SetSource(new FairFileSource(filename));
#endif

    run->SetOutputFile(outputFileName.Data());

    /* Runtime data base ------------------------------------ */
    FairRuntimeDb* rtdb1 = run->GetRuntimeDb();

    FairParAsciiFileIo* parInput = new FairParAsciiFileIo();
    TList* parList = new TList();
    parList->Add(new TObjString(pspxpar_dir + parPspxMappedFileName));
    parList->Add(new TObjString(pspxpar_dir + parPspxPrecalFileName));
    parList->Add(new TObjString(pspxpar_dir + parPspxCalFileName));
    parList->Add(new TObjString(pspxpar_dir + parPspxHitFileName));
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

    /* Initialize ------------------------------------------- */
    run->Init();
    rtdb1->print();
    //    FairLogger::GetLogger()->SetLogScreenLevel("WARNING");
    //    FairLogger::GetLogger()->SetLogScreenLevel("DEBUG");
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");

    /* Run -------------------------------------------------- */
#ifdef RUN_ONLINE
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
#else
    run->Run();
#endif
    //    rtdb1->saveOutput();

    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << outputFileName << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl << endl;

    gSystem->Exec("rm  all_*.par");
    //gSystem->Exec("rm  core_dump_*");

    gApplication->Terminate();
}
