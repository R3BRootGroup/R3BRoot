#include "FairRuntimeDb.h"
#include "R3BNeulandNeutron2DPar.h"
#include "RecoTDR.h"
#include "gtest/gtest.h"
#include <algorithm>

namespace
{
    using Neuland::RecoTDR;

    class testRecoTDR : public testing::Test
    {
      protected:
        void SetUp() override
        {
            fClusters.push_back(new R3BNeulandCluster(R3BNeulandDigi{
                874, 74.0923, 75.5003, 65.1535, 2.44689, 1.85643, 2.13131, { -7.5, 9.85647, 1487.5 } }));
            fClusters.push_back(new R3BNeulandCluster({
                R3BNeulandDigi{ 425, 71.403, 69.528, 60.8227, 32.1342, 35.0932, 33.5811, { -13.1249, -2.5, 1442.5 } },
                R3BNeulandDigi{ 426, 70.7576, 69.5846, 60.5282, 43.3598, 46.4116, 44.8598, { -8.21099, 2.5, 1442.5 } },
                R3BNeulandDigi{ 475, 69.8556, 71.4452, 61.0075, 14.7869, 11.5298, 13.0572, { -2.5, 11.1271, 1447.5 } },
                R3BNeulandDigi{ 428, 70.7084, 69.9433, 60.683, 17.4303, 19.8725, 18.6114, { -5.3562, 12.5, 1442.5 } },
            }));
            fClusters.push_back(new R3BNeulandCluster(
                R3BNeulandDigi{ 523, 70.0093, 72.2564, 61.49, 65.1076, 50.0853, 57.1045, { 15.7294, -12.5, 1452.5 } }));
            fClusters.push_back(new R3BNeulandCluster(
                R3BNeulandDigi{ 829, 71.509, 73.4678, 62.8456, 5.41213, 4.57457, 4.97576, { 13.7115, 17.5, 1482.5 } }));
            fClusters.push_back(new R3BNeulandCluster({
                R3BNeulandDigi{ 925, 73.6385, 71.8902, 63.1215, 1.43794, 1.76745, 1.59421, { -12.2376, -2.5, 1492.5 } },
                R3BNeulandDigi{ 974, 72.915, 72.227, 62.9281, 11.6925, 13.4288, 12.5306, { -7.5, -4.81552, 1497.5 } },
                R3BNeulandDigi{
                    1024, 73.4672, 71.8807, 63.0311, 12.1289, 13.7671, 12.9221, { -11.1056, -7.5, 1502.5 } },
                R3BNeulandDigi{
                    1124, 74.1117, 72.4394, 63.6327, 11.5875, 14.7202, 13.0602, { -11.7065, -7.5, 1512.5 } },
                R3BNeulandDigi{
                    1074, 73.7828, 72.3994, 63.4482, 11.9622, 14.7002, 13.2607, { -7.5, -9.68424, 1507.5 } },
                R3BNeulandDigi{
                    1173, 74.4282, 72.7551, 63.9488, 13.5886, 12.9317, 13.2561, { -12.5, -11.7118, 1517.5 } },
            }));
            fClusters.push_back(new R3BNeulandCluster({
                R3BNeulandDigi{ 1083, 71.0819, 79.2814, 65.5388, 23.3921, 8.39904, 14.0168, { 37.5, 57.3966, 1507.5 } },
                R3BNeulandDigi{ 1084, 71.2626, 79.1224, 65.5497, 64.1623, 24.6824, 39.7954, { 42.5, 55.0182, 1507.5 } },
                R3BNeulandDigi{ 1035, 72.1257, 77.3548, 65.0974, 16.9382, 8.4339, 11.9522, { 36.6034, 47.5, 1502.5 } },
                R3BNeulandDigi{ 1138, 72.5849, 78.9514, 66.1253, 71.6596, 39.9796, 53.5249, { 44.5655, 62.5, 1512.5 } },
                R3BNeulandDigi{ 1036, 71.8915, 77.676, 65.1409, 21.0253, 10.3993, 14.7868, { 40.4911, 52.5, 1502.5 } },
            }));
        }

        void TearDown() override
        {
            for (auto cluster : fClusters)
            {
                delete cluster;
            }
            fClusters.clear();
        }

        std::vector<R3BNeulandCluster*> fClusters;
    };

    // FIXME: The whole thing is too tightly coupled with FairRoot Parameter Cointainers.
    // Nothing to do about that right now.
    class testRecoTDRElasticAnd2DPar : public testing::Test
    {
      protected:
        void SetUp() override
        {
            fClusters.push_back(new R3BNeulandCluster(
                // Some Random Cluster that is not related to the others
                R3BNeulandDigi{
                    874, 74.0923, 75.5003, 65.1535, 2.44689, 1.85643, 2.13131, { -7.5, 9.85647, 1487.5 } }));
            fClusters.push_back(new R3BNeulandCluster(
                R3BNeulandDigi{ 627, 70.3447, 73.3119, 62.1854, 24.4598, 18.9215, 21.5131, { 20.7703, 7.5, 1462.5 } }));
            fClusters.push_back(new R3BNeulandCluster({
                // Triggers IsElastic with the Cluster above
                R3BNeulandDigi{ 1032, 72.2410, 76.5411, 64.7482, 45.0953, 23.9844, 32.8874, { 30.1012, 32.5, 1502.5 } },
                R3BNeulandDigi{ 1079, 72.9925, 76.5725, 65.1396, 3.79452, 2.56439, 3.11939, { 17.5, 25.0597, 1507.5 } },
                R3BNeulandDigi{ 1081, 72.0231, 75.9261, 64.3317, 120.891, 83.8838, 100.702, { 27.5, 27.3209, 1507.5 } },
                R3BNeulandDigi{ 1130, 76.3580, 77.9297, 67.5001, 1.53590, 1.30780, 1.41727, { 11.0018, 22.5, 1512.5 } },
            }));

            auto rtdb = FairRuntimeDb::instance();
            fPar = (R3BNeulandNeutron2DPar*)rtdb->getContainer("R3BNeulandNeutron2DPar");
            rtdb->addRun(1);

            fCuts[1] = new TCutG(TString::UItoa(1, 10), 4);
            auto cut = fCuts.at(1);
            cut->SetVarX("Total Energy [MeV]");
            cut->SetVarY("Number of Clusters");
            // Dummy values
            cut->SetPoint(0, -1, 50.);
            cut->SetPoint(1, 1, -1);
            cut->SetPoint(2, 90000, -1);
            cut->SetPoint(3, -1, 500000.);

            fPar->SetNeutronCuts(fCuts);
            fPar->setChanged();
        }

        void TearDown() override
        {
            for (auto cluster : fClusters)
            {
                delete cluster;
            }
            fClusters.clear();

            for (auto cutpair : fCuts)
            {
                delete cutpair.second;
            }
            fCuts.clear();
            FairRuntimeDb::instance()->removeAllContainers();
            delete FairRuntimeDb::instance();
        }

        std::vector<R3BNeulandCluster*> fClusters;
        std::map<UInt_t, TCutG*> fCuts;
        R3BNeulandNeutron2DPar* fPar;
    };

    TEST_F(testRecoTDR, takesAVectorOfClustersAndReturnsAVectorOfNeutrons)
    {
        RecoTDR rs{};
        std::vector<R3BNeulandNeutron> neutrons = rs.GetNeutrons(fClusters);
        EXPECT_EQ(0u, neutrons.size());
    }

    TEST_F(testRecoTDR, sortsClustersByRValueAsc)
    {
        RecoTDR rs{};
        const Double_t EnergyBeam = 600; // MeV
        const Double_t BetaBeam = 0.7922;
        rs.SetBeam(EnergyBeam, BetaBeam);

        rs.SortClustersByRValue(fClusters);

        const auto sorting = [=](const R3BNeulandCluster* a, const R3BNeulandCluster* b) {
            return a->GetRCluster(BetaBeam) < b->GetRCluster(BetaBeam);
        };

        EXPECT_TRUE(std::is_sorted(fClusters.cbegin(), fClusters.cend(), sorting));
    }

    TEST_F(testRecoTDR, filtersClustersWithTooLittleEnergy)
    {
        RecoTDR rs{};
        rs.FilterClustersByEnergyDeposit(fClusters);

        EXPECT_TRUE(std::all_of(
            fClusters.cbegin(), fClusters.cend(), [](const R3BNeulandCluster* a) { return a->GetE() > 2.5; }));
    }

    TEST_F(testRecoTDR, filtersClustersByEToF)
    {
        RecoTDR rs{};
        const Double_t EnergyBeam = 600; // MeV
        const Double_t BetaBeam = 0.7922;
        rs.SetBeam(EnergyBeam, BetaBeam);

        rs.FilterClustersByBeta(fClusters);

        EXPECT_TRUE(std::all_of(fClusters.cbegin(), fClusters.cend(), [=](const R3BNeulandCluster* a) {
            // Magic numbers from R3BNeutronTracker2D Advanced Method
            return std::abs(a->GetBeta() - BetaBeam) < (0.05 * 600. / EnergyBeam);
        }));
    }

    TEST_F(testRecoTDR, putsFirstClusterInTimeToTheFront)
    {
        RecoTDR rs{};
        rs.PrioritizeTimeWiseFirstCluster(fClusters);

        EXPECT_EQ(60.5282, fClusters.at(0)->GetT());
    }

    TEST_F(testRecoTDRElasticAnd2DPar, filtersClustersByElasticScattering)
    {
        RecoTDR rs{};
        rs.Init(); // not required for ElasticScattering, but I've skimped on the number of Test Fixtures
        rs.FilterClustersByElasticScattering(fClusters);
        EXPECT_EQ(2u, fClusters.size());
        EXPECT_EQ(1u, fClusters.at(0)->GetSize());
        EXPECT_EQ(1u, fClusters.at(1)->GetSize());
    }

    TEST_F(testRecoTDRElasticAnd2DPar, loadsR3BNeulandNeutron2DParOnInit)
    {
        RecoTDR rs{};
        rs.Init();
        EXPECT_EQ(fPar, rs.Get2DPar());
    }

    TEST_F(testRecoTDRElasticAnd2DPar, getNumberOfNeutronsFrom2DParCuts)
    {
        RecoTDR rs{};
        rs.Init();
        auto nNeutrons = rs.FindNumberOfNeutrons(fClusters);
        EXPECT_EQ(1u, nNeutrons);
    }

    TEST_F(testRecoTDRElasticAnd2DPar, reconstructsTheCorrectNeutron)
    {
        RecoTDR rs{};
        rs.Init();
        auto neutrons = rs.GetNeutrons(fClusters);
        EXPECT_EQ(62.1854, neutrons.at(0).GetT());
    }
}
