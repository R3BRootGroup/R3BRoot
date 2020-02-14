void MCtransport(Int_t const TotalNumberOfThreads = 1, Int_t const CurrentThread = 0)
{
    // Connect to InputsFile:
    R3BInputClass* Inputs = new R3BInputClass();
    Inputs->DisableErrorPrinting();
    Inputs->LinkFile("../InputFiles/InputsFile.txt");
    Inputs->ReadFile();
    Inputs->CheckPhysicsList();
    
    // Manually define probabilities. Put <ParticleGun_MultDistribution> to <Custom> in the inputs!
    std::vector<Double_t> Multprobs;
    Multprobs.push_back(0.2);
    Multprobs.push_back(0.2);
    Multprobs.push_back(0.2);
    Multprobs.push_back(0.2);
    Multprobs.push_back(0.2);
    
    // Generate a new ASCII-file for the particle gun:
    R3BASCIIFileGenerator* ASCIIGen = new R3BASCIIFileGenerator();
    ASCIIGen->DisableErrorPrinting();
    ASCIIGen->SetNThreads(TotalNumberOfThreads,CurrentThread);
    ASCIIGen->LinkInputClass(Inputs);
    // ASCIIGen->SetCutsomBoundaries(Multprobs);
    ASCIIGen->ObtainParticleGun();
    ASCIIGen->CreateFile();
    
    // Obtain the required inputs:
    Int_t nEvents = Inputs->GetInputInteger("R3BRoot_nEvents");
    TString TheVMC = Inputs->GetInputString("TheVMCFolder");
    TString TheOutputPath = Inputs->GetInputString("TheOutputPath");
    TString ASCII_DataFile = TheOutputPath + Inputs->GetInputString("ParticleGun_ASCII_FileName");
    TString OutputFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_OutputFile");
    TString TheParFile = TheOutputPath + Inputs->GetInputString("R3BRoot_MonteCarlo_ParFile");
    Bool_t CaveIsVac = Inputs->GetInputBoolian("R3BRoot_Cave_IsVacuum");
    
    // Manage MultiThreading:
    if (TotalNumberOfThreads>1)
    {
        TString nst = "";
        TString nT_tot_str = nst.Itoa(TotalNumberOfThreads,10);
        TString nT_cur_str = nst.Itoa(CurrentThread,10);
        OutputFile = OutputFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
        TheParFile = TheParFile.ReplaceAll(".root","_nT"+nT_tot_str+"_cT"+nT_cur_str+".root");
        ASCII_DataFile = ASCII_DataFile.ReplaceAll(".dat","_nT"+nT_tot_str+"_cT"+nT_cur_str+".dat");
        nEvents = nEvents/TotalNumberOfThreads;
    }
    
    // Target type
    TString target1 = "LeadTarget";
    TString target2 = "Para";
    TString target3 = "Para45";
    TString target4 = "LiH";
    TString targetType = target4;

    // ========================================================================
    
    // NOTE: R3B Simulation Macro starts here!
    if ((Inputs->ContainsNoErrors()==kTRUE)&&(ASCIIGen->ContainsNoErrors()))
    {
        
    // ------------------------------------------------------------------------
    
    // Set the general stuff:
        
    // Define the physics list that should be used:   
    gSystem->Setenv("PHYSICSLIST",(Inputs->GetInputString("R3BRoot_MonteCarlo_PhysicsList")).Data());
    // NOTE: This call will define an path-environment variable OUTSIDE of
    // ROOT (and into bash, just like the export SIMPATH in .bashrc). This variable
    // is read out by other R3BRoot macros that are automatically loaded.    

    // Define the Debug option:
    gDebug = 0;

    // Ignite the time:
    TStopwatch timer;
    timer.Start();

    // Create the primary simulation run:
    FairRunSim* run = new FairRunSim();
    
    // Select the Monte Carlo transport engine (TGeant3, TGeant4, Fluka):
    run->SetName((Inputs->GetInputString("R3BRoot_MonteCarlo_Engine")).Data());
    
    // Create the output file:
    run->SetOutputFile(OutputFile.Data());
    
    // Extract the runtime database:
    FairRuntimeDb* rtdb = run->GetRuntimeDb();
    
    // ------------------------------------------------------------------------
    
    // Create the geometry of the simulation:

    // Create the materials (the media):
    run->SetMaterials("media_r3b.geo");

    // Create the R3B cave:
    FairModule* cave = new R3BCave("CAVE");
    
    if (CaveIsVac==kTRUE) {cave->SetGeometryFileName("r3b_cave_vacuum.geo");} else {cave->SetGeometryFileName("r3b_cave.geo");}
    run->AddModule(cave);

    // Add the different detector modules to the macro. Start with NeuLAND:
    if (Inputs->GetInputBoolian("NeuLAND_Use_Original")==kTRUE)
    {
        // Then, add the Jan Mayers original NeuLAND, with all possible upgrades:
        run->AddModule(new R3BNeuland(TheOutputPath + Inputs->GetInputString("NeuLAND_Geometry_FileName"),{0.0,0.0,0.0}));
    }
    else
    {
        // Then, we use our own copy of NeuLAND. This is independent of the rest of R3BRoot and should, therefore, always work.
        run->AddModule(new R3BCADNeuland(TheOutputPath + Inputs->GetInputString("NeuLAND_Geometry_FileName"),{0.0,0.0,0.0}));
    }
    
    // Also add our VETO:
    if (Inputs->GetInputBoolian("VETO_Include_in_SETUP")==kTRUE)
    {
        run->AddModule(new R3BCADVETO(TheOutputPath + Inputs->GetInputString("VETO_Geometry_FileName"),{0.0,0.0,0.0}));
    }
    
    // Add NEBULA:
    if (Inputs->GetInputBoolian("NEBULA_Include_in_SETUP")==kTRUE)
    {
        run->AddModule(new R3BCADNEBULA(TheOutputPath + Inputs->GetInputString("NEBULA_Geometry_FileName"),{0.0,0.0,0.0}));
    }
    
    // Also add the NEBULA VETO:
    if (Inputs->GetInputBoolian("NEBULA_VET_Include_in_SETUP")==kTRUE)
    {
        run->AddModule(new R3BCADNEBULAVETO(TheOutputPath + Inputs->GetInputString("NEBULA_VET_Geometry_FileName"),{0.0,0.0,0.0}));
    }
    
    // Add SAMUARI setup:
    Bool_t IncludeSAMURAI = Inputs->GetInputBoolian("SAMURAI_SETUP_IncludeSetup");
    if (IncludeSAMURAI==kTRUE)
    {
        Double_t SAMURAI_xpos = Inputs->GetInputDouble("SAMURAI_SETUP_center_x_position","cm");
        Double_t SAMURAI_ypos = Inputs->GetInputDouble("SAMURAI_SETUP_center_y_position","cm");
        Double_t SAMURAI_zpos = Inputs->GetInputDouble("SAMURAI_SETUP_center_z_position","cm");
        run->AddModule(new R3BGeoGDML(TheVMC + "/DNN/Geometry/SAMURAI_MagnetSystem.gdml", TGeoTranslation(SAMURAI_xpos,SAMURAI_ypos,SAMURAI_zpos)));
    }
    
    // GLAD magnet; This should not be moved or rotated, since one then has to
    // transform the magnetic field too.
    Bool_t IncludeGLADmagnet = Inputs->GetInputBoolian("GLAD_Magnet_IncludeInSetup");
    if (IncludeGLADmagnet==kTRUE)
    {
        // run->AddModule(new R3BGladMagnet(TheOutputPath + Inputs->GetInputString("GLAD_Geometry_FileName")));
        // run->AddModule(new R3BGladMagnet(TheVMC + "/geometry/" + "glad_v17_flange.geo.root"));
    }
    
    // --------------------
    
    /*
    
    // Target
    run->AddModule(new R3BTarget(targetType, "target_" + targetType + ".geo.root"));

    // GLAD
    run->AddModule(new R3BGladMagnet("glad_v17_flange.geo.root")); // GLAD should not be moved or rotated

    // PSP
    run->AddModule(new R3BPsp("psp_v13a.geo.root", {}, -221., -89., 94.1));

    // R3B SiTracker Cooling definition
    run->AddModule(new R3BVacVesselCool(targetType, "vacvessel_v14a.geo.root"));

    // STaRTrack
    run->AddModule(new R3BStartrack("startrack_v16-300_2layers.geo.root", { 0., 0., 20. }));
    //run->AddModule(new R3BSTaRTra("startra_v16-300_2layers.geo.root", { 0., 0., 20. }));

    // CALIFA
    R3BCalifa* califa = new R3BCalifa("califa_10_v8.11.geo.root");
    califa->SelectGeometryVersion(10);
    // Selecting the Non-uniformity of the crystals (1 means +-1% max deviation)
    califa->SetNonUniformity(1.0);
    run->AddModule(califa);
    
    // Fi4 detector
    run->AddModule(new R3BFi4("fi4_v17a.geo.root", {-73.274339-TMath::Tan(TMath::DegToRad()*16.7)*100, 0.069976, 513.649524+100.}, {"" ,-90.,16.7,90.}));

    // Fi6 detector
    run->AddModule(new R3BFi6("fi6_v17a.geo.root", {-73.274339-TMath::Tan(TMath::DegToRad()*16.7)*500, 0.069976, 513.649524+500.}, {"" ,-90.,16.7,90.}));

    // Fi5 detector
    run->AddModule(new R3BFi5("fi5_v17a.geo.root", {-73.274339-TMath::Tan(TMath::DegToRad()*16.7)*300, 0.069976, 513.649524+300.}, {"" ,-90.,16.7,90.}));

    // sfi detector
    run->AddModule(new R3Bsfi("sfi_v17a.geo.root", {0, 0, -200}));

    // Tof
    run->AddModule(new R3BTof("tof_v17a.geo.root", { -417.359574, 2.400000, 960.777114 }, { "", -90., +31., 90. }));

    // dTof
    run->AddModule(new R3BdTof("dtof_v17a.geo.root", { -155.824045+(2.7*10)*TMath::Cos(16.7*TMath::DegToRad()), 0.523976, 761.870346 }, { "", -90., +16.7, 90. }));

    */

    // ------------------------------------------------------------------------
    
    // Build the R3B magnetic field. Note that if the Global Position 
    // of the Magnet is changed, the Field Map has to be transformed accordingly.
    R3BGladFieldMap* magField = new R3BGladFieldMap("R3BGladMap");
    Double_t fieldScale = -0.6*Inputs->GetInputDouble("GLAD_MagneticFieldScale","percent");
    Bool_t BuildMagneticField = Inputs->GetInputBoolian("GLAD_MagneticField_IncludeInSetup");
    magField->SetScale(fieldScale/100.0);

    if (BuildMagneticField==kTRUE)
    {
        run->SetField(magField);
    }
    else
    {
        run->SetField(NULL);
    }    
    
    // ------------------------------------------------------------------------

    // Create the particle gun (primary generator):
    
    // Declare the generator:
    FairPrimaryGenerator* primGen = new FairPrimaryGenerator();
    
    // In our simulations, we only work with ASCII-files:
    R3BAsciiGenerator* gen = new R3BAsciiGenerator(ASCII_DataFile.Data());
    primGen->AddGenerator(gen);

    // Add the primary generator to the run:
    run->SetGenerator(primGen);
    run->SetStoreTraj(Inputs->GetInputBoolian("R3BRoot_StoreTrajectories"));
    FairLogger::GetLogger()->SetLogVerbosityLevel("LOW");
    FairLogger::GetLogger()->SetLogScreenLevel("INFO");
    
    // ------------------------------------------------------------------------
    
    // Initialize the simulation run:
    
    Int_t randomSeed = 335566;
    if (Inputs->GetInputBoolian("R3BRoot_RandomSeed_IsTimeDependent")==kTRUE) {randomSeed = 0;}
    if (TotalNumberOfThreads>1) {randomSeed = 0;}
    
    run->Init();
    TVirtualMC::GetMC()->SetRandom(new TRandom3(randomSeed));

    // Increase nb of step for CALO:
    TVirtualMC::GetMC()->SetMaxNStep(-1*Inputs->GetInputInteger("R3BRoot_MonteCarloTransport_nMaxSteps"));

    // Properly define the Runtime database:
    R3BFieldPar* fieldPar = (R3BFieldPar*)rtdb->getContainer("R3BFieldPar");
    if (NULL != magField)
    {
        fieldPar->SetParameters(magField);
        fieldPar->setChanged();
    }
    
    Bool_t kParameterMerged = kTRUE;
    FairParRootFileIo* parOut = new FairParRootFileIo(kParameterMerged);
    parOut->open(TheParFile.Data());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();

    // ------------------------------------------------------------------------
    
    // Run the simulation:
    if (nEvents>0)
    {
        run->Run(nEvents);
    }

    // ------------------------------------------------------------------------
    
    // Finish up:
    
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    cout << endl << endl;
    cout << "Macro finished succesfully." << endl;
    cout << "Output file is " << OutputFile.Data() << endl;
    cout << "Parameter file is " << TheParFile.Data() << endl;
    cout << "Real time " << rtime << " s, CPU time " << ctime << "s" << endl << endl;

    cout << " Test passed" << endl;
    cout << " All ok " << endl;
    
    } // Close big IF-loop:
    
    // ========================================================================
    
    else
    {
        // This means that our own classes gave errors. So print them:
        Inputs->PrintAllErrors();
        ASCIIGen->PrintAllErrors();
    }
}
