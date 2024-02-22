#include "Mille.h"

#include <fmt/format.h>
#include <fmt/ranges.h>
#include <fstream>
#include <iostream>
#include <range/v3/view.hpp>

namespace rng = ranges;

namespace
{
    template <typename T>
    inline auto get_size_in_bytes()
    {
        return static_cast<std::streamsize>(sizeof(T));
    }

    template <typename T>
    inline auto get_size_in_bytes(const std::vector<T>& data)
    {
        return static_cast<std::streamsize>(sizeof(T) * data.size());
    }

} // namespace

namespace R3B
{
    Mille::Mille(std::string_view outFileName, bool asBinary, bool writeZero)
        : is_binary_(asBinary)
        , is_zero_written_(writeZero)
    {
        output_file_.open(
            outFileName.data(),
            (asBinary ? (std::ios::binary | std::ios::out | std::ios::trunc) : std::ios::out | std::ios::trunc));
        if (!output_file_.is_open())
        {
            throw std::runtime_error(fmt::format("Mille::Mille: Could not open {} as output file.", outFileName));
        }
    }

    void Mille::mille(const MilleDataPoint& data_point)
    {
        if (data_point.sigma <= 0.)
        {
            return;
        }

        check_buffer_size(data_point.locals.size(), data_point.globals.size());

        if (buffer_.get_current_size() == 0)
        {
            buffer_.add_entry(0, 0.);
        }

        buffer_.add_entry(0, data_point.measurement);

        for (const auto [index, value] :
             rng::views::enumerate(data_point.locals) |
                 rng::views::filter([this](const auto& index_deriv)
                                    { return index_deriv.second != 0 or is_zero_written_; }))
        {
            buffer_.add_entry(static_cast<int>(index + 1), value);
        }

        buffer_.add_entry(0, data_point.sigma);

        for (const auto& [label, deriv] : data_point.globals)
        {
            if (deriv != 0 or is_zero_written_)
            {
                if ((label > 0 or is_zero_written_) and label <= max_label_size_)
                {
                    buffer_.add_entry(label, deriv);
                }
                else
                {
                    fmt::print(stderr, "Mille::mille: Invalid label {} <= 0 or > ", label);
                }
            }
        }
    }

    void Mille::special(const std::vector<std::pair<int, float>>& special_data)
    {
        if (special_data.empty())
        {
            return;
        }
        if (has_special_done_)
        {
            throw std::logic_error("Mille::special: Special values already stored for this record.");
        }
        if (buffer_.get_current_size() == 0)
        {
            buffer_.add_entry(0, 0.);
        }

        buffer_.add_entry(0, 0.);
        buffer_.add_entry(0, -static_cast<float>(special_data.size()));
        for (const auto& [index, value] : special_data)
        {
            buffer_.add_entry(index, value);
        }
        has_special_done_ = true;
    }

    void Mille::end()
    {

        if (buffer_.is_empty())
        {
            return;
        }
        is_binary_ ? write_to_binary() : write_to_non_binary();
        kill();
    }
    void Mille::write_to_binary()
    {
        const auto data_size = static_cast<int>(buffer_.get_current_size());
        output_file_.write(reinterpret_cast<const char*>(&data_size), get_size_in_bytes<decltype(data_size)>());
        output_file_.write(reinterpret_cast<const char*>(buffer_.get_values().data()),
                           get_size_in_bytes(buffer_.get_values()));
        output_file_.write(reinterpret_cast<const char*>(buffer_.get_indices().data()),
                           get_size_in_bytes(buffer_.get_indices()));
    }

    void Mille::write_to_non_binary()
    {
        output_file_ << buffer_.get_current_size() << "\n";
        output_file_ << fmt::format("{}\n", fmt::join(buffer_.get_indices(), " "));
        output_file_ << fmt::format("{}\n", fmt::join(buffer_.get_values(), " "));
    }

    void Mille::check_buffer_size(std::size_t nLocal, std::size_t nGlobal)
    {
        if (buffer_.get_current_size() >= max_buffer_size_)
        {
            throw std::runtime_error(
                fmt::format("Mille::checkBufferSize: Buffer too short ({}), \n need space for nLocal ({}) \nGlobal "
                            "({}) local/global derivatives, {} already stored!",
                            max_buffer_size_,
                            nLocal,
                            nGlobal,
                            buffer_.get_current_size()));
        }
    }
} // namespace R3B
