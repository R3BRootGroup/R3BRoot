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

#include "FairParRootFileIo.h"
#include "FairRootFileSink.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "NeulandPointFilter.h"
#include "R3BDigitizingChannelMock.h"
#include "R3BDigitizingPaddleMock.h"
#include "R3BDigitizingPaddleNeuland.h"
#include "R3BDigitizingTacQuila.h"
#include "R3BDigitizingTamex.h"
#include "R3BEventHeader.h"
#include "R3BFileSource2.h"
#include "R3BNeulandDigitizer.h"
#include "R3BNeulandHitMon.h"
#include "R3BProgramOptions.h"
#include "TStopwatch.h"
#include <R3BNeulandSimCalToCal.h>
#include <TObjString.h>
#include <boost/program_options.hpp>
#include <gsl/span>
#ifdef HAS_MPI
#include <mpi.h>
#endif

namespace Digitizing = R3B::Digitizing;
using NeulandPaddle = Digitizing::Neuland::NeulandPaddle;
using MockPaddle = Digitizing::Neuland::MockPaddle;
using TamexChannel = Digitizing::Neuland::Tamex::Channel;
using TacquilaChannel = Digitizing::Neuland::TacQuila::Channel;
using MockChannel = Digitizing::Neuland::MockChannel;
using Digitizing::UseChannel;
using Digitizing::UsePaddle;

using gsl::span;

namespace
{
    template <typename T>
    auto get_partition_from(const std::vector<T>& elements, int num_of_partitions, int partition_num) -> span<const T>
    {
        const auto total_size = elements.size();
        const auto step =
            static_cast<int>(std::ceil(static_cast<float>(total_size) / static_cast<float>(num_of_partitions)));
        if (step * partition_num >= total_size)
        {
            return {};
        }
        const auto span_size = step * (partition_num + 1) < total_size ? step : total_size - step * partition_num;
        return span<const T>{ &(elements.at(step * partition_num)), span_size };
    }
} // namespace

auto main(int argc, char** argv) -> int
{
#ifdef HAS_MPI
    MPI_Init(&argc, &argv);
    auto num_proc = 0;
    auto num_rank = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &num_rank);
#endif

    auto timer = TStopwatch{};
    timer.Start();

    auto programOptions = R3B::ProgramOptions("options for neuland data analysis");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto paddleName =
        programOptions.create_option<std::string>("paddle", R"(set the paddle name. e.g. "neuland")", "neuland");
    auto use_mpi = programOptions.create_option<bool>("mpi", "Enable mpi", false);
    auto channelName =
        programOptions.create_option<std::string>("channel", R"(set the channel name. e.g. "tamex")", "tamex");
    auto simuFileName =
        programOptions.create_option<std::string>("simuFile", "set the filename of simulation input", "simu.root");
    auto simuTreeFileName =
        programOptions.create_option<std::string>("simuTreeFile", "set the filename of simulation tree-only input", "");
    auto paraFileName =
        programOptions.create_option<std::string>("paraFile", "set the filename of parameter sink", "para.root");
    auto paraFileName2 =
        programOptions.create_option<std::string>("paraFile2", "set the filename of the second parameter sink", "");
    auto par_output =
        programOptions.create_option<std::string>("par-out", "set the filename of the output parameter", "");
    auto digiFileName =
        programOptions.create_option<std::string>("digiFile", "set the filename of digitization output", "digi.root");
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "error");
    auto eventNum = programOptions.create_option<int>("eventNum,n", "set total event number", 0);
    auto run_id = programOptions.create_option<int>("run-id", "set the run ID (Only needed if reading only trees)", -1);
    auto hitLevelPar =
        programOptions.create_option<std::string>("hitLevelPar", "set the name of hit level parameter if needed.", "");

    // Paula:digi option for Caldata
    auto calData = programOptions.create_option<bool>("calData", "Doing CalData calculations", true);
    auto calDataOutput =
        programOptions.create_option<bool>("calDataOutput", "Output CalData2 instead of SimCalData", false);

    auto customPara = programOptions.create_option<bool>("customPar", "Custom parameter for CalDataAnalysis", false);
    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    if (help())
    {
        std::cout << programOptions.get_desc_ref() << "\n";
        return 0;
    }

    FairLogger::GetLogger()->SetLogScreenLevel(logLevel().c_str());

    auto all_filenames = R3B::GetFilesFromRegex(simuFileName());
    auto filenames = span<const std::string>{};
    auto digi_output_filename = digiFileName.value();
    auto par_output_filename = par_output.value();

    if (use_mpi.value())
    {
        if (not hitLevelPar.value().empty())
        {
            FairRuntimeDb::instance()->getContainer(hitLevelPar.value().c_str());
            Digitizing::Neuland::Tamex::Channel::GetHitPar(hitLevelPar.value());
        }
    };

    //=============================================================================
    // settings:
    auto tamexParameter = Digitizing::Neuland::Tamex::Params{ TamexChannel::GetDefaultRandomGen() };
    auto pileup_strategy = Digitizing::Neuland::Tamex::PeakPileUpStrategy::time_window;
    tamexParameter.fPMTThresh = 1.;
    tamexParameter.fTimeMin = 1.;

    const auto neulandEngines = std::map<std::pair<const std::string, const std::string>,
                                         std::function<std::unique_ptr<Digitizing::DigitizingEngineInterface>()>>{
        { { "neuland", "tamex" },
          [&customPara, &pileup_strategy, &tamexParameter, hit_par_ptr]()
          {
              if (customPara.value())
              {
                  return Digitizing::CreateEngine(
                      UsePaddle<NeulandPaddle>(hit_par_ptr),
                      UseChannel<TamexChannel>(pileup_strategy, tamexParameter, hit_par_ptr));
              }

              return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(),
                                              UseChannel<TamexChannel>(pileup_strategy, tamexParameter));
          } },
        { { "neuland", "tacquila" },
          []() { return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<TacquilaChannel>()); } },
        { { "mock", "tamex" },
          [&]()
          {
              return Digitizing::CreateEngine(UsePaddle<MockPaddle>(),
                                              UseChannel<TamexChannel>(pileup_strategy, tamexParameter));
          } },
        { { "neuland", "mock" },
          []() { return Digitizing::CreateEngine(UsePaddle<NeulandPaddle>(), UseChannel<MockChannel>()); } },
        { { "mock", "mock" },
          []() { return Digitizing::CreateEngine(UsePaddle<MockPaddle>(), UseChannel<MockChannel>()); } }
    };
    //=============================================================================

    FairLogger::GetLogger()->SetLogScreenLevel(logLevel.value().c_str());

    auto run = std::make_unique<FairRunAna>();
    auto fairroot_input_files = R3B::GetFilesFromRegex(simuFileName->value());
    auto tree_input_files = R3B::GetFilesFromRegex(simuTreeFileName->value());
    auto filesource = std::make_unique<R3BFileSource2>();
    if (run_id->value() >= 0)
    {
        filesource->SetInitRunID(run_id->value());
        R3BLOG(info, fmt::format("Filesource2: Set to run id {}", run_id->value()));
    }

    filesource->AddFile(std::move(tree_input_files), true);
    filesource->AddFile(std::move(fairroot_input_files), false);

    auto filesink = std::make_unique<FairRootFileSink>(digiFileName->value().c_str());
    run->SetSource(filesource.release());
    run->SetSink(filesink.release());

    auto fileio = std::make_unique<FairParRootFileIo>();
    fileio->open(paraFileName.value().c_str());
    run->GetRuntimeDb()->setFirstInput(fileio.release());

    auto parFileIO = std::make_unique<FairParRootFileIo>(true);
    parFileIO->open("test.para");
    auto* rtdb = run->GetRuntimeDb();
    rtdb->setOutput(parFileIO.release());

    if (const auto& filename = paraFileName2.value(); not filename.empty())
    {
        auto fileio2 = std::make_unique<FairParRootFileIo>();
        fileio2->open(paraFileName2.value().c_str());
        run->GetRuntimeDb()->setSecondInput(fileio2.release());
    }

    auto digiNeuland = std::make_unique<R3BNeulandDigitizer>();
    digiNeuland->SetEngine((neulandEngines.at({ paddleName.value(), channelName.value() }))());
    run->AddTask(digiNeuland.release());

    if (calDataOutput.value())
    {
        auto cal_data_converter = std::make_unique<R3B::Neuland::SimCal2Cal>();
        run->AddTask(cal_data_converter.release());
    }

    auto hitmon = std::make_unique<R3BNeulandHitMon>();
    run->AddTask(hitmon.release());

    run->Init();
    run->Run(0, eventNum());

    timer.Stop();
    auto* sink = run->GetSink();
    sink->Close();
    rtdb->writeContainers();
    std::cout << "Macro finished successfully.\n";
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s\n";
    if (use_mpi.value())
    {
#ifdef HAS_MPI
        MPI_Finalize();
#endif
    }
    return 0;
}
