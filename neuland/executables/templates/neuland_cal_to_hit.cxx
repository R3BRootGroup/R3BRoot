#include <FairParRootFileIo.h>
#include <FairRootFileSink.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>
#include <R3BEventHeader.h>
#include <R3BEventHeaderPropagator.h>
#include <R3BFileSource2.h>
#include <R3BLogger.h>
#include <R3BNeulandCalToHitTask.h>
#include <R3BNeulandHitPar.h>
#include <R3BProgramOptions.h>
#include <TStopwatch.h>
#include <filesystem>
#include <fmt/core.h>
#include <fmt/std.h>

namespace fs = std::filesystem;
constexpr int DEFAULT_EVENT_NUM = 0;
constexpr auto NEULAND_GLOBAL_TIME_OFFSET_NS = -400.;

using namespace std::string_literals;
auto main(int argc, const char** argv) -> int
{

    auto timer = TStopwatch{};
    timer.Start();

    auto programOptions = R3B::ProgramOptions("options for neuland exp cal to hit analysis");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "error");
    auto input_dir = programOptions.create_option<std::string>("inDir,i", "set the input directory", "");
    auto output_dir = programOptions.create_option<std::string>("outDir,o", "set the output directory", "");
    auto inputRunID = programOptions.create_option<int>("runid,r", "set the input runID", 0);
    auto eventNum = programOptions.create_option<int>("eventNum,n", "set the event number", DEFAULT_EVENT_NUM);

    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    if (help->value())
    {
        std::cout << programOptions.get_desc_ref() << std::endl;
        return 0;
    }
    FairLogger::GetLogger()->SetLogScreenLevel(logLevel->value().c_str());

    auto const fileRunID = inputRunID->value();
    auto const outputDir = output_dir->value().empty() ? input_dir->value() : output_dir->value();

    auto const inputDataFile = fmt::format("cal{0:04d}.root", fileRunID);
    auto const inputParFileName = fmt::format("calPar{0:04d}.root", fileRunID);
    auto const outputFileName = fmt::format("hit{0:04d}.root", fileRunID);

    const auto full_inputpath = fs::path(input_dir->value()) / fs::path(inputDataFile);
    const auto full_inputpar_filepath = fs::path(input_dir->value()) / fs::path{ inputParFileName };
    const auto full_outputpath = fs::path{ outputDir } / fs::path(outputFileName);

    R3BLOG(debug, fmt::format("input data file: {}", full_inputpath).c_str());
    R3BLOG(debug, fmt::format("input data par file: {}", full_inputpar_filepath).c_str());
    R3BLOG(debug, fmt::format("output data file: {}", full_outputpath).c_str());

    auto source = std::make_unique<R3BFileSource2>();
    source->AddFile(full_inputpath);

    auto sinkFile = R3B::make_rootfile(full_outputpath.c_str(), "RECREATE");
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
        fileio->open(full_inputpar_filepath.c_str());
        run->GetRuntimeDb()->setFirstInput(fileio.release());

        // Add analysis task --------------------------------------------------------
        auto runIdTask = std::make_unique<R3BEventHeaderPropagator>();
        run->AddTask(runIdTask.release());

        auto cal2hit = std::make_unique<R3B::Neuland::Cal2HitTask>();
        cal2hit->SetTrigger(R3B::Neuland::CalTrigger::offspill);
        cal2hit->SetGlobalTimeOffset(NEULAND_GLOBAL_TIME_OFFSET_NS);
        run->AddTask(cal2hit.release());

        run->Init();
        run->Run(eventNum->value());
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

    auto* sinkfile = run->GetSink();
    if (sinkfile != nullptr)
    {
        sinkfile->Close();
    }
    return 0;
}
