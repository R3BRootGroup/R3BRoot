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

// ---------------------------------------------------------------------
// -----            R3BFootStripCal2Hit source file                -----
// -----       Created 05/11/21 by J.L. Rodriguez-Sanchez          -----
// -----       Modified 05/2025 by Pablo Gonzalez Rusell           -----
// ---------------------------------------------------------------------

#pragma once

#include "R3BFootHitData.h"

#include <FairTask.h>

#include <Rtypes.h>
#include <TArrayF.h>
#include <TVector3.h>
#include <vector>

class TClonesArray;
class R3BFootMappingPar;
class R3BFootHitPar;

class R3BFootStripCal2Hit : public FairTask
{
  public:
    /** Standard constructor **/
    R3BFootStripCal2Hit(const TString& name = "R3BFootStripCal2Hit", int iVerbose = 1);

    /** Destructor **/
    ~R3BFootStripCal2Hit();

    /** Virtual method Exec **/
    void Exec(Option_t* /*option*/) override;

    /** Virtual method Reset **/
    virtual void Reset();

    // Fair specific
    /** Virtual method Init **/
    InitStatus Init() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

    /** Virtual method SetParContainers **/
    void SetParContainers() override;

    /** Virtual method SetParContainers **/
    void SetTimesSigmas(Double_t sigmas) { fTimesSigmas = sigmas; };

    /** Accessor for selecting online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

    /** Accessor for selecting max. number of clusters per FOOT detector **/
    void SetMaxNumClusters(int max) { fMaxNumClusters = max; }

    /** Accessor to set up the threshold for the cluster energy sum **/
    void SetClusterEnergy(double thsum) { fThSum = thsum; }

    // Method for setting the maximum number of strips for each cluster
    void SetMaxNumStrips(int max) { fMaxNumStrips = max; }

    // Method to disable transformation of hits to lab coordinates (default: true)
    void SetTransform2Lab(bool flag) { fTransform2Lab = flag; }

    // Method to set the number of inter-cluster strips that need to be disrigarded
    void SetInterClusterWindow(int val) { fInterClusterWindow = val; }

    // Method to use the power-law algoritm (Z = aE**gamma) instead of the default linear
    // calibration
    void SetUsePowerLawCal() { fUsePowerLawCal = true; }

  private:
    void SetParameter();
    void FillCalData(int nHits);
    void ClusterizeStrips();
    void ComputeClusterParams();
    void EtaCorrectionAndChargeCal();
    TVector3 ComputeHitPosition(int detId, double pos);

    bool fTransform2Lab = true;

    static constexpr double fFootSize = 96.;
    static constexpr double fMiddle = fFootSize / 2.;
    static constexpr int fNumAsic = 10;
    static constexpr int fNumStrips = 640;
    static constexpr double fEtaCenter = 0.5;

    int fMaxNumDet = 16;
    double fThSum = 20.;
    double fTimesSigmas = 3.;

    int fMaxNumClusters = 10;
    int fMaxNumStrips = 640;
    int fInterClusterWindow = 2;
    bool fUsePowerLawCal = false;

    std::vector<double> fDistTarget;
    std::vector<double> fAngleTheta;
    std::vector<double> fAnglePhi;
    std::vector<double> fAnglePsi;
    std::vector<double> fOffsetX;
    std::vector<double> fOffsetY;

    std::vector<std::vector<int>> StripI;                   // Strip Id
    std::vector<std::vector<double>> StripE;                // Strip energy
    std::vector<std::vector<double>> StripS;                // Strip Sigma
    std::vector<int> ClusterMult;                           // Cluster multiplicity
    std::vector<std::vector<double>> ClusterPos;            // Position of Cluster from Weighted Average
    std::vector<std::vector<double>> Eta;                   // Decimal part of the average position of the cluster
    std::vector<std::vector<double>> ClusterESum;           // Sum of Energies in the Cluster
    std::vector<std::vector<double>> ClusterCharge;         // Sum of Energies in the Cluster
    std::vector<std::vector<int>> ClusterNStrip;            // Number of Strips in Cluster
    std::vector<std::vector<std::vector<double>>> ClusterI; // Id Strip in Cluster
    std::vector<std::vector<std::vector<double>>> ClusterE; // Energy of Strip in Cluster

    // Parameters for the eta correction and calibration
    int fNumParsCal = 2;
    int fNumParsEtaCorr = 5;
    std::vector<int> fMultCharPar;
    std::vector<std::vector<double>> fEtaCorrPar;
    std::vector<std::vector<double>> fCharCalPar;
    std::vector<std::vector<double>> fCharCalParSM;

    R3BFootMappingPar* fMap_Par = nullptr; // Parameter container with mapping
    R3BFootHitPar* fHit_Par = nullptr;     // Parameter container with hit params

    TClonesArray* fFootCalData = nullptr; // Array with FOOT Cal-input data
    TClonesArray* fFootHitData = nullptr; // Array with FOOT Hit-output data

    bool fOnline = false; // Don't store data for online

    // Private method AddHitData
    R3BFootHitData* AddHitData(uint8_t detid,
                               uint16_t numhit,
                               double s,
                               TVector3 master,
                               double energy_s,
                               uint16_t mulS,
                               double eta,
                               double charge);

  public:
    // Class definition
    ClassDefOverride(R3BFootStripCal2Hit, 1);
};
