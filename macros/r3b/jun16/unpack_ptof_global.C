
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
 */

/*#include <FairRunOnline.h>
#include <FairParRootFileIo.h>
#include <FairSource.h>
#include <TStopwatch.h>
#include <TString.h>
#include <TObject.h>
#include <R3BReader.h>
#include <R3BUcesbSource.h>
#include <R3BUnpackReader.h>
#include <R3BLosReader.h>
#include <R3BPtofReader.h>
extern "C" {
#include <r3bsource/ext_h101_ptof.h>
#include <r3bsource/ext_h101_unpack.h>
#include <r3bsource/ext_h101_los.h>
}*/

#define USE_HTTP_SERVER 
#define ONLINE_SPECTRA

struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_LOS_t los;
	EXT_STR_h101_PTOF_t ptof;
};

void
unpack_ptof(Int_t RunId)
{
	TString runNumber = Form("%03d", RunId);
	TStopwatch timer;
	timer.Start();

	const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
	const Int_t max_events_ucesb = 10000000;
	TString max_events = Form("%d", max_events_ucesb);

	/* Create source using ucesb for input ------------------ */

	TString filename = "/d/land2/bloeher/nyx_cache/jun2016/run" + runNumber + "_*.lmd";
//	TString filename = "/SAT/nyx/land/jun2016/stitched/run" + runNumber + "_*.lmd";
		TString outputFileName = "/d/land2/mheil2/workshop2017/rootfiles/run"
	    + runNumber + "_los_ptof_hit.root";
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

	const Int_t refresh = 1;  /* refresh rate for saving */

	/* Create online run ------------------------------------ */
#define RUN_ONLINE
#ifdef RUN_ONLINE
	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);
#ifdef USE_HTTP_SERVER
	run->ActivateHttpServer(refresh);
	run->SetAutoFinish(kFALSE);
#endif
#else
	/* Create analysis run ---------------------------------- */
	FairRunAna* run = new FairRunAna();
#endif
	/* Set output file name --------------------------------- */
	run->SetOutputFile(outputFileName.Data());


	/* Runtime data base ------------------------------------ */
#ifdef USE_RUNTIME_DB_OLD
	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut1 = new FairParRootFileIo(kParameterMerged);
	TString parFileName1 = "tofd_params_25-05-16.root";
	parOut1->open(parFileName1);
	FairParRootFileIo* parOut2 = new FairParRootFileIo(kParameterMerged);
	TString parFileName2 = "los_params_25-05-16.root";
	parOut2->open(parFileName2);
	rtdb1->setFirstInput(parOut1);
	rtdb1->setSecondInput(parOut2);
	/* rtdb1->setOutput(parOut1); */
	rtdb1->print();
#endif
	/* ------------------------------------------------------ */

	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut1 = new FairParRootFileIo(kParameterMerged);
	TList *parList = new TList();
	parList->Add(new TObjString("parameter/ptof_time_params_run165.root"));
	parList->Add(new TObjString("parameter/los_time_params_run165.root"));
	parOut1->open(parList);
	rtdb1->setFirstInput(parOut1);
	rtdb1->print();

	rtdb1->addRun(RunId);
	rtdb1->getContainer("LosTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"LosTCalPar", 1, 1);
	rtdb1->getContainer("PtofTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"PtofTCalPar", 1, 1);


	/* Create ALADIN field map ------------------------------ */
#ifdef USE_ALADIN_FIELDMAP
	R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
	Double_t fMeasCurrent = 2500.; // I_current [A]
	magField->SetCurrent(fMeasCurrent);
	magField->SetScale(1.);
	run->SetField(magField);
#endif


	/* Add analysis tasks ----------------------------------- */
	/* convert Mapped => Cal */

	R3BLosMapped2Cal* losMapped2Cal = new R3BLosMapped2Cal("LosTCalPar", 1);
	losMapped2Cal->SetNofModules(1, 5);
	run->AddTask(losMapped2Cal);
	R3BLosCal2Hit* losCal2Hit = new R3BLosCal2Hit("losCal2Hit", 1);
	run->AddTask(losCal2Hit);

	R3BPtofMapped2Cal* ptofMapped2Cal =
	    new R3BPtofMapped2Cal("PtofTCalPar", 1);
	run->AddTask(ptofMapped2Cal);

#ifdef RUN_TASKS
//	R3BTofdCal2Hit* tofdCal2Hit = new R3BTofdCal2Hit("TofdCal2Hit", 1);
//	tofdCal2Hit->SetTofdQ(6.); /* Set nuclear charge Z of main beam */
//	run->AddTask( tofdCal2Hit );

#endif
	/* ------------------------------------------------------ */

#ifdef ONLINE_SPECTRA
	R3BGlobalAnalysis* r3bGlobalAnalysis =
	    new R3BGlobalAnalysis("GlobalAnalysis", 1);

	run->AddTask( r3bGlobalAnalysis );
#endif

	/* Initialize ------------------------------------------- */

	run->Init();
	/*rtdb1->print();*/

	/* Set log level either to WARNING, DEBUG or ERROR */
	FairLogger::GetLogger()->SetLogScreenLevel("WARNING");

	/* Run -------------------------------------------------- */
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);

#ifdef SAVE_DB_OUTPUT
	rtdb1->saveOutput();
#endif

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

