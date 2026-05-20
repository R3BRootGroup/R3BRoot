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

void testNeulandDigitizer()
{
    auto timer = TStopwatch{};
    timer.Start();

    FairLogger::GetLogger()->SetLogScreenLevel("info");

    auto run = FairRunAna{};
    run.SetSource(new FairFileSource("test.simu.root"));
    run.SetSink(new FairRootFileSink("test.digi.root"));

    auto io = new FairParRootFileIo();
    io->open("test.para.root");
    run.GetRuntimeDb()->setFirstInput(io);

    auto digi_options = R3B::Neuland::DigiTaskOptions();
    digi_options.channel = "tamex";
    digi_options.paddle = "neuland";
    digi_options.enable_sim_cal = false;
    auto task = R3B::Neuland::Digitizer::Create(digi_options, &run);
    run.AddTask(task.release());
    run.AddTask(std::make_unique<R3BNeulandClusterFinder>().release());
    run.AddTask(std::make_unique<R3BNeulandPrimaryInteractionFinder>().release());
    run.AddTask(std::make_unique<R3BNeulandPrimaryClusterFinder>().release());

    run.Init();
    run.Run(0, 0);

    timer.Stop();
    std::cout << "Macro finished successfully.\n";
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s\n";
}
