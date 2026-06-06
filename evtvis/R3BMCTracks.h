/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019-2026 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#pragma once

#include <FairTask.h>

#include <Rtypes.h>              // for Double_t, etc
#include <TEveTrackPropagator.h> // IWYU pragma: keep needed by cint
#include <TString.h>             // for TString

class FairEventManager;
class TClonesArray;
class TEveTrackList;
class TObjArray;
class TParticle;

class R3BMCTracks : public FairTask
{
  public:
    /** Default constructor **/
    R3BMCTracks() = default;

    /** Standard constructor
     *@param name        Name of task
     *@param iVerbose    Verbosity level
     **/
    explicit R3BMCTracks(const char* name = "R3BMCTracks", Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BMCTracks() = default;

    /** Set verbosity level. For this task and all of the subtasks. **/
    void SetVerbose(Int_t iVerbose) { fVerbose = iVerbose; }
    /** Executed task **/
    void Exec(Option_t*) override;
    InitStatus Init() override;

    /** Action after each event**/
    virtual void Reset();
    TEveTrackList* GetTrGroup(TParticle* P);

  protected:
    TClonesArray* fTrackList = nullptr;
    TEveTrackPropagator* fTrPr = nullptr;
    FairEventManager* fEventManager = nullptr;
    TObjArray* fEveTrList = nullptr;
    TString fEvent = "";
    TEveTrackList* fTrList = nullptr;

    Double_t MinEnergyLimit = -1.;
    Double_t MaxEnergyLimit = -1.;
    Double_t PEnergy = -1.;

  private:
    R3BMCTracks(const R3BMCTracks&);
    R3BMCTracks& operator=(const R3BMCTracks&);

  public:
    ClassDefOverride(R3BMCTracks, 1);
};
