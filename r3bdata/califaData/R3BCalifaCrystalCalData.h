// -------------------------------------------------------------------------
// -----              R3BCalifaCrystalCalData header file              -----
// -----               Created 08/07/10  by H. Alvarez Pol             -----
// -----	     Last modification 23/12/16 by P.Cabanelas         -----
// -------------------------------------------------------------------------

/**  R3BCalifaCrystalCalData.h
 **  A calorimeter crystalCalData hit is the representation of the 
 **  information obtained from each crystal in the calorimeter.
 **/

#ifndef R3BCALIFACRYSTALCALDATA_H
#define R3BCALIFACRYSTALCALDATA_H

#include "TObject.h"
#include "FairMultiLinkedData.h"

class R3BCalifaCrystalCalData : public FairMultiLinkedData 
{
public:

  /** Default constructor **/
  R3BCalifaCrystalCalData();

  /** Constructor with arguments
   *@param fCrystalId   Crystal unique identifier
   *@param fEnergy      Total energy deposited on the crystal ([GeV] in sim)
   *@param fToT_Energy  Total energy deposited on the crystal from ToT ([GeV] in sim)
   *@param fTime        Time since event start [ns]
   **/
  R3BCalifaCrystalCalData(Int_t ident, Double_t energy, Double_t Nf, Double_t Ns, ULong64_t time, Double_t tot_energy=0);

  /** Copy constructor **/
  R3BCalifaCrystalCalData(const R3BCalifaCrystalCalData&);

  R3BCalifaCrystalCalData& operator=(const R3BCalifaCrystalCalData&) { return *this; }

  /** Destructor **/
  virtual ~R3BCalifaCrystalCalData();

  /** Accessors **/
  Int_t    GetCrystalId()     const { return fCrystalId; }
  Double_t GetEnergy()        const { return fEnergy; }
  Double_t GetToT_Energy()    const { return fToT_Energy; }
  Double_t GetNf()            const { return fNf; }
  Double_t GetNs()            const { return fNs; }
  ULong64_t GetTime()         const { return fTime; }
	
  Int_t    GetCrystalType()   const { return 0; }   //Backwards compatibility: not crashing macros
  Int_t    GetCrystalCopy()   const { return 0; }   //Backwards compatibility: not crashing macros

  /** Modifiers **/
  void SetCrystalId(Int_t ident)            { fCrystalId = ident; }
  void SetEnergy(Double32_t energy){fEnergy = energy;}
  void SetToT_Energy(Double32_t energy){fToT_Energy = energy;}
  void SetNf(Double32_t Nf){fNf = Nf;}
  void SetNs(Double32_t Ns){fNs = Ns;}
  void SetTime(ULong64_t time){fTime = time;}

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:
  Double32_t fEnergy;    //total energy in the crystal
  Double32_t fNf;        //total Nf in the crystal
  Double32_t fNs;        //total Nf in the crystal
  Double32_t fToT_Energy;//total energy in the crystal from ToT
  ULong64_t fTime;      //time of the interaction
  Int_t fCrystalId;      //crystal unique identifier

  ClassDef(R3BCalifaCrystalCalData,1)

};

#endif
