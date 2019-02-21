// -------------------------------------------------------------------------
// -----              R3BAmsHitData header file                        -----
// -----             Created 01/06/18  by J.L. Rodriguez-Sanchez       -----
// -------------------------------------------------------------------------

/**  R3BAmsHitData.h
 **  A HitData is the representation of the 
 **  information obtained from each ams detector.
 **/

#ifndef R3BAmsHitData_H
#define R3BAmsHitData_H

#include "TObject.h"
#include "FairMultiLinkedData.h"

class R3BAmsHitData : public FairMultiLinkedData 
{
public:

  /** Default constructor **/
  R3BAmsHitData();

  /** Constructor with arguments
   *@param fDetId     Detector unique identifier
   *@param fNumHit    Number of hits
   *@param fX         Position X [mm]
   *@param fY         Position Y [mm]
   *@param fTheta     Angle theta [rad]
   *@param fPhi       Angle Phi [rad]
   *@param fEnergyX   Total energy deposited in X direction by hit ([GeV] in sim)
   *@param fEnergyY   Total energy deposited in Y direction by hit ([GeV] in sim)
   **/
  R3BAmsHitData(Int_t detid, Int_t numhit, Double_t x, Double_t y, Double_t theta, Double_t phi, Double_t energy_x, Double_t energy_y);

  /** Copy constructor **/
  R3BAmsHitData(const R3BAmsHitData&);

  R3BAmsHitData& operator=(const R3BAmsHitData&) { return *this; }

  /** Destructor **/
  virtual ~R3BAmsHitData();

  /** Accessors **/
  Int_t    GetDetId()      const { return fDetId;  }
  Int_t    GetNumHit()     const { return fNumHit; }
  Double_t    GetX()       const { return fX;      }
  Double_t    GetY()       const { return fY;      }
  Double_t GetTheta()      const { return fTheta;  }
  Double_t GetPhi()        const { return fPhi;    }
  Double_t GetEnergyX()    const { return fEnergyX;}
  Double_t GetEnergyY()    const { return fEnergyY;}

  /** Modifiers **/
  void SetDetId(Int_t detid)       { fDetId = detid;   }
  void SetNumHit(Int_t numhit)     { fNumHit = numhit; }
  void SetX(Double_t x)            { fX = x;           }
  void SetY(Double_t y)            { fY = y;           }
  void SetTheta(Double_t theta)    { fTheta = theta;   }
  void SetPhi(Double_t phi)        { fPhi = phi;       }
  void SetEnergyX(Double_t energy) { fEnergyX = energy;}
  void SetEnergyY(Double_t energy) { fEnergyY = energy;}

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;

protected:
  Int_t fDetId;
  Int_t fNumHit;    
  Double_t fX, fY, fTheta, fPhi;  
  Double_t fEnergyX,fEnergyY; 

  ClassDef(R3BAmsHitData,1)

};

#endif
