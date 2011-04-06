// --------------------------------------------------------------------------------------------
// -----                          R3BLandData header file                                 -----
// -----    read data from a ROOT file that could be LMD data converted to Root data      -----
// -----             for further analysis and comparasion with simulated data             -----
// -----                       Created by M.I.Cherciu -25.03.2011                         -----
// --------------------------------------------------------------------------------------------

/** R3BLandData
**/


#ifndef R3BLANDDATA_H
#define R3BLANDDATA_H 1


#include "FairTask.h"
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
#include "TLeaf.h"
#include "TH1F.h"
#include "TH2F.h"


class R3BLandData
{

 public: 

  /** Default constructor without arguments should not be used. **/
  R3BLandData();


  /** Standard constructor. 
   ** @param fileName The input file name
   ** @param treeName The input tree name
   **/
  R3BLandData(const char* fileName, const char* treeName);


  /** Destructor. **/
  virtual ~R3BLandData();


  /** Reads the events from the input file and get different quantities
   ** for further analysis and ploting.
   **/
  //TLeaf* leaf;
  //TString leafname;
  Double_t lvalue;
  Bool_t flaghisto1; 
  Bool_t flaghisto2;
  Int_t  nEntries;

  int GetEntries();
  TLeaf* GetLeaf(const char*, int);
  void FillHisto1D(TLeaf*);
  void FillHisto1D(TLeaf*, Int_t, Double_t, Double_t);
  void FillHisto2D(TLeaf*, TLeaf*);
  void FillHisto2D(TLeaf*, TLeaf*, Int_t bin1, Double_t, Double_t, Int_t, Double_t, Double_t);
  void Draw1D();
  void Draw2D();

 private:

  TFile* fInputFile;
  TTree* tree;
  TLeaf* leaf;
  TCanvas* c1;
  TCanvas* c2;
  TH1F* H1;
  TH2F* H2;

  const char*  fFileName;           //! Input file Name
  const char*  fTreeName;

  /** Private method CloseInput. Just for convenience. Closes the 
   ** input file properly. Called from destructor. **/
  void CloseInput();


  ClassDef(R3BLandData,1);

};

#endif
