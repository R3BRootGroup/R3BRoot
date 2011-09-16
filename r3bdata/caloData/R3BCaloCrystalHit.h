// -------------------------------------------------------------------------
// -----                  R3BCaloCrystalHit header file                -----
// -----               Created 08/07/10  by H. Alvarez Pol             -----
// -----               Modified 05/09/11 by Enrico Fiori                  -----
// -------------------------------------------------------------------------

/**  R3BCaloCrystalHit.h
 **  A calorimeter crystal hit is the representation of the 
 **  information obtained from each crystal in the calorimeter.
 **/

#ifndef R3BCALOCRYSTALHIT_H
#define R3BCALOCRYSTALHIT_H

#include "TObject.h"
#include "FairMultiLinkedData.h"

class R3BCaloCrystalHit : public FairMultiLinkedData 
{
public:

  /** Default constructor **/
  R3BCaloCrystalHit();

  /** Constructor with arguments
   *@param fCrystalId		Crystal unique identifier
   *@param fCrystalType		Crystal type
   *@param fCrystalCopy     Crystal copy
   *@param fEnergy		total energy deposited on the crystal [GeV]
   *@param fTime      Time since event start [ns]
   **/
	R3BCaloCrystalHit(Int_t type, Int_t copy, Int_t ident,
			  Double_t energy, Double_t time, Double_t theta, Double_t phi, Double_t rho);

  /** Copy constructor **/
  R3BCaloCrystalHit(const R3BCaloCrystalHit& hit) { *this = hit; };

  /** Destructor **/
  virtual ~R3BCaloCrystalHit();

  /** Accessors **/
  Double_t GetCrystalType()   const { return fCrystalType; }
  Double_t GetCrystalCopy()   const { return fCrystalCopy; }
  Double_t GetCrystalId()   const { return fCrystalId; }
  Double_t GetEnergy()   const { return fEnergy; }
  Double_t GetTime()  const { return fTime; }
	
  /** Modifiers **/
  void SetCrystalType(Int_t type){fCrystalType = type;}
  void SetCrystalCopy(Int_t copy){fCrystalCopy = copy;}
  void SetCrystalId(Int_t ident){fCrystalId = ident;}
  void SetEnergy(Double32_t energy){fEnergy = energy;}
  void AddMoreEnergy(Double32_t moreEnergy){fEnergy += moreEnergy;}
  void SetTime(Double32_t time){fTime = time;}

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:

  Double32_t fEnergy;  //total energy in the crystal
  Double32_t fTime;    //time of the interaction
  Int_t fCrystalType;  //crystal type
  Int_t fCrystalCopy;  //crystal copy
  Int_t fCrystalId;    //crystal unique identifier
  Double32_t fCrystalTheta;  // Crystal theta EF 05/09/11
  Double32_t fCrystalPhi;    // Crystal phi   EF 05/09/11
  Double32_t fCrystalRho;    // Crystal rho   EF 05/09/11


  ClassDef(R3BCaloCrystalHit,1)

};

#endif
