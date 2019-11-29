/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

inline void ConnectParFileToRuntimeDb(const TString parFile, FairRuntimeDb* rtdb)
{
    auto io = new FairParRootFileIo();
    io->open(parFile);
    rtdb->setFirstInput(io);
    rtdb->setOutput(io);
    rtdb->saveOutput();
}

void testNeulandDigitizer(const TString simFile = "test.sim.root")
{
    TStopwatch timer;
    timer.Start();

    const TString parFile = TString(simFile).ReplaceAll(".sim.", ".par.");
    const TString outFile = TString(simFile).ReplaceAll(".sim.", ".digi.");

    FairRunAna run;
    run.SetSource(new FairFileSource(simFile));
    run.SetSink(new FairRootFileSink(outFile));
    ConnectParFileToRuntimeDb(parFile, run.GetRuntimeDb());

    run.AddTask(new R3BNeulandDigitizer());

    run.Init();
    run.Run(0, 0);

    timer.Stop();
    cout << "Macro finished succesfully!" << endl;
    cout << "Output file writen: " << outFile << endl;
    cout << "Parameter file writen: " << parFile << endl;
    cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << endl;
}
