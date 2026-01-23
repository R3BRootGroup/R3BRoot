#pragma once

#include "JsonParse/SpecialParsing.h" // IWYU pragma: keep
#include <R3BException.h>
#include <R3BNeulandCLIAbstract.h>
#include <TStopwatch.h>
#include <algorithm>
#include <cstdint>
#include <fairlogger/Logger.h>
#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <glaze/core/context.hpp>
#include <glaze/core/opts.hpp>
#include <glaze/core/ostream_buffer.hpp>
#include <glaze/core/reflect.hpp>
#include <glaze/json/write.hpp> // IWYU pragma: keep
#include <ios>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

class FairRun;
class R3BFileSource2;

namespace CLI
{
    class App;
}

namespace R3B
{
    enum class WriteMode : uint8_t
    {
        NEW,
        recreate,
        update,
        read,
    };

}

namespace R3B::Neuland
{
    constexpr auto DEFAULT_EVENT_NUM = 100;
    constexpr auto DEFAULT_RUN_ID = 999;
    const auto DEFAULT_JSON_FILENAME = "config.json";
    class CLIApplication : public CLIAbstract
    {
      public:
        struct Options
        {
            int run_id = DEFAULT_RUN_ID;
            int number_of_events = DEFAULT_EVENT_NUM;
            bool enable_mpi = false;
            std::string log_level = "error";
            std::string verbose_level = "user1";
            struct Input
            {
                std::string working_dir;
                std::vector<std::string> data;
                std::vector<std::string> tree_data;
                std::vector<std::string> par;
            } input;
            struct Output
            {
                std::string working_dir;
                std::string data = "output.root";
                std::string par = "output.par.root";
                WriteMode mode = WriteMode::update;
            } output;
        };

        explicit CLIApplication(std::string_view name,
                                std::unique_ptr<FairRun> run,
                                std::reference_wrapper<Options> option);
        CLIApplication(const CLIApplication&) = delete;
        CLIApplication(CLIApplication&&) = delete;
        auto operator=(const CLIApplication&) -> CLIApplication& = delete;
        auto operator=(CLIApplication&&) -> CLIApplication& = delete;
        ~CLIApplication() override;

        // common APIs:

        template <typename CLIAPP, typename OptionType>
        void setup_common_options(CLIAPP& program_options, OptionType& options);

        template <typename OptionType>
        void print_json_options(const OptionType& options);

        template <typename OptionType>
        void dump_json_options(const OptionType& options, const std::string& filename);

        // setters:

        void set_log_level(std::string_view log_level) { option_.get().log_level = log_level; }
        void set_run_id(int run_id) { option_.get().run_id = run_id; }
        void set_output_filename(std::string_view output_filename) { option_.get().output.data = output_filename; }
        void add_inputpar_filename(std::string_view inputpar_filename)
        {
            option_.get().input.par.emplace_back(inputpar_filename);
        }
        void set_outputpar_filename(std::string_view outputpar_filename)
        {
            option_.get().output.par = outputpar_filename;
        }
        void set_event_num(int val) { option_.get().number_of_events = val; }
        void set_input_filename(const std::vector<std::string>& input_filename)
        {
            std::ranges::copy(input_filename, std::back_inserter(option_.get().input.data));
        }
        void set_tree_input_filename(const std::vector<std::string>& input_filename)
        {
            std::ranges::copy(input_filename, std::back_inserter(option_.get().input.tree_data));
        }

        // Getters:

        auto get_run() -> FairRun* { return run_.get(); }
        auto has_mpi() -> bool { return option_.get().enable_mpi; }

      protected:
        template <typename OptionType>
        void ParseApplicationOptionImp(const std::vector<std::string>& filename, OptionType& options);

      private:
        bool is_already_parsed_ = false; // guards for callbacks.
        bool is_dump_ = false;           // guards for callbacks.
        bool has_print_default_options_ = false;
        int num_of_procs_ = 0;
        int rank_num_ = 0;
        std::string app_name_;
        std::string dump_json_filename_;
        std::unique_ptr<FairRun> run_;
        std::reference_wrapper<Options> option_;
        TStopwatch timer_;
        std::vector<std::pair<std::string, bool>> input_files_;

        // private overriden virtual function:
        void set_num_of_procs(int val) override { num_of_procs_ = val; }
        void set_rank_num(int val) override { rank_num_ = val; }
        void dump_options() override { dump_json_options(dump_json_filename_); }
        void init() override;
        void run() override;
        void print_options() override { print_json_options(); }
        [[nodiscard]] auto has_print_default_options() const -> bool override { return has_print_default_options_; }
        [[nodiscard]] auto has_dump() const -> bool override { return is_dump_; }
        void setup_options(CLI::App& program_options) override;
        void post_parse() override;

        // private virtual methods:
        virtual void pre_init(FairRun* run) = 0;
        virtual void ParseApplicationOption(const std::vector<std::string>& filename_or_option) = 0;
        virtual void post_init(FairRun* run) {}
        void add_input_filename(R3BFileSource2* filesource);
        virtual void print_json_options() {}
        virtual void dump_json_options(const std::string& filename) {}
        virtual void run_action(FairRun* run, int num_of_events);
        virtual void setup_application_options(CLI::App& program_options) {}

        // private non-virtual methods:
        void setup_common_options(CLI::App& program_options);
        void add_inout_files();
        void add_inout_pars();
        void extract_input_files();
        static void setup_logger();

        enum class JSONConfigInputType : uint8_t
        {
            native,
            file,
            string,
            invalid
        };
        static auto check_json_input_type(std::string_view input) -> JSONConfigInputType;
        static void transform_to_json_string(std::string_view input, std::string& buffer);
    };

    template <typename OptionType>
    void CLIApplication::print_json_options(const OptionType& options)
    {
        fmt::println("{}", glz::write<glz::opts{ .prettify = true }>(options).value_or("error occurred"));
    }

    template <typename OptionType>
    void CLIApplication::dump_json_options(const OptionType& options, const std::string& filename)
    {

        auto file = std::ofstream{ filename, std::ios::trunc };
        auto buffer = glz::basic_ostream_buffer<std::ofstream>{ file };
        auto error = glz::write<glz::opts{ .prettify = true }>(options, buffer);

        if (error)
        {
            fmt::println("Error occurred:{}", glz::format_error(error));
        }
        else
        {
            fmt::println("Configuration of {} is saved into the file {:?}", app_name_, filename);
        }
    }

    template <typename OptionType>
    void CLIApplication::ParseApplicationOptionImp(const std::vector<std::string>& filenames_or_options,
                                                   OptionType& options)
    {
        auto buffer = std::string{};

        for (const auto& filename_or_option : filenames_or_options)
        {
            buffer.clear();
            auto error_code = glz::error_ctx{};
            switch (check_json_input_type(filename_or_option))
            {
                case JSONConfigInputType::native:
                {
                    buffer = filename_or_option;
                    LOGP(info, "Reading the configuration from the native JSON string {:?}.", buffer);
                    error_code = glz::read_json(options, buffer);
                    break;
                }
                case JSONConfigInputType::string:
                {
                    transform_to_json_string(filename_or_option, buffer);
                    LOGP(info, "Reading the configuration from the string {:?}.", buffer);
                    error_code = glz::read_json(options, buffer);
                    break;
                }
                case JSONConfigInputType::file:
                {
                    LOGP(info, "Reading the configuration from the JSON file {:?}.", filename_or_option);
                    error_code = glz::read_file_json(options, filename_or_option, buffer);
                    break;
                }
                case JSONConfigInputType::invalid:
                    throw R3B::logic_error(fmt::format("Cannot parse the string {:?}", filename_or_option));
            }
            if (error_code)
            {
                throw R3B::logic_error(fmt::format("Failed to parse to the JSON object from {:?}:\n{}",
                                                   filename_or_option,
                                                   glz::format_error(error_code, buffer)));
            }
        }
    }
} // namespace R3B::Neuland
