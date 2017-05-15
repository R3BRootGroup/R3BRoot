// -------------------------------------------------------------------------
// -----               R3BCalifaHitDataSim header file                 -----
// -----               Created 05/07/12 by P.Cabanelas                 -----
// -----               Modified 23/12/16 by P.Cabanelas                -----
// -------------------------------------------------------------------------


/**  R3BCalifaHitDataSim.h
 **  A califaHitData sim is the representation of the information 
 **  reconstructed from a set of crystalHits in the calorimeter in simulation.
 **/


#ifndef R3BCALIFAHITDATASIM_H
#define R3BCALIFAHITDATASIM_H

#include "TObject.h"

#include "R3BCalifaHitData.h"


class R3BCalifaHitDataSim : public R3BCalifaHitData 
{
	
public:
	
	/** Default constructor **/
	R3BCalifaHitDataSim();
	
	/** Constructor with arguments
	 *@param fNbOfCrystalHits		Crystal unique identifier
	 *@param fEnergy			Total energy deposited 
	 *@param fTheta				Reconstructed theta
	 *@param fPhi				Reconstructed phi
	 *@param fEinc				Total incident energy
	 **/
	R3BCalifaHitDataSim(UInt_t Nb, Double_t ene, Double_t nf, Double_t ns,
			   Double_t theta, Double_t phi, Double_t einc);
	
	
	/** Copy constructor **/
	R3BCalifaHitDataSim(const R3BCalifaHitDataSim&);

	R3BCalifaHitDataSim& operator=(const R3BCalifaHitDataSim&) { return *this; }
	
	
	/** Destructor **/
	virtual ~R3BCalifaHitDataSim();
	
	/** Accessors **/
	Double_t GetEinc()   const { return fEinc; }

	/** Modifiers **/
	void SetEinc(Double_t einc){fEinc = einc;}


	/** Output to screen **/
	virtual void Print(const Option_t* opt) const;
	
	
protected:
	
	//Basic Hit information
        Double_t fEinc;      // total incident energy
		
	
	ClassDef(R3BCalifaHitDataSim,2)
	
};

#endif
