// -------------------------------------------------------------------------
// -----                  R3BCaloHitSim header file                    -----
// -----               Created  05/07/12  by P. Cabanelas              -----
// -------------------------------------------------------------------------


/**  R3BCaloHitSim.h
 **  A calorimeter hit sim is the representation of the information 
 **  reconstructed from a set of crystalHits in the calorimeter in simulation.
 **/


#ifndef R3BCALOHITSIM_H
#define R3BCALOHITSIM_H

#include "TObject.h"

#include "R3BCaloHit.h"


class R3BCaloHitSim : public R3BCaloHit 
{
	
public:
	
	/** Default constructor **/
	R3BCaloHitSim();
	
	/** Constructor with arguments
	 *@param fNbOfCrystalHits		Crystal unique identifier
	 *@param fEnergy			Total energy deposited 
	 *@param fTheta				Reconstructed theta
	 *@param fPhi				Reconstructed phi
	 *@param fEinc				Total incident energy
	 **/
	R3BCaloHitSim(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns,
			   Double_t theta, Double_t phi, Double_t einc);
	
	
	/** Copy constructor **/
	R3BCaloHitSim(const R3BCaloHitSim&);

	R3BCaloHitSim& operator=(const R3BCaloHitSim&) { return *this; }
	
	
	/** Destructor **/
	virtual ~R3BCaloHitSim();
	
	/** Accessors **/
	Double_t GetEinc()   const { return fEinc; }

	/** Modifiers **/
	void SetEinc(Double_t einc){fEinc = einc;}


	/** Output to screen **/
	virtual void Print(const Option_t* opt) const;
	
	
protected:
	
	//Basic Hit information
        Double_t fEinc;      // total incident energy
		
	
	ClassDef(R3BCaloHitSim,1)
	
};

#endif
