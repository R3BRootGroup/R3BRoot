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

// ------------------------------------------------------------------------
// -----            R3BFootStripCal2Hit source file                   -----
// -----       Created 05/11/21 by J.L. Rodriguez-Sanchez             -----
// ------------------------------------------------------------------------

#ifndef R3BFootStripCal2Hit_H
#define R3BFootStripCal2Hit_H 1

#include "FairTask.h"

#include "R3BFootHitData.h"

#include "TVector3.h"
#include <Rtypes.h>

class TClonesArray;
class R3BFootMappingPar;

class R3BFootStripCal2Hit : public FairTask
{

  public:
    /** Default constructor **/
    R3BFootStripCal2Hit();

    /** Standard constructor **/
    R3BFootStripCal2Hit(const TString& name, Int_t iVerbose = 1);

    /** Destructor **/
    virtual ~R3BFootStripCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* option) override;

    /** Virtual method Reset **/
    virtual void Reset();

    // Fair specific
    /** Virtual method Init **/
    virtual InitStatus Init() override;

    /** Virtual method ReInit **/
    virtual InitStatus ReInit() override;

    /** Virtual method SetParContainers **/
    virtual void SetParContainers() override;

    /** Accessor for selecting online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

    /** Accessor for selecting max. number of clusters per ams detector **/
    void SetMaxNumClusters(Int_t max) { fMaxNumClusters = max; }

    /** Accessor to set up the threshold for the cluster energy sum **/
    void SetClusterEnergy(Float_t thsum) { fThSum = thsum; }

  private:
    void SetParameter();

    Double_t fPitch;
    Double_t fMiddle;
    Float_t fThSum;
    Int_t fMaxNumDet, fMaxNumClusters, fPolPar;
    Double_t fEnevsPosCorrPar[20][10];
    Double_t fDistTarget[20];
    Double_t fAngleTheta[20];
    Double_t fAnglePhi[20];
    Double_t fOffsetX[20];
    Double_t fOffsetY[20];
    TH1F* hssd[20];

    Int_t ClusterMult[20];           // Cluster multiplicity
    Double_t ClusterPos[20][100];    // Position of Cluster from Weighted Average
    Double_t ClusterESum[20][100];   // Sum of Energies in the Cluster
    Double_t Nu[20][100];            // Nu for Energy/Position correction
    Int_t ClusterNStrip[20][100];    // Number of Strips in Cluster
    Double_t ClusterI[20][100][200]; // Id Strip in Cluster
    Double_t ClusterE[20][100][200]; // Energy of Strip in Cluster

    R3BFootMappingPar* fMap_Par; // Parameter container with mapping
    TClonesArray* fFootCalData;  // Array with FOOT Cal-input data
    TClonesArray* fFootHitData;  // Array with FOOT Hit-output data

    Bool_t fOnline; // Don't store data for online
    Double_t* fChannelPeaks;

    // Private method AddHitData
    R3BFootHitData* AddHitData(Int_t detid, Int_t numhit, Double_t s, TVector3 master, Double_t energy_s, Int_t mulS);

  public:
    // Class definition
    ClassDefOverride(R3BFootStripCal2Hit, 1)
};

#endif /* R3BFootStripCal2Hit_H */
