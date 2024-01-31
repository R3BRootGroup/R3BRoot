#pragma once
#include <fstream>
#include <string>
#include <vector>

struct DataPoint
{
    std::vector<float> locals;                  // local derivatives
    std::vector<std::pair<int, float>> globals; // global label and derivatives pair
    float measurement = 0.;                     // measurement corresponding to the error value
    float sigma = 0.;                           // error value
};

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
    inline auto get_current_size() -> std::size_t { return index_buffer_.size() + value_buffer_.size(); }
    inline void add_entry(IndexType index, ValueType value) // NOLINT
    {
        index_buffer_.emplace_back(index);
        value_buffer_.emplace_back(value);
    }
    inline auto is_empty() -> bool { return get_current_size() == 0; }

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

    void mille(const DataPoint& data_point);
    void special(const std::vector<std::pair<int, float>>& special_data);
    inline void kill()
    {
        buffer_.clear();
        has_special_done_ = false;
    }
    void end();

  private:
    bool has_special_done_ = false; // if true, special(..) already called for this record
    bool is_binary_ = true;         // if false output as text
    bool is_zero_written_ = false;  // if true also write out derivatives/labels ==0
    MilleBuffer<int, float> buffer_;
    static constexpr std::size_t max_buffer_size_ = 5000;
    static constexpr unsigned int max_label_size_ = (0xFFFFFFFF - (1U << 31U));
    std::ofstream output_file_; // C-binary for output

    void check_buffer_size(std::size_t nLocal, std::size_t nGlobal);
    void write_to_binary();
    void write_to_non_binary();
};
