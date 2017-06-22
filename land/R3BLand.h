// -------------------------------------------------------------------------
// -----                        R3BLand header file                     -----
// -----                  Created 26/03/09  by D.Bertini               -----
// -------------------------------------------------------------------------

/**  R3BLand.h
 **/


#ifndef R3BLAND_H
#define R3BLAND_H

#include "R3BDetector.h"
#include "TLorentzVector.h"
#include "TGeoMedium.h"
#include <string>
#include <iostream>
#include <map>
#include "R3BLandFirstHits.h"

using namespace std;

class TClonesArray;
class R3BLandPoint;
class FairVolume;



class R3BLand : public R3BDetector
{
public:
  /** Default constructor **/
  R3BLand();
  
  /** Standard constructor.
   *@param name    detetcor name
   *@param active  sensitivity flag
   **/
  R3BLand(const char* name, Bool_t active);

  /** Standard constructor.
   *@param geoFile name of the ROOT geometry file
   *@param active  sensitivity flag
   *@param x       position in cave
   *@param y       position in cave
   *@param z       position in cave
   *@param rot_x   rotation in cave
   *@param rot_y   rotation in cave
   *@param rot_z   rotation in cave
   **/
  R3BLand(const char* name,
          TString geoFile,
          Bool_t active,
          Float_t x = 0.,
          Float_t y = 0.,
          Float_t z = 0.,
          Float_t rot_x = 0.,
          Float_t rot_y = 0.,
          Float_t rot_z = 0.);

  /** Destructor **/
  virtual ~R3BLand();
  
  /** Virtual method ProcessHits
   **
   ** Defines the action to be taken when a step is inside the
   ** active volume. Creates a R3BLandPoint and adds it to the
   ** collection.
   *@param vol  Pointer to the active volume
   **/
  virtual Bool_t ProcessHits(FairVolume* vol = 0);
  
  /** Virtual method BeginEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void BeginEvent();
  
  /** Virtual method EndOfEvent
   **
   ** If verbosity level is set, print hit collection at the
   ** end of the event and resets it afterwards.
   **/
  virtual void EndOfEvent();
  
  virtual void FinishEvent();
  
  /** Virtual method Register
   **
   ** Registers the hit collection in the ROOT manager.
   **/
  virtual void Register();
  
  virtual void FinishRun();
  
  /** Accessor to the hit collection **/
  virtual TClonesArray* GetCollection(Int_t iColl) const;
  
  /** Virtual method Print
   **
   ** Screen output of hit collection.
   **/
  virtual void Print(Option_t *option = "") const;
  
  /** Virtual method Reset
   **
   ** Clears the hit collection
   **/
  virtual void Reset();
  
  /** Virtual method CopyClones
   **
   ** Copies the hit collection with a given track index offset
   *@param cl1     Origin
   *@param cl2     Target
   *@param offset  Index offset
   **/
  virtual void CopyClones(TClonesArray* cl1, TClonesArray* cl2,
                          Int_t offset);
  
  /** Virtaul method Construct geometry
   **
   ** Constructs the STS geometry
   **/
  virtual void ConstructGeometry();
  
  virtual void Initialize();
  
  virtual void SetSpecialPhysicsCuts() {}
  
  virtual Bool_t CheckIfSensitive(std::string name);
  
  
private:
  /** Track information to be stored until the track leaves the
   active volume. **/
  Int_t          fTrackID;           //!  track index
  Int_t          fVolumeID;          //!  volume id
  Int_t          fCopyNo;            //!  copy no
  Int_t          fPaddleTyp;         //!  box number
  TLorentzVector fPosIn, fPosOut;    //!  position
  TLorentzVector fMomIn, fMomOut;    //!  momentum
  Double32_t     fTime;              //!  time
  Double32_t     fLength;            //!  length
  Double32_t     fELoss;             //!  energy loss
  Double32_t     fLightYield;        //!  light yield
  
  Int_t          fPosIndex;          //!
  TClonesArray*  fLandCollection;    //!  The hit collection
  TClonesArray*  fLandFirstHits;     //!  First Hits
  Bool_t         kGeoSaved;          //!
  TList *flGeoPar;                   //!
  Double_t fBirkC0,fBirkC1,fBirkC2;  //!
  
  Int_t          fMotherID;          //!  Mother ID
  Int_t          fNprim;             //!  Number of primaries
  
  Double_t firstHitX[6];             //! position and time of first our hits
  Double_t firstHitY[6];             //!
  Double_t firstHitZ[6];             //!
  Double_t firstT[6];                //!
  
  /** Private method AddHit
   **
   ** Adds a LandPoint to the HitCollection
   **/
  R3BLandPoint* AddHit(Int_t trackID, Int_t detID, Int_t box, Int_t id1, Int_t id2,
                       TVector3 posIn,
                       TVector3 pos_out, TVector3 momIn,
                       TVector3 momOut, Double_t time,
                       Double_t length, Double_t eLoss,Double_t lightYield);
  
  R3BLandFirstHits* AddHit1(Double_t x0, Double_t y0, Double_t z0, Double_t T0,
                            Double_t x1, Double_t y1, Double_t z1, Double_t T1,
                            Double_t x2, Double_t y2, Double_t z2, Double_t T2,
                            Double_t x3, Double_t y3, Double_t z3, Double_t T3,
                            Double_t x4, Double_t y4, Double_t z4, Double_t T4,
                            Double_t x5, Double_t y5, Double_t z5, Double_t T5);
  
  /** Private method ResetParameters
   **
   ** Resets the private members for the track parameters
   **/
  void ResetParameters();
  
  /** Map of MCis with fixed VolID */
  map <Int_t,Int_t> fMapMcId;    //!
  
  void StepHistory();
  
  ClassDef(R3BLand,4);
};


inline void R3BLand::ResetParameters()
{
  fTrackID = fVolumeID = 0;
  fPosIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fPosOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomIn.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fMomOut.SetXYZM(0.0, 0.0, 0.0, 0.0);
  fTime = fLength = fELoss = fLightYield = 0;
  fPosIndex = 0;
};


#endif 
