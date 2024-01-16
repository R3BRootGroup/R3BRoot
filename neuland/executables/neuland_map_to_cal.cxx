#include "R3BEventHeader.h"
#include "R3BEventHeaderPropagator.h"
#include "R3BFileSource2.h"
#include "R3BNeulandHitPar.h"
#include "R3BNeulandMapToCalParTask.h"
#include "R3BNeulandMapToCalTask.h"
#include "R3BTCalContFact.h"
#include <FairParRootFileIo.h>
#include <FairRootFileSink.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <R3BLogger.h>
#include <R3BNeulandCalToHitParTask.h>
#include <R3BProgramOptions.h>
#include <R3BTCalPar.h>
#include <TObjString.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/std.h>
#include <iostream>

namespace fs = std::filesystem;
constexpr int DEFAULT_EVENT_NUM = 10;
constexpr int DEFAULT_OFFSPILL_POS = 14;

using namespace std::string_literals;
auto main(int argc, const char** argv) -> int
{
    auto timer = TStopwatch{};
    timer.Start();

    auto programOptions = R3B::ProgramOptions("options for neuland exp data analysis");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "error");
    auto input_file = programOptions.create_option<std::string>("in,i", "set the input files (regex)");
    auto input_par = programOptions.create_option<std::string>("in-par,p", "set the input parameter");
    auto output_file = programOptions.create_option<std::string>("out,o", "set the output file");
    auto eventNum = programOptions.create_option<int>("eventNum,n", "set the event number", DEFAULT_EVENT_NUM);
    auto off_spill_bit = programOptions.create_option<int>(
        "offspill", "set the position of bin set when offspill", DEFAULT_OFFSPILL_POS);

    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    FairLogger::GetLogger()->SetLogScreenLevel(logLevel->value().c_str());
    const auto outputDir = R3B::GetParentDir(output_file->value());

    const auto outputfile_path = fs::path{ output_file->value() };
    const auto outputParFileName =
        outputDir / fmt::format("{}.par.{}", outputfile_path.stem(), outputfile_path.extension());
    const auto input_filenames = R3B::GetFilesFromRegex(input_file->value());
    R3B::Neuland::NeulandOffSpillTpatPos = off_spill_bit->value();

    R3BLOG(debug, fmt::format("input data file: {}", fmt::join(input_filenames, ";")).c_str());
    R3BLOG(debug, fmt::format("input data par file: {}", input_par->value()).c_str());
    R3BLOG(debug, fmt::format("output data file: {}", output_file->value()).c_str());
    R3BLOG(debug, fmt::format("output data par file: {}", outputParFileName).c_str());

    auto source = std::make_unique<R3BFileSource2>();
    for (const auto& filename : input_filenames)
    {
        source->AddFile(filename);
    }

    auto sinkFile = R3B::make_rootfile(outputfile_path.c_str(), "RECREATE");
    auto sink = std::make_unique<FairRootFileSink>(sinkFile.release());

    auto run = std::make_unique<FairRunAna>();
    FairRootManager::SetTreeName("evt");

    try
    {
        auto EvntHeader = std::make_unique<R3BEventHeader>();
        run->SetEventHeader(EvntHeader.release());
        run->SetSource(source.release());
        run->SetSink(sink.release());

        auto fileio = std::make_unique<FairParRootFileIo>();
        fileio->open(input_par->value().c_str());
        run->GetRuntimeDb()->setFirstInput(fileio.release());

        // Add analysis task --------------------------------------------------------
        auto runIdTask = std::make_unique<R3BEventHeaderPropagator>();
        run->AddTask(runIdTask.release());

        auto map2Cal = std::make_unique<R3BNeulandMapped2Cal2>();
        map2Cal->SetTrigger(R3B::Neuland::CalTrigger::all);
        run->AddTask(map2Cal.release());

        auto cal2hitParMaker = std::make_unique<R3B::Neuland::Cal2HitParTask>();
        cal2hitParMaker->SetTrigger(R3B::Neuland::CalTrigger::offspill);
        run->AddTask(cal2hitParMaker.release());

        // set par output--------------------------------------------------------
        auto* rtdb = run->GetRuntimeDb();
        auto parOut = std::make_unique<FairParRootFileIo>(true);
        parOut->open(outputParFileName.c_str());
        rtdb->setOutput(parOut.release());
        rtdb->saveOutput();

        run->Init();
        run->Run(0, eventNum->value());
    }
    catch (R3B::runtime_error& ex)
    {
        std::cout << "A runtime error has occured: \n";
        std::cerr << ex.what();
        std::cout << "\n\n";
    }
    catch (R3B::logic_error& ex)
    {
        std::cout << "A logic error has occured: \n";
        std::cerr << ex.what();
        std::cout << "\n\n";
    }
    catch (std::exception& ex)
    {
        std::cout << "A error has occured: \n";
        std::cerr << ex.what();
        std::cout << "\n\n";
    }
    catch(...)
    {
        std::cout << "An unrecognized error has occured: \n";
        std::cout << "\n\n";
    }

    timer.Stop();
    auto* sinkfile = run->GetSink();
    if (sinkfile != nullptr)
    {
        sinkfile->Close();
    }
    std::cout << "Cal level data written to file " << outputfile_path << "\n";
    std::cout << "Cal level parameter written to file " << outputParFileName << "\n";
    std::cout << "Macro finished successfully." << std::endl;
    std::cout << "Real time: " << timer.RealTime() << "s, CPU time: " << timer.CpuTime() << "s" << std::endl;

    return 0;
}
