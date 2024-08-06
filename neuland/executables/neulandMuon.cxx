
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
#include "R3BNeulandHitMon.h"
#include "R3BProgramOptions.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include <R3BNeulandDigitizer.h>
#include <TObjString.h>
#include <boost/program_options.hpp>
#include <memory>

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

auto main(int argc, char** argv) -> int
{
    auto timer = TStopwatch{};
    timer.Start();

    auto programOptions = R3B::ProgramOptions("options for neuland data analysis");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto paddleName =
        programOptions.create_option<std::string>("paddle", R"(set the paddle name. e.g. "neuland")", "neuland");
    auto channelName =
        programOptions.create_option<std::string>("channel", R"(set the channel name. e.g. "tamex")", "tamex");
    auto simuFileName =
        programOptions.create_option<std::string>("simuFile", "set the filename of simulation input", "simu.root");
    auto paraFileName =
        programOptions.create_option<std::string>("paraFile", "set the filename of parameter sink", "para.root");
    auto paraFileName2 =
        programOptions.create_option<std::string>("paraFile2", "set the filename of the second parameter sink", "");
    auto digiFileName =
        programOptions.create_option<std::string>("digiFile", "set the filename of digitization output", "digi.root");
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "error");
    auto eventNum = programOptions.create_option<int>("eventNum,n", "set total event number", 0);
    auto hitLevelPar =
        programOptions.create_option<std::string>("hitLevelPar", "set the name of hit level parameter if needed.", "");

    // Paula: Error not yet used in the code
    auto errorcal = programOptions.create_option<bool>("errorCal", "usage of errors in calculations", false);

    // Paula:digi option for Caldata
    auto calData = programOptions.create_option<bool>("calData", "Doing CalData calculations", true);

    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    if (help())
    {
        std::cout << programOptions.get_desc_ref() << std::endl;
        return 0;
    }

    //=============================================================================
    // settings:
    auto tamexParameter = Digitizing::Neuland::Tamex::Params{ TamexChannel::GetDefaultRandomGen() };
    auto pileup_strategy = Digitizing::Neuland::Tamex::PeakPileUpStrategy::time_window;
    tamexParameter.fPMTThresh = 1.;
    tamexParameter.fTimeMin = 1.;

    // Paula: CalData is only implimented in Tamex for now
    // const auto neulandEngines = std::map<std::pair<const std::string, const std::string>,
    //                                      std::function<std::unique_ptr<Digitizing::DigitizingEngineInterface>()>>{
    //     { { "neuland", "tamex" },
    //       [&]()
    //       {
    //           return Digitizing::CreateEngine(
    //               UsePaddle<NeulandPaddle>(), UseChannel<TamexChannel>(pileup_strategy, tamexParameter),
    //               channelInit);
    //       } },
    //     { { "neuland", "tacquila" },
    //       []() { return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<TacquilaChannel>()); } },
    //     { { "mock", "tamex" },
    //       [&]()
    //       {
    //           return Digitizing::CreateEngine(
    //               UsePaddle<MockPaddle>(), UseChannel<TamexChannel>(pileup_strategy, tamexParameter), channelInit);
    //       } },
    //     { { "neuland", "mock" },
    //       []() { return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<MockChannel>()); } },
    //     { { "mock", "mock" },
    //       []() { return Digitizing::CreateEngine(UsePaddle<MockPaddle>(), UseChannel<MockChannel>()); } }
    // };
    // Paula:test

    //=============================================================================

    FairLogger::GetLogger()->SetLogScreenLevel(logLevel().c_str());

    auto run = std::make_unique<FairRunAna>();
    auto filesource = std::make_unique<R3BFileSource2>(simuFileName().c_str());
    auto filesink = std::make_unique<FairRootFileSink>(digiFileName().c_str());
    run->SetSource(filesource.release());
    run->SetSink(filesink.release());

    auto fileio = std::make_unique<FairParRootFileIo>();
    fileio->open(paraFileName().c_str());
    run->GetRuntimeDb()->setFirstInput(fileio.release());

    if (const auto& filename = paraFileName2(); not filename.empty())
    {
        auto fileio2 = std::make_unique<FairParRootFileIo>();
        fileio2->open(paraFileName2().c_str());
        run->GetRuntimeDb()->setSecondInput(fileio2.release());
    }

    auto hit_par = std::make_unique<R3B::Neuland::Cal2HitPar>();

    auto* hit_par_ptr = hit_par.get();

    run->GetRuntimeDb()->addContainer(hit_par.release());

    const auto neulandEngines = std::map<std::pair<const std::string, const std::string>,
                                         std::function<std::unique_ptr<Digitizing::DigitizingEngineInterface>()>>{
        { { "neuland", "tamex" },
          [&pileup_strategy, &tamexParameter, hit_par_ptr]()
          {
              return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(hit_par_ptr),
                                              UseChannel<TamexChannel>(pileup_strategy, tamexParameter,hit_par_ptr));
          } }
    };

    // // Paula: par_name hardcoded will change to flag later
    // auto par_name = std::string_view{ "NeulandCal2HitPar" };
    // auto* par = dynamic_cast<R3B::Neuland::Cal2HitPar*>(run->GetRuntimeDb()->findContainer(par_name.data()));
    // if (par == nullptr)
    // {
    //     par = std::make_unique<R3B::Neuland::Cal2HitPar>(par_name.data()).release();
    //     if (run->GetRuntimeDb()->addContainer(par); par == nullptr)
    //     {
    //         throw R3B::runtime_error("Calibration parameter becomes nullptr!");
    //     }
    // }

    auto digiNeuland = std::make_unique<R3BNeulandDigitizer>();
    digiNeuland->EnableCalDataOutput(calData.value());
    auto neulandEngine = neulandEngines.at({ paddleName(), channelName() });
    // Paula: If stuff needs to be added here
    digiNeuland->SetEngine((neulandEngine)());
    run->AddTask(digiNeuland.release());

    run->Init();
    run->Run(0, eventNum());

    timer.Stop();
    auto* sink = run->GetSink();
    sink->Close();
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;
}
