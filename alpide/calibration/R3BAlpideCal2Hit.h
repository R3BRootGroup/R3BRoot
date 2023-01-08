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

// -------------------------------------------------------------
// -----               R3BAlpideCal2Hit                    -----
// -----     Created 22/03/22 by J.L. Rodriguez-Sanchez    -----
// -------------------------------------------------------------

#ifndef R3BAlpideCal2Hit_H
#define R3BAlpideCal2Hit_H 1

#include "FairTask.h"
#include <Rtypes.h>

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
    virtual ~R3BAlpideCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option) override;

    /** Virtual method Reset **/
    virtual void Reset();

    virtual void SetParContainers() override;

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init() override;

    /** Virtual method ReInit **/
    virtual InitStatus ReInit() override;

    // Method to setup online mode
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    void SetParameter();
    void FindClusters();
    Double_t fPixelSize;
    Int_t fNbSensors;
    Bool_t fOnline; // Don't store data for online

    TVector3 fTargetPos;
    TVector3 fAlpidePos;
    TVector3 fAlpidetoTargetPos;

    R3BTGeoPar* fTargetGeoPar;
    R3BTGeoPar* fAlpideGeoPar;
    Int_t fGeoversion;
    R3BAlpideGeometry* fAlpideGeo;
    R3BAlpideMappingPar* fMap_Par; /**< Parameter container. >*/
    TClonesArray* fAlpideCalData;  // Array with Alpide Cal input data
    TClonesArray* fAlpideHitData;  // Array with Alpide Hit output data
    TClonesArray* fAlpideCluster;  // Array with clusters
                                   // TClonesArray* fAlpidePixel;

    // Private method AddHitData
    R3BAlpideHitData* AddHitData(UShort_t senId, UInt_t clustersize, Double_t x, Double_t y);

  public:
    // Class definition
    ClassDefOverride(R3BAlpideCal2Hit, 1)
};

#endif /*  R3BAlpideCal2Hit_H */
