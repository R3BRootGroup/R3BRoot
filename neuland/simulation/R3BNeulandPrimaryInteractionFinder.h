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

#ifndef R3BROOT_R3BNEULANDPRIMARYINTERACTIONFINDER_H
#define R3BROOT_R3BNEULANDPRIMARYINTERACTIONFINDER_H

#include "FairTask.h"
#include "R3BMCTrack.h"
#include "R3BNeulandHit.h"
#include "R3BNeulandPoint.h"
#include <R3BIOConnector.h>

class TH1D;
class TH2D;

class R3BNeulandPrimaryInteractionFinder : public FairTask
{
  public:
    explicit R3BNeulandPrimaryInteractionFinder(std::string_view pointsIn = "NeulandPoints",
                                                std::string_view hitsIn = "NeulandHits",
                                                std::string_view pointsOut = "NeulandPrimaryPoints",
                                                std::string_view hitsOut = "NeulandPrimaryHits",
                                                std::string_view tracksOut = "NeulandPrimaryTracks");

    ~R3BNeulandPrimaryInteractionFinder() override = default;

    // No copy and no move is allowed (Rule of three/five)
    R3BNeulandPrimaryInteractionFinder(const R3BNeulandPrimaryInteractionFinder&) = delete;
    R3BNeulandPrimaryInteractionFinder(R3BNeulandPrimaryInteractionFinder&&) = delete;
    auto operator=(const R3BNeulandPrimaryInteractionFinder&) -> R3BNeulandPrimaryInteractionFinder& = delete;
    auto operator=(R3BNeulandPrimaryInteractionFinder&&) -> R3BNeulandPrimaryInteractionFinder& = delete;

  protected:
    auto Init() -> InitStatus override;
    void Finish() override;

  public:
    void Exec(Option_t* /*option*/) override;

  private:
    R3B::InputTCAConnector<R3BMCTrack> fTracksIn;
    R3B::InputVectorConnector<R3BNeulandPoint> fPointsIn;
    R3B::InputVectorConnector<R3BNeulandHit> fHitsIn;

    R3B::OutputVectorConnector<R3BMCTrack> fTracksOut;
    R3B::OutputVectorConnector<R3BNeulandPoint> fPointsOut;
    R3B::OutputVectorConnector<R3BNeulandHit> fHitsOut;

    TH1D* fhDistance;
    TH2D* fhPointsVsHits;
    TH2D* fhPointVsHitPaddle;

    ClassDefOverride(R3BNeulandPrimaryInteractionFinder, 0);
};

#endif // R3BROOT_R3BNEULANDPRIMARYINTERACTIONFINDER_H
