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

#include <FairParSet.h>
#include <FairRtdbRun.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <R3BProgramOptions.h>
#include <TFile.h>
#include <TKey.h>
#include <fmt/format.h>
#include <string>

auto filenames_split(std::string_view namelist, std::string_view delimeter = ";") -> std::vector<std::string>
{
    auto filenames = std::vector<std::string>{};
    auto filename_begin = std::string::size_type{ 0 };
    for (auto filename_end = std::string::size_type{ 0 }; filename_end != std::string::npos;
         filename_begin = filename_end + 1)
    {
        filename_end = namelist.find(delimeter, filename_begin);
        auto filename = namelist.substr(filename_begin, filename_end - filename_begin);
        filenames.emplace_back(filename);
    }
    return filenames;
}

void fill_pars(std::vector<FairParSet*>& pars, TDirectory* input)
{
    auto* keys = input->GetListOfKeys();
    for (auto* key : TRangeDynCast<TKey>(keys))
    {
        auto* obj = key->ReadObj();
        if (auto* dir = dynamic_cast<TDirectory*>(obj); dir != nullptr)
        {
            fill_pars(pars, dir);
        }
        else if (auto* par = dynamic_cast<FairParSet*>(obj); par != nullptr)
        {
            pars.push_back(par);
        }
    }
}

auto get_par_run(TFile* root_file) -> FairRtdbRun*
{
    auto* keys = root_file->GetListOfKeys();
    for (auto* key : TRangeDynCast<TKey>(keys))
    {
        if (auto* par_run = dynamic_cast<FairRtdbRun*>(key->ReadObj()); par_run != nullptr)
        {
            return par_run;
        }
    }
    return nullptr;
}

auto make_default_par_version(FairParSet* par) -> std::unique_ptr<FairParVersion>
{
    auto version = std::make_unique<FairParVersion>(par->GetName());
    version->setRootVersion(1);
    version->resetInputVersions();
    return version;
}

void copy_par_version(FairRtdbRun* output_run, FairRtdbRun* input_run, FairParSet* par)
{
    if (input_run != nullptr)
    {
        if (auto* par_version = input_run->getParVersion(par->GetName()); par_version != nullptr)
        {
            output_run->addParVersion(par_version);
            return;
        }
    }
    output_run->addParVersion(make_default_par_version(par).release());
}

void write_parameters(TFile* output, TFile* input)
{
    auto pars = std::vector<FairParSet*>{};
    fill_pars(pars, input);

    auto* output_par_run = get_par_run(output);
    auto* input_par_run = get_par_run(input);
    if (output_par_run == nullptr)
    {
        throw R3B::runtime_error("Cannot get the FairRtdbRun from the output root file!");
    }

    for (auto* par : pars)
    {
        R3BLOG(info, fmt::format(R"(parameter "{}" is written to {}.)", par->GetName(), output->GetName()));
        output->WriteObject<FairParSet>(par, par->GetName(), "overwrite");
        copy_par_version(output_par_run, input_par_run, par);
    }
    // overwrite or writedelete
    output->WriteObject<FairRtdbRun>(output_par_run, output_par_run->GetName(), "overwrite");
}

void merge_into_rootfile(TFile* output, const std::vector<std::string>& filenames)
{
    for (const auto& filename : filenames)
    {
        auto inputfile = std::make_unique<TFile>(filename.c_str(), "READ");
        write_parameters(output, inputfile.get());
    }
}

void initialize_output_file(TFile* output, std::string_view base)
{
    auto base_file = std::make_unique<TFile>(base.data(), "READ");
    auto* par_run = get_par_run(base_file.get());
    if (par_run == nullptr)
    {
        throw R3B::runtime_error(fmt::format(R"(Base file {} doesn't contain any FairRtdbRun!)", base));
    }
    output->WriteObject<FairRtdbRun>(par_run, par_run->GetName(), "overwrite");
}

auto main(int argc, const char** argv) -> int
{

    using namespace std::string_literals;
    //====================================================================================
    // Program options:
    auto programOptions = R3B::ProgramOptions("Parameter merge");
    auto help = programOptions.create_option<bool>("help,h", "help message", false);
    auto logLevel = programOptions.create_option<std::string>("logLevel,v", "set log level of fairlog", "info");
    auto parameter_out = programOptions.create_option<std::string>(
        "par-out", "set the path of the parameter file to merge into (only one allowed)");
    auto parameter_in = programOptions.create_option<std::string>(
        "par-in", "set the path of the parameter files to merge from (semicolon seperated)");
    if (!programOptions.verify(argc, argv))
    {
        return EXIT_FAILURE;
    }

    FairLogger::GetLogger()->SetLogScreenLevel(logLevel().c_str());

    const auto input_files = filenames_split(parameter_in());
    const auto output_file = parameter_out();

    if (input_files.empty())
    {
        R3BLOG(error, "empty input parameter file!");
    }

    auto output_rootfile = std::make_unique<TFile>(output_file.c_str(), "RECREATE");

    try
    {
        initialize_output_file(output_rootfile.get(), input_files.front());
        merge_into_rootfile(output_rootfile.get(), input_files);
    }
    catch (const std::exception& ex)
    {
        fmt::print("An exception has occurred: {}\n", ex.what());
    }
    catch (...)
    {
        fmt::print("An unrecgonised exception has occurred!\n");
    }

    return 0;
}
