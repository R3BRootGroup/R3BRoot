#pragma once

#include "FairBoxGenerator.h"
#include "FairPrimaryGenerator.h"
#include "R3BNeulandCommon.h"
#include "R3BValueError.h"
#include <Math/Vector3D.h> // NOLINT
#include <Math/Vector3Dfwd.h>
#include <R3BMinMaxValue.h>
#include <R3BPDGParticle.h>
#include <TRandom3.h>
#include <cstdint>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace R3B::Neuland
{
    enum class GeneratorType : uint8_t
    {
        box,
        muon
    };

    class GeneratorFactory
    {
      public:
        struct Options
        {
            int multiplicity = 1;                                   //!< Multiplicity of particles
            GeneratorType generator_type = GeneratorType::box;      //!< Type of generator
            Particle::Type particle_type = Particle::Type::neutron; //!< Particle type
            ValueErrorD energy = DEFAULT_GENERATOR_ENERGY;          //!< energy value with error [MeV]
            MinMaxValueD theta = DEFAULT_BOX_GENERATOR_THETA;       //!< Theta min and max value for outgoing direction
            MinMaxValueD phi = DEFAULT_BOX_GENERATOR_PHI;           //!< Phi min and max value for outgoing direction
            ROOT::Math::XYZVectorD position{ 0., 0., 0. };          //!< Location of particle generation
        };

        /**
         * @brief Default constructor
         */
        explicit GeneratorFactory(Options& options)
            : options_{ options }
        {
        }

        /**
         * @brief Create a primary generator
         *
         * Create a primary generator from the setup options.
         */
        auto Create() -> std::unique_ptr<FairPrimaryGenerator>;

        // setters:
        void SetRandomGen(TRandom3* random_gen) { random_gen_ = random_gen; }
        void SetNumOfPlanes(int num_of_planes) { num_of_planes_ = num_of_planes; }

      private:
        int num_of_planes_ = MaxNumberOfPlanes; //!< Number of planes
        std::reference_wrapper<Options> options_;
        TRandom3* random_gen_ = nullptr;
        auto create_muon_generator() -> std::unique_ptr<FairPrimaryGenerator>;
        auto create_box_generator() -> std::unique_ptr<FairPrimaryGenerator>;
    };

    void to_json(nlohmann::ordered_json& json_obj, const GeneratorFactory::Options& options);

    void from_json(const nlohmann::ordered_json& json_obj, GeneratorFactory::Options& options);
} // namespace R3B::Neuland
