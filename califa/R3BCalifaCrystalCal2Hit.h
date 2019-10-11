// -----------------------------------------------------------------------------
// -----                R3BCalifaCrystalCal2Hit source file                -----
// -----                  Created 27/08/10  by H.Alvarez                   -----
// ----- Last modification: if you have to ask, you don't need to know     -----
// -----------------------------------------------------------------------------

/**  R3BCalifaCrystalCal2Hit.h
 **/


#ifndef R3BCALIFACRYSTALCAL2HIT_H
#define R3BCALIFACRYSTALCAL2HIT_H

#include "FairTask.h"
#include "R3BCalifaHitData.h"
#include "R3BCalifaHitDataSim.h"
#include "R3BCalifaGeometry.h"
//#include "R3BCalifaCrystalCal2HitPar.h"

class TClonesArray;
#include <TVector3.h>

class R3BCalifaCrystalCal2Hit : public FairTask
{
public:
  static int AngleToPetalId(TVector3& vec)
  {
    //internal [double] petal number, not corresponding to anything real
    //only relevant for 2019 beam times
    auto a=vec.Phi();
    if (a<0)
      {
	if (a<-2)
	  return 0;
	else
	  return 1;
      }
    else
      {
	if (a<2)
	  {
	    if (a<0.2)
	      return 2;
	    else
	      return 3;
	  }
	else
	  return 4;
      }
  }
  
  /** Default constructor **/
  R3BCalifaCrystalCal2Hit();

  /** Destructor **/
  ~R3BCalifaCrystalCal2Hit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  /** Virtual method Reset **/
  virtual void Reset();

  /** Public method SelectGeometryVersion
   **
   ** Defines the geometry
   *@param version  Integer parameter used to select the geometry:
   ** (see documentation /r3broot/cal/perlScripts/README))
   **/
  virtual void SelectGeometryVersion(Int_t version);

  /** Public method SetExperimentalResolution
   **
   ** Defines the experimental resolution of the crystals. The parameter sets the value
   ** at 1 MeV, and it scales according to and 1/sqrt(E) law.
   *@param crystalRes  Double parameter used to set the experimental resolution @ 1 MeV
   **/
  virtual void SetExperimentalResolution(Double_t crystalRes);

  /** Public method SetComponentResolution
   **
   ** Defines the experimental resolution of the CsI(Tl) components.
   *@param componentRes  Double parameter used to set the experimental resolution in MeV
   **/
  virtual void SetComponentResolution(Double_t componentRes);

  /** Public method SetPhoswichResolution
   **
   ** Defines the experimental resolution of the LaBr and LaCl crystals.
   *@param LaBr  Double parameter used to set the experimental resolution in % for LaBr
   *@param Lacl  Double parameter used to set the experimental resolution in % for LaCl
   **/
  virtual void SetPhoswichResolution(Double_t LaBr, Double_t LaCl);

  /** Public method SetDetectionThreshold
   **
   ** Defines the minimum energy requested in a crystal to be considered in a calorimeter Hit
   *@param thresholdEne  Double parameter used to set the threshold
   **/
  virtual void SetDetectionThreshold(Double_t thresholdEne);

  /** Public method SetDRThreshold
   **
   ** Defines the minimum energy requested in a crystal to be considered in a calorimeter Hit
   *@param thresholdEne  Double parameter used to set the threshold
   **/
  virtual void SetDRThreshold(Double_t DRthresholdEne);

  /** Public method SetAngularWindow
   **
   ** Sets the angular window open around the crystal with the largest energy
   ** to search for additional crystal hits and addback to the same cal hit
   **/

  // usability thing: select an algorithm and set the relevant parameters
  virtual void SetRectAlg(double xDeltaPolar=0.25, double xDeltaAzimuthal=0.25)
  {
    this->clusterAlg=RECT;
    this->fDeltaPolar=xDeltaPolar;
    this->fDeltaAzimuthal=xDeltaAzimuthal;
  }

  virtual void SetRoundAlg(double xDeltaAngleClust=0.25)
  {
    this->clusterAlg=ROUND;
    this->fDeltaAngleClust=xDeltaAngleClust;
  }
  
  virtual void SetRoundEnScaledAlg(double xDeltaAngleClust=0.25, double xenergyFactor=0)
  {
    this->clusterAlg=ROUND_SCALED;
    this->fDeltaAngleClust=xDeltaAngleClust;
    this->energyFactor=xenergyFactor;
  }
  
  virtual void SetConeAlg(double xDeltaAngleClust=0.25)
  {
    this->clusterAlg=CONE;
    this->fDeltaAngleClust=xDeltaAngleClust;
  }

  virtual void SetPetalAlg()
  {
    this->clusterAlg=PETAL;
  }

  /** Virtual method SetParContainers **/
  virtual void SetParContainers();

  // virtual void Finish(); -- was empty, just like the the inherited method. No need to
  //  clutter code by overriding it. 

  /** Accessor to select online mode **/
  void SetOnline(Bool_t option){fOnline=option;} 

 
protected:
  // the following is now protected, let the rest of the world use GetAngles
  /** method  GetAngles **/
  //  virtual void GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal);

  /** method  GetAngles **/
  //virtual void GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal, Double_t* rho);

  TVector3 GetAnglesVector(int id);


  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method ReInit **/
  virtual InitStatus ReInit()
  { // overriding behaviour from base class
    // someone probably has a reason for that. 
    return kSUCCESS;
  }


  TClonesArray* fCrystalHitCA{};
  TClonesArray* fCalifaHitCA{};

  //Don't store data for online
  Bool_t fOnline=false;
  // Selecting the geometry of the CALIFA calorimeter
  Int_t fGeometryVersion=444;
  
  // Minimum energy requested in a crystal to be considered in a calorimeter Hit
  Double_t fThreshold=0.;     //no threshold
  // Threshold for selecting gamma or proton branch in double reading channels
  Double_t fDRThreshold=15000; //in keV, for real data

  // simulation peak broadening:
  // (why is this in this class?)
  // Experimental resolution @ 1 MeV
  Double_t fCrystalResolution=0.; //perfect crystals
  // Experimental resolution for Nf and Ns
  Double_t fComponentResolution=0.;
  // Experimental resolution for LaBr
  Double_t fLaBrResolution=0.;
  // Experimental resolution for LaCl
  Double_t fLaClResolution=0.;
  // Flag for simulation
  Bool_t kSimulation=false;

  // Clustering algorithm selector
  enum
    {
      INVALID=0,
      RECT=1,
      ROUND=2,
      ROUND_SCALED=3,
      CONE=4,
      ALL=5,
      NONE=6,
      PETAL=7
    } clusterAlg;
  // RECT
  // Angular window (polar)
  Double_t fDeltaPolar{};
  // Angular window (azimuthal)
  Double_t fDeltaAzimuthal{};
  // ROUND, ROUND_SCALED, CONE:
  // Angular opening used for the clustering condition
  Double_t fDeltaAngleClust{};
  //ROUND_SCALED:
  double energyFactor{};

  // Parameter class
  //R3BCalifaCrystalCal2HitPar* fCalifaHitFinderPar;
  // some geometry pointer thing
  R3BCalifaGeometry *fGeo{};
  
  virtual void SmearAllCrystalHits();
  
  // decides if hit is in the cluster started by ref
  
  virtual bool Match(R3BCalifaCrystalCalData *ref, R3BCalifaCrystalCalData *hit); 

  /** Protected method ExpResSmearing
   **
   ** Smears the energy according to some Experimental Resolution distribution
   ** Very simple preliminary scheme where the Experimental Resolution
   ** is introduced as a gaus random distribution with a width given by the
   ** parameter fCrystalResolution(in % @ MeV). Scales according to 1/sqrt(E)
   **/
  Double_t ExpResSmearing(Double_t inputEnergy);

  /** Protected method CompSmearing
   **
   ** Smears the CsI(Tl) components Ns and Nf
   **/
  Double_t CompSmearing(Double_t inputComponent);

  /** Protected method CompSmearing
   **
   ** Smears the LaBr and LaCl
   **/
  Double_t PhoswichSmearing(Double_t inputEnergy, bool isLaBr);

  /** Protected method isPhoswich
   **
   ** Smears the CsI(Tl) components Ns and Nf
   **/
  bool IsPhoswich(Int_t crystalid);

  ClassDef(R3BCalifaCrystalCal2Hit,2);

};


#endif
