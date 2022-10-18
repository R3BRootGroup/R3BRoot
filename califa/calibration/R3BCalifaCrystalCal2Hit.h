/******************************************************************************
 *   Copyright (C) 2019 GSI Helmholtzzentrum für Schwerionenforschung GmbH    *
 *   Copyright (C) 2019 Members of R3B Collaboration                          *
 *                                                                            *
 *             This software is distributed under the terms of the            *
 *                 GNU General Public Licence (GPL) version 3,                *
 *                    copied verbatim in the file "LICENSE".                  *
 *                                                                            *
 * In applying this license GSI does not waive the privileges and immunities  *
 * granted to it by virtue of its status as an Intergovernmental Organization *
 * or submit itself to any jurisdiction.                                      *
 ******************************************************************************/

#ifndef R3BCALIFACRYSTALCAL2HIT_H
#define R3BCALIFACRYSTALCAL2HIT_H

#include "FairTask.h"
#include "R3BCalifaGeometry.h"
#include "R3BCalifaHitData.h"
#include "Rtypes.h"

#include <TVector3.h>
#include "TH2F.h"

class TClonesArray;
class R3BTGeoPar;
class R3BCalifaMappingPar;


class R3BCalifaCrystalCal2Hit : public FairTask
{

  public:
    /** Default constructor
     **/
    R3BCalifaCrystalCal2Hit();

    /** Destructor **/
    virtual ~R3BCalifaCrystalCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    /** Public method SelectGeometryVersion **/
    void SelectGeometryVersion(Int_t version);


    void SetCrystalThreshold(Double_t cryThresh)    {fCrystalThreshold = cryThresh;}

    void SetProtonThreshold(Double_t protonThresh)  {fProtonThreshold = protonThresh;}

    void SetGammaClusterThreshold(Double_t clusterThresh){fGammaClusterThreshold = clusterThresh;}
    void SetProtonClusterThreshold(Double_t clusterThresh){fProtonClusterThreshold = clusterThresh;}

    void SetSaturationEnergy(Double_t saturation) {fSaturation = saturation;}

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }


    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    void SetRandomization(Bool_t rand){fRand=rand;}

    void SetRandomizationFile(TString file) {

      fRandFile = file;
      fHistoFile = new TFile(fRandFile);

    }

    // Defines the maximum "opening" angle between two crystals inside a cluster
    void SetClusterWindow(Double_t window){fRoundWindow = window;}

    void IsSimulation(Bool_t simu) {fSimulation = simu;}

  private:


    TClonesArray* fCrystalCalData;
    TClonesArray* fCalifaHitData;
    R3BCalifaMappingPar* fMap_Par;

    R3BTGeoPar* fTargetGeoPar;
    R3BTGeoPar* fCalifaGeoPar;

    Bool_t fOnline;                   // Selector for online data storage
    Int_t fGeometryVersion;           // Selecting the geometry of the CALIFA calorimeter

    Double_t fCrystalThreshold;       // Minimum energy requested in a crystal to be included in a cluster
    Double_t fProtonClusterThreshold; // Minimum energy in a crystal to be considered as a proton cluster candidate
    Double_t fGammaClusterThreshold;  // Minimum energy in a crystal to be considered as a gamma cluster candidate
    Double_t fProtonThreshold;        // Defines the cut energy between proton and gamma readout

    Double_t fRoundWindow;        // Cluster window
    Bool_t fSimulation;           // Simulation flag
    Double_t fSaturation;         // Crystal Saturation

    R3BCalifaGeometry* fCalifaGeo;
    Bool_t fRand;                // Flag to set randomization procedure
    TString fRandFile;           // File with angular coverages for each crystal
    TFile *fHistoFile;
    TH2F **fAngularDistributions;

     /** Private method AddHit
     **
     ** Adds a CalifaHit to the HitCollection
     **/
    R3BCalifaHitData* AddHit(std::vector<Int_t> crystalList,
                             Double_t ene,
                             Double_t Nf,
                             Double_t Ns,
                             Double_t pAngle,
                             Double_t aAngle,
                             ULong64_t time,
                             Int_t clusterType);


    ClassDef(R3BCalifaCrystalCal2Hit, 2);
};

#endif /* R3BCALIFACRYSTALCAL2HIT_H */
