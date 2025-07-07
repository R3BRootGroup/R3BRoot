#include "R3BNeulandParSet.h"
#include "R3BDetParRootFileIo.h"
#include <FairDetParIo.h>
#include <FairParIo.h>
#include <R3BException.h>
#include <Rtypes.h>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <gsl/span>
#include <range/v3/algorithm/fill.hpp>
#include <string_view>

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
        LOGP(debug, "Try to init parameter {}", GetName());
        if (auto* detParIo = inputFile->getDetParIo(DetParRootFileIo::DEFAULT_NAME.data()); detParIo != nullptr)
        {
            LOGP(debug, "Found DetParIo {}. Try to init with this.", detParIo->GetName());
            auto res = detParIo->init(this);
            if (not res)
            {
                LOGP(error, "Parameter {} init failed.", detParIo->GetName());
            }
            return res;
        }

        LOGP(error, "Failed to init parameter {} because no FairDetParIo object is found!", GetName());
        return false;
    }

    auto ParSet::write(FairParIo* outputFile) -> int
    {
        // writes the container to an output
        if (auto* detParIo = outputFile->getDetParIo("FairGenericParIo"); detParIo != nullptr)
        {
            LOGP(info, "writing parameter {} to root file.", GetName());
            return detParIo->write(this);
        }
        throw R3B::runtime_error(
            fmt::format("Failed to write parameter {} because no FairDetParIo object is found!", GetName()));
    }
} // namespace R3B::Neuland

ClassImp(R3B::Neuland::ParSet);
