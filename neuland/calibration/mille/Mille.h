#pragma once
#include "MilleEntry.h"
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string_view>
#include <utility>
#include <vector>

namespace R3B
{
    constexpr auto DEFAULT_BUFFER_SIZE = std::size_t{ 10000 };
    template <typename IndexType = int, typename ValueType = float>
    class MilleBuffer
    {
      public:
        MilleBuffer() = default;
        void clear()
        {
            index_buffer_.clear();
            value_buffer_.clear();
        }
        auto get_current_size() -> std::size_t { return index_buffer_.size() + value_buffer_.size(); }
        inline void add_entry(IndexType index, ValueType value) // NOLINT
        {
            index_buffer_.emplace_back(index);
            value_buffer_.emplace_back(value);
        }
        auto is_empty() -> bool { return get_current_size() == 0; }

        auto get_indices() -> const auto& { return index_buffer_; }
        auto get_values() -> const auto& { return value_buffer_; }

      private:
        std::vector<IndexType> index_buffer_; // buffer to store local and global indices
        std::vector<ValueType> value_buffer_; // buffer to store local and global derivatives
    };

    class Mille
    {
      public:
        explicit Mille(std::string_view outFileName, bool asBinary = true, bool writeZero = false);
        void set_buffer_size(std::size_t buffer_size) { max_buffer_size_ = buffer_size; }

        /**
         * @brief Write the MilleDataPoint structure to the internal data buffer of the type MilleBuffer.
         *
         * The conversion of the MilleDataPoint to the MilleBuffer follows the sequence:
         * 1. If the sigma value is negative, returns immediately.
         * 2. Check if the current internal buffer has enough of space to store the latest data point.
         * 3. Check if the size of the global and local values from the data point is zero. If so, store [0, 0] to the
         * internal buffer.
         * 4. Add an index-value pair [0, measurement]
         * 5. Add all local derivatives with indices.
         * 6. Add an index-value pair [0, sigma]
         * 7. Add all global derivatives with indices.
         *
         * @param data_point Input data structure which stores the local/global derivatives, measurement and error.
         */
        void mille(const MilleDataPoint& data_point);
        void special(const std::vector<std::pair<int, float>>& special_data);
        void reset();

        /**
         * @brief Streaming an entry data to the output file.
         *
         * Streaming the entry data to the output file and call the reset() function to clear the internal buffer.
         * Before writing the data from the buffer, the size of the buffer is first written, as the start of the entry
         * data and also the indicator of the end of the entry data.
         */
        void end();
        void close();

        auto get_n_entries() const -> uint64_t { return num_of_entries_; }

      private:
        bool has_special_done_ = false;                     //!< if true, special(..) already called for this record
        bool is_binary_ = true;                             //!< if false output as text
        bool is_zero_written_ = false;                      //!< if true also write out derivatives/labels ==0
        MilleBuffer<int, float> buffer_;                    //!< Data buffer to store the points of the events
        std::size_t max_buffer_size_ = DEFAULT_BUFFER_SIZE; //!< Maximum size of the data buffer
        std::ofstream output_file_;                         //!< C-binary for output
        uint64_t num_of_entries_ = 0;

        static constexpr unsigned int max_label_size_ = (0xFFFFFFFF - (1U << 31U));

        void check_buffer_size(std::size_t nLocal, std::size_t nGlobal);
        void write_to_binary();
        void write_to_non_binary();
    };
} // namespace R3B
