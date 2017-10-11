// -------------------------------------------------------------------------
// -----                   R3BSTaRTraHitFinder source file                -----
// -----                  Created 01/06/11  by H.Alvarez               -----
// -------------------------------------------------------------------------

/**  R3BSTaRTraHitFinder.h
 **/


#ifndef R3BTRAHITFINDER_H
#define R3BTRAHITFINDER_H

#include "FairTask.h"

#include "R3BSTaRTrackerHit.h"

class TClonesArray;

class R3BSTaRTraHitFinder : public FairTask
{
	
public:
	
	/** Default constructor **/  
	R3BSTaRTraHitFinder();
	
	/** Destructor **/
	~R3BSTaRTraHitFinder();

	/** Virtual method Exec **/
	virtual void Exec(Option_t* opt);

	/** Virtual method Reset **/
	virtual void Reset();
	
	/** Public method SetExperimentalResolution
	 **
	 ** Defines the experimental resolution of the tracker. 
	 **
	 **/
	void SetExperimentalResolution(Double_t trackerRes);
	
	/** Public method SetDetectionThreshold
	 **
	 ** Defines the minimum energy requested in a strip to be considered in a tracker Hit 
	 **
	 **/
	void SetDetectionThreshold(Double_t thresholdEne);
	
	
		
protected:

	/** Virtual method Init **/
	virtual InitStatus Init();
	
	/** Virtual method ReInit **/
	virtual InitStatus ReInit();
		
	/** Virtual method Finish **/
	virtual void Finish();
	
	
	TClonesArray* fTrackerHitCA;
	TClonesArray* fTraHitCA;

		
	// Minimum energy requested in a crystal to be considered in a calorimeter Hit 
	Double_t fThreshold;
	// Experimental resolution @ 1 MeV  
	Double_t fTrackerResolution;

	
private:
	/** Private method ExpResSmearing **/
       Double_t ExpResSmearing(Double_t inputEnergy);
	
       R3BSTaRTrackerHit* AddHit(UInt_t Nb,Double_t ene);
       //R3BSTaRTraHit* AddHit(UInt_t Nb,Double_t ene);

		
	ClassDef(R3BSTaRTraHitFinder,1);
	
};


#endif 
