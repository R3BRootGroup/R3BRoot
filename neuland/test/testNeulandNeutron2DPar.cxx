#include <map>
#include "TCutG.h"
#include "R3BNeulandNeutron2DPar.h"
#include "gtest/gtest.h"

namespace
{
    TEST(testNeutron2DPar, HandlesConversionBetweenMaps)
    {
        std::map<UInt_t, TCutG*> m;
        m[1] = new TCutG("cut1", 4);
        m[2] = new TCutG("cut2", 4);

        R3BNeulandNeutron2DPar par;
        par.SetNeutronCuts(m);

        EXPECT_STREQ(par.GetNeutronCuts().at(2)->GetName(), "cut2");
    }

    TEST(testNeutron2DPar, GetNeutronMultiplicity)
    {
        std::map<UInt_t, TCutG*> m;
        m[0] = new TCutG("cut0", 4);
        m[0]->SetPoint(0, 0,  0);
        m[0]->SetPoint(1, 0, 10);
        m[0]->SetPoint(2, 10, 0);
        m[0]->SetPoint(3, 0,  0);
        m[1] = new TCutG("cut1", 4);
        m[1]->SetPoint(0, 0, 10);
        m[1]->SetPoint(1, 0, 20);
        m[1]->SetPoint(2, 20, 0);
        m[1]->SetPoint(3, 10, 0);
        m[2] = new TCutG("cut2", 4);
        m[2]->SetPoint(0, 0, 20);
        m[2]->SetPoint(1, 0, 30);
        m[2]->SetPoint(2, 30, 0);
        m[2]->SetPoint(3, 20, 0);

        R3BNeulandNeutron2DPar par;
        par.SetNeutronCuts(m);

        EXPECT_EQ(par.GetNeutronMultiplicity(4,4), 0);
        EXPECT_EQ(par.GetNeutronMultiplicity(9,9), 1);
        EXPECT_EQ(par.GetNeutronMultiplicity(14,14), 2);
        EXPECT_EQ(par.GetNeutronMultiplicity(19,19), 3);
    }

} // namespace

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
