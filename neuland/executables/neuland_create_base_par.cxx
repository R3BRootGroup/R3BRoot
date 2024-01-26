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
#include "R3BNeulandCalToHitPar.h"
#include "R3BNeulandMapToCalPar.h"
#include <FairBaseParSet.h>
#include <FairGeoParSet.h>
#include <FairParSet.h>
#include <FairRtdbRun.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <R3BNeulandBasePar.h>
#include <R3BProgramOptions.h>
#include <fstream>
#include <regex>

namespace nlh = nlohmann;

// auto get_json_from_url(std::string_view url) -> nlh::json
// {
//     auto response = cpr::Get(cpr::Url{ url });
//     if (response.status_code == 0)
//     {
//         throw R3B::runtime_error(fmt::format("{}", response.error.message));
//     }
//     constexpr auto DEFAULT_CODE_ERROR = 400;
//     if (response.status_code >= DEFAULT_CODE_ERROR)
//     {
//         throw R3B::runtime_error(fmt::format("Error \"{}\" making request", response.error.message));
//     }
//     std::cout << "JSON url request successful! Took " << response.elapsed << std::endl;
//     return nlh::json::parse(response.text);
// }

auto get_json_from_file(std::string_view filepath) -> nlh::json
{
    fmt::print("Reading trigIDMap from file {} ...\n", filepath);
    auto istream = std::ifstream{ filepath.data() };
    if (not istream.is_open())
    {
        throw R3B::runtime_error(fmt::format("Cannot open JSON file {}", filepath));
    }
    return nlh::json::parse(istream);
}

inline auto is_http_address(const std::string& name) { return std::regex_search(name, std::regex{ "^http" }); }

void add_parameter(FairParSet* par, FairRuntimeDb* rtdb)
{
    par->setChanged();
    if (rtdb->addContainer(par); par == nullptr)
    {
        throw R3B::runtime_error("Calibration parameter becomes nullptr!");
    }
}

auto main(int argc, const char** argv) -> int
{
    constexpr auto DEFAULT_RUN_ID = 999;
    auto programOptions = R3B::ProgramOptions("NeuLAND calibration base parameter generator");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "info");
    auto output = programOptions.create_option<std::string>("output,o", "set the output root filename");
    auto source = programOptions.create_option<std::string>("source,s", "set the JSON file source").make_positional(1);
    auto runID = programOptions.create_option<int>("runID,r", "set the event number", DEFAULT_RUN_ID);

    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    try
    {
        auto json_obj = get_json_from_file(source());
        auto base_par = std::make_unique<R3B::Neuland::CalibrationBasePar>();
        json_obj.get_to(*base_par);

        FairLogger::GetLogger()->SetLogScreenLevel(logLevel().c_str());
        auto rtdb = std::unique_ptr<FairRuntimeDb>(FairRuntimeDb::instance());
        auto parOut = std::make_unique<FairParRootFileIo>(false);
        parOut->open(output().c_str(), "RECREATE");
        add_parameter(base_par.release(), rtdb.get());
        add_parameter(std::make_unique<FairBaseParSet>().release(), rtdb.get());
        add_parameter(std::make_unique<R3B::Map2CalPar>("LandTCalPar").release(), rtdb.get());
        add_parameter(std::make_unique<R3B::Map2CalPar>("LandTrigTCalPar").release(), rtdb.get());
        add_parameter(std::make_unique<R3B::Neuland::Cal2HitPar>("NeulandHitPar").release(), rtdb.get());
        rtdb->setOutput(parOut.release());
        rtdb->saveOutput();
        rtdb->addRun(runID());
        rtdb->writeContainers();
    }
    catch (const std::exception& ex)
    {
        fmt::print("An exception has occurred: {}\n", ex.what());
    }
    catch (...)
    {
        fmt::print("An unrecgonised exception has occurred!\n");
    }

    fmt::print("Parameters are created successfully in file {:?}!", output());

    return 0;
}
