#include "R3BDataMonitorCanvas.h"

template <typename T>
using remove_const_ref = std::remove_const_t<std::remove_reference_t<T>>;

namespace R3B
{
    DataMonitorCanvas::DataMonitorCanvas(DataMonitor* monitor)
        : monitor_{ monitor }
    {
    }

    void DataMonitorCanvas::draw()
    {
        for (auto& [div_num, figures] : figures_)
        {
            canvas_->cd(div_num);
            for (auto iter = figures.begin(); iter != figures.end(); ++iter)
            {
                auto is_first = (iter == figures.begin());
                std::visit(
                    [is_first](auto& figure)
                    {
                        const auto* option = figure->GetOption();
                        if (is_first)
                        {

                            figure->Draw(option);
                        }
                        else
                        {
                            figure->Draw(fmt::format("same {}", option).c_str());
                        }
                    },
                    *iter);
            }
        }
    }

    void DataMonitorCanvas::reset()
    {
        for (auto& [div_num, hists] : figures_)
        {
            for (auto& hist : hists)
            {
                std::visit(
                    [](auto& figure)
                    {
                        if constexpr (std::is_same_v<TGraph*, remove_const_ref<decltype(figure)>>)
                        {
                            figure->Set(0);
                        }
                        else if constexpr (std::is_same_v<TH1*, remove_const_ref<decltype(figure)>>)
                        {
                            figure->Reset();
                        }
                    },
                    hist);
            }
        }
    }
} // namespace R3B
