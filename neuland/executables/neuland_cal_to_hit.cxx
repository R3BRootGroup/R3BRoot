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
    auto input_file = programOptions.create_option<std::string>("in,i", "set the input files (regex)");
    auto input_par = programOptions.create_option<std::string>("in-par,p", "set the input parameter");
    auto output_file = programOptions.create_option<std::string>("out,o", "set the output file");
    auto eventNum = programOptions.create_option<int>("eventNum,n", "set the event number", DEFAULT_EVENT_NUM);

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


    R3BLOG(debug, fmt::format("input data file: {}", fmt::join(input_filenames, ";")).c_str());
    R3BLOG(debug, fmt::format("input data par file: {}", input_par->value()).c_str());
    R3BLOG(debug, fmt::format("output data file: {}", output_file->value()).c_str());


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

        auto cal2hit = std::make_unique<R3B::Neuland::Cal2HitTask>();
        cal2hit->SetTrigger(R3B::Neuland::CalTrigger::onspill);
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

    auto* sinkfile = run->GetSink();
    if (sinkfile != nullptr)
    {
        sinkfile->Close();
    }
    return 0;
}
