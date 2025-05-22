#include "R3BNeulandApp.h"
#include "R3BException.h"
#include "R3BFileSource2.h"
#include "R3BShared.h"
#include <CLI/CLI.hpp>
#include <FairParRootFileIo.h>
#include <FairRootFileSink.h>
#include <FairRun.h>
#include <FairRuntimeDb.h>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/range/adaptor/reversed.hpp>
#include <cmath>
#include <cstdint>
#include <fairlogger/Logger.h>
#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <gsl/span>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <string_view>
#include <utility>
#include <vector>
#ifdef HAS_MPI
#include <mpi.h>
#endif

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
        const auto span_size = step * (partition_num + 1) < total_size ? step : total_size - (step * partition_num);
        return span<const T>{ &(elements.at(step * partition_num)), span_size };
    }

    enum class JSONConfigInputType : uint8_t
    {
        file,
        string,
        invalid
    };

    auto check_json_input_type(std::string_view input) -> JSONConfigInputType
    {
        auto is_json_string = [](std::string_view string) -> bool { return string.find('=') != std::string::npos; };
        auto is_a_file = [](std::string_view string) -> bool { return std::filesystem::exists(string); };

        if (is_json_string(input))
        {
            return JSONConfigInputType::string;
        }
        if (is_a_file(input))
        {
            return JSONConfigInputType::file;
        }
        return JSONConfigInputType::invalid;
    }

    auto transform_to_json_string(std::string_view input) -> std::string
    {
        auto raw_string = std::string{ input };
        auto equal_pos = raw_string.find('=');
        auto keys_string = raw_string.substr(0, equal_pos);
        auto value_string = raw_string.substr(equal_pos + 1);

        boost::algorithm::trim(keys_string);
        boost::algorithm::trim(value_string);

        auto keys = std::vector<std::string>{};
        boost::split(keys, keys_string, boost::is_any_of("."));

        for (const auto& key : boost::adaptors::reverse(keys))
        {
            value_string = fmt::format("{{ {:?} : {}}}", key, value_string);
        }
        return value_string;
    }

} // namespace

namespace R3B::Neuland
{
    using json = nlohmann::ordered_json;
    CLIApplication::CLIApplication(std::string_view name,
                                   std::unique_ptr<FairRun> run,
                                   std::reference_wrapper<Options> option)
        : app_name_{ name }
        , dump_json_filename_{ fmt::format("{}_{}", name, DEFAULT_JSON_FILENAME) }
        , run_(std::move(run))
        , option_{ option }
    {
        setup_logger();
        timer_.Start();
    }

    CLIApplication::~CLIApplication() // NOLINT: an unknown place may throw
    {
        timer_.Stop();

        if (has_inited())
        {
            LOGP(info, "Writting all parameters to files");
            run_->GetRuntimeDb()->writeContainers();
            run_->GetSink()->Close();

            if (has_failed())
            {
                fmt::print(fmt::emphasis::bold | fg(fmt::color::red),
                           "\nNeuland Application finished with a failure!\n\n");
            }
            else
            {
                fmt::print(fmt::emphasis::bold | fg(fmt::color::green),
                           "\nNeuland Application finished successfully!\n\n");
            }
            fmt::println("Real time: {}s, cpu time: {}s", timer_.RealTime(), timer_.CpuTime());
        }
    }

    void CLIApplication::setup_logger()
    {
        auto spec1 = fair::VerbositySpec::Make(fair::VerbositySpec::Info::severity,
                                               fair::VerbositySpec::Info::file_line_function);
        fair::Logger::DefineVerbosity("user1", spec1);

        auto spec2 = fair::VerbositySpec::Make(fair::VerbositySpec::Info::severity,
                                               fair::VerbositySpec::Info::timestamp_s,
                                               fair::VerbositySpec::Info::file_line_function);
        fair::Logger::DefineVerbosity("user2", spec2);

        fair::Logger::SetConsoleColor(true);
    }

    void CLIApplication::post_parse()
    {
        fair::Logger::SetConsoleSeverity(option_.get().log_level);
        fair::Logger::SetVerbosity(option_.get().verbose_level);
    }

    void CLIApplication::init()
    {
        LOGP(info, "Initializaing application ...");
        set_inited(true);
        add_inout_files();
        add_inout_pars();
        pre_init(run_.get());
        run_->Init();
        post_init(run_.get());
        LOGP(info, "Application is initialized.");
    }

    void CLIApplication::setup_options(CLI::App& program_options)
    {
        setup_common_options(program_options);
        setup_application_options(program_options);
    }

    void CLIApplication::setup_common_options(CLI::App& program_options)
    {
        auto dump_config_callback = [this](const std::string& filename)
        {
            is_dump_ = true;
            dump_json_filename_ = filename;
        };

        auto use_config_callback = [this](const std::vector<std::string>& filename_or_option)
        {
            if (not is_already_parsed_)
            {
                ParseApplicationOption(filename_or_option);
                is_already_parsed_ = true;
            }
        };

        auto& options = option_.get();
        program_options
            .add_option_function<std::vector<std::string>>(
                "-c, --use-config", use_config_callback, "Set the json config file")
            ->default_val(fmt::format("{}_{}", app_name_, DEFAULT_JSON_FILENAME))
            ->run_callback_for_default()
            ->allow_extra_args()
            ->trigger_on_parse();
        program_options.add_flag("--print-config", has_print_default_options_, "Print default option value");
        program_options
            .add_option_function<std::string>("--dump-config", dump_config_callback, "Dump the config into a json file")
            ->default_val(dump_json_filename_)
            ->run_callback_for_default()
            ->expected(0, 1);
        program_options.add_option("-s, --severity", options.log_level, "Set the severity level");
        program_options.add_option("-v, --verbose", options.verbose_level, "Set the verbose level");
        program_options.add_option("-n, --event-num", options.event_num, "Set the event number")->capture_default_str();
        program_options.add_option("--run-id", options.run_id, "Set the run id")->capture_default_str();

        program_options.add_option("-i, --input-file", options.input.data, "Set the input filenames (regex)")
            ->capture_default_str()
            ->group("Input options");
        program_options
            .add_option("--input-tree-file",
                        options.input.tree_data,
                        "Set the input filenames (regex) containing only root tree")
            ->group("Input options");
        program_options.add_option("--par-in", options.input.par, "Set the filename of the input parameter root file")
            ->capture_default_str()
            ->group("Input options");
        program_options
            .add_option(
                "--par-in-second", options.input.par_2, "Set the filename of the second input parameter root file")
            ->group("Input options");

        program_options.add_option("-o, --output-file", options.output.data, "Set the output filename")
            ->capture_default_str()
            ->group("Output options");
        program_options
            .add_option("--par-out", options.output.par, "Set the filename of the output parameter root file")
            ->capture_default_str()
            ->group("Output options");
    }

    void CLIApplication::add_inout_files()
    {
        const auto& option = option_.get();

        // output files:
        const auto output_name =
            option.enable_mpi ? fmt::format("{}.{}", option.output.data, rank_num_) : option.output.data;
        if (not option_.get().output.data.empty())
        {
            // check if path if relative or full
            auto file_path = option.output.working_dir.empty()
                                 ? fs::path{ output_name }
                                 : fs::path{ option.output.working_dir } / fs::path{ output_name };
            auto file_sink = std::make_unique<FairRootFileSink>(file_path.c_str());
            run_->SetSink(file_sink.release());
        }

        // input files:
        auto file_source = std::make_unique<R3BFileSource2>();
        if (option_.get().run_id >= 0)
        {
            file_source->SetInitRunID(option_.get().run_id);
            run_->SetRunId(option_.get().run_id);
            LOGP(info, "Filesource2: Set to run id {}", option_.get().run_id);
        }
        add_input_filename(file_source.get());
        if (not file_source->IsEmpty())
        {
            run_->SetSource(file_source.release());
        }
    }

    void CLIApplication::add_inout_pars()
    {
        auto file_path = fs::path{};
        const auto& input_option = option_.get().input;
        const auto& output_option = option_.get().output;
        const auto& input_wd = input_option.working_dir;
        const auto& output_wd = output_option.working_dir;

        if (not input_option.par.empty())
        {
            file_path =
                input_wd.empty() ? fs::path{ input_option.par } : fs::path{ input_wd } / fs::path{ input_option.par };
            auto fileio = std::make_unique<FairParRootFileIo>();
            LOGP(info, "Input first parameter file is {:?}", file_path.string());
            fileio->open(file_path.c_str(), "READ");
            run_->GetRuntimeDb()->setFirstInput(fileio.release());
        }

        if (not input_option.par_2.empty())
        {
            file_path = input_wd.empty() ? fs::path{ input_option.par_2 }
                                         : fs::path{ input_wd } / fs::path{ input_option.par_2 };
            auto fileio = std::make_unique<FairParRootFileIo>();
            LOGP(info, "Input second parameter file is {:?}", file_path.string());
            fileio->open(file_path.c_str(), "READ");
            run_->GetRuntimeDb()->setSecondInput(fileio.release());
        }

        if (not output_option.par.empty())
        {
            const auto& option = option_.get();
            const auto output_name =
                option.enable_mpi ? fmt::format("{}.{}", output_option.par, rank_num_) : output_option.par;
            file_path = output_wd.empty() ? fs::path{ output_name } : fs::path{ output_wd } / fs::path{ output_name };
            LOGP(info, "Ouptut parameter file is {:?}", file_path.string());
            auto fileio = std::make_unique<FairParRootFileIo>(true);
            fileio->open(file_path.c_str(), "RECREATE");
            auto* rtdb = run_->GetRuntimeDb();
            rtdb->setOutput(fileio.release());
        }
    }

    void CLIApplication::run()
    {
        auto max_event = option_.get().event_num;
        max_event = max_event > 0 ? max_event : 0;
        if (max_event > 0)
        {
            LOGP(info, "{} is set to run with {} events", app_name_, max_event);
        }
        run_action(run_.get(), max_event);
    }

    void CLIApplication::run_action(FairRun* run, int num_of_events) { run->Run(0, num_of_events); }

    void CLIApplication::add_input_filename(R3BFileSource2* filesource)
    {
        extract_input_files();
        auto input_span = option_.get().enable_mpi ? get_partition_from(input_files_, num_of_procs_, rank_num_)
                                                   : span{ input_files_ };
        for (const auto& [filename, is_tree] : input_span)
        {
            filesource->AddFile(filename, is_tree);
        }
    }

    void CLIApplication::extract_input_files()
    {
        auto file_path = fs::path{};
        const auto& working_dir = option_.get().input.working_dir;
        for (const auto& filename : option_.get().input.data)
        {
            // check if path if relative or full
            file_path = working_dir.empty() ? fs::path{ filename } : fs::path{ working_dir } / fs::path{ filename };
            auto fairroot_input_files = R3B::GetFilesFromRegex(file_path.string());
            for (const auto& fairroot_input_file : fairroot_input_files)
            {
                input_files_.emplace_back(fairroot_input_file, false);
            }
        }
        for (const auto& filename : option_.get().input.tree_data)
        {
            auto tree_input_files = R3B::GetFilesFromRegex(filename);
            for (const auto& tree_input_file : tree_input_files)
            {
                input_files_.emplace_back(tree_input_file, true);
            }
        }
    }

    void CLIApplication::patch_files_or_strings(nlohmann::ordered_json& json_obj,
                                                const std::vector<std::string>& filenames_or_options)
    {
        for (const auto& filename_or_option : filenames_or_options)
        {
            auto json_file_obj = [&filename_or_option]()
            {
                switch (check_json_input_type(filename_or_option))
                {
                    case JSONConfigInputType::string:
                    {
                        auto json_string = transform_to_json_string(filename_or_option);
                        LOGP(info, "Reading the configuration from the string {:?}.", json_string);
                        return nlohmann::ordered_json::parse(std::move(json_string), nullptr, true, true);
                    }
                    case JSONConfigInputType::file:
                    {
                        auto file = std::ifstream{ filename_or_option };
                        LOGP(info, "Reading the configuration from the json file {:?}.", filename_or_option);
                        return nlohmann::ordered_json::parse(file, nullptr, true, true);
                    }
                    case JSONConfigInputType::invalid:
                        break;
                }
                throw R3B::logic_error(fmt::format("Cannot parse the string {:?}", filename_or_option));
            }();
            json_obj.merge_patch(json_file_obj);
        }
    }
} // namespace R3B::Neuland
