#pragma once

#include <R3BShared.h>
#include <filesystem>
#include <map>
#include <optional>
#include <unordered_map>

namespace R3B ::Neuland
{

    struct TrigIDMappingRecorder
    {
        using Record = std::map<unsigned int, uint64_t>;
        Record left;
        Record right;
        void add(unsigned int trigID, R3B::Side side, unsigned int counts = 1);
        [[nodiscard]] auto findID(R3B::Side side) const -> std::optional<unsigned int>;
    };

    class TrigIDMappingFinder
    {
      public:
        using TrigIDMap = std::unordered_map<unsigned int, std::pair<unsigned int, unsigned int>>;
        TrigIDMappingFinder() = default;
        // add moduleID and trigID pair
        void add_id_pair(std::pair<unsigned int, unsigned int> pair, R3B::Side side, unsigned int counts = 1);
        void clear() { records_.clear(); }
        void set_trigIDMap(TrigIDMap idMap) { idMap_ = std::move(idMap); }
        auto extract_trigIDMap(bool force_find = false) -> TrigIDMap;

      private:
        std::unordered_map<unsigned int, TrigIDMappingRecorder> records_;
        TrigIDMap idMap_;
        void FindTrigIDMap();
        static void AutoResolve(TrigIDMap& trigIDMap);
    };

    enum class TrigIDMappingPrintFormat
    {
        screen,
        none
    };

    class TrigMappingIO
    {
      public:
        TrigMappingIO() = default;

        void SetFormat(TrigIDMappingPrintFormat format) { format_ = format; }
        void SetFileName(std::string_view filename) { filename_ = filename; }
        void SetDir(std::string_view dirname) { fileDir_ = dirname; }
        void SetNumOfModule(unsigned int num) { numOfModule_ = num; }
        void Print(const TrigIDMappingFinder::TrigIDMap& trigMap) const;
        [[nodiscard]] auto Read_json() const -> TrigIDMappingFinder::TrigIDMap;
        void Save_json(const TrigIDMappingFinder::TrigIDMap& trigMap) const;

      private:
        unsigned int numOfPlane_ = 0;
        unsigned int numOfBar_ = 0;
        unsigned int numOfModule_ = 0;
        TrigIDMappingPrintFormat format_ = TrigIDMappingPrintFormat::none;
        std::string filename_ = "trigIDMap.json";
        std::string fileDir_ = ".";
        void Print_screen(const TrigIDMappingFinder::TrigIDMap& trigMap) const;
    };
} // namespace R3B::Neuland
