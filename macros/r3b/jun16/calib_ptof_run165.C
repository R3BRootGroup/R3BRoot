
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
calib_ptof_run165(Int_t RunId = 165)
{
	TString runNumber = Form("%03d", RunId);
	TStopwatch timer;
	timer.Start();

	const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
	const Int_t max_events_ucesb = 1000000;
	TString max_events = Form("%d", max_events_ucesb);

	/* Create source using ucesb for input ------------------ */
	// ToDo: adjust all paths accordingly
	TString filename = "/SAT/nyx/land/jun2016/lmd/run" + runNumber + "_*.lmd";
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



	FairParRootFileIo* parOut2 = new FairParRootFileIo(kParameterMerged);
	parOut2->open("ptof_cal_params_run165.root");
	rtdb1->setOutput(parOut2);

	rtdb1->print();

	rtdb1->addRun(RunId);
	rtdb1->getContainer("PtofTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"PtofTCalPar", 1, 1);




	/* Add analysis tasks ----------------------------------- */
    const Int_t updateRate = 15000;
    const Int_t minStats = 10000;        // minimum number of entries for TCAL calibration
    R3BPtofMapped2CalPar* ptofCalibrator = new R3BPtofMapped2CalPar("R3BPtofMapped2CalPar", 1);
    ptofCalibrator->SetUpdateRate(updateRate);
    ptofCalibrator->SetMinStats(minStats);
	// ToDo: You probably have not implemented this optional function.
	// Then skip it. The standard solution from Ralf needs it though.
	//ptofCalibrator->SetNofModules(10, 100); // planes, bars per plane
	run->AddTask(ptofCalibrator);


	/* Initialize ------------------------------------------- */
	run->Init();

	/* Set log level either to WARNING, DEBUG or ERROR */
	FairLogger::GetLogger()->SetLogScreenLevel("INFO");

	/* Run -------------------------------------------------- */
	run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);

	rtdb1->saveOutput();

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

