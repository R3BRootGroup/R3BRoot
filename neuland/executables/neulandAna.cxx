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
#include "R3BFileSource2.h"
#include "R3BNeulandDigitizer.h"
#include "R3BNeulandHitMon.h"
#include "R3BProgramOptions.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include <boost/program_options.hpp>

namespace Digitizing = R3B::Digitizing;
using NeulandPaddle = Digitizing::Neuland::NeulandPaddle;
using MockPaddle = Digitizing::Neuland::MockPaddle;
using TamexChannel = Digitizing::Neuland::Tamex::Channel;
using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;
using MockChannel = Digitizing::Neuland::MockChannel;
using Digitizing::UseChannel;
using Digitizing::UsePaddle;

// auto GetHitPar(const std::string& parName)
// {
//     return []() { Digitizing::Neuland::Tamex::Channel::GetHitPar("test"); };
// }

auto main(int argc, const char** argv) -> int
{
    auto timer = TStopwatch{};
    timer.Start();

    auto programOptions = R3B::ProgramOptions("options for neuland data analysis");
    auto help = programOptions.Create_Option<bool>("help,h", "help message", false);
    auto paddleName =
        programOptions.Create_Option<std::string>("paddle", R"(set the paddle name. e.g. "neuland")", "neuland");
    auto channelName =
        programOptions.Create_Option<std::string>("channel", R"(set the channel name. e.g. "tamex")", "tacquila");
    auto simuFileName =
        programOptions.Create_Option<std::string>("simuFile", "set the filename of simulation input", "simu.root");
    auto paraFileName =
        programOptions.Create_Option<std::string>("paraFile", "set the filename of parameter sink", "para.root");
    auto digiFileName =
        programOptions.Create_Option<std::string>("digiFile", "set the filename of digitization output", "digi.root");
    auto logLevel = programOptions.Create_Option<std::string>("logLevel,v", "set log level of fairlog", "error");
    auto eventNum = programOptions.Create_Option<int>("eventNum,n", "set total event number", 0);
    auto hitLevelPar =
        programOptions.Create_Option<std::string>("hitLevelPar", "set the name of hit level parameter if needed.", "");

    if (!programOptions.Verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    if (help->value())
    {
        std::cout << programOptions.Get_DescRef() << std::endl;
        return 0;
    }

    auto const channelInit = [&]() { Digitizing::Neuland::Tamex::Channel::GetHitPar(hitLevelPar->value()); };

    auto tamexParameter = Digitizing::Neuland::Tamex::Params{ TamexChannel::GetDefaultRandomGen() };
    tamexParameter.fPMTThresh = 1.;
    tamexParameter.fTimeMin = 1.;

    const auto neulandEngines = std::map<std::pair<const std::string, const std::string>,
                                         std::function<std::unique_ptr<Digitizing::DigitizingEngineInterface>()>>{
        { { "neuland", "tamex" },
          [&]()
          {
              return Digitizing::CreateEngine(
                  UsePaddle<NeulandPaddle>(), UseChannel<TamexChannel>(tamexParameter), channelInit);
          } },
        { { "neuland", "tacquila" },
          []() { return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<TacquilaChannel>()); } },
        { { "mock", "tamex" },
          [&]() {
              return Digitizing::CreateEngine(
                  UsePaddle<MockPaddle>(), UseChannel<TamexChannel>(tamexParameter), channelInit);
          } },
        { { "neuland", "mock" },
          []() { return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<MockChannel>()); } },
        { { "mock", "mock" },
          []() { return Digitizing::CreateEngine(UsePaddle<MockPaddle>(), UseChannel<MockChannel>()); } }
    };

    FairLogger::GetLogger()->SetLogScreenLevel(logLevel->value().c_str());

    auto run = std::make_unique<FairRunAna>();
    auto filesource = std::make_unique<R3BFileSource2>(simuFileName->value().c_str());
    auto filesink = std::make_unique<FairRootFileSink>(digiFileName->value().c_str());
    run->SetSource(filesource.release());
    run->SetSink(filesink.release());

    auto fileio = std::make_unique<FairParRootFileIo>();
    fileio->open(paraFileName->value().c_str());
    run->GetRuntimeDb()->setFirstInput(fileio.release());

    auto digiNeuland = std::make_unique<R3BNeulandDigitizer>();
    digiNeuland->SetEngine((neulandEngines.at({ paddleName->value(), channelName->value() }))());
    run->AddTask(digiNeuland.release());
    auto hitmon = std::make_unique<R3BNeulandHitMon>();
    run->AddTask(hitmon.release());

    run->Init();
    run->Run(0, eventNum->value());

    timer.Stop();
    auto* sink = run->GetSink();
    sink->Close();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
}
