// --------------------------------------------------------------------------------------------
// -----                          R3BLandGenerator header file                            -----
// -----                       Created by M.I.Cherciu -18.06.2010                         -----
// --------------------------------------------------------------------------------------------

/** R3BLandGenerator
**/


#ifndef R3BLANDGENERATOR_H
#define R3BLANDGENERATOR_H 1


#include "FairGenerator.h"
#include <map>

//Root Headers
#include "TROOT.h"
#include "TApplication.h"
#include "TSystem.h"
#include "TBranch.h"
#include "TPad.h"
#include "TCanvas.h"
#include "TTree.h"
#include "TFile.h"



class TDatabasePDG;
class FairPrimaryGenerator;
class FairIon;

class R3BLandGenerator : public FairGenerator  
{

 public: 

  /** Default constructor without arguments should not be used. **/
  R3BLandGenerator();


  /** Standard constructor. 
   ** @param fileName The input file name
   **/
  R3BLandGenerator(const char* fileName);


  R3BLandGenerator(const R3BLandGenerator&);


  R3BLandGenerator& operator=(const R3BLandGenerator&) { return *this; }


  /** Destructor. **/
  virtual ~R3BLandGenerator();


  /** Reads on event from the input file and pushes the tracks onto
   ** the stack. Abstract method in base class.
   ** @param primGen  pointer to the R3BPrimaryGenerator
   **/
  virtual Bool_t ReadEvent(FairPrimaryGenerator* primGen);



 private:

  TFile* fInputFile;          //! Input file stream
  TTree* tree;

  TLeaf* lPDG;
  TLeaf* lM;
  TLeaf* lZ;
  TLeaf* lE;
  TLeaf* lp;
  TLeaf* ltheta;
  TLeaf* lphi;
  TLeaf* lpx;
  TLeaf* lpy;
  TLeaf* lpz;
  TLeaf* lx;
  TLeaf* ly;
  TLeaf* lz;

  const char*  fFileName;           //! Input file Name
  TDatabasePDG*  fPDG;                //!  PDG database

  Double_t PDGo,Mo,Zo,Eo,po,thetao,phio;
  Double_t xo,yo,zo,pxo,pyo,pzo;
  Double_t vx,vy,vz;

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor and from ReadEvent. **/
  void CloseInput();


  ClassDef(R3BLandGenerator,1);

};

#endif
