// -------------------------------------------------------------------------
// -----            R3BCalifaEventDisplay header file                  -----
// -----            Created 01/10/2012  by P.Cabanelas                 -----
// -------------------------------------------------------------------------

#ifndef R3BCALIFAEVENTDISPLAY_H
#define R3BCALIFAEVENTDISPLAY_H

#include "FairTask.h"
#include "FairMCTracks.h"

#include "TEveProjections.h"
#include "TEveProjectionManager.h"
#include "TEveCalo.h"
#include "TEveCaloData.h"
#include "TEveWindow.h"
#include "TEveViewer.h"
#include "TEveScene.h"

class TClonesArray;
class FairEventManager;
class FairRootManager;

class TH2F;

class R3BCalifaEventDisplay : public FairTask 
{
  public:

    /** Default constructor **/
    R3BCalifaEventDisplay();

    /** Standard Constructor **/
    R3BCalifaEventDisplay(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BCalifaEventDisplay();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Finish **/
    virtual void Finish();

    /** Public method SelectGeometryVersion
     **
     ** Defines the geometry
     *@param version  Integer parameter used to select the geometry:
     ** (see documentation /r3broot/cal/perlScripts/README))
     **/
    void SelectGeometryVersion(Int_t version);


    /** Accessors **/

  protected:

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    TClonesArray*     fCrystalHitCA;

    FairEventManager* fEventManager;
    FairRootManager*  fManager;

    // Selecting the geometry of the CALIFA calorimeter
    Int_t fGeometryVersion;
    
    TEveCaloDataHist* fDataHist;
    TEveCalo3D*       fCalo3d;
    TEveCalo2D*       fCalo2d;
    TEveCaloLego*     fLego;

    TEveWindowSlot*   slotLeftTop;
    TEveWindowSlot*   slotLeftBottom;
    TEveWindowSlot*   slotRightTop;
    TEveWindowSlot*   slotRightBottom;
    TEveViewer*       viewerLeftTop;
    TEveScene*        sceneLeftTop;
    TEveViewer*       viewerRightTop;
    TEveScene*        sceneRightTop;
    TEveViewer*       viewerLeftBottom;
    TEveScene*        sceneLeftBottom;
    TEveViewer*       viewerRightBottom;
    TEveScene*        sceneRightBottom;
    TEveWindowSlot*   legoSlot;
    TEveScene*        legoScene;
    TEveViewer*       legoViewer;

    TEveProjectionManager*   fProjManager1;
    TEveProjectionManager*   fProjManager2;
	
    // Flag for simulation
    Bool_t kSimulation;


  private:

    /** Data members **/

    TH2F* hcalo;


    /** Private Methods **/

    void CreateHistograms();
    void MakeSlots();
    void MakeViewerScene();

    void GetAngles(Int_t geoVersion, Int_t iD, Double_t* polar, Double_t* azimuthal, Double_t* rho);


    ClassDef(R3BCalifaEventDisplay,2);

};

#endif
