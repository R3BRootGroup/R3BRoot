void r3ball(Int_t nEvents = 1,
	    TObjArray& fDetList,
	    TString Target = "LeadTarget",
            Bool_t fVis=kFALSE,
            TString fMC="TGeant4",
	    TString fGenerator="land",
	    Bool_t fUserPList= kFALSE,
            Bool_t fR3BMagnet= kTRUE)
{
  TString dir = getenv("VMCWORKDIR");
  TString r3bdir = dir + "/macros";

  TString r3b_geomdir = dir + "/geometry";
  gSystem->Setenv("GEOMPATH",r3b_geomdir.Data());

  TString r3b_confdir = dir + "gconfig";
  gSystem->Setenv("CONFIG_DIR",r3b_confdir.Data());


  // Output files
  TString OutFile = "r3bsim.root";
  TString ParFile = "r3bpar.root";


  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------


  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  run->SetName(fMC.Data());                    // Transport engine
  run->SetOutputFile(OutFile.Data());          // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();


  // Magnetic field map type
  Int_t fFieldMap = 0;


  //  R3B Special Physics List in G4 case
  if((fUserPList == kTRUE) &&
     (fMC.CompareTo("TGeant4") == 0)) {
    cout<<"Geant4******************************************"<<endl;
    run->SetUserConfig("g4R3bConfig.C");
    run->SetUserCuts("SetR3BCuts.C");
  }
  

  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media_r3b.geo");       // Materials
  // ------------------------------------------------------------------------
  

  // Global Transformations
  //- Two ways for a Volume Rotation are supported
  //-- 1) Global Rotation (Euler Angles definition)
  //-- This represent the composition of : first a rotation about Z axis with
  //-- angle phi, then a rotation with theta about the rotated X axis, and
  //-- finally a rotation with psi about the new Z axis.
  Double_t phi,theta,psi;

  //-- 2) Rotation in Ref. Frame of the Volume
  //-- Rotation is Using Local Ref. Frame axis angles
  Double_t thetaX,thetaY,thetaZ;

  //- Global Translation  Lab. frame.
  Double_t tx,ty,tz;


  // -----   Create R3B geometry --------------------------------------------
  //R3B Cave definition
  FairModule* cave= new R3BCave("CAVE");
  cave->SetGeometryFileName("r3b_cave.geo");
  run->AddModule(cave);


  //R3B Target definition
  if (fDetList.FindObject("TARGET") ) {
      R3BModule* target= new R3BTarget(Target.Data());
      target->SetGeometryFileName(("target_" + Target + ".geo.root").Data());
      run->AddModule(target);
  }


  //R3B Magnet definition
  if (fDetList.FindObject("ALADIN") ) {
      fFieldMap = 0;
      R3BModule* mag = new R3BMagnet("AladinMagnet");
      mag->SetGeometryFileName("aladin_v12a.geo.root");
      run->AddModule(mag);
  }

    //R3B Magnet definition
  if (fDetList.FindObject("GLAD") ) {
      fFieldMap = 1;
      R3BModule* mag = new R3BGladMagnet("GladMagnet");
      mag->SetGeometryFileName("glad_v13a.geo.root");
      run->AddModule(mag);
  }


  if (fDetList.FindObject("CRYSTALBALL") ) {
      //R3B Crystal Calorimeter
      R3BDetector* cal = new R3BCal("CrystalCal", kTRUE);
      cal->SetGeometryFileName("cal_v13a.geo.root");
      run->AddModule(cal);
  }


  if (fDetList.FindObject("CALIFA") ) {
      // CALIFA Calorimeter
      R3BDetector* calo = new R3BCalo("Califa", kTRUE);
      // Global position of the Module
      phi   =  0.0; // (deg)
      theta =  0.0; // (deg)
      psi   =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx    =  0.0; // (cm)
      ty    =  0.0; // (cm)
      tz    =  0.0; // (cm)
      //calo->SetRotAnglesEuler(phi,theta,psi);
      calo->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      calo->SetTranslation(tx,ty,tz);
      run->AddModule(calo);
  }


  // Tracker
  if (fDetList.FindObject("TRACKER")  ) {
      R3BDetector* tra = new R3BTra("Tracker", kTRUE);
      // Global position of the Module
      phi   =  0.0; // (deg)
      theta =  0.0; // (deg)
      psi   =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx    =  0.0; // (cm)
      ty    =  0.0; // (cm)
      tz    =  0.0; // (cm)
      //tra->SetRotAnglesEuler(phi,theta,psi);
      tra->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      tra->SetTranslation(tx,ty,tz);
      // User defined Energy CutOff
      Double_t fCutOffSi = 1.0e-06;  // Cut-Off -> 10KeV only in Si
      ((R3BTra*) tra)->SetEnergyCutOff(fCutOffSi);
      run->AddModule(tra);
  }

  
  // DCH drift chambers
  if (fDetList.FindObject("DCH") ) {
      R3BDetector* dch = new R3BDch("Dch", kTRUE);
      dch->SetGeometryFileName("dch_v13a.geo.root");
      run->AddModule(dch);
  }


  // Tof
  if (fDetList.FindObject("TOF") ) {
      R3BDetector* tof = new R3BTof("Tof", kTRUE);
      // Global position of the Module
      thetaX   =  0.0; // (deg)
      thetaY   =  0.0; // (deg)
      thetaZ   =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx       =  0.0; // (cm)
      ty       =  0.0; // (cm)
      tz       =  0.0; // (cm)
      tof->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      tof->SetTranslation(tx,ty,tz);
      // User defined Energy CutOff
      Double_t fCutOffSci = 1.0e-05;  // Cut-Off -> 10.KeV only in Sci.
      ((R3BTof*) tof)->SetEnergyCutOff(fCutOffSci);
      run->AddModule(tof);
  }


  // mTof
  if (fDetList.FindObject("MTOF") ) {
      R3BDetector* mTof = new R3BmTof("mTof", kTRUE);
      // Global position of the Module
      phi   =  0.0; // (deg)
      theta =  0.0; // (deg)
      psi   =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx    =  0.0; // (cm)
      ty    =  0.0; // (cm)
      tz    =  0.0; // (cm)
      //mTof->SetRotAnglesEuler(phi,theta,psi);
      mTof->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      mTof->SetTranslation(tx,ty,tz);
      // User defined Energy CutOff
      Double_t fCutOffSci = 1.0e-05;  // Cut-Off -> 10.KeV only in Sci.
      ((R3BmTof*) mTof)->SetEnergyCutOff(fCutOffSci);
      run->AddModule(mTof);
  }


  // GFI detector
  if (fDetList.FindObject("GFI") ) {
      R3BDetector* gfi = new R3BGfi("Gfi", kTRUE);
      // Global position of the Module
      phi   =  0.0; // (deg)
      theta =  0.0; // (deg)
      psi   =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx    =  0.0; // (cm)
      ty    =  0.0; // (cm)
      tz    =  0.0; // (cm)
      gfi->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      gfi->SetTranslation(tx,ty,tz);
      // User defined Energy CutOff
      Double_t fCutOffSci = 1.0e-05;  // Cut-Off -> 10.KeV only in Sci.
      ((R3BGfi*) gfi)->SetEnergyCutOff(fCutOffSci);
      run->AddModule(gfi);
  }


  // Land Detector
  if(fDetList.FindObject("LAND")) {
      R3BDetector* land = new R3BLand("Land", kTRUE);
      land->SetVerboseLevel(1);
      land->SetGeometryFileName("land_v12a_10m.geo.root");
      run->AddModule(land);
  }
  
  // NeuLand Scintillator Detector
  if(fDetList.FindObject("SCINTNEULAND")) {
    R3BDetector* land = new R3BLand("Land", kTRUE);
    land->SetVerboseLevel(1);
    land->SetGeometryFileName("neuland_v12a_14m.geo.root");
    run->AddModule(land);
  }


  // Chimera
  if (fDetList.FindObject("CHIMERA") ) {
      R3BDetector* chim = new R3BChimera("Chimera", kTRUE);
      chim->SetGeometryFileName("chimera.root");
      // Global position of the Module
      thetaX   =  0.0; // (deg)
      thetaY   =  0.0; // (deg)
      thetaZ   =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx       =  0.0; // (cm)
      ty       =  0.0; // (cm)
      tz       =  0.0; // (cm)
      chim->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      chim->SetTranslation(tx,ty,tz);
      // User defined Energy CutOff
      //Double_t fCutOffSci = 1.0e-05;  // Cut-Off -> 10.KeV only in Sci.
      //((R3BChimera*) chim)->SetEnergyCutOff(fCutOffSci);
      run->AddModule(chim);
  }


  // Luminosity detector
  if (fDetList.FindObject("LUMON") ) {
      R3BDetector* lumon = new ELILuMon("LuMon", kTRUE);
      //lumon->SetGeometryFileName("lumon.root");
      // Global position of the Module
      thetaX   =  0.0; // (deg)
      thetaY   =  0.0; // (deg)
      thetaZ   =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx       =  0.0; // (cm)
      ty       =  0.0; // (cm)
      tz       =  200.0; // (cm)
      lumon->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      lumon->SetTranslation(tx,ty,tz);
      // User defined Energy CutOff
      //Double_t fCutOffSci = 1.0e-05;  // Cut-Off -> 10.KeV only in Sci.
      //((ELILuMon*) lumon)->SetEnergyCutOff(fCutOffSci);
      run->AddModule(lumon);
  }

  
  // -----   Create R3B  magnetic field ----------------------------------------
  Int_t typeOfMagneticField = 0;
  Double_t fieldScale = 1.;
  Bool_t fVerbose = kFALSE;

  //NB: <D.B>
  // If the Global Position of the Magnet is changed
  // the Field Map has to be transformed accordingly
  cout<<"***************************"<<endl;
  cout<<"fFieldMap "<<fFieldMap<<endl;
  cout<<"fR3BMagnet "<<fR3BMagnet<<endl;
  
  if (fFieldMap == 0) {
    R3BAladinFieldMap* magField = new R3BAladinFieldMap("AladinMaps");
//     R3BFieldMap* magField = new R3BFieldMap(0, kFALSE);
//     Double_t fMeasCurrent = 2000.;// I_current [A]
    Double_t fMeasCurrent = 2500.;// I_current [A]
    magField->SetCurrent(fMeasCurrent);
    magField->SetScale(fieldScale);

    if ( fR3BMagnet == kTRUE ) {
	run->SetField(magField);
    } else {
	run->SetField(NULL);
    }
  } else if(fFieldMap == 1){
    R3BGladFieldMap* magField = new R3BGladFieldMap("R3BGladMap");
    magField->SetPosition(0., 0., +350-119.94);
    magField->SetScale(fieldScale);

    if ( fR3BMagnet == kTRUE ) {
	run->SetField(magField);
    } else {
	run->SetField(NULL);
    }
  }//! end of field map section


  // -----   Create PrimaryGenerator   --------------------------------------
  // 1 - Create the Main API class for the Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  if (fGenerator.CompareTo("ascii") == 0  ) {
    TString iFile = dir + TString("/input/signal.1500AMeV.dat");
//     TString iFile = "/Users/kresan/neuland/input/felix/dp_2pn_1014.dat";
    R3BAsciiGenerator* gen = new R3BAsciiGenerator(iFile.Data());
    primGen->AddGenerator(gen);
  } 

  if (fGenerator.CompareTo("box") == 0  ) {
    // 2- Define the BOX generator
    Double_t pdgId=2212; // proton
    Double_t theta1= 0.0;  // polar angle distribution
    Double_t theta2= 180.0;
    Double_t momentum=1.692041; // 10 GeV/c
//  Double_t momentum=sqrt(2.*500.*938.27231)/1000.; // 10 GeV/c
//  Double_t momentum=1215.649/1000.;  // 600 MeV 
    FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 1);
    boxGen->SetThetaRange (   theta1,   theta2);
    boxGen->SetPRange     (0.,momentum);
    boxGen->SetPhiRange   (0.,360.);
    boxGen->SetXYZ(0.,0.,1250.);
    // add the box generator
    primGen->AddGenerator(boxGen);
  } 

  if (fGenerator.CompareTo("land") == 0  ) {
    // 2- Define the Land generator
    //Double_t pdgId=211; // pion beam
    //Double_t theta1= 0.;  // polar angle distribution
    //Double_t theta2= 7.;
    //Double_t momentum=.8; // 10 GeV/c
    //boxGen->SetThetaRange (   theta1,   theta2);
    //boxGen->SetPRange     (momentum,momentum*2.);
    //boxGen->SetPhiRange   (0.,360.);
    //boxGen->SetXYZ(0.0,0.0,-1.5);
    
    //  const char fname = output.root
    R3BLandGenerator* LandGen = new R3BLandGenerator("output.root");
    
    // add the box generator
    primGen->AddGenerator(LandGen);
  } 

  
  if (fGenerator.CompareTo("r3b") == 0  ) {
    R3BSpecificGenerator *pR3bGen = new R3BSpecificGenerator();
    
    // R3bGen properties
    pR3bGen->SetBeamInteractionFlag("off");
    pR3bGen->SetBeamInteractionFlag("off");
    pR3bGen->SetRndmFlag("off");
    pR3bGen->SetRndmEneFlag("off");
    pR3bGen->SetBoostFlag("off");
    pR3bGen->SetReactionFlag("on");
    pR3bGen->SetGammasFlag("off");
    pR3bGen->SetDecaySchemeFlag("off");
    pR3bGen->SetDissociationFlag("off");
    pR3bGen->SetBackTrackingFlag("off");
    pR3bGen->SetSimEmittanceFlag("off");
    
    // R3bGen Parameters
    pR3bGen->SetBeamEnergy(1.); // Beam Energy in GeV
    pR3bGen->SetSigmaBeamEnergy(1.e-03); // Sigma(Ebeam) GeV
    pR3bGen->SetParticleDefinition(2212); // Use Particle Pdg Code
    pR3bGen->SetEnergyPrim(0.3); // Particle Energy in MeV
    Int_t fMultiplicity = 50;
    pR3bGen->SetNumberOfParticles(fMultiplicity); // Mult.
    
    // Reaction type
    //        1: "Elas"
    //        2: "iso"
    //        3: "Trans"
    pR3bGen->SetReactionType("Elas");
    
    // Target  type
    //        1: "LeadTarget"
    //        2: "Parafin0Deg"
    //        3: "Parafin45Deg"
    //        4: "LiH"
    
    pR3bGen->SetTargetType(Target.Data());
    Double_t thickness = (0.11/2.)/10.;  // cm
    pR3bGen->SetTargetHalfThicknessPara(thickness); // cm
    pR3bGen->SetTargetThicknessLiH(3.5); // cm
    pR3bGen->SetTargetRadius(1.); // cm
    
    pR3bGen->SetSigmaXInEmittance(1.); //cm
    pR3bGen->SetSigmaXPrimeInEmittance(0.0001); //cm

    // Dump the User settings
    pR3bGen->PrintParameters();  
    primGen->AddGenerator(pR3bGen);
  }


  run->SetGenerator(primGen);


  //-------Set visualisation flag to true------------------------------------
  if (fVis==kTRUE){
     run->SetStoreTraj(kTRUE);
  }else{
     run->SetStoreTraj(kFALSE);
  }
  // ------------------------------------------------------------------------   


  // -----   Initialize simulation run   ------------------------------------
  run->Init();


  // ------  Increase nb of step for CALO
  Int_t nSteps = -15000;
  gMC->SetMaxNStep(nSteps);


  // -----   Runtime database   ---------------------------------------------
  R3BFieldPar* fieldPar = (R3BFieldPar*) rtdb->getContainer("R3BFieldPar");
  fieldPar->SetParameters(magField);
  fieldPar->setChanged();
  //fieldPar->setInputVersion(run->GetRunId(),1);
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(ParFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();

   
  // -----   Start run   ----------------------------------------------------
  if(nEvents > 0) {
    run->Run(nEvents);
  }

  
  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Output file is "    << OutFile << endl;
  cout << "Parameter file is " << ParFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime 
       << "s" << endl << endl;
  // ------------------------------------------------------------------------


  cout << " Test passed" << endl;
  cout << " All ok " << endl;
}


