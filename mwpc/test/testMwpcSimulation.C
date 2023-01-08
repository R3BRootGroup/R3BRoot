/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
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

void testMwpcSimulation(int nbevents = 100)
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
    const TString simufile = "test.simu.root";
    // const TString parafile = "test.para.root";

    // Basic simulation setup
    auto run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetStoreTraj(false);
    run->SetMaterials("media_r3b.geo");
    run->SetSink(new FairRootFileSink(simufile));

    // Primary particle generator
    auto boxGen = new FairIonGenerator(82, 208, 82, 1, 0., 0., 1.09, 0., 0., 0.);
    auto primGen = new FairPrimaryGenerator();
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);

    // Geometry: Cave
    auto cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);

    // Geometry: Mwpc0
    run->AddModule(new R3BMwpc0("mwpc_0.geo.root", { 0., 0., 20. }));

    // Geometry: Mwpc1
    run->AddModule(new R3BMwpc1("mwpc_1.geo.root", { 0., 0., 40. }));

    // Geometry: Mwpc2
    // run->AddModule(new R3BMwpc2("mwpc_2.geo.root", { 0., 0., 60. }));

    // Geometry: Mwpc3
    // run->AddModule(new R3BMwpc3("mwpc_3.geo.root", { 0., 0., 100. }));

    // Digitizer: Mwpc0
    auto mw0digitizer = new R3BMwpcDigitizer("Mwpc0", 1);
    run->AddTask(mw0digitizer);

    // Digitizer: Mwpc1
    auto mw1digitizer = new R3BMwpcDigitizer("Mwpc1", 1);
    run->AddTask(mw1digitizer);

    // Digitizer: Mwpc2
    // auto mw2digitizer = new R3BMwpcDigitizer("Mwpc2", 1);
    // run->AddTask(mw2digitizer);

    // Digitizer: Mwpc3
    // auto mw3digitizer = new R3BMwpcDigitizer("Mwpc3", 1);
    // run->AddTask(mw3digitizer);

    // Init
    run->Init();

    // Simulate
    run->Run(nbevents);

    // Report
    timer.Stop();
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
}
