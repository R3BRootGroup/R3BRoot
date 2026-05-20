#include "R3BNeulandMapToCalParTask.h"
#include "R3BException.h"
#include "R3BNeulandCalibrationTask.h"
#include "R3BNeulandMapToCalPar.h"
#include <FairParRootFileIo.h>
#include <FairRootManager.h>
#include <FairRun.h>
#include <FairRuntimeDb.h>
#include <R3BLogger.h>
#include <R3BParRootFileIo.h>
#include <TDirectory.h>
#include <fmt/core.h>
#include <string_view>
#include <utility>

namespace
{
    template <typename EngineType, typename... Args>
    void FillEngine(EngineType& engine, R3B::FTType type, unsigned int ftValue, Args&&... args)
    {
        engine.Fill(type, ftValue, std::forward<Args>(args)...);
    }
} // namespace

namespace R3B::Neuland
{
    // NOLINTNEXTLINE: bugprone-easily-swappable-parameters
    Map2CalParTask::Map2CalParTask(std::string_view mapped_data_name,
                                   std::string_view trig_mapped_data_name,
                                   std::string_view par_name,
                                   std::string_view trig_par_name)
        : CalibrationTask("NeulandMapToCalParTask", 1)
        , map_data_{ mapped_data_name }
        , trig_map_data_{ trig_mapped_data_name }
        , map_to_cal_par_{ par_name }
        , map_to_cal_trig_par_{ trig_par_name }
    {
        cal_strategy_.Set_cycle_period(1000. / coarse_time_frequency_);
    }

    void Map2CalParTask::ExtraInit(FairRootManager* /*rootMan*/)
    {
        map_data_.init();
        map_to_cal_par_.init(this);
        map_to_cal_trig_par_.init(this);
        if (is_trig_enabled_)
        {
            trig_map_data_.init();
            // if (is_trigID_auto_)
            // {
            //     R3BLOG(info, "TrigIDMap is automatically determined!");
            // }
            // else
            // {
            //     R3BLOG(info, "TrigIDMap is read from the base par file!");
            //     // trigIDMappingFinder_.set_trigIDMap(base_par_->GetTrigIDMap());
            // }
        }

        if (auto* base_par = GetBasePar(); base_par != nullptr)
        {
            plane_num_ = base_par->get_num_of_planes();
        }
        map_to_cal_par_->SetTrigEnabled(is_trig_enabled_);
        // trigIDIO.SetNumOfModule(plane_num_ * BarsPerPlane);
    }

    void Map2CalParTask::TriggeredExec()
    {
        FillMapData();
        if (is_trig_enabled_)
        {
            FillTrigMapData();
            // if (is_trigID_auto_)
            // {
            //     RecordTrigMappingID();
            // }
        }
    }

    void Map2CalParTask::EndOfTask()
    {
        WriteHists();
        write_parameter();
    }

    // void Map2CalParTask::RecordTrigMappingID()
    // {
    //     for (const auto& planeSignals : mappedData_)
    //     {
    //         auto planeNum = planeSignals.plane_num;
    //         for (const auto& [barID, barSignals] : planeSignals.bars)
    //         {
    //             auto moduleID = Neuland_PlaneBar2ModuleNum(planeNum, barID);
    //             for (const auto& [trigID, trigSignal] : trigMappedData_)
    //             {
    //                 trigIDMappingFinder_.add_id_pair(
    //                     std::make_pair(moduleID, trigID), Side::left, barSignals.left.size());
    //                 trigIDMappingFinder_.add_id_pair(
    //                     std::make_pair(moduleID, trigID), Side::right, barSignals.right.size());
    //             }
    //         }
    //     }
    // }

    void Map2CalParTask::FillMapData()
    {
        for (const auto& [plane_num, planeSignals] : map_data_)
        {
            for (const auto& [barNum, barSignals] : planeSignals.bars)
            {
                auto barNum_tmp = barNum;
                R3BLOG(debug,
                       fmt::format("Calibrating with the map-level bar signal: {}, barNum: {}", barSignals, barNum));
                // C++17 can't capture structure bindings
                const auto plane_number = plane_num;
                auto FillData = [&](FTType type, auto value)
                { FillEngine(map_cal_engine_, type, value, plane_number, barNum_tmp); };
                for (const auto& signal : barSignals.left)
                {
                    FillData(FTType::leftleading, signal.leading.fine);
                    FillData(FTType::lefttrailing, signal.trailing.fine);
                }
                for (const auto& signal : barSignals.right)
                {
                    FillData(FTType::rightleading, signal.leading.fine);
                    FillData(FTType::righttrailing, signal.trailing.fine);
                }
            }
        }
    }

    void Map2CalParTask::FillTrigMapData()
    {
        for (const auto& [moduleNum, moduleSignals] : trig_map_data_)
        {
            R3BLOG(debug,
                   fmt::format("Calibrating with the map-level bar trig signal: {}, module num: {}",
                               moduleSignals.signal,
                               moduleNum));
            FillEngine(trig_map_cal_engine_, FTType::trigger, moduleSignals.signal.fine, moduleNum);
        }
    }

    void Map2CalParTask::write_parameter()
    {
        R3BLOG(debug, "Starting to write calibration parameter...");
        map_cal_engine_.Writer_to_TCalPar(cal_strategy_, *map_to_cal_par_);
        // if (is_trigID_auto_)
        // {
        //     GetBasePar()->set_trig_id_map(trigIDMappingFinder_.extract_trigIDMap());
        // }
        map_to_cal_par_->SetSlowClockFrequency(coarse_time_frequency_);

        trig_map_cal_engine_.Writer_to_TCalPar(cal_strategy_, *map_to_cal_trig_par_);
        R3BLOG(debug, "Calibration parameter written complete.");
    }

    void Map2CalParTask::WriteHists()
    {
        auto old_dir = gDirectory;
        auto* runDb = FairRuntimeDb::instance();
        if (auto* parRootFileIo = dynamic_cast<ParRootFileIo*>(runDb->getOutput()); parRootFileIo != nullptr)
        {
            auto* parRootFile = parRootFileIo->get_first_root_file();
            if (!parRootFile->IsOpen() || !parRootFile->IsWritable())
            {
                throw R3B::runtime_error("R3B::ParRootFile is either closed or unable to be written!");
            }
            parRootFile->cd();
            auto* new_dir = gDirectory->mkdir("CalParHistograms", "", true);
            if (new_dir == nullptr)
            {
                throw R3B::runtime_error("Cannot create a directory for the histrogams!");
            }
            map_cal_engine_.WriteHist2File(new_dir);
            if (is_trig_enabled_)
            {
                trig_map_cal_engine_.WriteHist2File(new_dir);
            }
            old_dir->cd();
        }
        else
        {
            throw R3B::runtime_error("unable to obtain the R3B::ParRootFileIo object!");
        }
    }

    // void Map2CalParTask::PrintTrigID() const
    // {
    //     if (calibrationPar_ == nullptr)
    //     {
    //         throw R3B::runtime_error("Cannot print trigIDMapping from a nullptr parameter!");
    //     }

    //     const auto& trigID = base_par_->GetTrigIDMap();
    //     if (is_trigID_auto_)
    //     {
    //         trigIDIO.Save_json(trigID);
    //     }
    //     trigIDIO.Print(trigID);
    // }
} // namespace R3B::Neuland
