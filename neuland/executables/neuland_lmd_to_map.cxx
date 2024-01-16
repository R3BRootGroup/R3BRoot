#include "R3BEventHeaderPropagator.h"
#include "R3BNeulandMapToCalParTask.h"
#include "ext_h101_unpack.h"
#include "ext_h101_wrmaster.h"
#include <FairParRootFileIo.h>
#include <FairRootFileSink.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>
#include <R3BEventHeader.h>
#include <R3BNeulandTamexReader.h>
#include <R3BNeulandTamexReader2.h>
#include <R3BProgramOptions.h>
#include <R3BTrloiiTpatReader.h>
#include <R3BUcesbSource.h>
#include <R3BUcesbSource2.h>
#include <R3BUnpackReader.h>
#include <R3BWhiterabbitMasterReader.h>
#include <TROOT.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include <ext_h101_tpat.h>
#include <filesystem>
#include <fmt/core.h>
#include <iostream>
#include <regex>
#include <string_view>

namespace fs = std::filesystem;

struct EXT_STR_h101_t
{
    EXT_STR_h101_unpack_t unpack;
    EXT_STR_h101_TPAT_t tpat;
    EXT_STR_h101_raw_nnp_tamex_onion_t raw_nnp;
    EXT_STR_h101_WRMASTER_t wrmaster;
};

constexpr int DEFAULT_EVENT_NUM = 1000;
constexpr int DEFAULT_RUN_ID = 999;
constexpr auto DEFAULT_UNPACKER_PATH = "202205_s509/202205_s509";
constexpr auto NEULAND_DEFAULT_DOUBLE_PLANE = 13;

auto main(int argc, const char** argv) -> int
{
    using namespace std::string_literals;
    TStopwatch timer;
    timer.Start();

    //====================================================================================
    // Program options:
    auto programOptions = R3B::ProgramOptions("options for neuland data analysis");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto auto_id = programOptions.create_option<bool>("autoTrigID", "auto detect trigID", false);
    auto no_trig_neuland = programOptions.create_option<bool>("no-trig", "auto detect trigID", false);
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "info");
    auto eventNum = programOptions.create_option<int>("eventNum,n", "set the event number", DEFAULT_EVENT_NUM);
    auto input_files = programOptions.create_option<std::string>("in,i", "set the input files");
    auto output_file = programOptions.create_option<std::string>("out,o", "set the output file");
    auto jsonName =
        programOptions.create_option<std::string>("json", "set the json filename of trigIDMap", "TrigMapping.json");
    auto inputRunID = programOptions.create_option<int>("runID,r", "set the input runID", DEFAULT_RUN_ID);
    auto neulandDP = programOptions.create_option<int>(
        "dp", "set the number of double planes for neuland", NEULAND_DEFAULT_DOUBLE_PLANE);
    auto wr_ID = programOptions.create_option<std::string>("wrID", "set the white rabbit id", "0x1000U");
    auto unpacker_path = programOptions.create_option<std::string>(
        "unpack", "set the path of unpacker executable relative to ${UCESB_DIR}/../unexps/", DEFAULT_UNPACKER_PATH);
    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    //====================================================================================
    // Input definitions:
    FairLogger::GetLogger()->SetLogScreenLevel(logLevel->value().c_str());

    const auto whiterabbit_id = std::stoi(wr_ID->value(), nullptr, 16);

    const unsigned int planeNum = neulandDP->value() * 2;
    const auto runID = inputRunID->value();
    const auto outputfile_path = fs::path{ output_file->value() };
    const auto outputDir = R3B::GetParentDir(output_file->value());
    const auto parfile = outputDir / fmt::format("{}.par.{}", outputfile_path.stem(), outputfile_path.extension());
    const auto ntuple_options = "RAW,time-stitch=4000"s;
    // const auto ntuple_options = "RAW"s;
    auto const* ucesb_dir = getenv("UCESB_DIR");
    if (ucesb_dir == nullptr)
    {
        R3BLOG(error, "ucesb_dir is not defined!");
        return 1;
    }
    const auto upexps_dir = std::string{ ucesb_dir } + "/../upexps"s;
    const auto upexps_exe = fs::path{ upexps_dir } / unpacker_path->value();
    const auto max_event_num = (eventNum->value() == 0) ? -1 : eventNum->value();

    auto ucesb_command = upexps_exe.string() + " --allow-errors --input-buffer=150Mi"s;
    // auto ucesb_command = upexps_exe.string();
    ucesb_command = std::regex_replace(ucesb_command, std::regex("//"), "/");

    auto ucesbStruct = EXT_STR_h101{};
    auto source = std::make_unique<R3BUcesbSource2>(
        input_files->value(), ntuple_options, ucesb_command, &ucesbStruct, sizeof(ucesbStruct));
    source->SetMaxEvents(max_event_num);

    //====================================================================================
    // Adding readers
    source->AddReader<R3BUnpackReader>(&ucesbStruct.unpack, offsetof(EXT_STR_h101, unpack));
    source->AddReader<R3BTrloiiTpatReader>(&ucesbStruct.tpat, offsetof(EXT_STR_h101, tpat));

    auto* neulandReader =
        source->AddReader<R3BNeulandTamexReader2>(&ucesbStruct.raw_nnp, offsetof(EXT_STR_h101, raw_nnp));
    neulandReader->AddExtraConditions(R3B::UcesbMap::array_fewer);
    neulandReader->SetMaxNbPlanes(planeNum);
    if (no_trig_neuland->value())
    {
        R3BLOG(info, "Disable NeuLAND trigger times");
        neulandReader->SetSkipTriggerTimes();
    }

    source->AddReader<R3BWhiterabbitMasterReader>(
        static_cast<EXT_STR_h101_WRMASTER*>(&ucesbStruct.wrmaster), offsetof(EXT_STR_h101, wrmaster), whiterabbit_id);

    //====================================================================================
    auto run = std::make_unique<FairRunOnline>();
    run->SetSource(source.release());
    auto EvntHeader = std::make_unique<R3BEventHeader>();
    run->SetEventHeader(EvntHeader.release());
    run->SetRunId(runID);
    run->SetSink(std::make_unique<FairRootFileSink>(output_file->value().c_str()));

    //=====================================================================================
    // set parameter files:
    auto* rtdb = run->GetRuntimeDb();
    auto parOut = std::make_unique<FairParRootFileIo>(true);
    parOut->open(parfile.c_str());
    rtdb->setOutput(parOut.release());
    rtdb->saveOutput();

    //=====================================================================================
    // set tasks:
    auto calPar = std::make_unique<R3B::Neuland::Map2CalParTask>();
    calPar->SetTrigger(R3B::Neuland::CalTrigger::all);
    calPar->SetPlaneNum(planeNum);
    calPar->SetTrigEnabled(not no_trig_neuland->value());
    // calPar->SetTrigIDMapPrintFormat(R3B::Neuland::TrigIDMappingPrintFormat::screen);
    calPar->SetTrigIDMapAutoFind(auto_id->value());
    calPar->SetTrigIDMapDir(outputDir.string());
    calPar->SetTrigIDMapFileName(jsonName->value());
    run->AddTask(calPar.release());

    //=====================================================================================
    // init
    try
    {
        run->Init();
        run->Run(-1, eventNum->value());
        std::cout << "Macro finished succesfully." << std::endl;
        std::cout << "Output file is " << output_file->value() << std::endl;
    }
    catch (fair::FatalException& ex)
    {
        std::cerr << ex.What();
        std::cout << "\n\n";
    }
    catch (std::runtime_error& ex)
    {
        std::cout << "A runtime error has occured: \n";
        std::cerr << ex.what();
        std::cout << "\n\n";
    }
    catch (std::logic_error& ex)
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

    timer.Stop();
    const double rtime = timer.RealTime();
    const double ctime = timer.CpuTime();
    std::cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << std::endl;
    return 0;
}
