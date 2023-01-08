/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2023 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BNEULANDHITMON_H
#define R3BNEULANDHITMON_H

/** Neuland Digi Monitor
 * @author Jan Mayer
 * @since  10.06.2015
 *
 *  Input:  Digis. Should work both on Digis from Monte Carlo simulations
 *          as well as experimental data on digi level.
 *  Output: Vairous diagrams, 3D scatterplot for each event if option 3DTRACK is given.
 */

#include "FairTask.h"
#include "R3BNeulandHit.h"
#include "TCAConnector.h"

class TH1D;
class TH2D;
class TH3D;

class R3BNeulandHitMon : public FairTask
{
  public:
    R3BNeulandHitMon(TString input = "NeulandHits", TString output = "NeulandHitMon", const Option_t* option = "");

    ~R3BNeulandHitMon() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandHitMon(const R3BNeulandHitMon&) = delete;            // copy constructor
    R3BNeulandHitMon(R3BNeulandHitMon&&) = delete;                 // move constructor
    R3BNeulandHitMon& operator=(const R3BNeulandHitMon&) = delete; // copy assignment
    R3BNeulandHitMon& operator=(R3BNeulandHitMon&&) = delete;      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;

  public:
    void Exec(Option_t*) override;

    void SetDistanceToTarget(double x) { fDistanceToTarget = x; }

  private:
    TString fOutput;

    TCAInputConnector<R3BNeulandHit> fHits;

    double fDistanceToTarget;

    Bool_t fIs3DTrackEnabled;
    TH3D* fh3;

    TH1D* hTime;
    TH1D* hTimeAdj;
    TH1D* hMult;
    TH1D* hDepth;
    TH1D* hForemostEnergy;
    TH1D* hSternmostEnergy;
    TH2D* hDepthVSForemostEnergy;
    TH2D* hDepthVSSternmostEnergy;
    TH1D* hEtot;
    TH1D* hE;
    TH1D* hX;
    TH1D* hY;
    TH1D* hT;
    TH1D* hTNeigh;
    TH2D* hDepthVSEtot;
    TH2D* hPosVSEnergy;
    TH2D* hdeltaEE;
    TH1D* hBeta;

    ClassDefOverride(R3BNeulandHitMon, 0);
};

#endif // R3BNEULANDHITMON_H
