// -------------------------------------------------------------------------
// -----              R3BAmsStripCalData header file                   -----
// -----             Created 29/05/18  by J.L. Rodriguez-Sanchez       -----
// -------------------------------------------------------------------------

/**  R3BAmsStripCalData.h
 **  A strip CalData is the representation of the 
 **  information obtained from each strip in the silicon detector.
 **/

#ifndef R3BAmsStripCalData_H
#define R3BAmsStripCalData_H

#include "TObject.h"
#include "FairMultiLinkedData.h"

class R3BAmsStripCalData : public FairMultiLinkedData 
{
public:

  /** Default constructor **/
  R3BAmsStripCalData();

  /** Constructor with arguments
   *@param fDetId    Detector unique identifier
   *@param fSideId   Side unique identifier
   *@param fStripId  Strip unique identifier
   *@param fEnergy   Total energy deposited on the strip ([GeV] in sim)
   **/
  R3BAmsStripCalData(Int_t detid, Int_t sideid, Int_t stripid, Double_t energy);

  /** Copy constructor **/
  R3BAmsStripCalData(const R3BAmsStripCalData&);

  R3BAmsStripCalData& operator=(const R3BAmsStripCalData&) { return *this; }

  /** Destructor **/
  virtual ~R3BAmsStripCalData();

  /** Accessors **/
  Int_t    GetDetId()      const { return fDetId; }
  Int_t    GetSideId()     const { return fSideId; }
  Int_t    GetStripId()    const { return fStripId; }
  Double_t GetEnergy()     const { return fEnergy; }

  /** Modifiers **/
  void SetDetId(Int_t detid)         { fDetId = detid; }
  void SetSideId(Int_t sideid)       { fSideId = sideid; }
  void SetStripId(Int_t stripid)     { fStripId = stripid; }
  void SetEnergy(Double32_t energy)  { fEnergy = energy;}

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:
  Int_t fDetId;        //detector unique identifier
  Int_t fSideId;       //side unique identifier
  Int_t fStripId;      //strip unique identifier
  Double_t fEnergy;  //total energy in the strip

  ClassDef(R3BAmsStripCalData,1)

};

#endif
