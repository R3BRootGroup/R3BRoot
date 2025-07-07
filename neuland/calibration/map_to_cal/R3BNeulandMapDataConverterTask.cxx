#include "R3BNeulandMapDataConverterTask.h"
#include "R3BException.h"
#include "R3BPaddleTamexMappedData.h"
#include "R3BPaddleTamexMappedData2.h"
#include "R3BShared.h"
#include <FairRootManager.h>
#include <FairTask.h>
#include <RtypesCore.h>
#include <TCollection.h>
#include <string_view>

namespace R3B::Neuland::Calibration
{

    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    MapDataConverterTask::MapDataConverterTask(std::string_view in_map_data_name,
                                               std::string_view in_trig_map_data_name,
                                               std::string_view out_map_data_name,
                                               std::string_view out_trig_map_data_name)
        : FairTask{ "NeulandMappedDataConverter", 1 }
        , output_mapped_data_{ out_map_data_name }
        , output_trig_mapped_data_{ out_trig_map_data_name }
        , input_mapped_data_name{ in_map_data_name }
        , input_trig_mapped_data_name{ in_trig_map_data_name }

    {
    }
    auto MapDataConverterTask::Init() -> InitStatus
    {

        FairRootManager* mgr = FairRootManager::Instance();
        input_mapped_data_ = dynamic_cast<TClonesArray*>(mgr->GetObject(input_mapped_data_name.c_str()));
        if (input_mapped_data_ == nullptr)
        {
            throw R3B::runtime_error("Branch NeulandMapped not found");
        }
        input_trig_mapped_data_ = dynamic_cast<TClonesArray*>(mgr->GetObject(input_trig_mapped_data_name.c_str()));
        if (input_trig_mapped_data_ == nullptr)
        {
            throw R3B::runtime_error("Branch NeulandTrigMapped not found");
        }

        output_mapped_data_.init();
        output_trig_mapped_data_.init();

        return kSUCCESS;
    }

    void MapDataConverterTask::Exec(Option_t* /*option*/)
    {
        output_mapped_data_.clear();
        output_trig_mapped_data_.clear();

        convert_mapped_data();
        convert_trig_mapped_data();
    }

    void MapDataConverterTask::convert_mapped_data()
    {

        auto& output_mapped_data = output_mapped_data_.get();

        for (auto* input_mapped_data : TRangeDynCast<R3BPaddleTamexMappedData>(input_mapped_data_))
        {
            const auto plane_number = input_mapped_data->GetPlaneId();
            const auto bar_number = input_mapped_data->GetBarId();

            auto double_edge_signal = DoubleEdgeSignal{};

            // TODO: is this true?
            const auto side = (input_mapped_data->GetFineTime1LE() > 0) ? Side::left : Side::right;

            if (side == Side::left)
            {
                double_edge_signal.leading.fine = input_mapped_data->GetFineTime1LE();
                double_edge_signal.leading.coarse = input_mapped_data->GetCoarseTime1LE();
                double_edge_signal.trailing.fine = input_mapped_data->GetFineTime1TE();
                double_edge_signal.trailing.coarse = input_mapped_data->GetCoarseTime1TE();
            }
            else
            {
                double_edge_signal.leading.fine = input_mapped_data->GetFineTime2LE();
                double_edge_signal.leading.coarse = input_mapped_data->GetCoarseTime2LE();
                double_edge_signal.trailing.fine = input_mapped_data->GetFineTime2TE();
                double_edge_signal.trailing.coarse = input_mapped_data->GetCoarseTime2TE();
            }

            auto& plane_map_data = output_mapped_data.try_emplace(plane_number).first->second;
            plane_map_data.push_back(side, bar_number, double_edge_signal);
        }
    }

    void MapDataConverterTask::convert_trig_mapped_data()
    {

        auto& output_trig_mapped_data = output_trig_mapped_data_.get();

        for (auto* input_trig_mapped_data : TRangeDynCast<R3BPaddleTamexMappedData>(input_trig_mapped_data_))
        {
            const auto module_number = input_trig_mapped_data->GetBarId();
            auto& module_map_data = output_trig_mapped_data.try_emplace(module_number).first->second;
            module_map_data.signal.coarse = input_trig_mapped_data->GetCoarseTime1LE();
            module_map_data.signal.fine = input_trig_mapped_data->GetFineTime1LE();
        }
    }

    void MapDataConverterTask::FinishTask() {}
    void MapDataConverterTask::FinishEvent() {}
    void MapDataConverterTask::SetParContainers() {}

} // namespace R3B::Neuland::Calibration
