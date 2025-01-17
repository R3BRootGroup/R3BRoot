#define JSON_USE_IMPLICIT_CONVERSIONS 0

#include <CLI/CLI.hpp>
#include <R3BLogger.h>
#include <R3BNeulandAppOptionJson.h>
#include <fmt/format.h>
#ifdef HAS_MPI
#include <mpi.h>
#endif

auto main(int argc, char** argv) -> int
{
    auto is_failed = false;
    auto num_proc = 0;
    auto num_rank = 0;
#ifdef HAS_MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &num_proc);
    MPI_Comm_rank(MPI_COMM_WORLD, &num_rank);
#endif

    auto app = std::unique_ptr<R3B::Neuland::Application>{};
    try
    {
        auto program_options = CLI::App{ "Neuland command line interface" };

        auto instantiate_app = [&app, &program_options](std::size_t)
        {
            if (program_options.got_subcommand("sim"))
            {
                app = std::make_unique<R3B::Neuland::SimulationApplication>();
                app->setup_options(*program_options.get_subcommand("sim"));
            }
            if (program_options.got_subcommand("ana"))
            {
                app = std::make_unique<R3B::Neuland::AnalysisApplication>();
                app->setup_options(*program_options.get_subcommand("ana"));
            }
        };

        program_options.require_subcommand(1, 1);
        program_options.add_subcommand("sim", "Run simulation application.")->preparse_callback(instantiate_app);
        program_options.add_subcommand("ana", "Run analysis application.")->preparse_callback(instantiate_app);

        CLI11_PARSE(program_options, argc, argv);

        if (app == nullptr)
        {
            throw R3B::runtime_error("Application is not instantiated!");
        }

        if (app->has_print_default_options())
        {
            app->print_options();
            return EXIT_SUCCESS;
        }
        if (app->has_dump())
        {
            app->dump_options();
            return EXIT_SUCCESS;
        }

        app->init();
        app->run();
    }
    catch (const std::exception& ex)
    {
        R3BLOG(error, fmt::format("Exception ocurred: {}\n", ex.what()));
        is_failed = true;
        app->set_fail(is_failed);
    }

#ifdef HAS_MPI
    MPI_Finalize();
#endif

    if (is_failed)
    {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
