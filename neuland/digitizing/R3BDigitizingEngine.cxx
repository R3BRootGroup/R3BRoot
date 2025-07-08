#include "R3BDigitizingEngine.h"
#include "R3BDigitizingPaddle.h"
#include <cmath>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/algorithm/min_element.hpp>
#include <range/v3/view/take.hpp>

namespace R3B::Digitizing
{
    void EngineInterface::Construct()
    {
        for (auto& paddle : ranges::views::take(paddles_, size_))
        {
            paddle->Construct();
        }
    }
    [[nodiscard]] auto EngineInterface::GetTriggerTime() const -> double
    {
        auto paddles_view = ranges::views::take(paddles_, size_);
        auto min_element = ranges::min_element(paddles_view,
                                               [](const auto& left, const auto& right)
                                               { return left->GetTrigTime() < right->GetTrigTime(); });
        return (min_element == paddles_view.end()) ? NAN : min_element->get()->GetTrigTime();
    }

    void EngineInterface::Init(int initial_capacity)
    {
        ExtraInit(initial_capacity);
        reserve_additional_paddles(initial_capacity);
    }

    void EngineInterface::reserve_additional_paddles(int num)
    {
        {
            paddles_.reserve(num + get_capacity());
            for (int idx{}; idx < num; ++idx)
            {
                paddles_.emplace_back(make_new_paddle());
            }
        }
    }

    auto EngineInterface::add_paddle(int paddle_id) -> AbstractPaddle&
    {
        // find if a paddle with the paddle_id is already added
        auto valid_view = ranges::views::take(paddles_, size_);
        auto iter =
            ranges::find_if(valid_view, [paddle_id](auto& paddle) { return paddle->GetPaddleID() == paddle_id; });
        if (iter != valid_view.end())
        {
            return *(*iter);
        }
        // check if size is still smaller than capacity
        if (size_ >= get_capacity())
        {
            // underlying data storage need to grow
            reserve_additional_paddles(size_ / 2);
        }
        auto* new_paddle = paddles_.at(size_++).get();
        new_paddle->Reset();
        new_paddle->SetPaddleID(paddle_id);
        return *new_paddle;
    }
} // namespace R3B::Digitizing
