
/*
 * In order to generate input for this, please go to $UCESB_DIR and run:
 *
 * Additional info:
 * To generate the header file used for the R3BUcesbSource (ext_h101.h), use:
 *
 * $UCESB_DIR/upexps/s438b/s438b --ntuple=UNPACK:TRIGGER,UNPACK:EVENTNO,RAW\
 *     STRUCT_HH,ext_h101.h
 *
 * Put this header file into the 'r3bsource' directory and recompile.
 *
 * Author: B. Löher
 * Based on: unpack_offline_160.C from M. Heil
 * 
 * Bastis UCESB: export UCESB_DIR=/u/bloeher/git/ucesb/
 */

// ToDo: replace /u/rplag with the actual path to your installation
extern "C" {
#include </u/rplag/R3BRoot/r3bsource/ext_h101_ptof.h>
#include </u/rplag/R3BRoot/r3bsource/ext_h101_unpack.h>
#include </u/rplag/R3BRoot/r3bsource/ext_h101_los.h>
}

struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_LOS_t los;
	EXT_STR_h101_PTOF_t ptof;
};

void
unpack_ptof_run165(Int_t RunId = 165)
{
	TString runNumber = Form("%03d", RunId);
	TStopwatch timer;
	timer.Start();

	const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
	const Int_t max_events_ucesb = 10000;
	TString max_events = Form("%d", max_events_ucesb);

	/* Create source using ucesb for input ------------------ */
	// ToDo: adjust all paths accordingly
	TString filename = "/d/land2/bloeher/nyx_cache/jun2016/run" + runNumber + "_*.lmd";
	TString outputFileName = "/tmp/test_ralf.root";
	TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path = ucesb_dir + "/../upexps/jun16/jun16_ptof"
	    " --max-events=" + max_events;

	EXT_STR_h101 ucesb_struct;
	R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
	    ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);

	source->AddReader(new R3BUnpackReader(
	    (EXT_STR_h101_unpack_t *)&ucesb_struct,
	    offsetof(EXT_STR_h101, unpack)));
	source->AddReader(new R3BLosReader(
	    (EXT_STR_h101_LOS_t *)&ucesb_struct.los,
	    offsetof(EXT_STR_h101, los)));
	source->AddReader(new R3BPtofReader(
	    (EXT_STR_h101_PTOF_t *)&ucesb_struct.ptof,
	    offsetof(EXT_STR_h101, ptof)));

	const Int_t refresh = 100;  /* refresh rate for saving */

	/* Create online run ------------------------------------ */
#define RUN_ONLINE
#ifdef RUN_ONLINE
	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);
#ifdef USE_HTTP_SERVER
	run->ActivateHttpServer(refresh);
#endif
#else
	/* Create analysis run ---------------------------------- */
	FairRunAna* run = new FairRunAna();
#endif
	/* Set output file name --------------------------------- */
	run->SetOutputFile(outputFileName.Data());
	/* ------------------------------------------------------ */

	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut1 = new FairParRootFileIo(kParameterMerged);
	TList *parList = new TList();
	// ToDo: Adjust these paths to your setup:
	parList->Add(new TObjString("parameter/ptof_time_params_run165.root"));
	parList->Add(new TObjString("parameter/los_time_params_run165.root"));
	parList->Add(new TObjString("parameter/tofd_hit_params_run165.root"));
	parOut1->open(parList);
	rtdb1->setFirstInput(parOut1);
	rtdb1->print();

	rtdb1->addRun(RunId);
	rtdb1->getContainer("LosTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"LosTCalPar", 1, 1);
	rtdb1->getContainer("PtofTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"PtofTCalPar", 1, 1);
	rtdb1->getContainer("TofdHitPar");
	rtdb1->setInputVersion(RunId, (char*)"TofdHitPar", 1, 1);


	/* Add analysis tasks ----------------------------------- */
	/* convert Mapped => Cal */
	R3BLosMapped2Cal* losMapped2Cal = new R3BLosMapped2Cal("LosTCalPar", 1);
	losMapped2Cal->SetNofModules(1, 5);
	run->AddTask(losMapped2Cal);
	run->AddTask( new R3BLosCal2Hit("losCal2Hit", 1) );
	run->AddTask( new R3BPtofMapped2Cal("PtofTCalPar", 1) );
	/* ------------------------------------------------------ */


	/* Initialize ------------------------------------------- */

	run->Init();

	/* Set log level either to WARNING, DEBUG or ERROR */
	FairLogger::GetLogger()->SetLogScreenLevel("WARNING");

	/* Run -------------------------------------------------- */
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);

	/* Cleanup ---------------------------------------------- */
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
	    << endl << endl;
}

