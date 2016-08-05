
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
#include "/u/mheil/R3BRoot/r3bsource/ext_h101_full.h"
}

void tamex_multi_pc_ucesb_mh(TString runNumber)
{
	TStopwatch timer;
	timer.Start();

	const Int_t nev = -1; /* number of events to read, -1 - until CTRL+C */
    const Int_t trigger = -1;              // 1 - onspill, 2 - offspill. -1 - all

    TString dirIn1 = "/u/mheil/data/neuland_tamex/data/";
    TString dirOut = "/u/mheil/data/neuland_tamex/tcal/";
    TString dirOut1 = "/u/mheil/data/neuland_tamex/par/";
    TString dirHist = "/u/mheil/data/neuland_tamex/hist/";
//    TString inputFileName1 = dirIn1 + runNumber + "_mapped_neuland.root";              // name of input file
    TString parFileName    = dirOut1 + "params_" + runNumber + "_mapped_neuland.root";  // name of parameter file
    TString outputFileName = dirOut + runNumber + "_mapped_neuland.root";             // name of output file

	/* Create source using ucesb for input ------------------ */

	TString filename = "stream://x86l-3";
//	TString outputFileName = "tamex_text.root";
    TString histFileName = dirHist + runNumber + "hist_neuland.root";   // name of file with control histograms
    const Int_t refresh = 10000;                 // refresh rate for saving control histograms
//    TString parFileName = "params_neuland.root";  // name of parameter file
	TString ntuple_options = "UNPACK:EVENTNO,UNPACK:TRIGGER,RAW";
	TString ucesb_dir = getenv("UCESB_DIR");
	TString ucesb_path = ucesb_dir +
	    "/../upexps/tamex_multi_pc_readout/tamex_multi_pc_readout";

	EXT_STR_h101 ucesb_struct;
	R3BUcesbSource* source = new R3BUcesbSource(filename, ntuple_options,
	    ucesb_path, &ucesb_struct, sizeof(ucesb_struct));
	source->AddReader(new R3BUnpackReader(&ucesb_struct));
	source->AddReader(new R3BNeulandTamexReader(&ucesb_struct));

	/* ------------------------------------------------------ */

	/* Create online run ------------------------------------ */
	FairRunOnline* run = new FairRunOnline(source);
	run->SetOutputFile(outputFileName);
	run->SetRunId(5050);
    //run->SetGenerateHtml(kTRUE, "test_page.root", refresh);
    run->ActivateHttpServer();
	/* ------------------------------------------------------ */

	/* Create ALADIN field map ------------------------------ */
	R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
	Double_t fMeasCurrent = 2500.; // I_current [A]
	magField->SetCurrent(fMeasCurrent);
	magField->SetScale(1.);
	run->SetField(magField);
	/* ------------------------------------------------------ */

    
 	/* Add analysis task ------------------------------------ */
	R3BLandRawAna* ana = new R3BLandRawAna("R3BNeulandTamexMappedItem", 1);
	run->AddTask(ana);
	/* ------------------------------------------------------ */
   



    const Int_t nBarsPerPlane = 50;      // number of scintillator bars per plane
    const Int_t updateRate = 15000;
    const Int_t minStats = 10000;        // minimum number of entries for TCAL calibration
    const Int_t nPlanes = 1;             // number of planes (for TCAL calibration)


    // TCAL ----------------------------------------------------------------------
    R3BNeulandTcalFill* tcalFill = new R3BNeulandTcalFill("TcalFill", 1);
    tcalFill->SetUpdateRate(updateRate);
    tcalFill->SetMinStats(minStats);
    tcalFill->SetTrigger(trigger);
    tcalFill->SetNofModules(nPlanes,nBarsPerPlane, 0);

    run->AddTask(tcalFill);
       

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
	parOut->open(parFileName);
	rtdb->setOutput(parOut);
	rtdb->print();
	/* ------------------------------------------------------ */

	/* Run -------------------------------------------------- */
    run->Run((nev < 0) ? nev : 0, (nev < 0) ? 0 : nev);
	rtdb->saveOutput();
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

