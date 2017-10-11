// -------------------------------------------------------------------------
// -----                  R3BStartrackerHit header file                -----
// -----               Created 01/09/10  by N.Ashwood              -----
// -------------------------------------------------------------------------




#ifndef R3BSTARTRACKERHIT_H
#define R3BSTARTRACKERHIT_H

#include "TObject.h"

#include "FairMultiLinkedData.h"


class R3BStartrackerHit : public FairMultiLinkedData 
{
	
public:
	
	/** Default constructor **/
	R3BStartrackerHit();

	
	R3BStartrackerHit(Double_t ene, Int_t det, Double_t x, Double_t y, Double_t z, Double_t px, Double_t py, Double_t pz, Double_t th, Double_t phi);
	//R3BStartrackerHit(Double_t ene, Int_t det);
	
	
	/** Copy constructor **/
	R3BStartrackerHit(const R3BStartrackerHit& hit) { *this = hit; };
	
	
	/** Destructor **/
	virtual ~R3BStartrackerHit();
	
	/** Accessors **/
	//Int_t GetNbOfTrackerHits()   const { return fNbOfTrackerHits; }
	Double_t GetEnergy()   const { return fEnergy; }
	Int_t GetDetector()   const { return fDetector; }
	Double_t GetXHit()   const { return fXHit; } 
	Double_t GetYHit()   const { return fYHit; } 
	Double_t GetZHit()   const { return fZHit; } 
    Double_t GetPx()   const { return fPx; } 
    Double_t GetPy()   const { return fPy; } 
    Double_t GetPz()   const { return fPz; } 
	Double_t GetThetaZero()   const { return fThetaZero; } 
	Double_t GetPhiZero()   const { return fPhiZero; }
	/** Modifiers **/
	//void SetNbOfTrackerHits(Int_t number){fNbOfTrackerHits = number;}
	void SetEnergy(Double_t ene){fEnergy = ene;}
	void SetDetector(Int_t det){fDetector = det;}
	void SetXHit(Double_t x){fXHit = x;}
	void SetYHit(Double_t y){fYHit = y;}
	void SetZHit(Double_t z){fZHit = z;}
    void SetPx(Double_t px){fPx = px;}
    void SetPy(Double_t py){fPy = py;}
    void SetPz(Double_t pz){fPz = pz;}
	void SetThetaZero(Double_t th){fThetaZero = th;}
	void GetPhiZero(Double_t phi){fPhiZero = phi;}


	/** Output to screen **/
	//virtual void Print(const Option_t* opt) const;
	
	
protected:
	
	//Basic Hit information
	//Int_t fNbOfTrackerHits; //number of Si hits contributing to the R3BStartrackerHit
	Double_t fEnergy;        //total energy deposited 
	Int_t fDetector; //Detector hit by event
	Double_t fXHit; // Position in x of hit in detector fdetector	
	Double_t fYHit; // Position in y of hit in detector fdetector	
	Double_t fZHit; // Position in z of hit in detector fdetector
	Double_t fPx; // Momentum in the x direction of particle
	Double_t fPy; // Momentum in the y direction of particle
	Double_t fPz; // Momentum in the z direction of particle
	Double_t fThetaZero; // Theta from (0,0,0) in detector fdetector	
	Double_t fPhiZero; // Phi from (0,0,0) in detector fdetector	

	ClassDef(R3BStartrackerHit,1)
	
};

#endif
