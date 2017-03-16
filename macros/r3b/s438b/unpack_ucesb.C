
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
#include "/home/bloeher/git/R3BRoot/r3bsource/ext_h101_full.h"
}

void unpack_ucesb()
{
	TStopwatch timer;
	timer.Start();

	const Int_t nev = 2000; /* number of events to read, -1 - until CTRL+C */

	/* Create source using ucesb for input ------------------ */

	TString filename = "/data/r3b/r3broot_dev/lmd/run238*.lmd.gz";
	TString outputFileName = "/tmp/run238_mapped_cal_deleteme.root";
	TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path = ucesb_dir + "/../upexps/s438b/s438b";
	
	TString pspxpar_dir = "/u/syndikus/R3BRoot/psp/par/";
	TString parPspxMappedFileName = "s438b_pspx_mapped.par";
	TString parPspxCalFileName = "s438b_pspx_cal.par";

	TString pspxpar_dir = "/home/bloeher/git/R3BRoot/psp/par/";
	TString parPspxMappedFileName = "s438b_pspx_mapped.par";
	TString parPspxCalFileName = "s438b_pspx_cal.par";

	EXT_STR_h101_onion ucesb_struct;
	R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
	    ucesb_path, (EXT_STR_h101 *)&ucesb_struct, sizeof(ucesb_struct));
	source->SetMaxEvents(nev);
	//source->AddReader(new R3BUnpackReader(&ucesb_struct));
	source->AddReader(new R3BPspxReader(
		(EXT_STR_h101_PSP *)&ucesb_struct.PSPX[0]));
	/*source->AddReader(new R3BNeulandTamexReader(&ucesb_struct));*/

	/* ------------------------------------------------------ */

	/* Create online run ------------------------------------ */
	FairRunOnline* run = new FairRunOnline(source);
	run->SetOutputFile(outputFileName);

	/* Runtime Database--------------------------------------- */
	
	FairRuntimeDb* rtdb=run->GetRuntimeDb();
	
	//Bool_t kParameterMerged = kTRUE;
	//FairParAsciiFileIo* parInput = new  FairParAsciiFileIo(kParameterMerged);
	FairParAsciiFileIo* parInput = new  FairParAsciiFileIo();
	TList *parList = new TList();
	parList->Add(new TObjString(pspxpar_dir+parPspxMappedFileName));
	parList->Add(new TObjString(pspxpar_dir+parPspxCalFileName));
	//parList->Add(new TObjString(pspxpar_dir+parPspxHitFileName));
	parInput->open(parList);
	rtdb->setFirstInput(parInput);
	
	//FairParAsciiFileIo* parInputMapped = new FairParAsciiFileIo();
	//FairParAsciiFileIo* parInputCal = new FairParAsciiFileIo();
	//parInputMapped->open((pspxpar_dir+parPspxMappedFileName).Data(),"in");
	//parInputCal->open((pspxpar_dir+parPspxCalFileName).Data(),"in");

	//rtdb->setFirstInput(parInputMapped);
	//rtdb->setSecondInput(parInputCal);
	
        rtdb->print();
	
	/* Create ALADIN field map ------------------------------ */
	R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
	Double_t fMeasCurrent = 2500.; // I_current [A]
	magField->SetCurrent(fMeasCurrent);
	magField->SetScale(1.);
	run->SetField(magField);
	/* ------------------------------------------------------ */

	/* Add analysis task ------------------------------------ */
	//R3BLandRawAna* ana = new R3BLandRawAna("LandRawAna", 1);
	//run->AddTask(ana);
	//R3BFi4Mapped2Cal* Fi4Mapped2Cal = new R3BFi4Mapped2Cal("Fi4Mapped2Cal",1);
	//run->AddTask(Fi4Mapped2Cal);
	//R3BFi4Cal2Hit* Fi4Cal2Hit = new R3BFi4Cal2Hit("Fi4Cal2Hit",1);
	//Fi4Cal2Hit->SetGeometry("");
	//Fi4Cal2Hit->SetFiberWidth("");
	//run->AddTask(Fi4Cal2Hit);
	
	//R3BPspxMapped2Cal* pspxMapped2Cal = new R3BPspxMapped2Cal("PspxMapped2Cal", 1);
	//run->AddTask(pspxMapped2Cal);
	//R3BPspxCal2Hit* pspxCal2Hit = new R3BPspxCal2Hit("PspxCal2Hit", 1);
	//run->AddTask(pspxCal2Hit);
	
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

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;
	cout << "Output file is " << outputFileName << endl;
	cout << "Real time " << rtime << " s, CPU time " << ctime << " s"
	     << endl << endl;
	        
	gApplication->Terminate();
}

