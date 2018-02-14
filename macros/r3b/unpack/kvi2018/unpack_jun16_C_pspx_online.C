
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

extern "C" {
//#include "/home/bloeher/git/R3BRoot/r3bsource/ext_h101_full.h"
#include "/u/syndikus/R3BRoot/r3bsource/ext_h101.h"

}

void unpack_jun16_C_pspx_online(Int_t RunId=181)
{
//    Int_t RunId=100;
   TString runNumber=Form ("%03d", RunId);
   TStopwatch timer;
    timer.Start();

    const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */

    /* Create source using ucesb for input ------------------ */
 
//    TString filename = "/d/land2/mheil/jun16/test_Ralf.lmd"; // LOS data
//    TString filename = "stream://x86l-3";
    TString filename = "stream://lxfs186:8000";
//    TString filename = "/SAT/hera/land/jun2016/lmd/run"+runNumber+"_*.lmd";
    TString outputFileName = "/tmp/run"+runNumber + "_pspx_online.root";
    TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW,PSPX";
    //TString ucesb_dir = getenv("UCESB_DIR");
    //TString ucesb_path = ucesb_dir + "/../upexps/jun16Xe/jun16Xe";
    TString ucesb_path = "/lynx/Lynx/land/usr/land/landexp/jun2016/upexps/jun16C/jun16C";

    TString pspxpar_dir = "/u/syndikus/R3BRoot/psp/par/";
    TString parPspxMappedFileName = "sDET_C_pspx_mapped.par";
    TString parPspxCalFileName = "sDET_C_pspx_cal.par";
    TString parPspxHitFileName = "sDET_C_pspx_hit.par";
    
    EXT_STR_h101 ucesb_struct;
    R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
	ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
    source->SetMaxEvents(nev);

    source->AddReader( new R3BPspxReader (&ucesb_struct) );
    source->AddReader( new R3BLosReader (&ucesb_struct) );
//    source->AddReader( new R3BTofdReader(&ucesb_struct) );
//      source->AddReader(new R3BNeulandTamexReader(&ucesb_struct));

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
/*
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut1 = new FairParRootFileIo(kParameterMerged);
    TList *parList = new TList();
    parList->Add(new TObjString("tofd_time_params_run057.root"));
    parList->Add(new TObjString("los_params_25-05-16.root"));
    parList->Add(new TObjString("tofd_hit_params_run054.root"));
    parOut1->open(parList);
    rtdb1->setFirstInput(parOut1);
    */

    FairParAsciiFileIo* parInput = new  FairParAsciiFileIo();
    TList *parList = new TList();
    parList->Add(new TObjString(pspxpar_dir+parPspxMappedFileName));
    parList->Add(new TObjString(pspxpar_dir+parPspxCalFileName));
    parList->Add(new TObjString(pspxpar_dir+parPspxHitFileName));
    parInput->open(parList);
    rtdb1->setFirstInput(parInput);
    rtdb1->print();

/*
    rtdb1->addRun(RunId);
    rtdb1->getContainer("LosTCalPar");
    rtdb1->setInputVersion(RunId, (char*)"LosTCalPar", 1, 1);
    rtdb1->getContainer("TofdTCalPar");
    rtdb1->setInputVersion(RunId, (char*)"TofdTCalPar", 1, 1);
    rtdb1->getContainer("TofdHitPar");
    rtdb1->setInputVersion(RunId, (char*)"TofdHitPar", 1, 1);
*/

    /* Create ALADIN field map ------------------------------ */
/*    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
    Double_t fMeasCurrent = 2500.; // I_current [A]
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(1.);
    run->SetField(magField);
    */


    /* Add analysis task ------------------------------------ */
    // convert Mapped => Cal

    //R3BLosMapped2Cal* losMapped2Cal=new R3BLosMapped2Cal("LosTCalPar", 1);
    //losMapped2Cal->SetNofModules(1, 5);
    //run->AddTask( losMapped2Cal );

    //R3BLosCal2Hit* losCal2Hit=new R3BLosCal2Hit("losCal2Hit", 1);
    //run->AddTask( losCal2Hit );

    R3BPspxMapped2Cal* pspxMapped2Cal = new R3BPspxMapped2Cal("PspxMapped2Cal", 1);
    run->AddTask(pspxMapped2Cal);
    
    //R3BTofdMapped2TCal* tofdMapped2Cal=new R3BTofdMapped2TCal("TofdTCalPar", 1);
    //tofdMapped2Cal->SetNofModules(10, 100);
    //run->AddTask( tofdMapped2Cal );

    //R3BTofdCal2Hit* tofdCal2Hit=new R3BTofdCal2Hit("TofdCal2Hit", 1);
    //tofdCal2Hit->SetTofdQ(54.); // Set nuclear charge Z of main beam
    //run->AddTask( tofdCal2Hit );
    /* ------------------------------------------------------ */

    R3BOnlineSpectra* r3bOnlineSpectra=new R3BOnlineSpectra("OnlineSpectra", 1);
    /* Parameter set before problems with LOS_4 */
//    if(RunId < 54)
//    r3bOnlineSpectra->SetLosParameters(-10.0, -4.1, 7., 7.); // Set offset and effective speeds of light in x, y 
    /* Parameter set after problems with LOS_4 */
//    if(RunId >= 54)
    if(RunId < 54)
    {
		printf("Using run < 54\n");
		r3bOnlineSpectra->SetLosParameters(-2.15, -1.28,5.3,5.3); // Set offset and effective speeds of light in x, y 
	}
	else if(RunId >= 54 && RunId < 67)
    {
		printf("Using run >= 54\n");
   /* Parameter set after problems with LOS_4 */
		r3bOnlineSpectra->SetLosParameters(-2.126, -4.285, 5.3,5.3); // Set offset and effective speed of light in x, y 
    }
        else
    {
		r3bOnlineSpectra->SetLosParameters(-2.099, -2.65, 5.3,5.3); // Set offset and effective speed of light in x, y 
    
    }	
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

