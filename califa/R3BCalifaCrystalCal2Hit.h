// -----------------------------------------------------------------------------
// -----                R3BCalifaCrystalCal2Hit source file                -----
// -----                  Created 27/08/10  by H.Alvarez                   -----
// -----                Last modification 19/12/16 by P.Cabanelas          -----
// -----------------------------------------------------------------------------

/**  R3BCalifaCrystalCal2Hit.h
 **/


#ifndef R3BCALIFACRYSTALCAL2HIT_H
#define R3BCALIFACRYSTALCAL2HIT_H

#include "FairTask.h"
#include "R3BCalifaHitData.h"
#include "R3BCalifaHitDataSim.h"
//#include "R3BCalifaCrystalCal2HitPar.h"

class TClonesArray;

class R3BCalifaCrystalCal2Hit : public FairTask
{

  public:

    /** Default constructor **/
    R3BCalifaCrystalCal2Hit();

    /** Destructor **/
    ~R3BCalifaCrystalCal2Hit();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    /** Public method  GetAngles **/
    void GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal);

    /** Public method  GetAngles **/
    void GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal, Double_t* rho);

    /** Public method SelectGeometryVersion
     **
     ** Defines the geometry
     *@param version  Integer parameter used to select the geometry:
     ** (see documentation /r3broot/cal/perlScripts/README))
     **/
    void SelectGeometryVersion(Int_t version);

    /** Public method SetExperimentalResolution
     **
     ** Defines the experimental resolution of the crystals. The parameter sets the value
     ** at 1 MeV, and it scales according to and 1/sqrt(E) law.
     *@param crystalRes  Double parameter used to set the experimental resolution @ 1 MeV
     **/
    void SetExperimentalResolution(Double_t crystalRes);

    /** Public method SetComponentResolution
     **
     ** Defines the experimental resolution of the CsI(Tl) components.
     *@param componentRes  Double parameter used to set the experimental resolution in MeV
     **/
    void SetComponentResolution(Double_t componentRes);

    /** Public method SetPhoswichResolution
     **
     ** Defines the experimental resolution of the LaBr and LaCl crystals.
     *@param LaBr  Double parameter used to set the experimental resolution in % for LaBr
     *@param Lacl  Double parameter used to set the experimental resolution in % for LaCl
     **/
    void SetPhoswichResolution(Double_t LaBr, Double_t LaCl);
    /** Public method SetDetectionThreshold
     **
     ** Defines the minimum energy requested in a crystal to be considered in a calorimeter Hit
     *@param thresholdEne  Double parameter used to set the threshold
     **/
    void SetDetectionThreshold(Double_t thresholdEne);

    /** Public method SetAngularWindow
     **
     ** Sets the angular window open around the crystal with the largest energy
     ** to search for additional crystal hits and addback to the same cal hit
     **/
    void SetAngularWindow(Double_t deltaPolar, Double_t deltaAzimuthal, Double_t DeltaAngleClust=0.);

    /** Public method SetClusteringAlgorithm
     **
     ** Select the clustering algorithm to be used and set the parameter to be used by one of them
     //  1  ->  square window
     //  2  ->  round window
     //  3  ->  advanced round window with opening proportional to the
     //         energy of the hit, need ParCluster1
     //  4  ->  advanced round window with opening proportional to the
     //         energy of the two hit, need ParCluster1 NOT IMPLEMENTED YET!
     **
     **/
    void SetClusteringAlgorithm(Int_t ClusteringAlgorithmSelector, Double_t ParCluster1) ;





  protected:


    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    /** Virtual method SetParContainers **/
    virtual void SetParContainers();

    /** Virtual method Finish **/
    virtual void Finish();


    TClonesArray* fCrystalHitCA;
    TClonesArray* fCalifaHitCA;

    // Selecting the geometry of the CALIFA calorimeter
    Int_t fGeometryVersion;
    // Minimum energy requested in a crystal to be considered in a calorimeter Hit
    Double_t fThreshold;
    // Experimental resolution @ 1 MeV
    Double_t fCrystalResolution;
    // Experimental resolution for Nf and Ns
    Double_t fComponentResolution;
    // Experimental resolution for LaBr
    Double_t fLaBrResolution;
    // Experimental resolution for LaCl
    Double_t fLaClResolution;
    // Angular window (polar)
    Double_t fDeltaPolar;
    // Angular window (azimuthal)
    Double_t fDeltaAzimuthal;
    // Angular opening used for the clustering condition
    Double_t fDeltaAngleClust;
    // Clustering algorithm selector
    Int_t fClusteringAlgorithmSelector;
    // Clustering parameter 1
    Double_t fParCluster1;
    // Flag for simulation
    Bool_t kSimulation;

    // Parameter class
    //R3BCalifaCrystalCal2HitPar* fCalifaHitFinderPar;

  private:

    UInt_t nEvents;

    /** Private method ExpResSmearing
    **
    ** Smears the energy according to some Experimental Resolution distribution
     ** Very simple preliminary scheme where the Experimental Resolution
     ** is introduced as a gaus random distribution with a width given by the
     ** parameter fCrystalResolution(in % @ MeV). Scales according to 1/sqrt(E)
     **/
    Double_t ExpResSmearing(Double_t inputEnergy);


     /** Private method CompSmearing
    **
    ** Smears the CsI(Tl) components Ns and Nf
    **/
    Double_t CompSmearing(Double_t inputComponent);

    /** Private method CompSmearing
    **
    ** Smears the LaBr and LaCl
    **/
    Double_t PhoswichSmearing(Double_t inputEnergy, bool isLaBr);


    /** Private method isPhoswich
    **
    ** Smears the CsI(Tl) components Ns and Nf
    **/
    bool isPhoswich(Int_t crystalid);

   /** Private method AddHit
    **
    ** Adds a CalifaHit to the HitCollection
    **/
    R3BCalifaHitData* AddHit(UInt_t Nbcrystals,Double_t ene, Double_t Nf, Double_t Ns, Double_t pAngle,Double_t aAngle, ULong64_t time);

    /** Private method AddHitSim
    **
    ** Adds a CalifaHitDataSim to the HitSimCollection
    **/
    R3BCalifaHitDataSim* AddHitSim(UInt_t Nbcrystals,Double_t ene, Double_t Nf, Double_t Ns,Double_t pAngle,Double_t aAngle, Double_t einc);


    ClassDef(R3BCalifaCrystalCal2Hit,1);

};


#endif
