#pragma once
/******************************************************************************
 *   Copyright (C) 2023 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2023-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// TODO: use C++20 std::source_location
#include <boost/assert/source_location.hpp>

#include <FairRootManager.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <TClonesArray.h>
#include <TCollection.h>
#include <TObject.h>
#include <cstddef>
#include <fmt/core.h>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

// TODO: Use C++20 Concept to put more constains on the template parameters
namespace R3B
{
    template <typename InputType>
    class InputConnector
    {
      public:
        using RawDataType = std::remove_const_t<std::remove_cv_t<InputType>>;
        explicit InputConnector(std::string_view branchName)
            : branch_name_{ branchName }
        {
        }

        /**
         * \brief Initialisation for the connector of non-TCA input data.
         *
         * Initialize a non-TCA input data from the root file.
         * @param is_optional Don't report error if the input data doesn't exist.
         * @param loc Location for the current call site for better logging.
         */
        void init(bool is_optional = false, const boost::source_location& loc = BOOST_CURRENT_LOCATION)
        {
            auto* ioman = FairRootManager::Instance();
            if (ioman == nullptr)
            {
                throw R3B::runtime_error(fmt::format("FairRootManager is nullptr during the initialisation of the "
                                                     "input data with  the branch name \"{}\"",
                                                     branch_name_),
                                         loc);
            }

            data_ = ioman->InitObjectAs<const RawDataType*>(branch_name_.c_str());

            if (data_ == nullptr)
            {
                if (is_optional)
                {
                    R3BLOG(warn,
                           fmt::format("The data branch {:?} doesn't exist in the input root file!", branch_name_));
                }
                else
                {
                    throw R3B::runtime_error(
                        fmt::format("Initialisation of the input data with the branch name \"{}\" failed!",
                                    branch_name_),
                        loc);
                }
            }
        }

        [[nodiscard]] auto get(const boost::source_location& loc = BOOST_CURRENT_LOCATION) const -> const RawDataType&
        {
            check_init(loc);
            return *data_;
        }

        auto size(const boost::source_location& loc = BOOST_CURRENT_LOCATION) const
        {
            check_init(loc);
            return data_->size();
        }

        // implement range-based for loop:
        // TODO: is there a simpler way?
        auto begin(const boost::source_location& loc = BOOST_CURRENT_LOCATION)
        {
            check_init(loc);
            return data_->cbegin();
        }
        auto begin(const boost::source_location& loc = BOOST_CURRENT_LOCATION) const
        {
            check_init(loc);
            return data_->cbegin();
        }
        auto end(const boost::source_location& loc = BOOST_CURRENT_LOCATION)
        {
            check_init(loc);
            return data_->cend();
        }
        auto end(const boost::source_location& loc = BOOST_CURRENT_LOCATION) const
        {
            check_init(loc);
            return data_->cend();
        }

      private:
        std::string branch_name_;
        const RawDataType* data_ = nullptr;
        void check_init(const boost::source_location& loc) const
        {
            if (data_ == nullptr)
            {
                throw R3B::runtime_error(fmt::format("Input data with the branch name \"{}\" cannot be "
                                                     "queried without an initialisation!",
                                                     branch_name_),
                                         loc);
            }
        }
    };

    template <typename InputType,
              typename = std::enable_if_t<std::is_base_of_v<TObject, std::remove_const_t<std::remove_cv_t<InputType>>>>>
    class InputTCAConnector
    {
      public:
        using RawDataType = std::remove_const_t<std::remove_cv_t<InputType>>;
        explicit InputTCAConnector(std::string_view branchName)
            : branch_name_{ branchName }
        {
        }

        void init(bool is_optional = false, const boost::source_location& loc = BOOST_CURRENT_LOCATION)
        {
            auto* ioman = FairRootManager::Instance();
            if (ioman == nullptr)
            {
                throw R3B::runtime_error(fmt::format("FairRootManager is nullptr during the initialisation of the "
                                                     "input data with  the branch name \"{}\"",
                                                     branch_name_),
                                         loc);
            }

            data_ = dynamic_cast<TClonesArray*>(ioman->GetObject(branch_name_.c_str()));
            if (data_ == nullptr)
            {
                const auto msg = fmt::format(
                    "Input TCA data with the branch name {:?} cannot be retrieved from the input file !", branch_name_);
                if (is_optional)
                {
                    R3BLOG(warn, msg.c_str());
                    return;
                }
                throw R3B::runtime_error(msg, loc);
            }

            check_element_type(is_optional, loc);
        }

        auto read() -> const std::vector<RawDataType>&
        {
            output_data_.clear();
            if (data_ == nullptr)
            {
                return output_data_;
            }

            output_data_.reserve(data_->GetEntriesFast());
            for (auto* element : TRangeDynCast<RawDataType>(data_))
            {
                output_data_.emplace_back(*element);
            }
            return output_data_;
        }

        [[nodiscard]] auto size() const -> std::size_t { return data_ == nullptr ? 0 : data_->GetEntriesFast(); }

        [[nodiscard]] auto get() const -> const std::vector<RawDataType>& { return output_data_; }
        [[nodiscard]] auto get_name() const -> const std::string& { return branch_name_; }

      private:
        std::string branch_name_;
        std::vector<RawDataType> output_data_;
        TClonesArray* data_ = nullptr;

        void check_element_type(bool is_optional, const boost::source_location& loc)
        {
            if (std::string_view{ data_->GetClass()->GetName() } != InputType::Class_Name())
            {
                const auto msg = fmt::format(
                    "The type of the retrieved data {:?} is not the same as the type given by the connector class {}!",
                    data_->GetClass()->GetName(),
                    InputType::Class_Name());
                if (is_optional)
                {
                    R3BLOG(warn, msg.c_str());
                    return;
                }
                throw R3B::runtime_error(msg, loc);
            }
        }
    };

    template <typename OutputType>
    class OutputConnector
    {
      public:
        using RawDataType = std::remove_const_t<std::remove_cv_t<OutputType>>;
        explicit OutputConnector(std::string_view branchName)
            : branch_name_{ branchName }
        {
        }

        // rule of 5
        ~OutputConnector() = default;
        OutputConnector(const OutputConnector&) = delete;
        OutputConnector(OutputConnector&&) = delete;
        OutputConnector& operator=(const OutputConnector& other) = delete;
        OutputConnector& operator=(OutputConnector&&) = delete;

        void init(bool persistance = true, const boost::source_location& loc = BOOST_CURRENT_LOCATION)
        {
            if (auto* ioman = FairRootManager::Instance(); ioman != nullptr)
            {
                ioman->RegisterAny(branch_name_.c_str(), data_ptr_, persistance);
            }
            else
            {
                throw R3B::runtime_error(fmt::format("FairRootManager is nullptr during the initialisation of the "
                                                     "output data with the branch name \"{}\"",
                                                     branch_name_),
                                         loc);
            }
        }

        [[nodiscard]] auto get() -> RawDataType& { return data_; }
        [[nodiscard]] auto get_constref() const -> const RawDataType& { return data_; }
        [[nodiscard]] auto get_name() const -> const std::string& { return branch_name_; }

        void clear() { data_.clear(); }

        template <typename ResetOp>
        void clear(ResetOp opn)
        {
            opn(data_);
        }

        auto size() const { return data_.size(); }

      private:
        std::string branch_name_;
        RawDataType data_;
        RawDataType* data_ptr_ = &data_;
    };

    template <typename ElementType>
    using InputVectorConnector = InputConnector<std::vector<ElementType>>;

    template <typename ElementType>
    using OutputVectorConnector = OutputConnector<std::vector<ElementType>>;

    template <typename KeyType, typename ValueType>
    using InputMapConnector = InputConnector<std::map<KeyType, ValueType>>;

    template <typename KeyType, typename ValueType>
    using OutputMapConnector = OutputConnector<std::map<KeyType, ValueType>>;

    template <typename KeyType, typename ValueType>
    using InputHashConnector = InputConnector<std::unordered_map<KeyType, ValueType>>;

    template <typename KeyType, typename ValueType>
    using OutputHashConnector = OutputConnector<std::unordered_map<KeyType, ValueType>>;
} // namespace R3B
