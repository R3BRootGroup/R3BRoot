/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#include <TStopwatch.h>
#include <TString.h>
#include <TSystem.h>
#include <memory>

void testGladSimulation(const int nbevents = 100)
{
    // Timer
    TStopwatch timer;
    timer.Start();

    // Logging
    auto logger = FairLogger::GetLogger();
    logger->SetLogVerbosityLevel("low");
    logger->SetLogScreenLevel("warn");
    logger->SetColoredLog(true);

    // System paths
    const TString workDirectory = getenv("VMCWORKDIR");
    gSystem->Setenv("GEOMPATH", workDirectory + "/geometry");
    gSystem->Setenv("CONFIG_DIR", workDirectory + "/gconfig");

    // Output files
    const TString simufile = "glad.simu.root";
    const TString parafile = "glad.para.root";

    // Input GLAD geometry
    const TString fGladGeo = "glad_v2025.1.geo.root";

    // Basic simulation setup
    auto run = std::make_unique<FairRunSim>();
    run->SetName("TGeant4");
    run->SetStoreTraj(false);
    run->SetMaterials("media_r3b.geo");

    auto config = std::make_unique<FairGenericVMCConfig>();
    run->SetSimulationConfig(std::move(config));
    run->SetSink(std::make_unique<FairRootFileSink>(simufile.Data()));

    // -----   Runtime data base   --------------------------------------------
    auto* rtdb = run->GetRuntimeDb();
    UInt_t runId = 1;
    rtdb->initContainers(runId);

    // Primary particle generator
    auto ionGen = std::make_unique<FairIonGenerator>(92, 238, 92, 1, 0., 0., 1.09, 0., 0., 0.);
    auto primGen = std::make_unique<FairPrimaryGenerator>();
    primGen->AddGenerator(ionGen.release());
    run->SetGenerator(primGen.release());

    // Geometry: Cave
    auto cave = std::make_unique<R3BCave>("CAVE");
    cave->SetGeometryFileName("r3b_cave_vacuum.geo");
    run->AddModule(cave.release());

    // Geometry: GLAD
    run->AddModule(new R3BGladMagnet(fGladGeo.Data()));

    // GLAD Filed
    auto* GladField = new R3BGladFieldMap("R3BGladMap");
    GladField->SetFieldfromCurrent(2600.); // Current in Amperes
    run->SetField(GladField);

    // Init
    run->Init();

    // Save field parameters
    auto* fieldPar = dynamic_cast<R3BFieldPar*>(rtdb->getContainer("R3BFieldPar"));
    fieldPar->SetParameters(GladField);
    fieldPar->setChanged();

    // Output file with parameters
    auto parOut = std::make_unique<FairParRootFileIo>(true);
    parOut->open(parafile.Data());
    rtdb->setOutput(parOut.release());
    rtdb->saveOutput();
    rtdb->print();

    // Simulate
    run->Run(nbevents);

    // Report
    timer.Stop();
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
}
