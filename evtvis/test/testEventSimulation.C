/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
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

void testEventSimulation(const int nbevents = 10)
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
    const TString parafile = "test.para.root";

    // Basic simulation setup
    auto run = new FairRunSim();
    run->SetName("TGeant4");
    run->SetStoreTraj(false);
    run->SetMaterials("media_r3b.geo");
    run->SetSink(new FairRootFileSink(simufile));

    auto rtdb = run->GetRuntimeDb();
    rtdb->initContainers(1);

    // Primary particle generator
    auto boxGen = new FairBoxGenerator(2212, 3);
    boxGen->SetXYZ(0, 0, 0.);
    boxGen->SetThetaRange(0., 145.);
    boxGen->SetPhiRange(0., 360.);
    boxGen->SetEkinRange(0.4, 0.8);
    auto primGen = new FairPrimaryGenerator();
    primGen->AddGenerator(boxGen);
    run->SetGenerator(primGen);

    // Geometry: Cave
    auto cave = new R3BCave("CAVE");
    cave->SetGeometryFileName("r3b_cave.geo");
    run->AddModule(cave);

    // Geometry: GLAD
    run->AddModule(new R3BGladMagnet("glad_v2025.1.geo.root"));

    // Magnetic field for GLAD
    auto magField = new R3BGladFieldMap("R3BGladMap");
    run->SetField(magField);
    auto fieldPar = static_cast<R3BFieldPar*>(rtdb->getContainer("R3BFieldPar"));
    fieldPar->SetParameters(magField);
    fieldPar->setChanged();

    // Geometry: Califa
    auto calsim = new R3BCalifa("califa_full.geo.root", { 0., 0., 0. });
    calsim->SelectGeometryVersion(0);
    run->AddModule(calsim);

    // Digitizer: Califa
    auto califaDig = new R3BCalifaDigitizer();
    run->AddTask(califaDig);

    auto califaCal2Cluster = new R3BCalifaCrystalCal2Cluster();
    califaCal2Cluster->SetCrystalThreshold(0.1); // 100 keV
    run->AddTask(califaCal2Cluster);

    // Init
    run->Init();

    // -----   Runtime database   ---------------------------------------------
    auto parOut = new FairParRootFileIo(true);
    parOut->open(parafile.Data());
    rtdb->setOutput(parOut);
    rtdb->saveOutput();
    rtdb->print();

    // Simulate
    run->Run(nbevents);

    // Report
    timer.Stop();
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
    std::cout << "Macro finished successfully." << std::endl;
}
