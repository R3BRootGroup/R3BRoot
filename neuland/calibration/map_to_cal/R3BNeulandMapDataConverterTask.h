#pragma once

#include "R3BIOConnector.h"
#include "R3BPaddleTamexMappedData2.h"
#include <FairTask.h>
#include <RtypesCore.h>
#include <TClonesArray.h>
#include <string>
#include <string_view>

namespace R3B::Neuland::Calibration
{
    class MapDataConverterTask : public FairTask
    {
      public:
        explicit MapDataConverterTask(std::string_view in_map_data_name = "NeulandMappedData",
                                      std::string_view in_trig_map_data_name = "NeulandTrigMappedData",
                                      std::string_view out_map_data_name = "NeulandMappedData",
                                      std::string_view out_trig_map_data_name = "NeulandTrigMappedData"

        );

      private:
        TClonesArray* input_mapped_data_ = nullptr;
        TClonesArray* input_trig_mapped_data_ = nullptr;
        OutputMapConnector<int, PaddleTamexMappedData> output_mapped_data_;
        OutputMapConnector<int, PaddleTamexTrigMappedData> output_trig_mapped_data_;

        std::string input_mapped_data_name;
        std::string input_trig_mapped_data_name;

        void convert_mapped_data();
        void convert_trig_mapped_data();

        auto Init() -> InitStatus override;
        void FinishTask() override;
        void FinishEvent() override;
        void SetParContainers() override;
        void Exec(Option_t* /*option*/) override;
    };

} // namespace R3B::Neuland::Calibration
