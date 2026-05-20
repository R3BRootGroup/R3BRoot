#pragma once

#include <string>
#include <string_view>

namespace R3B
{

    template <typename ParType>
    class ParView
    {
      public:
        using Type = ParType;

        /**
         * @brief Constructor
         *
         * @param par_name Paramter name
         */
        explicit ParView(std::string_view par_name)
            : name_{ par_name }
        {
        }

        /**
         * @brief Get the parameter pointer.
         *
         * @return A pointer to the underlying parameter.
         * @see ref
         */
        auto get() const -> Type* { return parameter_; }

        /**
         * @brief Get the name of the paramter
         *
         * @return Parameter name
         */
        [[nodiscard]] auto get_name() const -> std::string_view { return name_; }

        /**
         * @brief Set the pointer of the parameter
         *
         * @param par Parameter pointer
         */
        auto set(Type* par) -> Type* { return parameter_ = par; }

        auto operator->() -> Type* { return parameter_; }

        auto operator*() -> Type& { return *parameter_; }

      private:
        std::string name_;
        ParType* parameter_ = nullptr;
    };

    template <typename ParType>
    class InputParView : public ParView<ParType>
    {
      public:
        /**
         * @brief Constructor
         */
        explicit InputParView(std::string_view par_name)
            : ParView<ParType>{ par_name }
        {
        }

        /**
         * @brief Initialization of input parameter
         *
         * This initializes the parameter by adding the parameter name to the task and set the value from the return of
         * the AddInputPar method. The input task type must have AddInputPar implemented.
         * @param task The task pointer. Usually pass by *this*.
         */
        template <typename TaskType>
        void init(TaskType* task)
        {
            auto* par = task->template AddInputPar<ParType>(this->get_name());
            this->set(par);
        }
    };

    template <typename ParType>
    class OutputParView : public ParView<ParType>
    {
      public:
        /**
         * @brief Constructor
         */
        explicit OutputParView(std::string_view par_name)
            : ParView<ParType>{ par_name }
        {
        }

        /**
         * @brief Initialization of output parameter
         *
         * This initializes the parameter by adding the parameter name to the task and set the value from the return of
         * the AddInputPar method. The input task type must have AddOutputPar implemented.
         * @param task The task pointer. Usually pass by *this*.
         * @see ref
         */
        template <typename TaskType>
        void init(TaskType* task)
        {
            auto* par = task->template AddOutputPar<ParType>(this->get_name());
            this->set(par);
        }
    };

} // namespace R3B
