#pragma once

#include "R3BDataMonitor.h"
#include "R3BIOConnector.h"
#include "R3BNeulandCalData2.h"
#include "R3BNeulandCalibrationTask.h"
#include <FairRootManager.h>
#include <FairRuntimeDb.h>
#include <map>
#include <string_view>
#include <vector>

class TH2D;
class TH1D;

namespace R3B::Neuland
{
    class CalMonitorTask : public CalibrationTask
    {
      public:
        explicit CalMonitorTask(std::string_view cal_data_name = "NeulandCalData");

      private:
        InputVectorConnector<BarCalData> cal_data_;
        std::vector<int> plane_counter_;

        TH2D* hist_module_hit_num_ = nullptr;
        TH2D* hist_n_module_n_planes_ = nullptr;
        TH1D* hist_plane_hit_num_ = nullptr;
        TH1D* hist_bar_hit_sizes_ = nullptr;
        TH2D* hist_module_hit_n_half_planes_ = nullptr;
        std::map<int, TH2D*> hist_module_hit_n_planes_;

        // overriden functions:
        void HistogramInit(DataMonitor& histograms) override;
        void ExtraInit(FairRootManager* rootMan) override;
        void SetExtraPar(FairRuntimeDb* rtdb) override {}
        void BeginOfEvent() override {};
        void TriggeredExec() override;
        void EndOfTask() override {}
        [[nodiscard]] auto CheckConditions() const -> bool override { return true; }
    };

} // namespace R3B::Neuland
