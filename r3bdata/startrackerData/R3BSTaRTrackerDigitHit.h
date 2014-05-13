// -------------------------------------------------------------------------
// -----                  R3BSTaRTrackerDigitHit header file           -----
// -----               Created 28/04/14  by M.Labiche                  -----
// -------------------------------------------------------------------------




#ifndef R3BSTARTRACKERDIGITHIT_H
#define R3BSTARTRACKERDIGITHIT_H

#include "TObject.h"

#include "FairMultiLinkedData.h"


class R3BSTaRTrackerDigitHit : public FairMultiLinkedData 
{
	
public:
	
	/** Default constructor **/
	R3BSTaRTrackerDigitHit();

	
	R3BSTaRTrackerDigitHit(Double_t ene, Int_t det, Int_t Stripfrt, Int_t Stripbck, Double_t time);
	
	
	/** Copy constructor **/
	R3BSTaRTrackerDigitHit(const R3BSTaRTrackerDigitHit& hit) { *this = hit; };
	
	
	/** Destructor **/
	virtual ~R3BSTaRTrackerDigitHit();
	
	/** Accessors **/
	//Int_t GetNbOfTrackerHits()   const { return fNbOfTrackerHits; }
	Double_t GetEnergy()   const { return fEnergy; }
	Double_t GetTime()   const { return fTime; }
	Int_t GetDetector()   const { return fDetector; }
	Double_t GetStripfrt()   const { return fStripfrt; } 
	Double_t GetStripbck()   const { return fStripbck; } 
	 
	/** Modifiers **/
	//void SetNbOfTrackerHits(Int_t number){fNbOfTrackerHits = number;}
	void SetEnergy(Double_t ene){fEnergy = ene;}
	void SetDetector(Int_t det){fDetector = det;}
	void SetStripfrt(Int_t Stripfrt){fStripfrt = Stripfrt;}
	void SetStripbck(Int_t Stripbck){fStripbck = Stripbck;}
       


	/** Output to screen **/
	//virtual void Print(const Option_t* opt) const;
	
	
protected:
	
	//Basic Hit information
	//Int_t fNbOfTrackerHits; //number of Si hits contributing to the R3BSTaRTrackerHit
	Double_t fEnergy;        //total energy deposited 
	Double_t fTime;        // global time when hit occur 
	Int_t fDetector; //Detector hit by event
	Double_t fStripfrt; // front strip hit in detector fdetector	
	Double_t fStripbck; // back strip hit in detector fdetector	

	ClassDef(R3BSTaRTrackerDigitHit,1)
	
};

#endif
