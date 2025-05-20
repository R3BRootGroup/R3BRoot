#include "R3BNeulandGeoCreator.h"
#include <FairGeoBuilder.h>
#include <FairGeoInterface.h>
#include <FairGeoLoader.h>
#include <FairGeoMedia.h>
#include <R3BException.h>
#include <R3BLogger.h>
#include <R3BNeulandCommon.h>
#include <TGeoBBox.h>
#include <TGeoCompositeShape.h>
#include <TGeoCone.h>
#include <TGeoManager.h>
#include <TGeoMatrix.h>
#include <TGeoShape.h>
#include <TGeoVolume.h>
#include <cmath>
#include <cstdlib>
#include <fmt/core.h>
#include <string>
#include <string_view>

namespace R3B::Neuland::Geometry
{
    constexpr auto CONE_LENGHT_PADDING = 0.001; // cm
    constexpr auto BAR_BASE_LENGTH = 125.0;     // cm
    constexpr auto BAR_CONE_LENGTH = 5.;        // cm

    const auto BC408_THICKNESS = 2.4;   // cm
    const auto BC408_CONE_RADIUS = 1.2; // cm
    const auto AL_THICKNESS = 0.02;     // cm
    const auto TAPE_THICKNESS = 0.05;   // cm

    constexpr auto DEGREE_90 = 90.;
    namespace
    {
        template <typename T, typename... Args>
        auto TGeoManagerCreate(Args&&... args) -> T*
        {
            return std::make_unique<T>(std::forward<Args>(args)...).release();
        }

        auto create_cone_displacement(const std::string& name, double rotate_y_degree) -> TGeoCombiTrans*
        {
            auto rotation = TGeoRotation{};
            rotation.RotateY(rotate_y_degree);
            auto* transition = TGeoManagerCreate<TGeoCombiTrans>(
                TGeoTranslation{ -(BAR_BASE_LENGTH + BAR_CONE_LENGTH), 0., 0. }, rotation);
            transition->SetName(name.c_str());
            transition->RegisterYourself();
            return transition;
        }

        auto build_bar_shape(std::string_view name, const BarDimension& bar_dimension) -> TGeoShape*
        {
            auto* displace1 = create_cone_displacement("trc1", DEGREE_90);
            auto* displace2 = create_cone_displacement("trc2", -DEGREE_90);

            auto* main_bar_shape = TGeoManagerCreate<TGeoBBox>(
                fmt::format("{}Box", name).c_str(), bar_dimension.length, bar_dimension.width, bar_dimension.width);
            auto* cone_shape = TGeoManagerCreate<TGeoCone>(fmt::format("{}Cone", name).c_str(),
                                                           bar_dimension.cone_length + CONE_LENGHT_PADDING,
                                                           0.,
                                                           bar_dimension.cone_radius,
                                                           0.,
                                                           bar_dimension.width * std::sqrt(2.));
            auto* bar_tail_shape = TGeoManagerCreate<TGeoBBox>(fmt::format("{}ConeBox", name).c_str(),
                                                               bar_dimension.width,
                                                               bar_dimension.width,
                                                               bar_dimension.cone_length);
            return TGeoManagerCreate<TGeoCompositeShape>(name.data(),
                                                         fmt::format("{}+(({}*{}):{})+(({}*{}):{})",
                                                                     main_bar_shape->GetName(),
                                                                     bar_tail_shape->GetName(),
                                                                     cone_shape->GetName(),
                                                                     displace1->GetName(),
                                                                     bar_tail_shape->GetName(),
                                                                     cone_shape->GetName(),
                                                                     displace2->GetName())
                                                             .c_str());
        }

    } // namespace

    auto Creator::construct_volume(int num_of_planes, FairGeoLoader* geo_loader) -> TGeoVolume*
    {
        read_material_from_file(geo_loader);
        construct_all_material();
        construct_all_shapes();
        construct_all_volumes();
        return build_detector(num_of_planes);
    }

    auto Creator::build_detector(int num_of_planes) -> TGeoVolume*
    {
        auto* neuland = TGeoManagerCreate<TGeoVolumeAssembly>("volNeuland");

        auto total_module_size = num_of_planes * BarsPerPlane;
        const auto first_plane_front_z = -num_of_planes * BarSize_Z / 2.;

        auto rot_zero = TGeoRotation{};
        auto rot_90 = TGeoRotation{};
        rot_90.RotateZ(DEGREE_90);

        for (int module_id{}; module_id < total_module_size; ++module_id)
        {
            const auto plane_id = ModuleID2PlaneID(module_id);
            const auto is_horizontal = IsPlaneIDHorizontal(plane_id);
            const auto z_pos = PlaneID2ZPos(plane_id) + first_plane_front_z;
            const auto vertical_displacement = GetBarVerticalDisplacement(module_id + 1);
            auto* translation_rotation = TGeoManagerCreate<TGeoCombiTrans>();
            if (is_horizontal)
            {
                translation_rotation->SetTranslation(0, vertical_displacement, z_pos);
                translation_rotation->SetRotation(rot_zero);
            }
            else
            {
                translation_rotation->SetTranslation(vertical_displacement, 0, z_pos);
                translation_rotation->SetRotation(rot_90);
            }
            neuland->AddNode(bar_, module_id + 1, translation_rotation);
        }

        return neuland;
    }

    void Creator::construct_all_volumes()
    {
        scintillator_ = build_scintillator();
        Al_wrapping_ = build_Al_wrapping();
        tape_wrapping_ = build_tape_wrapping();
        bar_ = build_bar_volume();
    }

    void Creator::construct_all_material()
    {
        material_BC408_ = build_material("BC408");
        material_poly_ = build_material("polyethylene");
        material_Al_ = build_material("aluminium");
    }

    void Creator::construct_all_shapes()
    {
        auto dimension = BarDimension{};

        dimension.length = BAR_BASE_LENGTH;
        dimension.width = BC408_THICKNESS;
        dimension.cone_radius = BC408_CONE_RADIUS;
        dimension.cone_length = BAR_CONE_LENGTH;
        shape_scintillator_ = build_bar_shape("shapeBC408", dimension);

        dimension.width += AL_THICKNESS;
        dimension.cone_radius += AL_THICKNESS;
        auto* shape_Al_solid = build_bar_shape("shapeAlWrappingSolid", dimension);
        shape_Al_wrapping_ = TGeoManagerCreate<TGeoCompositeShape>(
            "shapeAlWrapping",
            fmt::format("{} - {}", shape_Al_solid->GetName(), shape_scintillator_->GetName()).c_str());

        dimension.width += TAPE_THICKNESS;
        dimension.cone_radius += TAPE_THICKNESS;
        auto* shape_tape_solid = build_bar_shape("shapeTapeWrappingSolid", dimension);
        shape_tape_wrapping_ = TGeoManagerCreate<TGeoCompositeShape>(
            "shapeTapeWrapping",
            fmt::format("{} - {}", shape_tape_solid->GetName(), shape_Al_solid->GetName()).c_str());
    }

    auto Creator::build_bar_volume() -> TGeoVolume*
    {
        auto* bar = TGeoManagerCreate<TGeoVolumeAssembly>("volPaddle");
        bar->AddNode(scintillator_, 1);
        bar->AddNode(Al_wrapping_, 1);
        bar->AddNode(tape_wrapping_, 1);
        return bar;
    }

    auto Creator::build_scintillator() -> TGeoVolume*
    {
        auto* scintillator = TGeoManagerCreate<TGeoVolume>("volBC408", shape_scintillator_, material_BC408_);
        static constexpr auto greyish_blue = 33;
        static constexpr auto transparency = 30;
        scintillator->SetLineColor(greyish_blue);
        scintillator->SetTransparency(transparency);
        return scintillator;
    }

    auto Creator::build_Al_wrapping() -> TGeoVolume*
    {
        auto* Al_wrapping = TGeoManagerCreate<TGeoVolume>("volAlWrapping", shape_Al_wrapping_, material_Al_);
        static constexpr auto greyish_silver = 17;
        Al_wrapping->SetLineColor(greyish_silver); // grey/silver
        return Al_wrapping;
    }

    auto Creator::build_tape_wrapping() -> TGeoVolume*
    {
        auto* tape_wrapping = TGeoManagerCreate<TGeoVolume>("volTapeWrapping", shape_tape_wrapping_, material_poly_);
        static constexpr auto black = 1;
        tape_wrapping->SetLineColor(black); // grey/silver
        return tape_wrapping;
    }

    auto Creator::build_material(const std::string& material_name) -> TGeoMedium*
    {
        auto* fair_medium = geo_media_->getMedium(material_name.c_str());
        if (fair_medium == nullptr)
        {
            R3BLOG(error, fmt::format("FairGeoMedium {} not found!", material_name));
        }

        if (geo_builder_ == nullptr)
        {
            throw R3B::runtime_error("geo_builder is nullptr!");
        }
        geo_builder_->createMedium(fair_medium);
        if (gGeoManager == nullptr)
        {
            throw R3B::runtime_error("gGeoManager is nullptr!");
        }
        auto* material = gGeoManager->GetMedium(material_name.c_str());
        if (material == nullptr)
        {
            R3BLOG(error, fmt::format("TGeoMedium {} not found!", material_name));
        }
        return material;
    }

    void Creator::read_material_from_file(FairGeoLoader* geo_loader)
    {
        auto* geo_interface = geo_loader->getGeoInterface();
        const auto* working_dir = std::getenv("VMCWORKDIR");
        if (working_dir == nullptr)
        {
            throw R3B::logic_error("Environment variable \"VMCWORKDIR\" is not defined!");
        }
        geo_interface->setMediaFile(fmt::format("{}/geometry/media_r3b.geo", working_dir).c_str());
        geo_interface->readMedia();

        geo_builder_ = geo_loader->getGeoBuilder();
        geo_media_ = geo_interface->getMedia();
    }
} // namespace R3B::Neuland::Geometry
