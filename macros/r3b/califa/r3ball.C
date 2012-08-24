//  -------------------------------------------------------------------------
//
//   ----- General Macro for R3B simulation
//
//         Author: Hector Alvarez <hector.alvarez@usc.es>
//
//         Last Update: 25/04/2012
//
//         Comments:
//
//
//  -------------------------------------------------------------------------
//
//   Usage inside ROOT interpreter: 
//        1) root -l
//        2)[root] .L r3ball.C 
//                         
//        3)[root] r3ball( nevt,
//                         fDetList,     // List of Detectors
//                         TargetType,    // "LeadTarget" 
//                         Visualization, // kFalse or kTRUE   
//                         fMC ,        // "TGeant3" or "TGeant4"   
//                         fGenerator   // Generator type
//
//  -------------------------------------------------------------------------



void r3ball(Int_t nEvents = 1,
	    TObjArray& fDetList,
	    TString Target = "LeadTarget",
		Bool_t fVis=kFALSE,
		TString fMC="TGeant3",
	    TString fGenerator="box",
	    Bool_t fUserPList= kFALSE,
		Bool_t fR3BMagnet= kTRUE )
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


  // In general, the following parts need not be touched
  // ========================================================================

  // ----    Debug option   -------------------------------------------------
  gDebug = 0;
  // ------------------------------------------------------------------------

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  // ----  Load libraries   -------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();

  gSystem->Load("libGenVector");

  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libMCStack");
  gSystem->Load("libField");
  gSystem->Load("libGen");

  //----  Load R3B specific libraries ---------------------------------------
  gSystem->Load("libR3Bbase");
  gSystem->Load("libR3BGen");
  gSystem->Load("libR3BPassive");
  gSystem->Load("libR3BData");
  gSystem->Load("libR3BCal");
  gSystem->Load("libR3BCalo");
  gSystem->Load("libR3BDch");
  gSystem->Load("libR3BGfi");
  gSystem->Load("libR3BLand");
  gSystem->Load("libR3BmTof");
  gSystem->Load("libR3BTof");
  gSystem->Load("libR3BTra");
  //gSystem->Load("libR3BChimera");
  gSystem->Load("libELILuMon");
 
  // -----   Create simulation run   ----------------------------------------
  FairRunSim* run = new FairRunSim();
  run->SetName(fMC.Data());                  // Transport engine
  run->SetOutputFile(OutFile.Data());        // Output file
  FairRuntimeDb* rtdb = run->GetRuntimeDb();

  //  R3B Special Physics List in G4 case
  if ( (fUserPList  == kTRUE ) &&
       (fMC.CompareTo("TGeant4")   == 0)
      ){
       run->SetUserConfig("g4R3bConfig.C");
       run->SetUserCuts("SetR3BCuts.C");
   }


  // -----   Create media   -------------------------------------------------
  run->SetMaterials("media_r3b.geo");       // Materials
  

  // Magnetic field map type
   Int_t fFieldMap = 0;


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
      // Global Lab. Rotation
      phi    =  0.0; // (deg)
      theta  =  0.0; // (deg)
      psi    =  0.0; // (deg)
      // Rotation in Ref. Frame.
      thetaX =  0.0; // (deg)
      thetaY =  0.0; // (deg)
      thetaZ =  0.0; // (deg)
      // Global translation in Lab
      tx    =  0.0; // (cm)
      ty    =  0.0; // (cm)
      tz    =  0.0; // (cm)
     //target->SetRotAnglesEuler(phi,theta,psi);
     target->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
     target->SetTranslation(tx,ty,tz);
      run->AddModule(target);
  }

  //R3B Magnet definition
  if (fDetList.FindObject("ALADIN") ) {
      fFieldMap = 0;
      R3BModule* mag = new R3BMagnet("AladinMagnet");
      // Selecting the geometry
          // 1- Old geometry(default).
          // 2- New geometry.
          ((R3BMagnet *)mag)->SelectGeometry(1);
      // Selecting the gap medium
          // 0- Air(default)
          // 1- Vacuum
          // 2- Helium
          ((R3BMagnet *)mag)->SetGapMedium(2);
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
      //mag->SetRotAnglesEuler(phi,theta,psi);
      mag->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      mag->SetTranslation(tx,ty,tz);
      run->AddModule(mag);
  }

    //R3B Magnet definition
  if (fDetList.FindObject("GLAD") ) {
      fFieldMap = 1;
      R3BModule* mag = new R3BGladMagnet("GladMagnet");
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
      //mag->SetRotAnglesEuler(phi,theta,psi);
      mag->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      mag->SetTranslation(tx,ty,tz);
      run->AddModule(mag);
  }


  if (fDetList.FindObject("CRYSTALBALL") ) {
      //R3B Crystal Calorimeter
      R3BDetector* cal = new R3BCal("CrystalCal", kTRUE);
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
      //cal->SetRotAnglesEuler(phi,theta,psi);
      cal->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      cal->SetTranslation(tx,ty,tz);
      run->AddModule(cal);
  }

  if (fDetList.FindObject("CALIFA") ) {
      // CALIFA Calorimeter
      R3BDetector* calo = new R3BCalo("Califa", kTRUE);
	  //Selecting the geometry
	  // 0- CALIFA 5.0, including BARREL and ENDCAP.
	  // 1- CALIFA 7.05, only BARREL
	  // 2- CALIFA 7.07, only BARREL
	  // 3- CALIFA 7.09, only BARREL (ongoing work)
	  // 4- CALIFA 7.17, only ENDCAP (in CsI[Tl])
	  // 5- CALIFA 7.07+7.17, 
	  // 6- CALIFA 7.09+7.17, (ongoing work)
	  // 10- CALIFA 8.11, 
	  // ...
	  ((R3BCalo *)calo)->SelectGeometryVersion(10);
	  //Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
	  ((R3BCalo *)calo)->SetNonUniformity(1.0);
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
      //calo->SetVerboseLevel(2);
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
      ((R3BDch*) dch )->SetHeliumBag(kTRUE);
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
     //dch->SetRotAnglesEuler(phi,theta,psi);
      dch->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      dch->SetTranslation(tx,ty,tz);
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
  if (fDetList.FindObject("LAND") ) {
      R3BDetector* land = new R3BLand("Land", kTRUE);
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
      tz    =  1000.0; // (cm)
      //land->SetRotAnglesEuler(phi,theta,psi);
      land->SetRotAnglesXYZ(thetaX,thetaY,thetaZ);
      land->SetTranslation(tx,ty,tz);
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
  Int_t fieldScale = 1;
  Bool_t fVerbose = kFALSE;

  //NB: <D.B>
  // If the Global Position of the Magnet is changed
  // the Field Map has to be transformed accordingly

  if (fFieldMap == 0) {
    R3BFieldMap* magField = new R3BFieldMap(typeOfMagneticField,fVerbose);
    magField->SetPosition(0., 0., 0.);
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
  }  //! end of field map section



  // -----   Create PrimaryGenerator   --------------------------------------

  // 1 - Create the Main API class for the Generator
  FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

  if (fGenerator.CompareTo("ion") == 0  ) {
	  // R3B Ion Generator
          Int_t z = 30;  // Atomic number
          Int_t a = 65; // Mass number
          Int_t q = 0;   // Charge State 
          Int_t m = 1;   // Multiplicity
          Double_t px = 40./a;  // X-Momentum / per nucleon!!!!!!
          Double_t py = 600./a;  // Y-Momentum / per nucleon!!!!!!
          Double_t pz = 0.01/a;  // Z-Momentum / per nucleon!!!!!!
	  R3BIonGenerator* ionGen = new R3BIonGenerator(z,a,q,m,px,py,pz);
	  ionGen->SetSpotRadius(1,1,0);
	  // add the ion generator
	  primGen->AddGenerator(ionGen);
  } 
	
  if (fGenerator.CompareTo("ascii") == 0  ) {
	  // R3B Ascii Generator
	  R3BAsciiGenerator* asciiGen = new R3BAsciiGenerator("muchos_iones.txt");
	  // add the ascii generator
	  primGen->AddGenerator(asciiGen);
  } 
	

  if (fGenerator.CompareTo("box") == 0  ) {
	  // 2- Define the BOX generator
	  Double_t pdgId=211; // pion beam
	  Double_t theta1= 30.;  // polar angle distribution
	  //Double_t theta2= 7.;
	  Double_t theta2= 160.;
	  Double_t momentum=.8; // 10 GeV/c
	  FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 10);
	  boxGen->SetThetaRange (   theta1,   theta2);
	  boxGen->SetPRange     (momentum,momentum*2.);
	  boxGen->SetPhiRange   (0.,360.);
	  //boxGen->SetXYZ(0.0,0.0,-1.5);
	  boxGen->SetXYZ(0.0,0.0,0.0);
	  // add the box generator
	  primGen->AddGenerator(boxGen);
  } 
	
  if (fGenerator.CompareTo("gammas") == 0  ) {
	// 2- Define the CALIFA Test gamma generator
	Double_t pdgId=22; // gamma emission
	//Double_t pdgId=2212; // proton emission
	Double_t theta1= 90.;  // polar angle distribution
	Double_t theta2= 180.;	
	//Double_t theta2= 90.;	
	//Double_t momentum=0.002; // 0.010 GeV/c = 10 MeV/c 
	Double_t momentumI=0.019; // 0.010 GeV/c = 10 MeV/c 
	Double_t momentumF=0.045; // 0.010 GeV/c = 10 MeV/c 
	//Double_t momentumF=0.808065; // 0.808065 GeV/c (300MeV Kin Energy for protons) 
	//Double_t momentumI=0.31016124; // 0.31016124 GeV/c (50MeV Kin Energy for protons)
	//Double_t momentum=0.4442972; // 0.4442972 GeV/c (100MeV Kin Energy for protons)
	//Double_t momentum=0.5509999; // 0.5509999 GeV/c (150MeV Kin Energy for protons)
	//Double_t momentumI=0.64405; // 0.64405 GeV/c (200MeV Kin Energy for protons) 
	Int_t multiplicity = 1;
	R3BCALIFATestGenerator* gammasGen = new R3BCALIFATestGenerator(pdgId, multiplicity);
	gammasGen->SetThetaRange (theta1,   theta2);
	gammasGen->SetCosTheta();
	gammasGen->SetPRange(momentumI,momentumF);
	gammasGen->SetPhiRange(0.,360.);
	//gammasGen->SetXYZ(0.0,0.0,-1.5);
	//gammasGen->SetXYZ(0.0,0.0,0);
	gammasGen->SetBoxXYZ(-0.1,0.1,-0.1,0.1,-0.1,0.1);
	gammasGen->SetLorentzBoost(0.8197505718204776); //beta=0.81975 for 700 A MeV
	// add the gamma generator
	primGen->AddGenerator(gammasGen);
  } 
	

  if (fGenerator.CompareTo("r3b") == 0  ) {
	  R3BSpecificGenerator *pR3bGen = new R3BSpecificGenerator();

	  // R3bGen properties
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

  // -----   Initialize simulation run   ------------------------------------
  run->Init();

  // ------  Increase nb of step for CALO
  Int_t nSteps = -15000;
  gMC->SetMaxNStep(nSteps);

  // -----   Runtime database   ---------------------------------------------
  Bool_t kParameterMerged = kTRUE;
  FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
  parOut->open(ParFile.Data());
  rtdb->setOutput(parOut);
  rtdb->saveOutput();
  rtdb->print();
   
  // -----   Start run   ----------------------------------------------------
  if (nEvents>0) run->Run(nEvents);
  
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


