#pragma once

#include <FairDetParIo.h>
#include <Rtypes.h>
#include <string_view>
namespace R3B
{
    class ParRootFileIo;
    class DetParRootFileIo : public FairDetParIo
    {
      public:
        constexpr static auto DEFAULT_NAME = std::string_view{ "FairGenericParIo" };

        explicit DetParRootFileIo(ParRootFileIo* file_io)
            : file_io_{ file_io }
        {
            SetName(DEFAULT_NAME.data());
        }

      private:
        ParRootFileIo* file_io_ = nullptr;

        // private virtual functions:
        auto init(FairParSet* par) -> bool override;
        auto write(FairParSet* par) -> int override;

      public:
        ClassDefOverride(DetParRootFileIo, 1);
    };
} // namespace R3B
