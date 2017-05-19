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

/*
 * TODO:
 * Add the PTOF data structure to the full EXT_STR_h101_t structure
 */
struct EXT_STR_h101_t
{
	EXT_STR_h101_unpack_t unpack;
	EXT_STR_h101_LOS_t los;
};

void
unpack_ptof_run160(Int_t RunId = 160)
{
	/* Create a stopwatch to tell the used time in the end */
	TStopwatch timer;
	timer.Start();

	/* User settings ---------------------------------------- */
	/* TODO:
	 * Adjust your data paths
	 */
	TString input_data_path = "/data/gsi/jun2016";
	TString output_data_path = "/data/gsi/jun2016/rootfiles";
	TString output_file_suffix = "los_ptof_cal";
	TString unpacker = "jun16/jun16_ptof";

	/* number of events to read, -1 - until CTRL+C */
	const Int_t nev = -1;
	/* Maximum number of events that ucesb should supply */
	const Int_t max_events_ucesb = 1000000;

	/* Create source using ucesb for input ------------------ */
	/*
	 * Set input, output paths and where to find the
	 * unpacker.
	 */
	TString runNumber = Form("%03d", RunId);
	TString max_events = Form("%d", max_events_ucesb);
	TString filename = input_data_path + "/run" + runNumber + "_*.lmd";
	TString outputFileName = output_data_path + "/run" + runNumber
	    + "_" + output_file_suffix + ".root";
	TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_options = " --max-events=" + max_events;
	TString ucesb_path = ucesb_dir + "/../upexps/" + unpacker
	    + ucesb_options;

	/*
	 * The main structure containing all data items
	 */
	EXT_STR_h101 ucesb_struct;

	/*
	 * Create the R3BUcesbSource
	 */
	R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
	    ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);

	/*
	 * Add Reader to the source
	 */
	source->AddReader(new R3BUnpackReader(
	    (EXT_STR_h101_unpack_t *)&ucesb_struct,
	    offsetof(EXT_STR_h101, unpack)));

	source->AddReader(new R3BLosReader(
	    (EXT_STR_h101_LOS_t *)&ucesb_struct.los,
	    offsetof(EXT_STR_h101, los)));

	/*
	 * TODO:
	 * Add the R3BPtofReader
	 */

	/* Create online run ------------------------------------ */
	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(RunId);

	/* Set output file name --------------------------------- */
	run->SetOutputFile(outputFileName.Data());

	/* Runtime data base ------------------------------------ */

	/*
	 * New list of calibration parameters
	 */
	TList *parList = new TList();

	/*
	 * Add files containing calibration parameters to the list
	 */
	/*
	 * TODO:
	 * Add the ptof_time_params_run165.root file.
	 */
	parList->Add(new TObjString("parameter/los_time_params_run165.root"));
	parList->Add(new TObjString("parameter/tofd_hit_params_run165.root"));

	/*
	 * Open the files and set as input to the runtime db
	 */
	Bool_t kParameterMerged = kTRUE;
	FairParRootFileIo* parOut1 = new FairParRootFileIo(kParameterMerged);
	parOut1->open(parList);

	FairRuntimeDb* rtdb1 = run->GetRuntimeDb();
	rtdb1->setFirstInput(parOut1);
	rtdb1->print();

	/*
	 * Get calibration parameter container and set the appropriate
	 * versions.
	 */
	rtdb1->addRun(RunId);
	rtdb1->getContainer("LosTCalPar");
	rtdb1->setInputVersion(RunId, (char*)"LosTCalPar", 1, 1);
	rtdb1->getContainer("TofdHitPar");
	rtdb1->setInputVersion(RunId, (char*)"TofdHitPar", 1, 1);
	/*
	 * TODO:
	 * Add the PtofTCalPar container
	 */

	/* Add analysis tasks ----------------------------------- */
	/*
	 * convert Mapped => Cal
	 */
	R3BLosMapped2Cal* losMapped2Cal = new R3BLosMapped2Cal("LosTCalPar", 1);
	losMapped2Cal->SetNofModules(1, 5);
	run->AddTask(losMapped2Cal);
	R3BLosCal2Hit* losCal2Hit = new R3BLosCal2Hit("losCal2Hit", 1);
	run->AddTask(losCal2Hit);

	/*
	 * TODO:
	 * Add the R3BPtofMapped2Cal task
	 */

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

