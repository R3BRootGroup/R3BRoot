/******************************************************************************
 *   Copyright (C) 2025 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2025 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// Created on 16/09/2025 by V.Panin

#ifndef R3BTRACKINGG249
#define R3BTRACKINGG249

#include "FairTask.h"
#include "R3BEventHeader.h"
#include "R3BFiberMAPMTHitData.h"
#include "R3BMDFWrapper.h"
#include "R3BTrackingParticle.h"
#include "TClonesArray.h"
#include "TH2F.h"
#include "TMath.h"
#include "TString.h"
#include "TVector3.h"
#include <Math/GSLMinimizer.h>
#include <Math/Minimizer.h>
#include <Minuit2/Minuit2Minimizer.h>
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

class R3BTrackingG249 : public FairTask
{
  public:
    R3BTrackingG249();
    R3BTrackingG249(const char* name, Int_t iVerbose = 1);
    ~R3BTrackingG249();
    InitStatus Init() override;
    void Exec(Option_t* /*option*/) override;
    void FinishEvent() override;
    void FinishTask() override;

    // GLAD current which was used for training MDF function
    // Should  be set form the steering macro
    void SetGladReferenceCurrent(Double_t cur) { GladReferenceCurrent = cur; }

    // GLAD current in the run being anlaysed, set from steering macro
    void SetGladCurrent(Double_t cur) { GladCurrent = cur; }

    // Set MDF functions from the steering macro
    void Set_MDF_PoQ(TString name) { MDF_PoQ_filename = name; }
    void SetTrigger(Int_t trigger) { fTrigger = trigger; }
    void SetTpat(Int_t tpat) { fTpat = tpat; }
    void SetMaxEvent(Int_t nev) { maxevent = nev; }

    // Set lab positions and angles of the detectors from the steering macro
    void SetPositionLOS(double x, double y, double z) { los_position.SetXYZ(x, y, z); }   // cm
    void SetPositionFib30(double x, double y, double z) { f30_position.SetXYZ(x, y, z); } // cm
    void SetPositionFib31(double x, double y, double z) { f31_position.SetXYZ(x, y, z); } // cm
    void SetPositionFib32(double x, double y, double z) { f32_position.SetXYZ(x, y, z); } // cm
    void SetPositionFib33(double x, double y, double z) { f33_position.SetXYZ(x, y, z); } // cm
    void SetPositionTofd(double x, double y, double z) { tofd_position.SetXYZ(x, y, z); } // cm

    void SetAnglesLOS(double x, double y, double z) { los_angles.SetXYZ(x, y, z); }   // rad
    void SetAnglesFib30(double x, double y, double z) { f30_angles.SetXYZ(x, y, z); } // rad
    void SetAnglesFib31(double x, double y, double z) { f31_angles.SetXYZ(x, y, z); } // rad
    void SetAnglesFib32(double x, double y, double z) { f32_angles.SetXYZ(x, y, z); } // rad
    void SetAnglesFib33(double x, double y, double z) { f33_angles.SetXYZ(x, y, z); } // rad
    void SetAnglesTofd(double x, double y, double z) { tofd_angles.SetXYZ(x, y, z); } // rad

    // Get lab positions and angles (needed by alignment function)
    inline TVector3 GetPositionLOS() { return los_position; }   // cm
    inline TVector3 GetPositionF30() { return f30_position; }   // cm
    inline TVector3 GetPositionF31() { return f31_position; }   // cm
    inline TVector3 GetPositionF32() { return f32_position; }   // cm
    inline TVector3 GetPositionF33() { return f33_position; }   // cm
    inline TVector3 GetPositionTofd() { return tofd_position; } // cm

    inline TVector3 GetAnglesLOS() { return los_angles; }   // rad
    inline TVector3 GetAnglesF30() { return f30_angles; }   // rad
    inline TVector3 GetAnglesF31() { return f31_angles; }   // rad
    inline TVector3 GetAnglesF32() { return f32_angles; }   // rad
    inline TVector3 GetAnglesF33() { return f33_angles; }   // rad
    inline TVector3 GetAnglesTofd() { return tofd_angles; } // rad

    bool Get_all_TCA_with_MinimumMul();
    bool Construct_FiberTrack();
    void Collect_FiberHits();

    R3BMDFWrapper* Get_MDF_PoQ() { return MDF_PoQ; }
    void MakeDetectorPlanes();
    void Reset();
    void FixupFiberTrack();
    // Transforming input detector hit(point) from lab->det or det->lab via flag = "Det2Lab" or "Lab2Det"
    void TransformPoint(TString flag, TVector3& hit_point, TVector3* rotation, TVector3* det_offset);
    bool MacroParametersAreOk();

    void SetTofOffset(double toffset) { tof_offset = toffset; } // ns
    // Setup incoming cuts on FRS data and Frs data
    void SetFrsEllipticPIDCut(double meanZ, double rZ, double meanAoZ, double rAoZ);
    bool IsInsideFrsEllipticPIDCut(double z, double aoz);
    void SetFrsZMinMax(double min, double max)
    {
        FrsZMin = min;
        FrsZMax = max;
    }
    void SetFRSBrhoMinMax(double min, double max)
    {
        FrsBrhoMin = min;
        FrsBrhoMax = max;
    }
    void SetFiberEnergyMinMax(double min, double max)
    {
        FiberEnergyMin = min;
        FiberEnergyMax = max;
    }

    void SetFiberTimeMinMax(double min, double max)
    {
        FiberTimeMin = min;
        FiberTimeMax = max;
    }

    void SetTofdEnergyMinMax(double min, double max)
    {
        TofdEnergyMin = min;
        TofdEnergyMax = max;
    }

    // Setters for the alignment procedure
    void SetDoAlignment(bool flag) { DoAlignment = flag; }
    void Alignment();
    static double AlignmentErrorG249(const double* par);
    void SetReferencePoQ(Double_t val) { reference_PoQ = val; }
    void SetDeltaPoQ(Double_t val) { delta_PoQ = val; }
    Double_t GetReferencePoQ() { return reference_PoQ; }

    std::map<uint8_t, R3BFiberMAPMTHitData*> maxHitPerFiber; // to store highest energy hit in each det
    std::map<uint8_t, int> maxHitTCAIndex;                   // to store TCA index of the max energy hits in each det

    struct fiber_points
    {
        TVector3 f1;
        TVector3 f2;
        TVector3 f3;
        TString last_fiber;
    };
    fiber_points fiber_track;

    struct align_track
    {
        R3BTrackingParticle foot_track;
        fiber_points fib_track;
        Double_t reference_PoQ;
    };
    align_track track_for_alignment;
    std::vector<align_track> align_data;

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

    // Structures for alignment
    void SetupMinimizer(); // Reset and configure minimizer
    double AlignmentError(const double* par);
    static constexpr UInt_t NVarsFunctor = 2; // X, Z (F32), X, Y, Z, (F30), X, Z (lastF), RotY x 3 fibers
    std::array<double, NVarsFunctor> step;
    std::array<double, NVarsFunctor> offset;
    std::array<double, NVarsFunctor> min_offset;
    std::array<double, NVarsFunctor> max_offset;
    std::unique_ptr<ROOT::Math::Minimizer> minimizer;

  private:
    //-- Input hit data from the TClonesArray
    // do not change the order, add new det in the end
    enum DetectorInstances
    {
        DET_FI_FIRST,
        DET_FI32 = DET_FI_FIRST,
        DET_FI30,
        DET_FI33,
        DET_FI31,
        DET_FI_LAST = DET_FI31,
        DET_TOFD,
        DET_FOOT,
        FRS_DATA,
        LOS_HITDATA,
        INCOMING_FOOT_TRACK,
        OUTGOING_FOOT_TRACK,
        DET_MAX
    };

#define NOF_FIB_DET (DET_FI_LAST - DET_FI_FIRST + 1)

    // Names of essential branches in the input tree
    // do not change the order! add new data in the end
    const char* fDetectorNames[DET_MAX + 1] = { "Fi32Hit",           "Fi30Hit",           "Fi33Hit", "Fi31Hit",
                                                "TofdHit",           "FootHitData",       "FrsData", "LosHit",
                                                "IncomingTrackFoot", "OutgoingTrackFoot", NULL };

    R3BEventHeader* fHeader;

    TVector3 los_position;
    TVector3 f30_position;
    TVector3 f31_position;
    TVector3 f32_position;
    TVector3 f33_position;
    TVector3 tofd_position;

    TVector3 los_angles;
    TVector3 f30_angles;
    TVector3 f31_angles;
    TVector3 f32_angles;
    TVector3 f33_angles;
    TVector3 tofd_angles;

    // Input MDF functions
    R3BMDFWrapper* MDF_PoQ;
    TString MDF_PoQ_filename;

    Double_t mdf_data[9]; // data container for the MDF function

    Int_t fTrigger;
    Int_t fTpat;
    unsigned long fNEvents; // Event counter
    Int_t maxevent;
    Bool_t DoAlignment;
    Double_t reference_PoQ;
    Double_t delta_PoQ;
    Double_t GladCurrent;
    Double_t GladReferenceCurrent;
    Double_t tof_offset; // ns

    // frs elliptic cuts:
    Double_t FrsCutMeanZ;
    Double_t FrsCutRadiusZ;
    Double_t FrsCutMeanAoZ;
    Double_t FrsCutRadiusAoZ;

    // frs elliptic cuts:
    Double_t FrsBrhoMin;
    Double_t FrsBrhoMax;

    // Z cut ranges set by SetFrsZMinMax()
    Double_t FrsZMin;
    Double_t FrsZMax;

    // Cut on fiber hit energy set by SetFiberEnergyMinMax():
    Double_t FiberEnergyMin;
    Double_t FiberEnergyMax;

    // Cut on fiber hit time set by SetFiberTimeMinMax():
    Double_t FiberTimeMin;
    Double_t FiberTimeMax;

    // Cut on Tofd hit energy set by SetTofdEnergyMinMax():
    Double_t TofdEnergyMin;
    Double_t TofdEnergyMax;

    // Essential constants
    static constexpr Double_t SPEED_OF_LIGHT = 29.9792458; // cm/ns
    static constexpr Double_t AMU = 0.9314940038;          // GeV/c2

    // Private method to fill output track data
    R3BTrackingParticle* AddTrackData(const R3BTrackingParticle& p);

    std::vector<TClonesArray*> fDataItems; // input data TCAs
    TClonesArray* los_DataItems = nullptr;
    TClonesArray* frs_DataItems = nullptr;
    TClonesArray* foot_DataItems = nullptr;
    TClonesArray* tofd_DataItems = nullptr;
    TClonesArray* foot_incoming_tracks = nullptr;
    TClonesArray* foot_outgoing_tracks = nullptr;
    TClonesArray* fTrackItems = nullptr; // output data

    TH2F* h2_lab_XZ;

  public:
    ClassDefOverride(R3BTrackingG249, 1)
};

#endif
