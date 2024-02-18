/******************************************************************************
 *   Copyright (C) 2022 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2022-2024 Members of R3B Collaboration                     *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

// -------------------------------------------------------------
// -----               R3BAlpideCal2Hit                    -----
// -----     Created 22/03/22 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#pragma once

#include <FairTask.h>
#include <Rtypes.h>
#include <TRotation.h>

#include "R3BAlpideHitData.h"

class TClonesArray;
class R3BAlpideMappingPar;
class R3BTGeoPar;
class R3BAlpideGeometry;
class TVector3;

class R3BAlpideCal2Hit : public FairTask
{
  public:
    /** Default constructor **/
    R3BAlpideCal2Hit();

    /** Standard constructor **/
    R3BAlpideCal2Hit(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BAlpideCal2Hit() override;

    /** Method Exec **/
    void Exec(Option_t*) override;

    /** Virtual method Reset **/
    virtual void Reset();

    /** Method SetParContainers **/
    void SetParContainers() override;

    /** Method Init **/
    InitStatus Init() override;

    /** Method ReInit **/
    InitStatus ReInit() override;

    // Method to setup online mode
    inline void SetOnline(bool option) { fOnline = option; }

  private:
    void SetParameter();
    void FindClusters();

    double fPixelSize = 0.0292968;
    int fNbSensors = 108;
    bool fOnline = false;

    TVector3 fTargetPos;
    TVector3 fAlpidePos;
    TVector3 fAlpidetoTargetPos;

    TVector3 fTrans;
    TRotation fRot;

    R3BTGeoPar* fTargetGeoPar = nullptr;
    R3BTGeoPar* fAlpideGeoPar = nullptr;
    int fGeoversion = 202402;
    R3BAlpideGeometry* fAlpideGeo = nullptr;
    R3BAlpideMappingPar* fMap_Par = nullptr;
    TClonesArray* fAlpideCalData = nullptr; // Array with Alpide Cal input data
    TClonesArray* fAlpideHitData = nullptr; // Array with Alpide Hit output data
    TClonesArray* fAlpideCluster = nullptr; // Array with clusters

    // Private method AddHitData
    R3BAlpideHitData* AddHitData(uint16_t senId,
                                 uint16_t clustersize,
                                 double xpos,
                                 double ypos,
                                 double zpos,
                                 double locxpos,
                                 double locypos);

  public:
    // Class definition
    ClassDefOverride(R3BAlpideCal2Hit, 1)
};
