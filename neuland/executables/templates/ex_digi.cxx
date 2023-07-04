#include "FairFileSource.h"
#include "FairParRootFileIo.h"
#include "FairRootFileSink.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "R3BDigitizingChannelMock.h"
#include "R3BDigitizingPaddleMock.h"
#include "R3BDigitizingPaddleNeuland.h"
#include "R3BDigitizingTacQuila.h"
#include "R3BDigitizingTamex.h"
#include "R3BNeulandDigitizer.h"
#include "R3BNeulandHitMon.h"
#include "R3BProgramOptions.h"
#include "TStopwatch.h"
#include <R3BNeulandClusterFinder.h>
#include <iostream>

namespace Digitizing = R3B::Digitizing;
using NeulandPaddle = Digitizing::Neuland::NeulandPaddle;
using MockPaddle = Digitizing::Neuland::MockPaddle;
using TamexChannel = Digitizing::Neuland::Tamex::Channel;
using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;
using MockChannel = Digitizing::Neuland::MockChannel;
using Digitizing::UseChannel;
using Digitizing::UsePaddle;

auto main(int argc, const char** argv) -> int
{

    auto timer = TStopwatch{};
    timer.Start();

    auto programOptions = r3b::ProgramOptions("options for neuland data analysis");
    auto help = programOptions.Create_Option<bool>("help,h", "help message", false);
    auto paraFileName =
        programOptions.Create_Option<std::string>("paraFile", "set the filename of parameter sink", "para.root");
    auto digiFileName =
        programOptions.Create_Option<std::string>("digiFile", "set the filename of digitization output", "digi.root");
    auto simuFileName =
        programOptions.Create_Option<std::string>("simuFile", "set the filename of simulation input", "simu.root");
    if (!programOptions.Verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    if (help->value())
    {
        std::cout << programOptions.Get_DescRef() << std::endl;
        return 0;
    }

    FairLogger::GetLogger()->SetLogScreenLevel("error");

    auto run = std::make_unique<FairRunAna>();
    auto filesource = std::make_unique<FairFileSource>(simuFileName->value().c_str());
    auto filesink = std::make_unique<FairRootFileSink>(digiFileName->value().c_str());
    run->SetSource(filesource.release());
    run->SetSink(filesink.release());

    auto fileio = std::make_unique<FairParRootFileIo>();
    fileio->open(paraFileName->value().c_str());
    run->GetRuntimeDb()->setFirstInput(fileio.release());

    // Tasks:

    // Set up digitization task:
    auto tamexParameter = Digitizing::Neuland::Tamex::Params{ TamexChannel::GetDefaultRandomGen() };
    tamexParameter.fPMTThresh = 1.;
    tamexParameter.fTimeMin = 1.;
    auto digiNeuland = std::make_unique<R3BNeulandDigitizer>();
    auto engine = CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<TamexChannel>(tamexParameter));
    digiNeuland->SetEngine(std::move(engine));
    run->AddTask(digiNeuland.release());

    // Set up monitoring task:
    auto hitmon = std::make_unique<R3BNeulandHitMon>();
    run->AddTask(hitmon.release());

    // Setup clustering task:
    auto r3bNeulandClusterFinder = std::make_unique<R3BNeulandClusterFinder>(2. * 7.5, 2. * 7.5, 2. * 7.5, 5.);
    run->AddTask(r3bNeulandClusterFinder.release());

    run->Init();
    run->Run();
    timer.Stop();
    auto* sink = run->GetSink();
    sink->Close();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
    return 0;
}
