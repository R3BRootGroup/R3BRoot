#include "R3BNeulandMapToCalPar.h"
#include <FairDetParIo.h>
#include <FairParIo.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <fmt/core.h>

namespace R3B
{
    Map2CalPar::Map2CalPar(std::string_view name, std::string_view title, std::string_view context, bool own)
        : Neuland::ParSet(name.data(), title.data(), context.data(), own)
    {
    }

    void Map2CalPar::AddModuleParam(unsigned int module_num, TCalVFTXModulePar modulePar)
    {
        moduleParams_.emplace(module_num, std::move(modulePar));
    }

    auto TCalVFTXModulePar::GetFineTime(FTType ftType, uint channel_num) const -> ValueError<double>
    {
        if (channel_num < finetime_correlation_.at(ftType).value_error.size())
        {
            const auto& relation = finetime_correlation_.at(ftType);
            return relation.value_error[channel_num];
        }

        throw R3B::logic_error(fmt::format("channel number {} exceeds the maximal bin number {}!",
                                           channel_num,
                                           finetime_correlation_.at(ftType).value_error.size()));
    }
} // namespace R3B

ClassImp(R3B::Map2CalPar);
ClassImp(R3B::TCalVFTXModulePar);
