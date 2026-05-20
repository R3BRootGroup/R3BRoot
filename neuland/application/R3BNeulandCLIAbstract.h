#pragma once

#include <CLI/CLI.hpp>

namespace R3B::Neuland
{
    class CLIAbstract
    {
      public:
        /**
         * @brief Default constructor
         */
        CLIAbstract() = default;

        // rule of 5
        virtual ~CLIAbstract() = default;
        CLIAbstract(const CLIAbstract&) = default;
        CLIAbstract(CLIAbstract&&) = default;
        auto operator=(const CLIAbstract&) -> CLIAbstract& = default;
        auto operator=(CLIAbstract&&) -> CLIAbstract& = default;

        /**
         * @brief Initialization of a CLI program.
         *
         * The initialization process must be done before calling the run method. Setters such as #set_rank_num should
         * be called before the initialization. detailed description
         */
        virtual void init() = 0;

        /**
         * @brief Run the CLI program.
         */
        virtual void run() = 0;

        /**
         * @brief Setup the CLI options given to the program.
         *
         * @param program_options Options fed to the program.
         */
        virtual void setup_options(CLI::App& program_options) = 0;

        /**
         * @brief Action done after the option parsing. Thus this method should be called immediately after calling
         * CLI11_PARSE.
         *
         */
        virtual void post_parse() {}

        /**
         * @brief Check the flag whether the default options should be printed in JSON strings.
         *
         * This metehod is only useful when if the dynamic type of the object is derived from
         * R3B::Neuland::CLIApplication
         * @return Flag of printing JSON string.
         * @see ref
         */
        [[nodiscard]] virtual auto has_print_default_options() const -> bool { return false; }

        /**
         * @brief Check whether the options should be dump to a JSON file.
         *
         * This metehod is only useful when if the dynamic type of the object is derived from
         * R3B::Neuland::CLIApplication
         * @return Flag value
         */
        [[nodiscard]] virtual auto has_dump() const -> bool { return false; }

        virtual void print_options() {}
        virtual void dump_options() {}
        virtual void set_rank_num(int val) {}
        virtual void set_num_of_procs(int val) {}

        // Setters:
        void set_fail(bool is_failed) { is_failed_ = is_failed; }

        // Getters:
        [[nodiscard]] auto has_failed() const -> bool { return is_failed_; }
        [[nodiscard]] auto has_inited() const -> bool { return is_inited_; }

      protected:
        void set_inited(bool is_inited) { is_inited_ = is_inited; }

      private:
        bool is_failed_ = false;
        bool is_inited_ = false;
    };
} // namespace R3B::Neuland
