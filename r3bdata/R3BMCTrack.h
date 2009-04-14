// -------------------------------------------------------------------------
// -----                      R3BMCTrack header file                   -----
// -----                  Created 03/08/04  by V. Friese               -----
// -----                  Adapted to Panda  M. Al-Turany               -----
// -------------------------------------------------------------------------


/** R3BMCTrack.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Data class for storing Monte Carlo tracks processed by the R3BStack.
 ** A MCTrack can be a primary track put into the simulation or a
 ** secondary one produced by the transport through decay or interaction.
 **/


#ifndef R3BMCTRACK_H
#define R3BMCTRACK_H 1

#include "R3BDetectorList.h"
#include "TObject.h"
#include "TParticle.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#ifndef ROOT_TParticlePDG
#include "TParticlePDG.h"
#endif
#ifndef ROOT_TDatabasePDG
#include "TDatabasePDG.h"
#endif

class R3BMCTrack : public TObject
{

 public:


  /**  Default constructor  **/
  R3BMCTrack();


  /**  Standard constructor  **/
  R3BMCTrack(Int_t pdgCode, Int_t motherID, TVector3 startVvertex, 
	     Double_t startTime, TVector3 momentum, Int_t  nPoint=0);


  /**  Copy constructor  **/
  R3BMCTrack(const R3BMCTrack& track);


  /**  Constructor from TParticle  **/
  R3BMCTrack(TParticle* particle);


  /**  Destructor  **/
  virtual ~R3BMCTrack();


  /**  Output to screen  **/
  void Print(Int_t iTrack) const;


  /**  Accessors  **/
  Int_t    GetPdgCode()     const { return fPdgCode; }
  Int_t    GetMotherID()    const { return fMotherID; }
  TVector3 GetStartVertex() const { return TVector3(fStartX, fStartY,fStartZ);}
  Double_t GetStartTime()   const { return fStartT; }
  TVector3 GetMomentum()    const { return TVector3(fPx, fPy, fPz); }
  Double_t GetPt()       const { return TMath::Sqrt(fPx*fPx+fPy*fPy); }
  /*
  Int_t GetStsPoints()  const { return   (fPoints &  15        )        ; }
  */
   
  Int_t  GetNPoints(DetectorId detId)  const;  
  /**  Modifiers  **/
  
  void SetMotherID(Int_t id) { fMotherID = id; }
  /*
  void SetStsPoints(Int_t np);
  */ 
 
  
  /**  Add one detector point to the fPoint variable  **/
 /*
  void AddStsPoint()  { SetStsPoints(  GetStsPoints()  + 1 ); }
 */ 
 
  
  void SetNPoints(Int_t iDet, Int_t  np);
  
  TLorentzVector Get4Momentum() const;


private:

  /**  PDG particle code  **/
  Int_t  fPdgCode;

  /** Momentum components at production [GeV]  **/
  Double32_t fPx, fPy, fPz;

  /**  Index of mother track. Zero for primary particles.  **/
  Int_t  fMotherID;

  /** Coordinates of start vertex [cm, ns]  **/
  Double32_t fStartX, fStartY, fStartZ, fStartT;

  /**  Bitvector representing the number of MCPoints for this track in 
   **  each subdetector. The detectors are represented by
   **  This goes to fPoints:
   **  CAL:  Bit  0 -  1  (2 bit max. value 3)
   **  DCH:  Bit  2 -  3  (2 bit max. value 3)
   **  GFI:  Bit  4 -  5  (2 bit max. value 3)
   **  LAND:  Bit  6 -  7  (2 bit max. value 3)
   **  MTOF:  Bit  8 -  9  (2 bit max. value 3)
   **  TOF:  Bit 10 - 11  (2 bit max. value 3)
   **  TRA:  Bit 12 - 13  (2 bit max. value 3)
   
   **  The respective point numbers can be accessed and modified **/

  Int_t  fPoints;

  ClassDef(R3BMCTrack,1);

};



#endif
