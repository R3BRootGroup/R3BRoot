#pragma once

#include <FairParIo.h>
#include <Rtypes.h>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

class TFile;
class FairParSet;

namespace R3B
{
    class ParRootFileIo : public FairParIo
    {
      public:
        explicit ParRootFileIo(bool is_input);
        ParRootFileIo(const ParRootFileIo&) = delete;
        ParRootFileIo(ParRootFileIo&&) = delete;
        ParRootFileIo& operator=(const ParRootFileIo&) = delete;
        ParRootFileIo& operator=(ParRootFileIo&&) = delete;
        ~ParRootFileIo() override;

        void open(std::string_view file_name, std::string_view file_option);
        void open(const std::vector<std::string>& filenames, std::string_view file_option = "read");
        void open(std::unique_ptr<TFile> root_file);

        /**
         * @brief Read a parameter from the input root file.
         *
         * Reading the parameter from the input root file and store the values directly to the given parset. The input
         * parset pointer must point to an already allocated parset.
         * @param par Parset variable to store the value.
         * @return true if the read operation is succeeded and false if not.
         */
        bool read_parameter(FairParSet* par);

        /**
         * @brief Write the parameter to the output root file.
         *
         * Writing the parameter to the output root file and return the cycle number of the written parameter.
         * @param par Parameter to be written
         * @return The cycle number of the written parameter. -1 if the writting process failed.
         */
        int write_parameter(FairParSet* par);

        /**
         * @brief Factory method to create an instance for parameter input.
         *
         * @return Instance of the file io
         */
        static auto Input() { return std::make_unique<ParRootFileIo>(true); }

        /**
         * @brief Factory method to create an instance for parameter output.
         *
         * @return Instance of the file io
         */
        static auto Output() { return std::make_unique<ParRootFileIo>(false); }

        auto get_first_root_file() -> auto* { return root_files_.front().get(); }

      private:
        bool is_input_ = true;
        std::string run_name_;
        std::vector<std::unique_ptr<TFile>> root_files_; //!

        // private non-virtual functions:
        auto read_parameter_from_run(FairParSet* par, bool is_ignored) -> bool;

        // private virtual functions:
        void readVersions(FairRtdbRun* current_par_run) override;
        auto check() -> bool override;
        void close() override;
        void print() override {}
        void cd() override;

      public:
        ClassDefOverride(ParRootFileIo, 1);
    };

} // namespace R3B
