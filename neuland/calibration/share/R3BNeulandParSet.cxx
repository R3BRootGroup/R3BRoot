#include "R3BNeulandParSet.h"
#include <FairDetParIo.h>
#include <FairParIo.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <gsl/span>
#include <range/v3/algorithm.hpp>

using gsl::span;

namespace R3B::Neuland
{
    ParSet::ParSet(std::string_view name, std::string_view title, std::string_view context, bool own)
        : FairParSet(name.data(), title.data(), context.data(), own)
    {
        // TODO: versions should be initialized in FairParSet!
        ranges::fill(span(versions), -1);
    }

    auto ParSet::init(FairParIo* inputFile) -> bool
    {
        R3BLOG(debug, fmt::format("Try to init parameter {}", GetName()));
        if (auto* detParIo = inputFile->getDetParIo("FairGenericParIo"); detParIo != nullptr)
        {
            R3BLOG(debug, fmt::format("Found DetParIo {}. Try to init with this.", detParIo->GetName()));
            return (detParIo->init(this));
        }

        R3BLOG(error, fmt::format("Failed to init parameter {} because no FairDetParIo object is found!", GetName()));
        return false;
    }

    auto ParSet::write(FairParIo* outputFile) -> int
    {
        // writes the container to an output
        if (auto* detParIo = outputFile->getDetParIo("FairGenericParIo"); detParIo != nullptr)
        {
            R3BLOG(info, fmt::format("writing parameter {} to root file.", GetName()));
            return detParIo->write(this);
        }
        throw R3B::runtime_error(
            fmt::format("Failed to write parameter {} because no FairDetParIo object is found!", GetName()));
    }
} // namespace R3B::Neuland

ClassImp(R3B::Neuland::ParSet);
