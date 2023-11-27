/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once
#include <FairParRootFileIo.h>
#include <FairRuntimeDb.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <fmt/format.h>

namespace R3B::Neuland
{
    class ParDirCreator
    {
      public:
        explicit ParDirCreator(
            FairParRootFileIo* parFileIO = dynamic_cast<FairParRootFileIo*>(FairRuntimeDb::instance()->getOutput()))
            : file_io_{ parFileIO }
            , old_dir_{ gDirectory }
        {
            if (file_io_ == nullptr)
            {
                throw R3B::runtime_error("unable to obtain the FairParRootFileIo object!");
            }
            par_root_file_ = parFileIO->getParRootFile();

            if (!par_root_file_->IsOpen() || !par_root_file_->IsWritable())
            {
                throw R3B::runtime_error("parRootFile is either closed or unable to be written!");
            }
        }

        ~ParDirCreator() noexcept { old_dir_->cd(); }
        ParDirCreator(const ParDirCreator&) = delete;
        ParDirCreator(ParDirCreator&&) = delete;
        auto operator=(const ParDirCreator&) -> ParDirCreator& = delete;
        auto operator=(ParDirCreator&&) -> ParDirCreator& = delete;

        auto mkdir(std::string_view dir_name) -> auto*
        {
            if (par_root_file_->IsZombie())
            {
                throw R3B::runtime_error(
                    fmt::format("File {} is not closed properly! Please delete it and create a new one!",
                                par_root_file_->GetName()));
            }
            par_root_file_->cd();
            if (dir_name.empty())
            {
                throw R3B::runtime_error("Cannot create a folder with empty string!");
            }
            if (not gDirectory->IsWritable())
            {
                throw R3B::runtime_error(fmt::format("Directory {} is not writable currently!", gDirectory->GetName()));
            }

            // This is very slow
            // // rm the dir and all its contents if existing
            // if (auto* existing_dir = gDirectory->GetDirectory(dir_name.data()); existing_dir != nullptr)
            // {
            //     R3BLOG(info, fmt::format("removing directory {} in {}", dir_name, gDirectory->GetName()));
            //     gDirectory->rmdir(dir_name.data());
            // }
            // else
            // {
            //     R3BLOG(
            //         info,
            //         fmt::format("No directory {} in {}. Creating a new directory!", dir_name,
            //         gDirectory->GetName()));
            // }

            R3BLOG(info, fmt::format(R"("Creating a directory "{}" in "{}"")", dir_name, gDirectory->GetName()));
            auto* new_dir = gDirectory->mkdir(dir_name.data(), dir_name.data(), true);
            if (new_dir == nullptr)
            {
                throw R3B::runtime_error("Cannot create a directory for the histrogams!");
            }
            return new_dir;
        }

      private:
        FairParRootFileIo* file_io_ = nullptr;
        FairParRootFile* par_root_file_ = nullptr;
        TDirectory* old_dir_ = nullptr;
    };
} // namespace R3B::Neuland
