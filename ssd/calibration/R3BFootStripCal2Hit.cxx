/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2025 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// ------------------------------------------------------------------------
// -----            R3BFootStripCal2Hit source file                   -----
// -----       Created 05/11/21 by J.L. Rodriguez-Sanchez             -----
// -----       Modified 05/2025 by Pablo Gonzalez Rusell              -----
// ------------------------------------------------------------------------

// ROOT headers
#include <TClonesArray.h>
#include <TF1.h>
#include <TMath.h>
#include <TRandom3.h>
#include <TRotation.h>
#include <TSpectrum.h>
#include <TVector3.h>
#include <iomanip>
#include <memory>
#include <vector>

// FAIR headers
#include <FairLogger.h>
#include <FairRootManager.h>
#include <FairRunAna.h>
#include <FairRuntimeDb.h>

// FOOT headers
#include "R3BFootCalData.h"
#include "R3BFootHitData.h"
#include "R3BFootHitPar.h"
#include "R3BFootMappingPar.h"
#include "R3BFootStripCal2Hit.h"
#include "R3BLogger.h"

// R3BFootStripCal2HitPar::Standard Constructor ---------------------------------
R3BFootStripCal2Hit::R3BFootStripCal2Hit(const TString& name, int iVerbose)
    : FairTask(name, iVerbose)
{
}

// Virtual R3BFootStripCal2Hit::Destructor --------------------------------------
R3BFootStripCal2Hit::~R3BFootStripCal2Hit()
{
    R3BLOG(debug1, "");

    if (fFootHitData)
        delete fFootHitData;
}

void R3BFootStripCal2Hit::SetParContainers()
{
    // Parameter Container
    // Reading footMappingPar from FairRuntimeDb
    FairRuntimeDb* rtdb = FairRuntimeDb::instance();
    R3BLOG_IF(fatal, !rtdb, "FairRuntimeDb not found");

    fMap_Par = dynamic_cast<R3BFootMappingPar*>(rtdb->getContainer("footMappingPar"));
    if (!fMap_Par)
    {
        R3BLOG(error, "Couldn't get handle on footMappingPar container");
    }
    else
    {
        R3BLOG(info, "footMappingPar found");
    }

    R3BLOG(info, "Calling SetParContainers()");

    fHit_Par = dynamic_cast<R3BFootHitPar*>(rtdb->getContainer("footHitPar"));
    if (!fHit_Par)
    {
        R3BLOG(error, "footHitPar not found");
    }
    else
    {
        R3BLOG(info, "footHitPar loaded correctly");
    }
}

void R3BFootStripCal2Hit::SetParameter()
{
    if (!fMap_Par)
    {
        R3BLOG(warn, "Container footMappingPar not found");
        return;
    }
    //--- Parameter Container ---
    fMaxNumDet = fMap_Par->GetNumDets(); // Number of foot detectors

    R3BLOG(info, "NumDet from mapping " << fMaxNumDet);
    for (auto i = 0; i < fMaxNumDet; i++)
    {
        fDistTarget.push_back(fMap_Par->GetDist2target(i + 1));
        fAngleTheta.push_back(fMap_Par->GetAngleTheta(i + 1));
        fAnglePhi.push_back(fMap_Par->GetAnglePhi(i + 1));
        fAnglePsi.push_back(fMap_Par->GetAnglePsi(i + 1));
        fOffsetX.push_back(fMap_Par->GetOffsetX(i + 1));
        fOffsetY.push_back(fMap_Par->GetOffsetY(i + 1));
    }
    fMap_Par->printParams();

    // Hit parameters
    if (!fHit_Par || fHit_Par->GetNumParsFit() == -1)
    {
        R3BLOG(error, "fHit_Par is NULL");
        fHit_Par = nullptr;
        return;
    }
    else
    {
        R3BLOG(info, TString::Format("fHit_Par is VALID, NumParsFit = %d", fHit_Par->GetNumParsFit()));
    }

    // Number of parameters for the eta-fit and the charge-energy fit
    fNumParsCal = fHit_Par->GetNumParsFit();
    fNumParsEtaCorr = fHit_Par->GetNumParsEtaCorr();

    // Get the multiplicity vector. It contains the amount of charge
    // states seen by each asic
    for (int i = 0; i < fMaxNumDet * fNumAsic; i++)
    {
        fMultCharPar.push_back(fHit_Par->GetMultCharParams()->At(i));
    }

    // One vector per asic
    fCharCalPar.resize(fMaxNumDet * fNumAsic);
    fCharCalParSM.resize(fMaxNumDet * fNumAsic);
    fEtaCorrPar.resize(fMaxNumDet * fNumAsic);

    // Now we iterate over the multiplicity of each asic. If the asic is from
    // a dead foot (i.e, mult == 0, we continue). Otherwise, we start filling
    // with the pol coefficients of each charge state (eta band) in each asic.
    int nParsEta = 0;
    int nParsCal = 0;

    for (size_t i = 0; i < fMultCharPar.size(); i++)
    {
        for (int j = nParsEta; j < nParsEta + fMultCharPar[i]; j++)
            fEtaCorrPar[i].push_back(fHit_Par->GetEtaCorrParams()->At(j));

        nParsEta += fMultCharPar[i];

        if (fMultCharPar[i] == 0)
            continue;

        for (int j = nParsCal; j < nParsCal + fNumParsCal; j++)
        {
            fCharCalPar[i].push_back(fHit_Par->GetCharCalParams()->At(j));
            fCharCalParSM[i].push_back(fHit_Par->GetCharCalParamsSM()->At(j));
        }

        nParsCal += fNumParsCal;
    }
}

// -----   Public method Init   -------------------------------------------------
InitStatus R3BFootStripCal2Hit::Init()
{
    R3BLOG(info, "");

    FairRootManager* rootManager = FairRootManager::Instance();
    R3BLOG_IF(fatal, rootManager == nullptr, "FairRootManager not found");

    // INPUT DATA
    fFootCalData = dynamic_cast<TClonesArray*>(rootManager->GetObject("FootCalData"));
    if (!fFootCalData)
    {
        R3BLOG(fatal, "FootCalData not found");
        return kFATAL;
    }

    // Output data
    fFootHitData = new TClonesArray("R3BFootHitData");
    rootManager->Register("FootHitData", "FOOT Hit", fFootHitData, !fOnline);
    fFootHitData->Clear();

    // Set container with mapping parameters
    SetParameter();

    return kSUCCESS;
}

// -----   Public method ReInit   -----------------------------------------------
InitStatus R3BFootStripCal2Hit::ReInit()
{
    SetParContainers();
    SetParameter();
    return kSUCCESS;
}

// -----    Filling the vector with CalData     ---------------------------------
void R3BFootStripCal2Hit::FillCalData(int nHits)
{
    // Data from cal level
    StripI.resize(fMaxNumDet);
    StripE.resize(fMaxNumDet);
    StripS.resize(fMaxNumDet);
    ClusterMult.resize(fMaxNumDet);

    ClusterPos.resize(fMaxNumDet);
    Eta.resize(fMaxNumDet);
    ClusterESum.resize(fMaxNumDet);
    ClusterCharge.resize(fMaxNumDet);
    ClusterNStrip.resize(fMaxNumDet);
    ClusterI.resize(fMaxNumDet);
    ClusterE.resize(fMaxNumDet);

    for (auto i = 0; i < nHits; i++)
    {
        auto calData = static_cast<R3BFootCalData*>(fFootCalData->At(i));
        auto detId = calData->GetDetId() - 1;
        auto stripId = calData->GetStripId() - 1;
        auto energy = calData->GetEnergy();
        auto sigma = calData->GetSigma();

        StripI[detId].push_back(stripId);
        StripE[detId].push_back(energy);
        StripS[detId].push_back(sigma);
    }
}

// -----    Clustering ----------------------------------------------------------
void R3BFootStripCal2Hit::ClusterizeStrips()
{
    // Sort elements
    for (auto i = 0; i < fMaxNumDet; ++i)
    {
        std::vector<std::tuple<int, double, double>> hits(StripI[i].size());
        for (size_t j = 0; j < hits.size(); ++j)
            hits[j] = std::make_tuple(StripI[i][j], StripE[i][j], StripS[i][j]);

        std::sort(
            hits.begin(), hits.end(), [](const auto& a, const auto& b) { return std::get<0>(a) < std::get<0>(b); });

        for (size_t j = 0; j < hits.size(); ++j)
        {
            StripI[i][j] = std::get<0>(hits[j]);
            StripE[i][j] = std::get<1>(hits[j]);
            StripS[i][j] = std::get<2>(hits[j]);
        }
    }

    // Clustering
    for (auto i = 0; i < fMaxNumDet; i++)
    {
        int ClusterCount = 0;
        int j = 0;

        while (j < StripE[i].size())
        {
            if (StripE[i][j] < fTimesSigmas * StripS[i][j])
            {
                j++;
                continue;
            }

            if (ClusterNStrip[i].size() <= ClusterCount)
            {
                ClusterNStrip[i].resize(ClusterCount + 1, 0);
                ClusterI[i].resize(ClusterCount + 1);
                ClusterE[i].resize(ClusterCount + 1);
            }

            ClusterNStrip[i][ClusterCount]++;
            ClusterI[i][ClusterCount].push_back(StripI[i][j]);
            ClusterE[i][ClusterCount].push_back(StripE[i][j]);

            int k = j + 1;
            while (k < StripE[i].size())
            {
                if (StripE[i][k] < fTimesSigmas * StripS[i][k])
                    break;

                ClusterNStrip[i][ClusterCount]++;
                ClusterI[i][ClusterCount].push_back(StripI[i][k]);
                ClusterE[i][ClusterCount].push_back(StripE[i][k]);

                k++;
            }

            j = k + 1;
            ClusterCount++;
        }

        ClusterMult[i] = ClusterCount;
    }
}

// -----   Calculate position and eta parameter ---------------------------------
void R3BFootStripCal2Hit::ComputeClusterParams()
{
    // Compute Sum Energy, Position and Eta
    for (auto i = 0; i < fMaxNumDet; i++)
    {

        ClusterPos[i].resize(ClusterMult[i], 0.0);
        Eta[i].resize(ClusterMult[i], 0.0);
        ClusterESum[i].resize(ClusterMult[i], 0.0);
        ClusterCharge[i].resize(ClusterMult[i], 0.0);

        for (auto j = 0; j < ClusterMult[i]; j++)
        {
            for (auto k = 0; k < ClusterNStrip[i][j]; k++)
            {
                ClusterESum[i][j] += ClusterE[i][j][k];

                ClusterPos[i][j] += ClusterE[i][j][k] * ClusterI[i][j][k];
            }

            ClusterPos[i][j] = ClusterPos[i][j] / ClusterESum[i][j];
            Eta[i][j] = ClusterPos[i][j] - (int)ClusterPos[i][j];
        }
    }

    // Sort Cluster from Higher to Lower Energy
    for (auto i = 0; i < fMaxNumDet; i++)
    {
        for (auto j = 0; j < ClusterMult[i] - 1; j++)
        {
            for (auto k = j + 1; k < ClusterMult[i]; k++)
            {
                if (ClusterESum[i][j] < ClusterESum[i][k])
                {
                    auto tempI = ClusterNStrip[i][j];
                    ClusterNStrip[i][j] = ClusterNStrip[i][k];
                    ClusterNStrip[i][k] = tempI;

                    double tempD = ClusterESum[i][j];
                    ClusterESum[i][j] = ClusterESum[i][k];
                    ClusterESum[i][k] = tempD;

                    tempD = ClusterPos[i][j];
                    ClusterPos[i][j] = ClusterPos[i][k];
                    ClusterPos[i][k] = tempD;

                    tempD = Eta[i][j];
                    Eta[i][j] = Eta[i][k];
                    Eta[i][k] = tempD;
                }
            }
        }

        for (auto j = 0; j < ClusterMult[i]; j++)
        {
            ClusterCharge[i][j] = ClusterESum[i][j];
        }
    }
}

// -----   Eta Correction and Charge Calibration --------------------------------
void R3BFootStripCal2Hit::EtaCorrectionAndChargeCal()
{
    for (int i = 0; i < fMaxNumDet; i++)
    {
        for (auto j = 0; j < ClusterMult[i]; j++)
        {

            int asicId = static_cast<int>(ClusterPos[i][j] / (fNumStrips / 10.));
            int equivStrip = static_cast<int>(ClusterPos[i][j]);

            // Avoid inter-asic clusters
            bool isBadAsic = false;
            if (fInterClusterWindow > 0)
                for (int iwin = -fInterClusterWindow; iwin <= fInterClusterWindow; iwin++)
                    if ((equivStrip + iwin) % static_cast<int>(fNumStrips / fNumAsic) == 0)
                        isBadAsic = true;

            if (isBadAsic)
            {
                ClusterCharge[i][j] = std::nan("");
                continue;
            }

            int generalIndex = i * fNumAsic + asicId;
            double energyCorrected = -1;
            double charge = -1;

            int nCharges = fMultCharPar[generalIndex] / fNumParsEtaCorr;

            // If the asic has no correction parameters we continue
            if (nCharges == 0)
                continue;

            // With a good asic, we have to identify how many charge
            // states does it see (i.e. how many eta bands)
            std::vector<std::unique_ptr<TF1>> polCoefs(nCharges);

            for (int iCharge = 0; iCharge < nCharges; iCharge++)
            {
                // One polynom per charge state
                auto name = Form("pol_foot_%i_asic_%i_charge_%i", i, asicId, iCharge);
                auto polType = Form("pol%i", fNumParsEtaCorr - 1);
                polCoefs[iCharge] = std::make_unique<TF1>(name, polType, 0, 1);

                for (int iCoef = 0; iCoef < fNumParsEtaCorr; iCoef++)
                {
                    polCoefs[iCharge]->SetParameter(iCoef,
                                                    fEtaCorrPar[generalIndex][iCoef + iCharge * fNumParsEtaCorr]);
                }
            }

            // Now, for our Eta value, we calculate all the predicted energies
            // (one per polynom), and then, the min of the would be the charge
            // state to which our hit belongs
            std::vector<double> diffCharge(nCharges);
            int minIndex = 0;

            for (int iCharge = 0; iCharge < nCharges; iCharge++)
            {
                diffCharge[iCharge] = polCoefs[iCharge]->Eval(Eta[i][j]) - ClusterESum[i][j];

                if (std::abs(diffCharge[iCharge]) < std::abs(diffCharge[minIndex]))
                    minIndex = iCharge;
            }

            // Correct the energy for eta > 0
            if (Eta[i][j] != 0)
                energyCorrected = polCoefs[minIndex]->Eval(fEtaCenter) - diffCharge[minIndex];
            else
                energyCorrected = ClusterESum[i][j];

            // *********** Charge calibration *********** //

            // Get the coefficients for this asic
            auto name = Form("fitFunc_foot_%i_asic_%i", i, asicId);

            TString polType = Form("pol%i", fNumParsCal - 1);

            // Use a power-law relation between charge and energy
            if (fUsePowerLawCal)
                polType = "[0]*pow(x,[1])";

            auto fitFunc = std::make_unique<TF1>(name, polType);

            auto chargeParams = fCharCalPar;

            if (Eta[i][j] == 0)
                chargeParams = fCharCalParSM;

            for (auto iPol = 0; iPol < fNumParsCal; iPol++)
                fitFunc->SetParameter(iPol, chargeParams[generalIndex][iPol]);

            charge = fitFunc->Eval(energyCorrected);
            ClusterESum[i][j] = energyCorrected;
            ClusterCharge[i][j] = charge;
        }
    }
}

// -----    Calculate detector frame positions ----------------------------------
TVector3 R3BFootStripCal2Hit::ComputeHitPosition(int i, double pos)
{
    TVector3 master(pos, 0., 0.);
    if (fTransform2Lab == true)
    {
        TRotation det2lab;
        det2lab.RotateZ(fAnglePhi[i] * TMath::DegToRad());
        det2lab.RotateY(fAngleTheta[i] * TMath::DegToRad());
        det2lab.RotateX(fAnglePsi[i] * TMath::DegToRad());

        master.Transform(det2lab);

        master += TVector3(fOffsetX[i], fOffsetY[i], fDistTarget[i] * 10);
    }

    return master;
}

// -----   Public method Execution   --------------------------------------------
void R3BFootStripCal2Hit::Exec(Option_t* /*option*/)
{
    // Reset entries in output arrays, local arrays
    Reset();

    // Check that the event has data
    auto nHits = fFootCalData->GetEntriesFast();
    if (nHits == 0)
        return;

    FillCalData(nHits);
    ClusterizeStrips();
    ComputeClusterParams();

    // If we have hit parameters, do eta correction and charge calibration
    if (fHit_Par)
        EtaCorrectionAndChargeCal();

    // Filling HitData
    for (uint8_t i = 0; i < fMaxNumDet; i++)
    {
        for (int j = 0; j < ClusterMult[i]; j++)
        {
            double pos = fFootSize * ClusterPos[i][j] / fNumStrips - fMiddle;
            TVector3 master = ComputeHitPosition(i, pos);

            if (ClusterESum[i][j] > fThSum && ClusterMult[i] < fMaxNumClusters && ClusterNStrip[i][j] < fMaxNumStrips)
            {

                AddHitData(i + 1,
                           ClusterMult[i],
                           pos,
                           master,
                           ClusterESum[i][j],
                           ClusterNStrip[i][j],
                           Eta[i][j],
                           ClusterCharge[i][j]);
            }
            else
            {
                // If this cluster is not above threshold then shouldn't be taken into account for
                // cluster multiplicity!!
                ClusterMult[i]--;
            }
        }
    }

    StripI.clear();
    StripE.clear();
    ClusterMult.clear();
    ClusterPos.clear();
    ClusterESum.clear();
    ClusterCharge.clear();
    ClusterNStrip.clear();
    Eta.clear();
    ClusterI.clear();
    ClusterE.clear();
    StripS.clear();

    return;
}

// -----   Public method Reset   ------------------------------------------------
void R3BFootStripCal2Hit::Reset()
{
    R3BLOG(debug, "Clearing HitData Structure");
    if (fFootHitData)
        fFootHitData->Clear();
}

// -----   Private method AddHitData  -------------------------------------------
R3BFootHitData* R3BFootStripCal2Hit::AddHitData(uint8_t detid,
                                                uint16_t numhit,
                                                double pos,
                                                TVector3 master,
                                                double energy_s,
                                                uint16_t mulS,
                                                double eta,
                                                double charge)
{
    TClonesArray& clref = *fFootHitData;
    int size = clref.GetEntriesFast();
    return new (clref.ConstructedAt(size)) R3BFootHitData(detid, numhit, pos, master, energy_s, mulS, eta, charge);
}

ClassImp(R3BFootStripCal2Hit)
