#include "R3BNeulandMapToCalParTask.h"
#include "R3BEventHeader.h"
#include "R3BException.h"
#include <FairParIo.h>
#include <FairParRootFileIo.h>
#include <FairRootManager.h>
#include <FairRun.h>
#include <FairRuntimeDb.h>
#include <R3BLogger.h>
#include <iostream>

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
    Map2CalParTask::Map2CalParTask(std::string_view name, int iVerbose)
        : CalibrationTask(name.data(), iVerbose)
    {
        cal_strategy_.Set_cycle_period(1000. / coarse_time_frequency_);
    }

    Map2CalParTask::Map2CalParTask()
        : Map2CalParTask("NeulandMapToCalParTask", 1)
    {
    }

    void Map2CalParTask::ExtraInit(FairRootManager* /*rootMan*/)
    {
        mappedData_.init();
        if (is_trig_enabled_)
        {
            trigMappedData_.init();
            if (is_trigID_auto_)
            {
                R3BLOG(info, "TrigIDMap is automatically determined!");
            }
            else
            {
                R3BLOG(info, "TrigIDMap is read from the base par file!");
                // trigIDMappingFinder_.set_trigIDMap(base_par_->GetTrigIDMap());
            }
        }

        plane_num_ = GetBasePar()->GetNumOfPlanes();
        calibrationPar_->SetTrigEnabled(is_trig_enabled_);
        // trigIDIO.SetNumOfModule(plane_num_ * BarsPerPlane);
    }

    void Map2CalParTask::TriggeredExec()
    {
        FillMapData();
        if (is_trig_enabled_)
        {
            FillTrigMapData();
            if (is_trigID_auto_)
            {
                RecordTrigMappingID();
            }
        }
    }

    void Map2CalParTask::EndOfTask()
    {
        WriteHists();
        write_parameter();

        // if (is_trig_enabled_)
        // {
        //     PrintTrigID();
        // }
    }

    void Map2CalParTask::RecordTrigMappingID()
    {
        for (const auto& planeSignals : mappedData_)
        {
            auto planeNum = planeSignals.plane_num;
            for (const auto& [barID, barSignals] : planeSignals.bars)
            {
                auto moduleID = Neuland_PlaneBar2ModuleNum(planeNum, barID);
                for (const auto& [trigID, trigSignal] : trigMappedData_)
                {
                    trigIDMappingFinder_.add_id_pair(
                        std::make_pair(moduleID, trigID), Side::left, barSignals.left.size());
                    trigIDMappingFinder_.add_id_pair(
                        std::make_pair(moduleID, trigID), Side::right, barSignals.right.size());
                }
            }
        }
    }

    void Map2CalParTask::FillMapData()
    {
        for (const auto& planeSignals : mappedData_)
        {
            auto planeID = planeSignals.plane_num;
            for (const auto& [barID, barSignals] : planeSignals.bars)
            {
                auto barID_tmp = barID;
                auto FillData = [&](FTType type, auto value)
                { FillEngine(mapCalEngine_, type, value, planeID, barID_tmp); };
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
        for (const auto& [moduleID, moduleSignals] : trigMappedData_)
        {
            FillEngine(trigMapCalEngine_, FTType::trigger, moduleSignals.signal.fine, moduleID);
        }
    }

    void Map2CalParTask::write_parameter()
    {
        R3BLOG(debug, "Starting to write calibration parameter...");
        mapCalEngine_.Writer_to_TCalPar(cal_strategy_, *calibrationPar_);
        if (is_trigID_auto_)
        {
            GetBasePar()->SetTrigIDMap(trigIDMappingFinder_.extract_trigIDMap());
        }
        calibrationPar_->SetSlowClockFrequency(coarse_time_frequency_);

        trigMapCalEngine_.Writer_to_TCalPar(cal_strategy_, *calibrationTrigPar_);
        R3BLOG(debug, "Calibration parameter written complete.");
    }

    void Map2CalParTask::WriteHists()
    {
        auto old_dir = gDirectory;
        auto* runDb = FairRuntimeDb::instance();
        if (auto* parRootFileIo = dynamic_cast<FairParRootFileIo*>(runDb->getOutput()); parRootFileIo != nullptr)
        {
            auto* parRootFile = parRootFileIo->getParRootFile();
            if (!parRootFile->IsOpen() || !parRootFile->IsWritable())
            {
                throw R3B::runtime_error("parRootFile is either closed or unable to be written!");
            }
            parRootFile->cd();
            auto* new_dir = gDirectory->mkdir("CalParHistograms", "", true);
            if (new_dir == nullptr)
            {
                throw R3B::runtime_error("Cannot create a directory for the histrogams!");
            }
            mapCalEngine_.WriteHist2File(new_dir);
            if (is_trig_enabled_)
            {
                trigMapCalEngine_.WriteHist2File(new_dir);
            }
            old_dir->cd();
        }
        else
        {
            throw R3B::runtime_error("unable to obtain the FairParRootFileIo object!");
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
