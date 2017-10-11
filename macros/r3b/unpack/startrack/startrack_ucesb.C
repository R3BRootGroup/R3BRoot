/* Additional info:
 * To generate the header file for a different number of febex modules,
 * add then in $UCESB_DIR/upexps/startrack/star.spec file and use:
 *
 * $UCESB_DIR/upexps/startrack/star --ntuple=RAW,STRUCT_HH,ext_h101_star.h
 *
 * Put this header file into the 'r3bsource' directory and recompile
 * 
 * */

 typedef struct EXT_STR_h101_t {
     EXT_STR_h101_unpack_t unpack;
     EXT_STR_h101_STAR_t startrack;
 } EXT_STR_h101;

void startrack_ucesb() {
	TStopwatch timer;
	timer.Start();

  const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
  //const Int_t nev = 10; /* number of events to read, -1 - until CTRL+C */

	/* Create source using ucesb for input ------------------ */

	TString filename = "/MIDAS/R3B/GSITest2014/lmd/run280_3364.lmd";
	TString outputFileName = "/home/mala/FAIR/startrack_0001.root";

	TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
	TString ucesb_dir = getenv("UCESB_DIR");

	TString ucesb_path = ucesb_dir + "/../upexps/startrack/star";

	EXT_STR_h101 ucesb_struct;

	R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
	    ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);

	source->AddReader(new R3BUnpackReader((EXT_STR_h101_unpack*)&ucesb_struct,
		offsetof(EXT_STR_h101, unpack)));
	source->AddReader(new R3BStartrackReader((EXT_STR_h101_STAR*)&ucesb_struct.startrack,
    offsetof(EXT_STR_h101, startrack)));
	/* ------------------------------------------------------ */

	/* Create online run ------------------------------------ */
	FairRunOnline* run = new FairRunOnline(source);
	run->SetRunId(1495624105);
	//run->SetRunId(1);
	run->SetOutputFile(outputFileName);

	/* Add analysis task ------------------------------------ */
	// ToDo: Add a task to order in time the Time stamped data
	//       and Energy calibration 
        //       -> see class: R3BStartrackMapped2Cal
	//R3BStartrackMapped2Cal* startrackMapped2Cal = new R3BStartrackMapped2Cal("StartrackMapped2Cal", 1);
	//run->AddTask(startrackMapped2Cal);

        // ToDo: Add another task to produce a root file for tracks
	//       -> see class R3BStartrackCal2Hit
	//R3BStartrackCal2Hit* startrackCal2Hit = new R3BStartrackCal2Hit("StartrackCal2Hit", 1);
	//run->AddTask(startrackCal2Hit);


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
	run->Run(nev, 0);
	/*rtdb->saveOutput();*/
        //delete run;
	/* ------------------------------------------------------ */

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
	     << endl << endl;

}
