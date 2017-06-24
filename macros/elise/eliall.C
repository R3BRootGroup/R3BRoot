//  -------------------------------------------------------------------------
//
//   ----- General Macro for ELISE simulation
//
//         Author: Denis Bertini <D.Bertini@gsi.de>
//
//         Last Update: 17/08/09
//
//         Comments:
//               - 17/08/09 Adding R3BModule R3BDetector
//                          for global positionning
//               - 12/08/09 Adding R3B Special Physics List
//               - 06/08/09 Adding R3B specific Event Generator
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

void eliall(Int_t nEvents,
            TObjArray& fDetList,
            TString Target,
            Bool_t fVis,
            TString fMC,
            TString fGenerator,
            Bool_t fUserPList,
            Bool_t fR3BMagnet)
{

    TString dir = getenv("VMCWORKDIR");
    TString r3bdir = dir + "/macros";

    TString r3b_geomdir = dir + "/geometry";
    gSystem->Setenv("GEOMPATH", r3b_geomdir.Data());

    TString r3b_confdir = dir + "gconfig";
    gSystem->Setenv("CONFIG_DIR", r3b_confdir.Data());

    // Output files
    TString OutFile = "elisim.root";
    TString ParFile = "elipar.root";

    // In general, the following parts need not be touched
    // ========================================================================

    // ----    Debug option   -------------------------------------------------
    gDebug = 0;
    // ------------------------------------------------------------------------

    // -----   Timer   --------------------------------------------------------
    TStopwatch timer;
    timer.Start();
    // ------------------------------------------------------------------------

    // -----   Create simulation run   ----------------------------------------
    FairRunSim* run = new FairRunSim();
    run->SetName(fMC.Data());           // Transport engine
    run->SetOutputFile(OutFile.Data()); // Output file
    FairRuntimeDb* rtdb = run->GetRuntimeDb();

    //  R3B Special Physics List in G4 case
    if ((fUserPList == kTRUE) && (fMC.CompareTo("TGeant4") == 0))
    {
        run->SetUserConfig("g4R3bConfig.C");
        run->SetUserCuts("SetCuts.C");
    }

    // -----   Create media   -------------------------------------------------
    run->SetMaterials("media_r3b.geo"); // Materials

    // Global Transformation

    //- Two ways for a Volume Rotation are supported
    //-- 1) Global Rotation (Euler Angles definition)
    //-- This represent the composition of : first a rotation about Z axis with
    //-- angle phi, then a rotation with theta about the rotated X axis, and
    //-- finally a rotation with psi about the new Z axis.
    Double_t phi, theta, psi;

    //-- 2) Rotation in Ref. Frame of the Volume
    //-- Rotation is Using Local Ref. Frame axis angles
    Double_t thetaX, thetaY, thetaZ;

    //- Global Translation  Lab. frame.
    Double_t tx, ty, tz;

    // -----   Create R3B geometry --------------------------------------------
    // R3B Cave definition
    FairModule* cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);

    // R3B Target definition
    if (fDetList.FindObject("TARGET"))
    {
        R3BModule* target = new R3BTarget(Target);
        run->AddModule(target);
    }

    // R3B Magnet definition
    if (fDetList.FindObject("ALADIN"))
    {
        fFieldMap = 0;
        run->AddModule(new R3BAladinMagnet("aladin_v13a.geo.root"));
    }

    // R3B Crystal Calorimeter
    if (fDetList.FindObject("CRYSTALBALL"))
    {
        run->AddModule(new R3BXBall("cal_v13a.geo.root"));
    }

    // CALIFA Calorimeter
    if (fDetList.FindObject("CALIFA"))
    {
        R3BCalifa* califa = new R3BCalifa("califa_v13_811.geo.root");
        califa->SelectGeometryVersion(10);
        // Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
        califa->SetNonUniformity(1.0);
        run->AddModule(califa);
    }

    // Luminosity detector
    if (fDetList.FindObject("LUMON"))
    {
        run->AddModule(new ELILuMon("lumon_v13a.geo.root"));
    }

    // -----   Create R3B  magnetic field ----------------------------------------
    Int_t typeOfMagneticField = 0;
    Int_t fieldScale = 1;
    Bool_t fVerbose = kFALSE;

    // NB: <D.B>
    // If the Global Position of the Magnet is changed
    // the Field Map has to be transformed accordingly
    R3BFieldMap* magField = new R3BFieldMap(typeOfMagneticField, fVerbose);
    magField->SetPosition(0., 0., 0.);
    magField->SetScale(fieldScale);

    if (fR3BMagnet == kTRUE)
    {
        run->SetField(magField);
    }
    else
    {
        run->SetField(NULL);
    }

    // -----   Create PrimaryGenerator   --------------------------------------

    // 1 - Create the Main API class for the Generator
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();

    if (fGenerator.CompareTo("box") == 0)
    {
        // 2- Define the BOX generator
        Double_t pdgId = 22;  // gamma beam
        Double_t theta1 = 0.; // polar angle distribution
        Double_t theta2 = .1;
        Double_t momentum = .2; // 0.1 GeV/c
        FairBoxGenerator* boxGen = new FairBoxGenerator(pdgId, 50);
        boxGen->SetThetaRange(theta1, theta2);
        boxGen->SetPRange(momentum, momentum);
        boxGen->SetPhiRange(0., 0.);
        boxGen->SetXYZ(0.0, 0.0, 0.0);
        // add the box generator
        primGen->AddGenerator(boxGen);
    }

    if (fGenerator.CompareTo("r3b") == 0)
    {
        R3BSpecificGenerator* pR3bGen = new R3BSpecificGenerator();

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
        pR3bGen->SetBeamEnergy(1.);           // Beam Energy in GeV
        pR3bGen->SetSigmaBeamEnergy(1.e-03);  // Sigma(Ebeam) GeV
        pR3bGen->SetParticleDefinition(2212); // Use Particle Pdg Code
        pR3bGen->SetEnergyPrim(0.3);          // Particle Energy in MeV
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
        Double_t thickness = (0.11 / 2.) / 10.;         // cm
        pR3bGen->SetTargetHalfThicknessPara(thickness); // cm
        pR3bGen->SetTargetThicknessLiH(3.5);            // cm
        pR3bGen->SetTargetRadius(1.);                   // cm

        pR3bGen->SetSigmaXInEmittance(1.);          // cm
        pR3bGen->SetSigmaXPrimeInEmittance(0.0001); // cm

        // Dump the User settings
        pR3bGen->PrintParameters();
        primGen->AddGenerator(pR3bGen);
    }

    run->SetGenerator(primGen);

    //-------Set visualisation flag to true------------------------------------
    if (fVis == kTRUE)
    {
        run->SetStoreTraj(kTRUE);
    }
    else
    {
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
    if (nEvents > 0)
        run->Run(nEvents);

    // -----   Finish   -------------------------------------------------------
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << OutFile << endl;
    cout << "Parameter file is " << ParFile << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;
    // ------------------------------------------------------------------------

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
}
