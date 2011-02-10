#ifndef _R3BLandDigiAnalyser_
#define _R3BLandDigiAnalyser_

#include "FairTask.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TGeoMatrix.h"
#include "R3BLandDigiPar.h"
#include "R3BDetector.h"
#include <fstream>
#include <string>
//#include <vector>

class TH1F;
class TH2F;

class R3BLandDigiAnalyser: public FairTask
{
  public:

  struct hitData
  {
    int paddleNb;
    double qdcL, qdcR;
    double tdcL, tdcR;
    double x, y, z, t;
    double E;
  };

  /** Default constructor **/  
  R3BLandDigiAnalyser(Int_t _iVerbose=1);

  /** Destructor **/
  ~R3BLandDigiAnalyser();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  /** Virtal method Finish **/
  virtual void Finish();

  void acceptEmptyEvents(bool arg) {emptyEventsAccepted = arg;};

  private:

  // Parameter class
  R3BLandDigiPar* fLandDigiPar;
  R3BDetector* land;

  Int_t iVerbose;
  Int_t iEvent;
  hitData* hits;
  Int_t hitArrayCapacity;

  bool emptyEventsAccepted;

  Int_t nPaddles;
  Int_t nPlanes;
  Int_t nPaddlesPerPlane;

  Double_t paddle_dimx;
  Double_t paddle_dimy;
  Double_t paddle_dimz;
  Double_t paddle_spacing;

  Double_t landPlacementZ;

  Double_t att;  //attenuation of light in land
  
  Double_t v_eff;
  Double_t v_eff_inv;
  

  TH1F** digiHists;
	TH1F** diffHists;
  TH1F** timeEnergyHists;
  TH1F** exitingHists;
  TH1F** energyHists;

  TClonesArray* fLandPoints;
  TClonesArray* fLandMCTrack; 
  TClonesArray* fLandDigi;
  TClonesArray* fLandGeoTracks;

  std::ofstream foutScen;
  std::ofstream foutPrim;

  void initialisePlots();
  bool calculatePosition(hitData* hit);
  void printHitsForTracker(int nHits);
  std::string getPrimariesData();
 
  void hitAnalysis();
  void checkForPossibleGhostHits(int nHits);

  virtual void SetParContainers();

  ClassDef(R3BLandDigiAnalyser,1);
};

#endif
