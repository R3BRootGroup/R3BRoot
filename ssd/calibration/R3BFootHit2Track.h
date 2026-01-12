/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
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

// ---------------------------------------------------------------
// -----      R3BFootHit2Track header file                   -----
// -----       Created 16/09/2025 by V. Panin                -----
// ---------------------------------------------------------------

#pragma once

#include "R3BEventHeader.h"
#include "R3BFootHitData.h"
#include "R3BFootMappingPar.h"
#include "R3BTrackingParticle.h"

#include <FairTask.h>

#include <Math/Factory.h>
#include <Math/Functor.h>
#include <Math/GSLMinimizer.h>
#include <Math/Minimizer.h>
#include <Minuit2/Minuit2Minimizer.h>
#include <Rtypes.h>
#include <TArrayF.h>
#include <TMath.h>
#include <TVector3.h>
#include <vector>

class TClonesArray;
class R3BFootMappingPar;

class R3BFootHit2Track : public FairTask
{
  public:
    /** Standard constructor **/
    R3BFootHit2Track(const TString& name = "R3BFootHit2Track", int iVerbose = 1);

    /** Destructor **/
    ~R3BFootHit2Track();

    /** Virtual method Exec **/
    void Exec(Option_t* /*option*/) override;

    /** Virtual method Reset **/
    virtual void Reset();

    // Fair specific
    /** Virtual method Init **/
    InitStatus Init() override;

    /** Virtual method ReInit **/
    InitStatus ReInit() override;

    void FinishTask() override;

    /** Accessor for selecting online mode **/
    inline void SetOnline(Bool_t option) { fOnline = option; }

    enum class TrackDirection
    {
        In,
        Out
    };

  public:
    void SetMappingPar();    // reading FOOT geometry from MappingPar
    void SetupMinimizer();   // Configure minimizer for FitParticle()
    void PrepareMinimizer(); // reset and set limits for the minimizer
    bool RunMinimizer();     // execute minimzation

    void SetVFTXNumber(int nb) { vftxNumber = nb; };

    R3BTrackingParticle* AddTrack(const R3BTrackingParticle& p);
    void Transform_Point(TString flag, TVector3& hit_point, TVector3* offset, TVector3* rotation);
    bool FitParticle(R3BTrackingParticle& part, TrackDirection direction); // flag = "in" or "out"
    double FitParticleError(const double* par);                            // error function for the minimizer
    Bool_t PropagateParticleToPlane(int detId, TVector3& intersect);
    void MakeParticle(double dtx, double dty, double particle_offset);

    R3BFootMappingPar* fMap_Par = nullptr; // Parameter container with mapping

    TClonesArray* fFootHitData = nullptr; // Array with FOOT Hit-input data
    TClonesArray* fFrsData = nullptr;     // Array with FRS-input data

    TClonesArray* fIncomingTrackFoot = nullptr; // Array with FOOT Track-output data
    TClonesArray* fOutgoingTrackFoot = nullptr; // Array with FOOT Track-output data

    bool fOnline = false; // Don't store data for online

    // FOOT geometry from MappingPar
    int fMaxNumDet = 8;
    std::vector<TVector3> foot_position;
    std::vector<TVector3> foot_rotation;

    std::map<uint8_t, R3BFootHitData*> maxHitPerDet; // to store highest energy hit in each det
    std::map<uint8_t, int> maxHitTCAIndex;           // to store TCA index of the max energy hits in each det

    std::unique_ptr<R3BTrackingParticle> thisTrackingParticle;

    std::array<TVector3, 4> det_points;

    // Arrays to store indices of the FootHitData corresponding to every track
    std::array<int, 4> upstream_foot_hit_index = { 0, 0, 0, 0 };
    std::array<int, 4> downstream_foot_hit_index = { 0, 0, 0, 0 };

    // Essential structures for the minimizer
    static constexpr UInt_t NVarsFunctor = 3; // dtx, dty and one position offset
    std::array<double, NVarsFunctor> step;
    std::array<double, NVarsFunctor> offset;
    std::array<double, NVarsFunctor> min_offset;
    std::array<double, NVarsFunctor> max_offset;

    std::unique_ptr<ROOT::Math::Minimizer> minimizer;

    struct DetectorPlane
    {
        int id;
        // Three points on the plane
        TVector3 v1;
        TVector3 v2;
        TVector3 v3;
        TVector3 norm;
        TVector3 position;
        TVector3 rotation;
        TString name;
    };
    std::vector<DetectorPlane> detector_planes;

    int NEventsTrig1;
    int NgoodEventsInFrs;
    int NgoodEventsInAllFoots;

    TrackDirection fInOutFlag;
    R3BEventHeader* fHeader = nullptr;

  private:
    // Select the VFTX
    int vftxNumber = 2;

    ClassDefOverride(R3BFootHit2Track, 2);
};
