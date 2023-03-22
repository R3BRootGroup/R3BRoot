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

using namespace std;

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

    void SetExpId(Int_t exp) { fExpId = exp; }

  private:
    void SetParameter();


    TF1 *fZ3;
    TF1 *fZ4;
    TF1 *fZ5;
    TF1 *fZ6;
    TF1 *fZ7;
    TF1 *fZ3NoOff;
    TF1 *fZ4NoOff;
    TF1 *fZ5NoOff;
    TF1 *fZ6NoOff;
    TF1 *fZ7NoOff;

    TF1 *fZ4_0;
    TF1 *fZ5_0;
    TF1 *fZ6_0;
    TF1 *fZ7_0;
    TF1 *fZ8_0;
    TF1 *fZ9_0;
    TF1 *fZ4NoOff_0;
    TF1 *fZ5NoOff_0;
    TF1 *fZ6NoOff_0;
    TF1 *fZ7NoOff_0;
    TF1 *fZ8NoOff_0;
    TF1 *fZ9NoOff_0;

    TF1 *fZ4_1;
    TF1 *fZ5_1;
    TF1 *fZ6_1;
    TF1 *fZ7_1;
    TF1 *fZ8_1;
    TF1 *fZ9_1;
    TF1 *fZ4NoOff_1;
    TF1 *fZ5NoOff_1;
    TF1 *fZ6NoOff_1;
    TF1 *fZ7NoOff_1;
    TF1 *fZ8NoOff_1;
    TF1 *fZ9NoOff_1;

    TF1 *fZ4_14;
    TF1 *fZ5_14;
    TF1 *fZ6_14;
    TF1 *fZ7_14;
    TF1 *fZ8_14;
    TF1 *fZ9_14;
    TF1 *fZ4NoOff_14;
    TF1 *fZ5NoOff_14;
    TF1 *fZ6NoOff_14;
    TF1 *fZ7NoOff_14;
    TF1 *fZ8NoOff_14;
    TF1 *fZ9NoOff_14;

    TF1 *fZ4_15;
    TF1 *fZ5_15;
    TF1 *fZ6_15;
    TF1 *fZ7_15;
    TF1 *fZ8_15;
    TF1 *fZ9_15;
    TF1 *fZ4NoOff_15;
    TF1 *fZ5NoOff_15;
    TF1 *fZ6NoOff_15;
    TF1 *fZ7NoOff_15;
    TF1 *fZ8NoOff_15;
    TF1 *fZ9NoOff_15;
    

    Double_t EtaCorr_Lagni(TF1* ffZ3,
                           TF1* ffZ4,
                           TF1* ffZ5,
                           TF1* ffZ6,
                           TF1* ffZ7,
                           TF1* ffZ3NoOff,
                           TF1* ffZ4NoOff,
                           TF1* ffZ5NoOff,
                           TF1* ffZ6NoOff,
                           TF1* ffZ7NoOff,
                           Double_t fEnergy,
                           Double_t fEta);

 Double_t EtaCorr_Barriere(TF1* ffZ4,
                           TF1* ffZ5,
                           TF1* ffZ6,
                           TF1* ffZ7,
                           TF1* ffZ8,
			   TF1* ffZ9,
                           TF1* ffZ4NoOff,
                           TF1* ffZ5NoOff,
                           TF1* ffZ6NoOff,
                           TF1* ffZ7NoOff,
                           TF1* ffZ8NoOff,
			   TF1* ffZ9NoOff,
                           Double_t fEnergy,
                           Double_t fEta);



    Double_t fPitch;
    Double_t fMiddle;
    Float_t fThSum;
    Int_t fMaxNumDet, fMaxNumClusters, fExpId;
    vector<Double_t> fDistTarget;
    vector<Double_t> fAngleTheta;
    vector<Double_t> fAnglePhi;
    vector<Double_t> fOffsetX;
    vector<Double_t> fOffsetY;
    vector<TH1F*> hssd;

    vector<Int_t> ClusterMult;           // Cluster multiplicity
    vector<vector<Double_t>> ClusterPos;    // Position of Cluster from Weighted Average
    vector<vector<Double_t>> ClusterESum;   // Sum of Energies in the Cluster
    vector<vector<Double_t>> Eta;            // Eta for Energy/Position correction
    vector<vector<Int_t>> ClusterNStrip;    // Number of Strips in Cluster
    vector<vector<vector<Double_t>>> ClusterI; // Id Strip in Cluster
    vector<vector<vector<Double_t>>> ClusterE;; // Energy of Strip in Cluster


    R3BFootMappingPar* fMap_Par; // Parameter container with mapping
    TClonesArray* fFootCalData;  // Array with FOOT Cal-input data
    TClonesArray* fFootHitData;  // Array with FOOT Hit-output data

    Bool_t fOnline; // Don't store data for online
    Double_t* fChannelPeaks;

    // Private method AddHitData
//    R3BFootHitData* AddHitData(Int_t detid, Int_t numhit, Double_t s, TVector3 master, Double_t energy_s, Int_t mulS);
    R3BFootHitData* AddHitData(Int_t detid, Int_t numhit, Double_t s, Double_t eta, TVector3 master, Double_t energy_s, Int_t mulS);

  public:
    // Class definition
    ClassDefOverride(R3BFootStripCal2Hit, 1)
};

#endif /* R3BFootStripCal2Hit_H */
