// -------------------------------------------------------------------------
// -----             R3BCalifaHitEventDisplay header file              -----
// -----              Created 26/10/2012  by P.Cabanelas               -----
// -------------------------------------------------------------------------

#ifndef R3BCALIFAHITEVENTDISPLAY_H
#define R3BCALIFAHITEVENTDISPLAY_H

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

class R3BCalifaHitEventDisplay : public FairTask 
{
  public:

    /** Default constructor **/
    R3BCalifaHitEventDisplay();

    /** Standard Constructor **/
    R3BCalifaHitEventDisplay(const char* name, Int_t iVerbose = 1);

    /** Destructor **/
    ~R3BCalifaHitEventDisplay();

    /** Virtual method Exec **/
    virtual void Exec(Option_t* opt);

    /** Virtual method Reset **/
    virtual void Reset();

    /** Virtual method Init **/
    virtual InitStatus Init();

    /** Virtual method Finish **/
    virtual void Finish();



    /** Accessors **/

  protected:

    /** Virtual method ReInit **/
    virtual InitStatus ReInit();

    TClonesArray*     fCaloHitCA;

    FairEventManager* fEventManager;
    FairRootManager*  fManager;

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

    TH2F* hcalohit;


    /** Private Methods **/

    void CreateHistograms();
    void MakeSlots();
    void MakeViewerScene();

    ClassDef(R3BCalifaHitEventDisplay,2);

};

#endif
