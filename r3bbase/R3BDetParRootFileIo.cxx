#include "R3BDetParRootFileIo.h"
#include "R3BException.h"
#include "R3BParRootFileIo.h"
#include <FairDetParIo.h>
#include <FairParSet.h>
#include <FairRuntimeDb.h>
#include <fairlogger/Logger.h>
#include <string_view>

namespace R3B
{
    auto DetParRootFileIo::init(FairParSet* par) -> bool
    {
        auto par_name = std::string_view{ par->GetName() };

        if (file_io_ == nullptr)
        {
            throw R3B::logic_error("ParRootFileIo is nullptr!");
        }

        auto is_ok = file_io_->read_parameter(par);
        if (not is_ok)
        {
            LOGP(error, "Failed to initialize the parameter {:?}", par_name);
            return false;
        }

        par->setChanged();
        LOGP(info, "Parameter {:?} is initialized successfully", par_name);
        return true;
    }

    auto DetParRootFileIo::write(FairParSet* par) -> int
    {

        if (file_io_ == nullptr)
        {
            throw R3B::logic_error("ParRootFileIo is nullptr!");
        }
        auto cycle_num = file_io_->write_parameter(par);
        par->setChanged(false);
        FairRuntimeDb::instance()->setVersionsChanged(true);
        return cycle_num;
    }

} // namespace R3B
