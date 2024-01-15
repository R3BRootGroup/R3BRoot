#include "R3BTrigIDMappingFinder.h"
#include "R3BException.h"
#include <FairRootFileSink.h>
#include <FairRun.h>
#include <R3BLogger.h>
#include <algorithm>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <fmt/std.h>
#include <fstream>
#include <iomanip>
#include <optional>
#include <range/v3/view.hpp>
#define JSON_USE_IMPLICIT_CONVERSIONS 0 // NOLINT
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
namespace nlh = nlohmann;

namespace R3B::Neuland
{
    void TrigIDMappingRecorder::add(unsigned int trigID, R3B::Side side, unsigned int counts)
    {
        auto& record = (side == R3B::Side::left) ? left : right;
        auto counter = record.find(trigID);
        if (counter == record.end())
        {
            counter = record.insert({ trigID, 0 }).first;
        }
        (counter->second) += counts;
    }

    auto TrigIDMappingRecorder::findID(R3B::Side side) const -> std::optional<unsigned int>
    {
        const auto& record = (side == R3B::Side::left) ? left : right;
        auto criteria = [](auto pre, auto post) { return pre.second < post.second; };
        auto result = std::max_element(record.begin(), record.end(), criteria);
        return (result != record.end()) ? result->first : std::optional<unsigned int>{};
    }

    void TrigIDMappingFinder::add_id_pair(std::pair<unsigned int, unsigned int> pair,
                                          R3B::Side side,
                                          unsigned int counts)
    {
        auto [moduleID, trigID] = pair;
        auto record = records_.find(moduleID);
        if (record == records_.end())
        {
            record = records_.insert({ moduleID, TrigIDMappingRecorder{} }).first;
        }
        record->second.add(trigID, side, counts);
    }

    void TrigIDMappingFinder::AutoResolve(TrigIDMap& trigIDMap)
    {
        for (auto& [moduleID, trigIDs] : trigIDMap)
        {
            if (trigIDs.first == 0)
            {
                auto pre = trigIDMap.find(moduleID - 1);
                auto post = trigIDMap.find(moduleID + 1);
                auto is_existed = (pre != trigIDMap.end() and post != trigIDMap.end());
                auto is_same_left = (pre->second.first == post->second.first);
                auto is_same_right = (pre->second.second == post->second.second);
                if (is_existed and is_same_left and is_same_right)
                {
                    trigIDs = { pre->second.first, pre->second.second };
                    continue;
                }
                R3BLOG(warn,
                       fmt::format("trigID can't be auto-resolved for moduleID: {}! Increase the event numbers to make "
                                   "trigIDs on the two sides different.",
                                   moduleID));
            }
        }
    }

    auto TrigIDMappingFinder::extract_trigIDMap(bool force_find) -> TrigIDMap
    {
        if (force_find or idMap_.empty())
        {
            FindTrigIDMap();
        }
        return std::move(idMap_);
    }

    // Precondition: the trigIDs on two sides must be different!
    void TrigIDMappingFinder::FindTrigIDMap()
    {
        idMap_.clear();
        for (const auto& [moduleID, record] : records_)
        {
            auto trigID_left = record.findID(Side::left);
            auto trigID_right = record.findID(Side::right);
            if (trigID_left.has_value() and trigID_right.has_value())
            {
                if (trigID_left.value() != trigID_right.value())
                {
                    idMap_.insert({ moduleID, { trigID_left.value(), trigID_right.value() } });
                }
                else
                {
                    idMap_.insert({ moduleID, { 0, 0 } });
                }
            }
            else
            {
                R3BLOG(error, fmt::format("Empty record for moduleID {}", moduleID));
            }
        }
        AutoResolve(idMap_);
    }

    void TrigMappingIO::Print(const TrigIDMappingFinder::TrigIDMap& trigMap) const
    {
        switch (format_)
        {
            case TrigIDMappingPrintFormat::none:
                break;
            case TrigIDMappingPrintFormat::screen:
                Print_screen(trigMap);
                break;
            default:
                throw R3B::logic_error("unrecognized TrigMappingPrinter option");
                break;
        }
    }

    void TrigMappingIO::Print_screen(const TrigIDMappingFinder::TrigIDMap& trigMap) const
    {
        fmt::print("==============TrigIDMapping=================\n");
        fmt::print("{0:^18s}|{1:^18s}|{2:^18s}\n", "barID", "trigID_l", "trigID_r");
        for (unsigned int moduleID{ 1 }; moduleID <= numOfModule_; ++moduleID)
        {
            if (auto trigID = trigMap.find(moduleID); trigID == trigMap.end())
            {
                fmt::print("{0:^18d}|{1:^18s}|{2:^18s}\n", moduleID, "NAN", "NAN");
            }
            else
            {
                fmt::print("{0:^18d}|{1:^18d}|{2:^18d}\n", moduleID, trigID->second.first, trigID->second.second);
            }
        }
        fmt::print("============================================\n");
    }

    void TrigMappingIO::Save_json(const TrigIDMappingFinder::TrigIDMap& trigMap) const
    {
        const auto filepath = fs::path{ fileDir_ } / fs::path{ filename_ };
        fmt::print("Writing trigIDMap to JSON file {} ...\n", filepath);
        auto ostream = std::ofstream(filepath, std::ios_base::out | std::ios_base::trunc);
        auto jsonData = nlh::json::array();
        auto jsonEntry = nlh::json::object();
        for (const auto& [moduleID, id_pair] : trigMap)
        {
            jsonEntry["barID"] = moduleID;
            jsonEntry["trigID_left"] = id_pair.first;
            jsonEntry["trigID_right"] = id_pair.second;
            jsonData.push_back(jsonEntry);
        }
        std::sort(jsonData.begin(),
                  jsonData.end(),
                  [](const auto& left, const auto& right) { return left.at("barID") < right.at("barID"); });
        ostream << std::setw(4) << jsonData << std::endl;
        // ostream << jsonData.dump(4) << std::endl;
    }

    auto TrigMappingIO::Read_json() const -> TrigIDMappingFinder::TrigIDMap
    {
        const auto filepath = fs::path{ fileDir_ } / fs::path{ filename_ };
        fmt::print("Reading trigIDMap from file {} ...\n", filepath);
        auto istream = std::ifstream(filepath);
        if (not istream.is_open())
        {
            throw R3B::runtime_error(fmt::format("Cannot open JSON file {}", filepath));
        }
        auto trigMap = TrigIDMappingFinder::TrigIDMap{};
        auto jsonData = nlh::json::parse(istream);
        for (const auto& jsonEntry : jsonData)
        {
            const auto moduleID = jsonEntry.at("barID").template get<int>();
            const auto trigID_left = jsonEntry.at("trigID_left").template get<int>();
            const auto trigID_right = jsonEntry.at("trigID_right").template get<int>();
            trigMap.insert({ moduleID, std::make_pair(trigID_left, trigID_right) });
        }
        return trigMap;
    }
} // namespace R3B::Neuland
