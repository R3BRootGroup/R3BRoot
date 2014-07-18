// -------------------------------------------------------------------------
// -----                   R3BSTaRTraHitFinder source file                 -----
// -----                  Created 01/06/11  by N.Ashwood               -----
// -------------------------------------------------------------------------

/**  R3BSTaRTraHitFinder.h
 **/


#ifndef R3BSTARTRADIGIT_H
#define R3BSTARTRADIGIT_H

#include "FairTask.h"
#include "R3BSTaRTrackerDigitHit.h"

class TClonesArray;

class R3BSTaRTraDigit : public FairTask
{
	
public:
	
	/** Default constructor **/  
	R3BSTaRTraDigit();
	
	/** Destructor **/
	~R3BSTaRTraDigit();
	
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
	
	
	TClonesArray* fSTaRTrackerHitCA;
	TClonesArray* fSTaRTraHitCA;
	
	// Minimum energy requested in a crystal to be considered in a calorimeter Hit 
	Double_t fThreshold;
	// Experimental resolution @ 1 MeV  
	Double_t fTrackerResolution;

	
private:
	
	/** Private method ExpResSmearing **/
    Double_t ExpResSmearing(Double_t inputEnergy);

	/** Private method Addhit **/
	//R3BSTaRTrackerDigitHit* AddHit(Double_t ene,Int_t det,Int_t Stripfrt,Int_t Stripbck,Double_t time);
        R3BSTaRTrackerDigitHit* AddHit( Int_t det, Int_t chip, Int_t Side, Int_t Strip, Double_t energy, Double_t time);
	
	
	ClassDef(R3BSTaRTraDigit,1);
	
};




#endif 
