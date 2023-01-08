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

#ifndef R3BROOT_R3BNEULANDONLINESPECTRA_H
#define R3BROOT_R3BNEULANDONLINESPECTRA_H

#include "FairTask.h"
#include "R3BNeulandCalData.h"
#include "R3BNeulandHit.h"
#include "R3BPaddleTamexMappedData.h"
#include "TCAConnector.h"
#include <array>

class TCanvas;
class TH1D;
class TH2D;
class R3BEventHeader;

class R3BNeulandOnlineSpectra : public FairTask
{

  public:
    R3BNeulandOnlineSpectra();

    InitStatus Init() override;
    void Exec(Option_t*) override;
    void FinishTask() override;

    void ResetHistos();
    void ResetHistosMapped();
    void SetDistanceToTarget(double x) { fDistanceToTarget = x; }

    void SetCosmicTpat(UInt_t CosmicTpat = 0) { fCosmicTpat = CosmicTpat; }

  private:
    static const unsigned int fNPlanes = 26;
    static const unsigned int fNBars = fNPlanes * 50;

    R3BEventHeader* fEventHeader;

    TCAInputConnector<R3BPaddleTamexMappedData> fNeulandMappedData;
    TCAInputConnector<R3BNeulandCalData> fNeulandCalData;
    TCAInputConnector<R3BNeulandHit> fNeulandHits;

    TH1D* hTstart;
    TH1D* hNstart;

    TH2D* hTestJump;
    TH2D* hJumpsvsEvnt;
    TH2D* hJumpsvsEvntzoom;

    std::array<TH1D*, 4> ahMappedBar1;
    std::array<TH1D*, 4> ahMappedBar2;

    std::array<TH2D*, 2> ahCalTvsBar;
    std::array<TH2D*, 2> ahCalEvsBar;

    TH2D* hHitEvsBar;

    TH2D* hHitEvsBarCosmics;

    TH2D* hTdiffvsBar;
    TH2D* hToFvsBar;
    TH2D* hTofvsEhit;

    TH2D* hToFcvsBar;
    TH2D* hTofcvsEhit;

    TH2D* hTofvsX;
    TH2D* hTofcvsX;
    TH2D* hTofvsY;
    TH2D* hTofcvsY;
    TH2D* hTofvsZ;
    TH2D* hTofcvsZ;

    TH2D* hTdiffvsBarCosmics;
    TH2D* hDT675;
    TH2D* hDT625;
    TH2D* hDT675c;
    TH2D* hDT625c;

    std::array<TH2D*, fNPlanes> ahXYperPlane;

    TH1D* hSofiaTime;
    TH2D* hNeuLANDvsStart;
    TH1D* hTOF;
    TH1D* hTOFc;

    double fDistanceToTarget;

    bool fIsOnline;

    UInt_t fCosmicTpat = 0;

  private:
    bool IsBeam() const;

    ClassDefOverride(R3BNeulandOnlineSpectra, 0)
};

#endif // R3BROOT_R3BNEULANDONLINESPECTRA_H
