#include "R3BNeulandMapToCalPar.h"
#include "R3BNeulandParSet.h"
#include "R3BValueError.h"
#include <R3BException.h>
#include <fmt/core.h>
#include <string_view>
#include <sys/types.h>
#include <utility>

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
            auto value_error = relation.value_error[channel_num];
            return value_error;
        }

        throw R3B::logic_error(fmt::format("channel number {} exceeds the maximal bin number {}!",
                                           channel_num,
                                           finetime_correlation_.at(ftType).value_error.size()));
    }
} // namespace R3B
