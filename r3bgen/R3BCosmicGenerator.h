// -------------------------------------------------------------------------
// -----                R3BCosmicGenerator header file                -----
// -----               Created 09/09/04  by Marc Labiche               -----
// -----            Derived from R3BBoxGenerator header file          ----- 
// -------------------------------------------------------------------------

/**  R3BCosmicGenerator.h 
 *@author Marc Labiche <marc.labiche@stfc.ac.uk>
 *
 * The R3BBoxGenerator generates particles within given range
 * over phi,theta and a fixed multiplicity per event.
 * Derived from R3BGenerator.
**/

/* $Id: R3BCosmicGenerator.h,v 1.0 2010/02/19 $ */

/* History of cvs commits:
 *
 *
 */

#ifndef R3B_COSMICGENERATOR_H
#define R3B_COSMICGENERATOR_H

#include "FairGenerator.h"

#include <iostream>

class FairPrimaryGenerator;

class R3BCosmicGenerator : public FairGenerator
{
public:

  /** Default constructor. **/
  R3BCosmicGenerator();

  /** Constructor with PDG-ID, multiplicity
   **@param pdgid Particle type (PDG encoding)
   **@param mult  Multiplicity (default is 1)
   **/
  R3BCosmicGenerator(Int_t pdgid, Int_t mult=1, Double32_t E_Threshold=0);

  /** Destructor **/
  virtual ~R3BCosmicGenerator() {};

  /** Modifiers **/
  void SetPDGType      (Int_t pdg)  {fPDGType = pdg;  };

  void SetMultiplicity (Int_t mult) {fMult    = mult; };

  void SetE_Threshold (Double32_t E_Threshold) {fE_Threshold    = E_Threshold; };

  void SetPhiRange     (Double32_t phimin=0  , Double32_t phimax=360)
    {fPhiMin=phimin; fPhiMax=phimax;};

  void SetThetaRange   (Double32_t thetamin=0, Double32_t thetamax=90)
    {fThetaMin=thetamin; fThetaMax=thetamax; fThetaRangeIsSet=kTRUE;};

  void SetCosTheta   ()
    {fCosThetaIsSet=kTRUE;};

  void SetXYZ   (Double32_t x=0, Double32_t y=0, Double32_t z=0) {
    fX=x; fY=y; fZ=z; fPointVtxIsSet=kTRUE;}

  void SetBoxXYZ (Double32_t x1=0, Double32_t y1=0, Double32_t x2=0, Double32_t y2=0, Double32_t z=0) {
    fX1=x1; fY1=y1; fX2=x2; fY2=y2; fZ=z; fBoxVtxIsSet=kTRUE;}

  void SetDebug(Bool_t debug=0) {fDebug = debug;}


  /** Initializer **/
  virtual Bool_t Init();

  /** Creates an event with given type and multiplicity.
   **@param primGen  pointer to the R3BPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

  /** E_Cosmic **/
  Double32_t E_Cosmic();


private:
  Int_t      fPDGType;             // Particle type (PDG encoding)
  Int_t      fMult;                // Multiplicity

  Double32_t fE_Threshold;         // Kinetic energy threshold

  Double32_t fPDGMass;             // Particle mass [GeV]
  Double32_t fPhiMin, fPhiMax;     // Azimuth angle range [degree]
  Double32_t fThetaMin, fThetaMax; // Polar angle range in lab system [degree]
  Double32_t fX, fY, fZ;           // Point vertex coordinates [cm]
  Double32_t fX1, fY1, fZ1, fX2, fY2, fZ2;   // Box vertex coords (x1,y1,z1)->(x2,y2,z2)

  Bool_t     fThetaRangeIsSet;     // True if theta range is set
  Bool_t     fCosThetaIsSet;       // True if uniform distribution in 
                                   //cos(theta) is set (default -> not set)
  Bool_t     fPointVtxIsSet;       // True if point vertex is set
  Bool_t     fBoxVtxIsSet;         // True if box vertex is set
  Bool_t     fDebug;               // Debug switch

  ClassDef(R3BCosmicGenerator,1);

};


#endif
