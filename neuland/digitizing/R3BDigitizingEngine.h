/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef NEULAND_DIGITIZING_ENGINE_H
#define NEULAND_DIGITIZING_ENGINE_H

#include "R3BDigitizingChannel.h"
#include "R3BDigitizingPaddle.h"
#include "R3BShared.h"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <memory>
#include <range/v3/algorithm/find_if.hpp>
#include <range/v3/view/take.hpp>
#include <range/v3/view/transform.hpp>
#include <type_traits>
#include <utility>
#include <vector>

namespace R3B::Digitizing
{
    class EngineInterface
    {
      public:
        /// Default constructor
        EngineInterface() = default;

        /// virtual destructor
        virtual ~EngineInterface() = default;

        /// Copy constructor
        EngineInterface(const EngineInterface& other) = delete;
        /// Copy assignment operator
        auto operator=(const EngineInterface& other) -> EngineInterface& = delete;
        /// Move constructor
        EngineInterface(EngineInterface&& other) = delete;
        /// Move assignment operator
        auto operator=(EngineInterface&& other) -> EngineInterface& = delete;

        /**
         * \brief Takes in a light deposition and pass it to the paddle
         *
         * This public interface takes in a light deposition with a certain paddle_id, time, light intensity and
         * distance to the center point. If the paddle corresponding to the light deposition doesn't exist, new paddle
         * will be appended in the engine.
         * @param paddle_id ID of the paddle with the light deposition
         * @param time Time of the light deposition
         * @param energy_dep Energy of the light deposition (MeV)
         * @param dist Distance to the center point of the bar
         */
        void DepositLight(int paddle_id, double time, double energy_dep, double dist)
        {
            auto& paddle = add_paddle(paddle_id);
            paddle.DepositLight({ time, energy_dep, dist });
        }

        /**
         * \brief Get the trigger time of the current event.
         *
         * Get the trigger time value from the paddle objects. The value is the minimal trigger time of each paddle.
         */
        [[nodiscard]] auto GetTriggerTime() const -> double
        {
            auto paddles_view = ranges::views::take(paddles_, size_);
            auto min_element = std::min_element(paddles_view.begin(),
                                                paddles_view.end(),
                                                [](const auto& left, const auto& right)
                                                { return left->GetTrigTime() < right->GetTrigTime(); });
            return (min_element == paddles_view.end()) ? NAN : min_element->get()->GetTrigTime();
        }

        /**
         * \brief Initialization of the engine class.
         *
         * The initialization method should be called in the `Init` function of each task. During the initialization,
         * memories for paddle class are allocated with extra initialization from the derived class.
         * @param initial_capacity Number of paddle objects to be allocated.
         */
        void Init(int initial_capacity = 1)
        {
            ExtraInit(initial_capacity);
            reserve_additional_paddles(initial_capacity);
        }

        /**
         * \brief Reset the engine for a new event.
         *
         * Resetting the engine for a new event. This public method should be called in the beginning of each event.
         */
        void Reset()
        {
            ExtraReset();
            size_ = 0;
        }

        /**
         * \brief Construct singals from the paddles and channels
         *
         * Call the Construct mumber function from each paddle object. This method must be called once per event after
         * all the points are read by the engine.
         * @see R3B::Digitizing::Paddle::Construct()
         */
        void Construct()
        {
            for (auto& paddle : ranges::views::take(paddles_, size_))
            {
                paddle->Construct();
            }
        }

        /**
         * \brief The number of preallocated bar objects in the engine.
         *
         * The number of preallocated bar objects in the engine. This should always be smaller than the size of stored
         * bar objects. If the size of stored bar objects grows larger than the capacity. Memeory allocation will occur!
         */
        [[nodiscard]] auto get_capacity() const { return paddles_.size(); };

        /**
         * \brief Accessor operator for each paddle stored in the current event.
         *
         * Perfrom an action on each paddle object in the engine. The action is specified by a lambda function,
         * whose input value should be `const R3B::Digitizing::Paddle&`.
         *
         * For example:
         *
         * ```c++
            auto action = [](const R3B::Digitizing::Paddle& paddle) {
                fmt::println("Paddle ID: {}", paddle.GetPaddleID());
            };
            engine.DoEachPaddles(action);
         * ```
         * This prints out all IDs of the paddles with hits in the current event.
         *
         * @param fnt A unary functor/lambda
         * @see DoAllPaddles()
         */
        template <typename Unary>
        void DoEachPaddle(Unary fnt) const
        {
            for (const auto& paddle : ranges::views::take(paddles_, size_))
            {
                fnt(*paddle);
            }
        }

        /**
         * \brief Accessor operator for all paddles stored in the current event.
         *
         * Perfrom an action on all paddle objects in the engine. The action is specified by a lambda function,
         * whose input value should a **[range](https://en.cppreference.com/w/cpp/ranges/range)** of `const
         R3B::Digitizing::Paddle&`.
         *
         * For example:
         *
         * ```c++
            auto action = [](auto paddles_view)
            {
                return static_cast<double>(
                    std::count_if(paddles_view.begin(), paddles_view.end(), [](const auto& paddle) { return
         paddle.HasFired(); }));
            }
            auto val = engine.DoAllPaddles(action);
         * ```
         * This get the number of paddles that have actually fired.
         *
         * @param fnt A unary functor/lambda
         * @see DoEachPaddle()
         */
        template <typename Unary>
        auto DoAllPaddles(Unary fnt) const
        {
            return fnt(ranges::views::take(paddles_, size_) |
                       ranges::views::transform([](auto& paddle) -> const AbstractPaddle& { return *paddle; }));
        }

      private:
        std::atomic<int> size_ = 0; //!< size of bars with valid signals in the current event
        std::vector<std::unique_ptr<AbstractPaddle>>
            paddles_; //!< main data. This vector should rarely grow in the event loop!

        [[nodiscard]] virtual auto make_new_paddle() const -> std::unique_ptr<AbstractPaddle> = 0;
        virtual void ExtraInit(int initial_capacity) {}
        virtual void ExtraReset() {}

        /**
         * \brief Reserve memories for additional number of paddles
         *
         * @param num Number of additional paddles to be reserved
         */
        void reserve_additional_paddles(int num)
        {
            {
                paddles_.reserve(num + get_capacity());
                for (int idx{}; idx < num; ++idx)
                {
                    paddles_.emplace_back(make_new_paddle());
                }
            }
        }

        auto add_paddle(int paddle_id) -> AbstractPaddle&
        {
            // find if a paddle with the paddle_id is already added
            auto valid_view = ranges::views::take(paddles_, size_);
            auto iter =
                ranges::find_if(valid_view, [paddle_id](auto& paddle) { return paddle->GetPaddleID() == paddle_id; });
            if (iter != valid_view.end())
            {
                return *(iter->get());
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
    };

    // factory classes for paddle and channel:
    template <typename ChannelClass, typename = std::enable_if_t<std::is_base_of_v<AbstractChannel, ChannelClass>>>
    struct UseChannel
    {
        template <typename... Args>
        explicit UseChannel(const Args&... args)
            : BuildChannel([=](Side side) { return std::make_unique<ChannelClass>(side, args...); })
        {
        }
        std::function<std::unique_ptr<ChannelClass>(Side)> BuildChannel;
    };

    template <typename PaddleClass, typename = std::enable_if_t<std::is_base_of_v<AbstractPaddle, PaddleClass>>>
    struct UsePaddle
    {
        template <typename... Args>
        explicit UsePaddle(const Args&... args)
            : BuildPaddle([=](int paddleID) { return std::make_unique<PaddleClass>(paddleID, args...); })
        {
        }
        std::function<std::unique_ptr<PaddleClass>(int)> BuildPaddle;
    };

    template <typename PaddleClass, typename ChannelClass, typename InitFunc = std::function<void()>>
    class Engine : public EngineInterface
    {
      public:
        Engine(
            const UsePaddle<PaddleClass>& p_paddleClass,
            const UseChannel<ChannelClass>& p_channelClass,
            InitFunc initFunc = []() {})
            : paddleClass_{ p_paddleClass }
            , channelClass_{ p_channelClass }
            , EngineInterface()
            , initFunc_{ initFunc }
        {
        }

        /**
         * \brief Initialize the underlying data.
         *
         * Constructing a number of bars for the input data. If the available bars are not enough for the new data, the
         * whole data will be reallocated (just like std::vector). The reallocation should happen rarely!
         * @param initFunc Initiator functor
         */
        void SetInit(const InitFunc& initFunc) { initFunc_ = initFunc; }
        void ExtraReset() override {}

      private:
        UsePaddle<PaddleClass> paddleClass_;    //!< Paddle class factory
        UseChannel<ChannelClass> channelClass_; //!< Channel class factory
        InitFunc initFunc_;

        void ExtraInit(int /*initial_capacity*/) override { initFunc_(); }
        [[nodiscard]] auto make_new_paddle() const -> std::unique_ptr<AbstractPaddle> override
        {
            auto new_paddle = paddleClass_.BuildPaddle(-1);
            new_paddle->SetChannel(channelClass_.BuildChannel(R3B::Side::left));
            new_paddle->SetChannel(channelClass_.BuildChannel(R3B::Side::right));
            return new_paddle;
        }
    };

    // helper to create owning digitizingEngine:
    template <typename... Args>
    [[nodiscard]] auto CreateEngine(Args&&... args) -> std::unique_ptr<decltype(Engine{ std::forward<Args>(args)... })>
    {
        return std::make_unique<decltype(Engine{ std::forward<Args>(args)... })>(std::forward<Args>(args)...);
    }

} // namespace R3B::Digitizing

#endif // NEULAND_DIGITIZING_ENGINE_H
