// -------------------------------------------------------------------------
// -----                      R3BCaloPoint header file                  -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------


/**  R3BCaloPoint.h
 **/


#ifndef R3BCALOPOINT_H
#define R3BCALOPOINT_H


#include "TObject.h"
#include "TVector3.h"

#include "FairMCPoint.h"


class R3BCaloPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  R3BCaloPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param posIn    Ccoordinates at entrance to active volume [cm]
   *@param posOut   Coordinates at exit of active volume [cm]
   *@param momIn    Momentum of track at entrance [GeV]
   *@param momOut   Momentum of track at exit [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  R3BCaloPoint(Int_t trackID, Int_t detID, Int_t volId, Int_t copy , Int_t ident,
	       TVector3 posIn,TVector3 posOut, TVector3 momIn, TVector3 momOut,
	       Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  R3BCaloPoint(const R3BCaloPoint&);

  R3BCaloPoint& operator=(const R3BCaloPoint&) { return *this; }


  /** Destructor **/
  virtual ~R3BCaloPoint();


  /** Accessors **/
  Int_t GetCrystalType()   const { return fCrystalType; }
  Int_t GetCrystalCopy()   const { return fCrystalCopy; }
  Int_t GetCrystalId()   const { return fCrystalId; }
  Double_t GetXIn()   const { return fX; }
  Double_t GetYIn()   const { return fY; }
  Double_t GetZIn()   const { return fZ; }
  Double_t GetXOut()  const { return fX_out; }
  Double_t GetYOut()  const { return fY_out; }
  Double_t GetZOut()  const { return fZ_out; }
  Double_t GetPxOut() const { return fPx_out; }
  Double_t GetPyOut() const { return fPy_out; }
  Double_t GetPzOut() const { return fPz_out; }
  void PositionIn(TVector3& pos)  { pos.SetXYZ(fX, fY, fZ); }
  void PositionOut(TVector3& pos) { pos.SetXYZ(fX_out,fY_out,fZ_out); }
  void MomentumOut(TVector3& mom) { mom.SetXYZ(fPx_out,fPy_out,fPz_out); }


  /** Point coordinates at given z from linear extrapolation **/
  Double_t GetX(Double_t z) const;
  Double_t GetY(Double_t z) const;


  /** Check for distance between in and out **/
  Bool_t IsUsable() const;


  /** Modifiers **/
  void SetPositionOut(TVector3 pos);
  void SetMomentumOut(TVector3 mom);

  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;



 protected:

  Double32_t fX_out,  fY_out,  fZ_out;
  Double32_t fPx_out, fPy_out, fPz_out;
  Int_t fCrystalType;
  Int_t fCrystalCopy;
  Int_t fCrystalId;

  ClassDef(R3BCaloPoint,1)

};



inline void R3BCaloPoint::SetPositionOut(TVector3 pos) {
  fX_out = pos.X();
  fY_out = pos.Y();
  fZ_out = pos.Z();
}


inline void R3BCaloPoint::SetMomentumOut(TVector3 mom) {
  fPx_out = mom.Px();
  fPy_out = mom.Py();
  fPz_out = mom.Pz();
}



#endif
