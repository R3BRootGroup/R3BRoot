// -------------------------------------------------------------------------
// -----                  R3BCalCrystalHit header file                -----
// -----               Created 13/06/12  by Pablo Cabanelas            -----
// -------------------------------------------------------------------------

/**  R3BCalCrystalHit.h
 **  A cal crystal hit is the representation of the 
 **  information obtained from each crystal in the DH-CrystalBall.
 **/

#ifndef R3BCALCRYSTALHIT_H
#define R3BCALCRYSTALHIT_H

#include "TObject.h"
#include "FairMultiLinkedData.h"

class R3BCalCrystalHit : public FairMultiLinkedData 
{
public:

  /** Default constructor **/
  R3BCalCrystalHit();

  /** Constructor with arguments
   *@param fCrystalNb      Crystal number
   *@param fEnergy	   total energy deposited on the crystal [GeV]
   *@param fTime           Time since event start [ns]
   **/
  R3BCalCrystalHit(Int_t crysnb, Double_t energy, Double_t time);

  /** Copy constructor **/
  R3BCalCrystalHit(const R3BCalCrystalHit& hit) { *this = hit; };

  /** Destructor **/
  virtual ~R3BCalCrystalHit();

  /** Accessors **/
  Int_t    GetCrystalNumber() const { return fCrystalNb; }
  Double_t GetEnergy()        const { return fEnergy; }
  Double_t GetTime()          const { return fTime; }
	
  /** Modifiers **/
  void SetCrystalNumber(Int_t crysnb)       { fCrystalNb = crysnb; }
  void SetEnergy(Double32_t energy)         { fEnergy = energy; }
  void SetTime(Double32_t time)             { fTime = time; }

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:

  Double32_t fEnergy;    //total energy in the crystal
  Int_t fCrystalNb;      //crystal number (1-162, number of crystals in the CB)
  Double32_t fTime;      //time of the interaction

  ClassDef(R3BCalCrystalHit,1)

};

#endif
