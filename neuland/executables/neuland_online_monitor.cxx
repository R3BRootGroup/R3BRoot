#include <FairParRootFileIo.h>
#include <FairRootFileSink.h>
#include <FairRunOnline.h>
#include <FairRuntimeDb.h>
#include <R3BEventHeader.h>
#include <R3BEventHeaderPropagator.h>
#include <R3BLogger.h>
#include <R3BLosMapped2Cal.h>
#include <R3BLosProvideTStart.h>
#include <R3BLosReader.h>
#include <R3BNeulandCalToHitTask.h>
#include <R3BNeulandMapToCalTask.h>
#include <R3BNeulandOnlineSpectra2.h>
#include <R3BNeulandTamexReader2.h>
#include <R3BProgramOptions.h>
#include <R3BTrloiiTpatReader.h>
#include <R3BUcesbSource.h>
#include <R3BUcesbSource2.h>
#include <R3BUnpackReader.h>
#include <R3BWhiterabbitMasterReader.h>
#include <TROOT.h>
#include <TRandom3.h>
#include <TStopwatch.h>
#include <TSystem.h>
#include <ext_h101_los.h>
#include <ext_h101_tpat.h>
#include <ext_h101_unpack.h>
#include <ext_h101_wrmaster.h>

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
    EXT_STR_h101_LOS_t los;
    EXT_STR_h101_WRMASTER_t wrmaster;
};

constexpr int DEFAULT_EVENT_NUM = 1000;
constexpr int DEFAULT_PORT_NUM = 11000;
constexpr int DEFAULT_REFRESH_RATE = 1;
constexpr int DEFAULT_RUNID = 999;
constexpr int DEFAULT_OFFSPILL_POS = 14;
constexpr auto DEFAULT_UNPACKER_PATH = "202205_s509/202205_s509";
constexpr auto NEULAND_DEFAULT_DOUBLE_PLANE = 13;
constexpr auto NEULAND_GLOBAL_TIME_OFFSET_NS = -400.;

auto main(int argc, const char** argv) -> int
{
    using namespace std::string_literals;
    TStopwatch timer;
    timer.Start();

    //====================================================================================
    // Program options:
    auto programOptions = R3B::ProgramOptions("Online monitor for neuland data");
    auto input_fstream = programOptions.create_option<std::string>("inStream,i", "set the input data stream");
    auto parameter_file =
        programOptions.create_option<std::string>("par", "set the path of the parameter file (only one allowed)");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto no_trig_neuland = programOptions.create_option<bool>("no-trig", "NeuLAND trigger times are disabled", false);
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "info");
    auto eventNum = programOptions.create_option<int>("eventNum,n", "set the event number", DEFAULT_EVENT_NUM);
    auto inputRunID = programOptions.create_option<int>("runID,r", "set the input runID", DEFAULT_RUNID);
    auto neulandDP = programOptions.create_option<int>(
        "dp", "set the number of double planes for neuland", NEULAND_DEFAULT_DOUBLE_PLANE);
    auto wr_ID = programOptions.create_option<std::string>("wrID", "set the white rabbit id", "0x1000U");
    auto unpacker_path = programOptions.create_option<std::string>(
        "unpack", "set the path of unpacker executable relative to ${UCESB_DIR}/../unexps/", DEFAULT_UNPACKER_PATH);
    auto port_number =
        programOptions.create_option<int>("port,p", "set the port number of the output https server", DEFAULT_PORT_NUM);
    auto Offspill_bit = programOptions.create_option<int>(
        "offspill", "set the position of bin set when offspill", DEFAULT_OFFSPILL_POS);

    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    FairLogger::GetLogger()->SetLogScreenLevel(logLevel->value().c_str());

    auto random_gen = TRandom3{};
    random_gen.SetSeed(0);

    const auto whiterabbit_id = std::stoi(wr_ID->value(), nullptr, 16);
    const unsigned int planeNum = neulandDP->value() * 2;
    const auto runID = inputRunID->value();
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
    R3B::Neuland::NeulandOffSpillTpatPos = Offspill_bit->value();

    const auto lmd_file_path = input_fstream->value();
    const auto parFile = parameter_file->value();

    auto ucesb_command = upexps_exe.string() + " --allow-errors --input-buffer=150Mi"s;
    ucesb_command = std::regex_replace(ucesb_command, std::regex("//"), "/");

    auto ucesbStruct = EXT_STR_h101{};
    auto source = std::make_unique<R3BUcesbSource2>(
        lmd_file_path, ntuple_options, ucesb_command, &ucesbStruct, sizeof(ucesbStruct));
    source->SetMaxEvents(max_event_num);

    //====================================================================================
    // Adding readers
    source->AddReader(std::make_unique<R3BUnpackReader>(&ucesbStruct.unpack, offsetof(EXT_STR_h101, unpack)));
    source->AddReader(std::make_unique<R3BTrloiiTpatReader>(&ucesbStruct.tpat, offsetof(EXT_STR_h101, tpat)));
    auto* los_reader = source->AddReader(std::make_unique<R3BLosReader>(&ucesbStruct.los, offsetof(EXT_STR_h101, los)));
    los_reader->SetOnline(true);
    auto* neulandReader = source->AddReader(
        std::make_unique<R3BNeulandTamexReader2>(&ucesbStruct.raw_nnp, offsetof(EXT_STR_h101, raw_nnp)));
    neulandReader->AddExtraConditions(R3B::UcesbMap::array_fewer);
    neulandReader->SetMaxNbPlanes(planeNum);
    neulandReader->SetOnline(true);
    if (no_trig_neuland->value())
    {
        R3BLOG(info, "Disable NeuLAND trigger times");
        neulandReader->SetSkipTriggerTimes();
    }
    source->AddReader(std::make_unique<R3BWhiterabbitMasterReader>(
        static_cast<EXT_STR_h101_WRMASTER*>(&ucesbStruct.wrmaster), offsetof(EXT_STR_h101, wrmaster), whiterabbit_id));

    // FairRun:
    auto run = FairRunOnline{ source.release() };
    run.SetRunId(inputRunID->value());
    run.ActivateHttpServer(DEFAULT_REFRESH_RATE, port_number->value());
    auto EvntHeader = std::make_unique<R3BEventHeader>();
    run.SetEventHeader(EvntHeader.release());
    run.SetRunId(runID);

    //=====================================================================================
    // set parameter files:
    auto fileio = std::make_unique<FairParRootFileIo>(false);
    fileio->open(parFile.c_str());
    run.GetRuntimeDb()->setFirstInput(fileio.release());

    //=====================================================================================
    // add tasks
    auto losMapped2Cal = std::make_unique<R3BLosMapped2Cal>("LosTCalPar", 1);
    constexpr auto LOS_MODULE_NUM = 8;
    losMapped2Cal->SetNofModules(1, LOS_MODULE_NUM);
    losMapped2Cal->SetTrigger(1);
    losMapped2Cal->SetOnline(true);
    run.AddTask(losMapped2Cal.release());

    auto map2Cal = std::make_unique<R3BNeulandMapped2Cal2>();
    map2Cal->SetTrigger(R3B::Neuland::CalTrigger::allspill);
    map2Cal->SetDisableHist();
    run.AddTask(map2Cal.release());

    run.AddTask(std::make_unique<R3BLosProvideTStart>().release());
    auto cal2hit = std::make_unique<R3B::Neuland::Cal2HitTask>();
    cal2hit->SetTrigger(R3B::Neuland::CalTrigger::allspill);
    cal2hit->SetGlobalTimeOffset(NEULAND_GLOBAL_TIME_OFFSET_NS);
    cal2hit->SetDisableHist();
    run.AddTask(cal2hit.release());

    auto online_spectra = std::make_unique<R3B::Neuland::OnlineSpectra>();
    online_spectra->SetRandomGenerator(&random_gen);
    online_spectra->AddCanvas<R3B::Neuland::EventHeaderCanvas>("EventHeader");
    online_spectra->AddCanvas<R3B::Neuland::MappedCanvas>("NeulandMapped");
    online_spectra->AddCanvas<R3B::Neuland::CalCanvas>("NeulandCal", R3B::Neuland::CalTrigger::allspill);
    online_spectra->AddCanvas<R3B::Neuland::TJumpCanvas>("NeulandJumps");
    online_spectra->AddCanvas<R3B::Neuland::HitCanvas>("NeulandHit", R3B::Neuland::CalTrigger::onspill);
    online_spectra->AddCanvas<R3B::Neuland::HitXYCanvas>("NeulandPlaneXY", R3B::Neuland::CalTrigger::onspill);
    online_spectra->AddCanvas<R3B::Neuland::HitCosmicCanvas>("NeulandHitCosmics", R3B::Neuland::CalTrigger::offspill);
    online_spectra->AddCanvas<R3B::Neuland::TimingCanvas>("NeulandTiming", R3B::Neuland::CalTrigger::onspill);
    run.AddTask(online_spectra.release());

    try
    {
        run.Init();
    }
    catch (R3B::runtime_error& ex)
    {
        std::cout << "A runtime error has occured: \n";
        std::cerr << ex.what();
        std::cout << "\n\n";
        return 0;
    }
    catch (R3B::logic_error& ex)
    {
        std::cout << "A logic error has occured: \n";
        std::cerr << ex.what();
        std::cout << "\n\n";
        return 0;
    }
    run.Run(-1, eventNum->value());

    return 0;
}
