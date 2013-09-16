// -------------------------------------------------------------------------
// -----                R3BAsciiGenerator header file                 -----
// -----                Created 27.11.09 <D.Bertini@gsi.de>           -----
// -------------------------------------------------------------------------

/** R3BAsciiGenerator
**/


#ifndef R3BASCIIGENERATOR_H
#define R3BASCIIGENERATOR_H 1


#include "FairGenerator.h"

#include <fstream>
#include <map>

class TDatabasePDG;
class FairPrimaryGenerator;
class FairIon;

class R3BAsciiGenerator : public FairGenerator  
{

 public: 

  /** Default constructor without arguments should not be used. **/
  R3BAsciiGenerator();


  /** Standard constructor. 
   ** @param fileName The input file name
   **/
  R3BAsciiGenerator(const char* fileName);


  R3BAsciiGenerator(const R3BAsciiGenerator&);


  R3BAsciiGenerator& operator=(const R3BAsciiGenerator&) { return *this; }


  /** Destructor. **/
  virtual ~R3BAsciiGenerator();

	
  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the R3BPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);

 void SetXYZ   (Double32_t x=0, Double32_t y=0, Double32_t z=0) {
      fX=x;
      fY=y;
      fZ=z;
      fPointVtxIsSet=kTRUE;
   }

 void SetDxDyDz(Double32_t sx=0, Double32_t sy=0, Double32_t sz=0) {
      fDX=sx;
      fDY=sy;
      fDZ=sz;
      fBoxVtxIsSet=kTRUE;

 } 

 private:

  ifstream*      fInputFile;          //! Input file stream
  const Char_t*  fFileName;           //! Input file Name
  TDatabasePDG*  fPDG;                //!  PDG database

	
  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  /** Private method RegisterIons. Goes through the input file and registers
   ** any ion needed. **/
  Int_t RegisterIons();



  /** STL map from ion name to FairIon **/
  std::map<TString, FairIon*> fIonMap;       //!
	
  Double32_t fX, fY, fZ;           // Point vertex coordinates [cm]	
  Bool_t     fPointVtxIsSet;       // True if point vertex is set
  Double32_t fDX, fDY, fDZ;           // Point vertex coordinates [cm]	
  Bool_t     fBoxVtxIsSet;       // True if point vertex is set



  ClassDef(R3BAsciiGenerator,1);

};

#endif
