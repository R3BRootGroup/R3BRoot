// -------------------------------------------------------------------------
// -----                  R3BCaloHit header file                       -----
// -----               Created  26/08/10  by H. Alvarez Pol            -----
// -----               Modified 15/09/11  by Enrico Fiori              ----- 
// -------------------------------------------------------------------------


/**  R3BCaloHit.h
 **  A calorimeter hit is the representation of the information 
 **  reconstructed from a set of crystalHits in the calorimeter.
 **/


#ifndef R3BCALOHIT_H
#define R3BCALOHIT_H

#include "TObject.h"

#include "FairMultiLinkedData.h"


class R3BCaloHit : public FairMultiLinkedData 
{
	
public:
	
	/** Default constructor **/
	R3BCaloHit();
	
	/** Constructor with arguments
	 *@param fNbOfCrystalHits		Crystal unique identifier
	 *@param fEnergy				Total energy deposited 
	 *@param fTheta					Reconstructed theta
	 *@param fPhi					Reconstructed phi
	 **/
	R3BCaloHit(UInt_t Nb, Double_t ene,
			   Double_t theta, Double_t phi);
	
	
	/** Copy constructor **/
	R3BCaloHit(const R3BCaloHit& hit) { *this = hit; };
	
	
	/** Destructor **/
	virtual ~R3BCaloHit();
	
	/** Accessors **/
	UInt_t GetNbOfCrystalHits()   const { return fNbOfCrystalHits; }
	Double_t GetEnergy()   const { return fEnergy; }
	Double_t GetTheta()   const { return fTheta; }
	Double_t GetPhi()   const { return fPhi; }

	/** Modifiers **/
	void SetNbOfCrystalHits(UInt_t number){fNbOfCrystalHits = number;}
	void SetEnergy(Double_t ene){fEnergy = ene;}
	void SetTheta(Double_t theta){fTheta = theta;}
	void SetPhi(Double_t phi){fPhi = phi;}


	/** Output to screen **/
	virtual void Print(const Option_t* opt) const;
	
	
protected:
	
	//Basic Hit information
	UInt_t fNbOfCrystalHits; //number of crystals contribuying to the R3BCaloHit
	Double_t fEnergy;        //total energy deposited 
	Double_t fTheta;         //reconstructed theta
	Double_t fPhi;           //reconstructed phi
		
	
	ClassDef(R3BCaloHit,1)
	
};

#endif
