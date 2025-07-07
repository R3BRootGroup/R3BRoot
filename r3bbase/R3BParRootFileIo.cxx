#include "R3BParRootFileIo.h"
#include "R3BDetParRootFileIo.h"
#include "R3BException.h"
#include <FairParIo.h>
#include <FairParSet.h>
#include <FairRtdbRun.h>
#include <TError.h>
#include <TFile.h>
#include <TObject.h>
#include <fairlogger/Logger.h>
#include <fmt/core.h>
#include <memory>
#include <root/TKey.h>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace R3B
{
    namespace
    {
        auto get_cycle_number(TFile* root_file, TObject* obj) -> int
        {
            if (auto* key = root_file->GetKey(obj->GetName()); key != nullptr)
            {
                return key->GetCycle();
            }
            LOGP(error,
                 "Failed to get the key with the name {:?} after it's written to the file {:?}",
                 obj->GetName(),
                 root_file->GetName());
            return -1;
        }

    } // namespace

    ParRootFileIo::~ParRootFileIo() = default;
    ParRootFileIo::ParRootFileIo(bool is_input)
        : is_input_{ is_input }
    {
        auto par_det_io = std::make_unique<DetParRootFileIo>(this);
        setDetParIo(par_det_io.release());
    }

    void ParRootFileIo::open(std::unique_ptr<TFile> root_file)
    {
        if (not root_file->IsOpen())
        {
            throw R3B::logic_error(fmt::format("Cannot open the root file {:?}", root_file->GetName()));
        }
        if (is_input_)
        {
            auto option = std::string_view{ root_file->GetOption() };
            if (option != "read" and option != "READ")
            {
                LOGP(warn, "Input parameter file {:?} should have read only permision", root_file->GetName());
            }
        }
        else
        {
            if (not root_file->IsWritable())
            {
                throw R3B::logic_error(fmt::format("Cannot write the root file {:?}", root_file->GetName()));
            }
        }
        root_files_.push_back(std::move(root_file));
    }

    void ParRootFileIo::open(std::string_view file_name, std::string_view file_option)
    {
        open(std::make_unique<TFile>(file_name.data(), file_option.data()));
    }

    void ParRootFileIo::open(const std::vector<std::string>& filenames, std::string_view file_option)
    {
        for (const auto& file_name : filenames)
        {
            open(file_name, file_option);
        }
    }

    bool ParRootFileIo::read_parameter(FairParSet* par)
    {
        auto is_ok = read_parameter_from_run(par, false);
        if (not is_ok)
        {
            is_ok = read_parameter_from_run(par, true);
        }
        return is_ok;
    }

    int ParRootFileIo::write_parameter(FairParSet* par)
    {

        if (par == nullptr)
        {
            throw R3B::runtime_error("Cannot write parameter which is nuulptr!");
        }
        auto* root_file = root_files_.front().get();

        auto ret_val = root_file->WriteObject(par, par->GetName());

        if (ret_val <= 0)
        {
            LOGP(error,
                 "Failed to write the parameter {:?} to the file {:?} with the return value: {}",
                 par->GetName(),
                 root_file->GetName(),
                 ret_val);
            return -1;
        }
        return get_cycle_number(root_file, par);
    }

    auto ParRootFileIo::read_parameter_from_run(FairParSet* par, bool is_ignored) -> bool
    {
        auto old_log = gErrorIgnoreLevel;
        gErrorIgnoreLevel = 1001; // NOLINT
        const auto* par_name = par->GetName();
        for (auto& root_file : root_files_)
        {
            if (not is_ignored)
            {
                auto* par_run = root_file->GetKey(run_name_.data());
                if (par_run == nullptr)
                {
                    continue;
                }
            }

            auto ret_val = root_file->ReadTObject(par, par_name);
            if (ret_val == 0)
            {
                continue;
            }

            LOGP(info, "Read parameter {:?} in the file {:?}", par_name, root_file->GetName());
            gErrorIgnoreLevel = old_log;
            return true;
        }
        gErrorIgnoreLevel = old_log;
        return false;
    }

    void ParRootFileIo::readVersions(FairRtdbRun* current_par_run) { run_name_ = current_par_run->GetName(); }

    void ParRootFileIo::cd()
    {
        if (not is_input_)
        {
            root_files_.front()->cd();
        }
    }

    auto ParRootFileIo::check() -> bool
    {
        if (root_files_.empty())
        {
            if (is_input_)
            {
                LOGP(error, "Input parameter IO has no files to read!");
            }
            else
            {
                LOGP(error, "Output parameter IO has no file to write!");
            }
            return false;
        }

        if (not is_input_ and root_files_.size() > 1)
        {
            if (root_files_.size() > 1)
            {
                LOGP(warn, "Output parameter IO has more than one files (choosing the first)!");
                for (auto& root_file : root_files_)
                {
                    LOGP(warn, "\t{}", root_file->GetName());
                }
            }
        }
        return true;
    }

    void ParRootFileIo::close()
    {
        for (const auto& root_file : root_files_)
        {
            LOGP(debug, "closing the par IO file {}", root_file->GetName());
            root_file->Close();
        }
        FairParIo::close();
    }
} // namespace R3B
