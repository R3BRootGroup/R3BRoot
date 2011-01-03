// -------------------------------------------------------------------------
// -----                   R3BCaloHitFinder source file                -----
// -----                  Created 27/08/10  by H.Alvarez               -----
// -----			Last modification 27/08/10 by H.Alvarez			   -----
// -------------------------------------------------------------------------

/**  R3BCaloHitFinder.h
 **/


#ifndef R3BCALOHITFINDER_H
#define R3BCALOHITFINDER_H

#include "FairTask.h"
#include "R3BCaloHit.h"

class TClonesArray;

class R3BCaloHitFinder : public FairTask
{
	
public:
	
	/** Default constructor **/  
	R3BCaloHitFinder();
	
	/** Destructor **/
	~R3BCaloHitFinder();
	
	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	/** Virtual method Reset **/
	virtual void Reset();
	
	/** Public method  GetAngles **/
	void GetAngles(Int_t iD, Double_t* polar, Double_t* azimuthal);
	
	/** Public method SelectGeometryVersion
	 **
	 ** Defines the geometry 
	 *@param version  Integer parameter used to select the geometry: 
	 **                             0 CALIFA V5 (old), 1 CALIFA V7.05, 2 USER DEFINED 
	 **/
	void SelectGeometryVersion(Int_t version);
	
	/** Public method SetExperimentalResolution
	 **
	 ** Defines the experimental resolution of the crystals. The parameter sets the value
	 ** at 1 MeV, and it scales according to and 1/sqrt(E) law.
	 *@param crystalRes  Double parameter used to set the experimental resolution @ 1 MeV 
	 **/
	void SetExperimentalResolution(Double_t crystalRes);
	
	/** Public method SetDetectionThreshold
	 **
	 ** Defines the minimum energy requested in a crystal to be considered in a calorimeter Hit 
	 *@param thresholdEne  Double parameter used to set the threshold 
	 **/
	void SetDetectionThreshold(Double_t thresholdEne);
	
	
		
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
	TClonesArray* fCaloHitCA;
	
	// Selecting the geometry of the CALIFA calorimeter
	Int_t fGeometryVersion;
	// Minimum energy requested in a crystal to be considered in a calorimeter Hit 
	Double_t fThreshold;
	// Experimental resolution @ 1 MeV  
	Double_t fCrystalResolution;

	// Parameter class
	//R3BCaloHitFinderPar* fCaloHitFinderPar;
	
private:
	
	/** Private method ExpResSmearing
     **
     ** Smears the energy according to some Experimental Resolution distribution 
	 ** Very simple preliminary scheme where the Experimental Resolution 
	 ** is introduced as a gaus random distribution with a width given by the 
	 ** parameter fCrystalResolution(in % @ MeV). Scales according to 1/sqrt(E)
	 **/
    Double_t ExpResSmearing(Double_t inputEnergy);
	
	/** Private method AddHit
     **
     ** Adds a CaloHit to the HitCollection
     **/
	R3BCaloHit* AddHit(UInt_t Nbcrystals,Double_t ene,Double_t pAngle,Double_t aAngle);
	
	
	ClassDef(R3BCaloHitFinder,1);
	
};


#endif 
