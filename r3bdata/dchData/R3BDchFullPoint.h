// -------------------------------------------------------------------------
// -----                      R3BDchFullPoint header file                  -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------


/**  R3BDchFullPoint.h
 **/


#ifndef R3BDCHFULLPOINT_H
#define R3BDCHFULLPOINT_H


#include "TObject.h"
#include "TVector3.h"

#include "FairMCPoint.h"


class R3BDchFullPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  R3BDchFullPoint();


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
  R3BDchFullPoint(Int_t trackId, Int_t module, Int_t layer, Int_t cell,
	      TVector3 pos, TVector3 lpos, TVector3 mom, TVector3 lmom,
	      Double_t tof, Double_t length, Double_t eLoss);


  /** Copy constructor **/
  R3BDchFullPoint(const R3BDchFullPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~R3BDchFullPoint();


  /** Accessors **/
  Double_t GetX()   const { return fX; }
  Double_t GetY()   const { return fY; }
  Double_t GetZ()   const { return fZ; }
  Double_t GetLocalX()  const { return fLocalX; }
  Double_t GetLocalY()  const { return fLocalY; }
  Double_t GetLocalZ()  const { return fLocalZ; }

  Double_t GetPx() const { return fPx; }
  Double_t GetPy() const { return fPy; }
  Double_t GetPz() const { return fPz; }
  Double_t GetLocalPx() const { return fLocalPx; }
  Double_t GetLocalPy() const { return fLocalPy; }
  Double_t GetLocalPz() const { return fLocalPz; }

  void Position(TVector3& pos)  { pos.SetXYZ(fX, fY, fZ); }
  void Momentum(TVector3& mom)  { mom.SetXYZ(fPx,fPy,fPz); }



  /** Check for distance between in and out **/
  Bool_t IsUsable() const;


  /** Modifiers **/
  void SetPosition(TVector3 pos);
  void SetMomentum(TVector3 mom);


  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;



 protected:

  Double32_t fX,  fY,  fZ;
  Double32_t fPx, fPy, fPz;
  Double32_t fLocalX,  fLocalY,  fLocalZ;
  Double32_t fLocalPx, fLocalPy, fLocalPz;
  Int_t fModule;
  Int_t fLayer;
  Int_t fCell;


  ClassDef(R3BDchFullPoint,1)

};



inline void R3BDchFullPoint::SetPosition(TVector3 pos) {
  fX = pos.X();
  fY = pos.Y();
  fZ = pos.Z();
}


inline void R3BDchFullPoint::SetMomentum(TVector3 mom) {
  fPx = mom.Px();
  fPy = mom.Py();
  fPz = mom.Pz();
}



#endif
