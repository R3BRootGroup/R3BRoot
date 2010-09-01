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
	
	// Parameter class
	//R3BCaloHitFinderPar* fCaloHitFinderPar;
	
private:
	
	
	/** Private method AddHit
     **
     ** Adds a CaloHit to the HitCollection
     **/
	R3BCaloHit* AddHit(UInt_t Nbcrystals,Double_t ene,Double_t pAngle,Double_t aAngle);
	
	
	ClassDef(R3BCaloHitFinder,1);
	
};


#endif 
