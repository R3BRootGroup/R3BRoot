/******************************************************************************
 *   Copyright (C) 2026 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2026 Members of R3B Collaboration                          *
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
// -----               R3BActafHit2Cluster                 -----
// -----     Created 12/04/26 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#pragma once

#include <FairTask.h>

#include "R3BActafClusterData.h"

#include <Rtypes.h>
#include <TVector3.h>

class TClonesArray;

class R3BActafHit2Cluster : public FairTask
{
  public:
    /** Default constructor **/
    R3BActafHit2Cluster();

    /** Standard constructor **/
    R3BActafHit2Cluster(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BActafHit2Cluster() override;

    struct Point
    {
        double x, y, z;
        int side, ring, pad;
        double energy;
    };

    struct HitPoint
    {
        double x, y, z, phi, energy;
        int side, ring, pad;
        bool used = false;
    };

    /** Method Exec **/
    void Exec(Option_t*) override;

    /** Virtual method Reset **/
    virtual void Reset();

    /** Method Init **/
    InitStatus Init() override;

    /** Method ReInit **/
    InitStatus ReInit() override;

    // Method to setup online mode
    void SetOnline(bool option = true) { fOnline = option; }

  private:
    double Distance(const HitPoint& a, const HitPoint& b);

    void FitTrack(const std::vector<Point>& pts,
                  double& x0,
                  double& y0,
                  double& z0,
                  double& phi,
                  double& theta,
                  double& Etot,
                  double& chi2polarfit);

    bool fOnline = false; // Don't store data for online

    TClonesArray* fActafHitData = nullptr; // Array with Actaf Hit input data
    TClonesArray* fActafCluster = nullptr; // Array with Actaf Cluster output data

    // Private method AddClusterData
    R3BActafClusterData* AddClusterData(UInt_t side,
                                        double xpos,
                                        double ypos,
                                        double zpos,
                                        double energy,
                                        TVector3 track,
                                        std::vector<uint16_t> padlist,
                                        double chi2polarfit);

  public:
    // Class definition
    ClassDefOverride(R3BActafHit2Cluster, 1);
};
