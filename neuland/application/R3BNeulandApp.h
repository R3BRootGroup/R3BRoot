#pragma once

#include <R3BNeulandCLIAbstract.h>
#include <TStopwatch.h>
#include <algorithm>
#include <fmt/core.h>
#include <fstream>
#include <functional>
#include <ios>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
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
            int event_num = DEFAULT_EVENT_NUM;
            bool enable_mpi = false;
            std::string log_level = "error";
            std::string verbose_level = "user1";
            struct Input
            {
                std::string working_dir;
                std::vector<std::string> data;
                std::vector<std::string> tree_data;
                std::string par;
            } input;
            struct Output
            {
                std::string working_dir;
                std::string data = "output.root";
                std::string par = "output.par.root";
                std::string mode = "update";
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
        void set_inputpar_filename(std::string_view inputpar_filename) { option_.get().input.par = inputpar_filename; }
        void set_outputpar_filename(std::string_view outputpar_filename)
        {
            option_.get().output.par = outputpar_filename;
        }
        void set_event_num(int val) { option_.get().event_num = val; }
        void set_input_filename(const std::vector<std::string>& input_filename)
        {
            std::copy(input_filename.begin(), input_filename.end(), std::back_inserter(option_.get().input.data));
        }
        void set_tree_input_filename(const std::vector<std::string>& input_filename)
        {
            std::copy(input_filename.begin(), input_filename.end(), std::back_inserter(option_.get().input.tree_data));
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
        static void patch_files_or_strings(nlohmann::ordered_json& json_obj,
                                           const std::vector<std::string>& filenames_or_options);
    };

    template <typename OptionType>
    void CLIApplication::print_json_options(const OptionType& options)
    {
        using json = nlohmann::ordered_json;
        auto json_obj = json{ options };
        if (json_obj.is_array())
        {
            fmt::print("{}\n", json_obj.front().dump(4));
        }
        else
        {
            fmt::print("{}\n", json_obj.dump(4));
        }
    }

    template <typename OptionType>
    void CLIApplication::dump_json_options(const OptionType& options, const std::string& filename)
    {
        using json = nlohmann::ordered_json;
        auto file = std::ofstream{ filename, std::ios::trunc };
        auto json_obj = json{ options };
        if (json_obj.is_array())
        {
            file << json_obj.front().dump(4);
        }
        else
        {
            file << json_obj.dump(4);
        }
        fmt::println("Configuration of {} is saved into the file {:?}", app_name_, filename);
    }

    template <typename OptionType>
    void CLIApplication::ParseApplicationOptionImp(const std::vector<std::string>& filenames_or_options,
                                                   OptionType& options)
    {
        auto json_obj = [&options]()
        {
            auto json_obj_tmp = nlohmann::ordered_json{ options };
            return json_obj_tmp.is_array() ? json_obj_tmp.front() : json_obj_tmp;
        }();
        patch_files_or_strings(json_obj, filenames_or_options);
        json_obj.get_to(options);
    }
} // namespace R3B::Neuland
