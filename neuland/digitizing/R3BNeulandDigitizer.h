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

#ifndef R3B_NEULAND_DIGITIZER_H
#define R3B_NEULAND_DIGITIZER_H

#include <TClonesArray.h>
#include "DigitizingEngine.h"
#include "FairTask.h"
#include "Filterable.h"
#include "R3BNeulandGeoPar.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandHitPar.h"
#include "R3BNeulandPoint.h"
#include "TCAConnector.h"

class TGeoNode;
class TH1F;
class TH2F;

/**
 * NeuLAND digitizing finder task
 * @author Jan Mayer
 *
 * For each event, get the R3BNeulandPoints and apply the detector response to create R3BNeulandHits.
 *   Input:  Branch NeulandPoints = TClonesArray("R3BNeulandPoint")
 *           Stored Neuland Geometry Parameter NeulandGeoPar
 *   Output: Branch NeulandHits  = TClonesArray("R3BNeulandDigi")
 *   Additional output: Some control histograms
 *
 */

class R3BNeulandDigitizer : public FairTask
{
  public:
    R3BNeulandDigitizer(TString input = "NeulandPoints", TString output = "NeulandHits");
    R3BNeulandDigitizer(Neuland::DigitizingEngine* engine,
                        TString input = "NeulandPoints",
                        TString output = "NeulandHits");

    ~R3BNeulandDigitizer() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandDigitizer(const R3BNeulandDigitizer&) = delete;            // copy constructor
    R3BNeulandDigitizer(R3BNeulandDigitizer&&) = delete;                 // move constructor
    R3BNeulandDigitizer& operator=(const R3BNeulandDigitizer&) = delete; // copy assignment
    R3BNeulandDigitizer& operator=(R3BNeulandDigitizer&&) = delete;      // move assignment

  protected:
    InitStatus Init() override;
    void Finish() override;
    void SetParContainers() override;

  public:
    void Exec(Option_t*) override;
    void AddFilter(const Filterable<R3BNeulandHit&>::Filter& f) { fHitFilters.Add(f); }
    void SetHitParName(const TString& name) { fHitParName = name; };

  private:
    TCAInputConnector<R3BNeulandPoint> fPoints;
    TCAOutputConnector<R3BNeulandHit> fHits;

    std::unique_ptr<Neuland::DigitizingEngine> fDigitizingEngine; // owning

    Filterable<R3BNeulandHit&> fHitFilters;

    R3BNeulandGeoPar* fNeulandGeoPar; // non-owning

    TString fHitParName{};
    TH1F* hMultOne;
    TH1F* hMultTwo;
    TH1F* hRLTimeToTrig;
    TH2F* hElossVSQDC;

  public:
    ClassDefOverride(R3BNeulandDigitizer, 0)
};

#endif // R3B_NEULAND_DIGITIZER_H
