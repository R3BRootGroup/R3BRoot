#include "FairPrimaryGenerator.h"
#include "R3BPhaseSpaceGenerator.h"
#include "TVector3.h"
#include "gtest/gtest.h"
#include <stdexcept>
#include <vector>

namespace
{
    class MockFairPrimaryGenerator : public FairPrimaryGenerator
    {
      public:
        Int_t nTracks;
        std::vector<Int_t> PDGs;
        std::vector<TVector3> ps;

        void AddTrack(Int_t pdgid,
                      Double_t px,
                      Double_t py,
                      Double_t pz,
                      Double_t vx,
                      Double_t vy,
                      Double_t vz,
                      Int_t parent = -1,
                      Bool_t wanttracking = true,
                      Double_t e = -9e9,
                      Double_t tof = 0.,
                      Double_t weight = 0.) override
        {
            nTracks++;
            PDGs.push_back(pdgid);
            ps.emplace_back(TVector3(px, py, pz));
        }
    };

    TEST(testR3BPhaseSpaceGenerator, can_add_tracks)
    {
        auto fpg = new MockFairPrimaryGenerator();

        R3BPhaseSpaceGenerator gen;
        gen.AddHeavyIon({ "U-238", 92, 238, 92 });
        gen.AddParticle(2112);
        gen.Init();
        gen.ReadEvent(fpg);

        EXPECT_EQ(fpg->nTracks, 2);
        EXPECT_EQ(fpg->PDGs.at(0), 1000922380);
        EXPECT_EQ(fpg->PDGs.at(1), 2112);
    }

    TEST(testR3BPhaseSpaceGenerator, does_some_physics)
    {
        auto fpg = new MockFairPrimaryGenerator();
        R3BPhaseSpaceGenerator gen;

        gen.SetBeamEnergyAMeV(600);
        gen.SetErelkeV(500);
        gen.AddParticle(2112);
        gen.AddParticle(2112);

        gen.Init();
        gen.ReadEvent(fpg);

        const Double_t mass = 0.93956536;
        EXPECT_NEAR(std::sqrt(fpg->ps.at(0).Mag2() + std::pow(mass, 2)) - std::pow(mass, 2), 0.6, 0.1);
    }

} // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
