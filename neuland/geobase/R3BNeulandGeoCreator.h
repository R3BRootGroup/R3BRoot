#pragma once

#include <string>
class TGeoVolume;
class FairGeoMedia;
class FairGeoBuilder;
class TGeoMedium;
class TGeoShape;
class FairGeoLoader;
class FairGeoInterface;
class TGeoManager;

namespace R3B::Neuland::Geometry
{
    struct BarDimension
    {
        double length{};
        double width{};
        double cone_radius{};
        double cone_length{};
    };

    class Creator
    {
      public:
        Creator() = default;

        auto construct_volume(int num_of_planes, FairGeoLoader* geo_loader) -> TGeoVolume*;

      private:
        FairGeoBuilder* geo_builder_ = nullptr;
        FairGeoMedia* geo_media_ = nullptr;

        TGeoMedium* material_BC408_ = nullptr;
        TGeoMedium* material_poly_ = nullptr;
        TGeoMedium* material_Al_ = nullptr;

        TGeoShape* shape_scintillator_ = nullptr;
        TGeoShape* shape_Al_wrapping_ = nullptr;
        TGeoShape* shape_tape_wrapping_ = nullptr;

        TGeoVolume* scintillator_ = nullptr;
        TGeoVolume* Al_wrapping_ = nullptr;
        TGeoVolume* tape_wrapping_ = nullptr;
        TGeoVolume* bar_ = nullptr;

        void construct_all_material();
        void construct_all_volumes();
        void construct_all_shapes();

        auto build_detector(int num_of_planes) -> TGeoVolume*;
        auto build_material(const std::string& material) -> TGeoMedium*;
        auto build_bar_volume() -> TGeoVolume*;
        auto build_scintillator() -> TGeoVolume*;
        auto build_Al_wrapping() -> TGeoVolume*;
        auto build_tape_wrapping() -> TGeoVolume*;

        void read_material_from_file(FairGeoLoader* geo_loader);
    };

} // namespace R3B::Neuland::Geometry
