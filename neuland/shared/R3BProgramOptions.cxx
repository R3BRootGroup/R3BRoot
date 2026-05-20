#include "R3BProgramOptions.h"
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <exception>
#include <iostream>
#include <ostream>
#include <stdexcept>

namespace R3B
{
    auto ProgramOptions::verify(int argc, char** argv) -> bool
    {
        try
        {
            po::store(po::command_line_parser(argc, argv).positional(pos_desc_).options(desc_).run(), varMap_);
            po::notify(varMap_);

            if (varMap_.count("help") == 1)
            {
                std::cout << get_desc_ref() << "\n";
                return false;
            }

            for (auto& registrie : registries_)
            {
                registrie.second->retrieve(varMap_);
            }
        }
        catch (const std::runtime_error& err)
        {
            std::cerr << "An exception occurs: " << err.what() << "\n";
            return false;
        }
        catch (const std::exception& err)
        {
            // std::cerr << "exception occurs: " << boost::diagnostic_information(err) << std::endl;
            std::cerr << "An exception occurs: " << err.what() << "\n";
            return false;
        }
        catch (...)
        {
            std::cerr << "An unrecognizable exception occurs!" << "\n";
            return false;
        }
        return true;
    }
} // namespace R3B
