#include "R3BFileSource2.h"
#include "R3BNeulandApp.h"
#include <CLI/CLI.hpp>
#include <FairParRootFileIo.h>
#include <FairRootFileSink.h>
#include <FairRun.h>
#include <FairRuntimeDb.h>
#include <TGeoManager.h>
#include <fmt/format.h>
#include <gsl/span>
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
} // namespace

namespace R3B::Neuland
{
    using json = nlohmann::ordered_json;
    Application::Application(std::string_view name,
                             std::unique_ptr<FairRun> run,
                             std::reference_wrapper<Options> option)
        : app_name_{ name }
        , dump_json_filename_{ fmt::format("{}_{}", name, DEFAULT_JSON_FILENAME) }
        , run_(std::move(run))
        , option_{ option }
    {
        timer_.Start();
    }

    Application::~Application()
    {
        if (is_inited_)
        {
            R3BLOG(info, "Writting all parameters to files");
            run_->GetRuntimeDb()->writeContainers();
            run_->GetSink()->Close();
        }

        timer_.Stop();

        if (is_inited_)
        {
            if (is_failed_)
            {
                fmt::print("Failed to finish Neuland Application successfully.\n");
            }
            else
            {
                fmt::print("Neuland Application finished successfully.\n");
            }
            fmt::print("Real time: {}s, cpu time: {}s\n", timer_.RealTime(), timer_.CpuTime());
        }
    }

    void Application::init()
    {
        R3BLOG(info, "Initializaing application ...");
        is_inited_ = true;
        FairLogger::GetLogger()->SetLogScreenLevel(option_.get().log_level.c_str());
        add_inout_files();
        add_inout_pars();
        pre_init(run_.get());
        run_->Init();
        post_init(run_.get());
        R3BLOG(info, "Application is initialized.");
    }

    void Application::setup_options(CLI::App& program_options)
    {
        setup_common_options(program_options);
        setup_application_options(program_options);
    }

    void Application::setup_common_options(CLI::App& program_options)
    {
        auto dump_config_callback = [this](const std::string& filename)
        {
            is_dump_ = true;
            dump_json_filename_ = filename;
        };

        auto use_config_callback = [this](const std::string& filename)
        {
            if (not is_already_parsed_)
            {
                ParseApplicationOption(filename);
                is_already_parsed_ = true;
            }
        };

        auto& options = option_.get();
        program_options
            .add_option_function<std::string>("-c, --config-file", use_config_callback, "Set the json config file")
            ->default_val(fmt::format("{}_{}", app_name_, DEFAULT_JSON_FILENAME))
            ->run_callback_for_default()
            ->expected(0, 1)
            ->trigger_on_parse();
        program_options.add_flag("--print-config", has_print_default_options_, "Print default option value");
        program_options
            .add_option_function<std::string>("--dump-config", dump_config_callback, "Dump the config into a json file")
            ->default_val(dump_json_filename_)
            ->run_callback_for_default()
            ->expected(0, 1);
        program_options.add_option("-v, --verbose", options.log_level, "Set the verbose level");
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

    void Application::add_inout_files()
    {
        const auto& option = option_.get();
        // output files:
        const auto output_name =
            option.enable_mpi ? fmt::format("{}.{}", option.output.data, rank_num_) : option.output.data;
        if (not option_.get().output.data.empty())
        {
            auto file_sink = std::make_unique<FairRootFileSink>(output_name.c_str());
            run_->SetSink(file_sink.release());
        }

        // input files:
        auto file_source = std::make_unique<R3BFileSource2>();
        if (option_.get().run_id >= 0)
        {
            file_source->SetInitRunID(option_.get().run_id);
            R3BLOG(info, fmt::format("Filesource2: Set to run id {}", option_.get().run_id));
        }
        add_input_filename(file_source.get());
        if (not file_source->IsEmpty())
        {
            run_->SetSource(file_source.release());
        }
    }

    void Application::add_inout_pars()
    {
        if (not option_.get().input.par.empty())
        {
            auto fileio = std::make_unique<FairParRootFileIo>();
            fileio->open(option_.get().input.par.c_str(), "READ");
            run_->GetRuntimeDb()->setFirstInput(fileio.release());
        }

        if (not option_.get().input.par_2.empty())
        {
            auto fileio = std::make_unique<FairParRootFileIo>();
            fileio->open(option_.get().input.par_2.c_str(), "READ");
            run_->GetRuntimeDb()->setSecondInput(fileio.release());
        }

        if (not option_.get().output.par.empty())
        {
            auto fileio = std::make_unique<FairParRootFileIo>(true);
            fileio->open(option_.get().output.par.c_str(), "RECREATE");
            auto* rtdb = run_->GetRuntimeDb();
            rtdb->setOutput(fileio.release());
        }
    }

    void Application::run()
    {
        auto max_event = option_.get().event_num;
        max_event = max_event > 0 ? max_event : 0;
        if (max_event > 0)
        {
            R3BLOG(info, fmt::format("{} is set to run with {} events", app_name_, max_event));
        }
        run_action(run_.get(), max_event);
    }

    void Application::run_action(FairRun* run, int num_of_events) { run->Run(0, num_of_events); }

    void Application::add_input_filename(R3BFileSource2* filesource)
    {
        extract_input_files();
        auto input_span = option_.get().enable_mpi ? get_partition_from(input_files_, num_of_procs_, rank_num_)
                                                   : span{ input_files_ };
        for (const auto& [filename, is_tree] : input_span)
        {
            filesource->AddFile(filename, is_tree);
        }
    }

    void Application::extract_input_files()
    {
        for (const auto& filename : option_.get().input.data)
        {
            auto fairroot_input_files = R3B::GetFilesFromRegex(filename);
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
} // namespace R3B::Neuland
