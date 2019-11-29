/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BNEULANDCAL2HIT_H
#define R3BNEULANDCAL2HIT_H

#include "FairTask.h"
#include "R3BLosCalData.h"
#include "R3BNeulandCalData.h"
#include "R3BNeulandHit.h"
#include "TCAConnector.h"
#include <map>

class R3BNeulandHitPar;

class R3BNeulandCal2Hit : public FairTask
{
  public:
    R3BNeulandCal2Hit();
    ~R3BNeulandCal2Hit() override = default;

    InitStatus Init() override;
    InitStatus ReInit() override;
    void SetParContainers() override;
    void Exec(Option_t*) override;

    inline void SetFirstPlaneHorizontal() { fFirstPlaneHorizontal = kTRUE; }

    // Distance to target in cm
    inline void SetDistanceToTarget(Double_t d) { fDistanceToTarget = d; }

    // Global time offset in ns
    inline void SetGlobalTimeOffset(Double_t t0) { fGlobalTimeOffset = t0; }

  private:
    void SetParameter();
    double GetTstart() const;

    TCAInputConnector<R3BNeulandCalData> fCalData;
    TCAOutputConnector<R3BNeulandHit> fHits;
    TCAOptionalInputConnector<R3BLosCalData> fLosCalData;

    R3BNeulandHitPar* fPar;

    Bool_t fFirstPlaneHorizontal;
    Double_t fDistanceToTarget;
    Double_t fGlobalTimeOffset;

    std::map<Int_t, Bool_t> fMapIsSet;
    std::map<Int_t, Double_t> fMapVeff;
    std::map<Int_t, Double_t> fMapTSync;
    std::map<Int_t, Double_t> fMapEGain;

  public:
    ClassDefOverride(R3BNeulandCal2Hit, 0)
};

#endif
