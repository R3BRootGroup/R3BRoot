// --------------------------------------------------------------------
// -----          R3BCalifaHitEventDisplay source file            -----
// --------------------------------------------------------------------

#include "R3BCalifaHitEventDisplay.h"

#include "TClonesArray.h"
#include "TObjArray.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairEventManager.h"
#include "FairMCTracks.h"
#include "FairTask.h"
#include "FairMCTracks.h"

#include "R3BCalifaHitData.h"
#include "R3BCalifaHitDataSim.h"

#include "TEveManager.h"
#include "TEveProjections.h"
#include "TEveCalo.h"
#include "TEveCaloData.h"
#include "TEveWindow.h"
#include "TGLViewer.h"
#include "TGLSAViewer.h"
#include "TGTab.h"
#include "TEveBrowser.h"
#include "TEveViewer.h"
#include "TEveScene.h"
#include "TEveProjectionManager.h"
#include "TEveProjectionAxes.h"
#include "TEveCaloLegoOverlay.h"
#include "TEveLegoEventHandler.h"
#include "TEveTrans.h"

#include "TH2F.h"
#include "TStyle.h"
#include "TCanvas.h"

using std::cout;
using std::endl;

// -----   Default constructor   -------------------------------------------
R3BCalifaHitEventDisplay::R3BCalifaHitEventDisplay() : FairTask("R3BCalifaHitEventDisplay",0),
fEventManager(NULL),
fDataHist(NULL),
fCalo3d(NULL),
fCalo2d(NULL),
fLego(NULL),
slotLeftTop(NULL),slotLeftBottom(NULL),slotRightTop(NULL),slotRightBottom(NULL),
viewerLeftTop(NULL),sceneLeftTop(NULL),viewerRightTop(NULL),sceneRightTop(NULL),
viewerLeftBottom(NULL),sceneLeftBottom(NULL),viewerRightBottom(NULL),sceneRightBottom(NULL),
legoSlot(NULL),legoScene(NULL),legoViewer(NULL),
fProjManager1(NULL),fProjManager2(NULL),kSimulation(false)
{
}
// -----------------------------------------------------------------------

// -----   Standard constructor   -------------------------------------------
R3BCalifaHitEventDisplay::R3BCalifaHitEventDisplay(const char* name, Int_t iVerbose) : FairTask(name, iVerbose),
fEventManager(NULL),
fDataHist(NULL),
fCalo3d(NULL),
fCalo2d(NULL),
fLego(NULL),
slotLeftTop(NULL),slotLeftBottom(NULL),slotRightTop(NULL),slotRightBottom(NULL),
viewerLeftTop(NULL),sceneLeftTop(NULL),viewerRightTop(NULL),sceneRightTop(NULL),
viewerLeftBottom(NULL),sceneLeftBottom(NULL),viewerRightBottom(NULL),sceneRightBottom(NULL),
legoSlot(NULL),legoScene(NULL),legoViewer(NULL),
fProjManager1(NULL),fProjManager2(NULL),kSimulation(false)
{
}
// -----------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
R3BCalifaHitEventDisplay::~R3BCalifaHitEventDisplay() {}
// -------------------------------------------------------------------------

// -----   Public method Init   --------------------------------------------
InitStatus R3BCalifaHitEventDisplay::Init()
{

  FairRootManager* ioManager = FairRootManager::Instance();
  if ( !ioManager ) Fatal("Init", "No FairRootManager");
  if( !ioManager->GetObject("CalifaHitDataSim") ) {
     fCaloHitCA = (TClonesArray*) ioManager->GetObject("CalifaHitData");
  } else {
     fCaloHitCA = (TClonesArray*) ioManager->GetObject("CalifaHitDataSim");
     kSimulation = true;
  }

  fEventManager = FairEventManager::Instance();

  CreateHistograms();
  MakeSlots();
  MakeViewerScene();

  return kSUCCESS;

}


// -----   Public method ReInit   --------------------------------------------
InitStatus R3BCalifaHitEventDisplay::ReInit()
{

  return kSUCCESS;

}


// -----   Public method Exec   --------------------------------------------
void R3BCalifaHitEventDisplay::Exec(Option_t* opt)
{
  if (IsActive()) {

    Reset();

    // Besides if conditions, both objects must be defined
    R3BCalifaHitData*    caloHit;
    R3BCalifaHitDataSim* caloHitSim;

    Int_t caloHits=0;        // Nb of CaloHits in current event
    caloHits = fCaloHitCA->GetEntriesFast();

    Int_t binx, biny;
    Double_t theta, phi, eta;

//    TAxis *ax = hcalohit->GetXaxis();
//    TAxis *ay = hcalohit->GetYaxis();
//    Int_t nx = ax->GetNbins();
//    Int_t ny = ay->GetNbins();

    // Loop in Calo Hits
    for (Int_t i=0; i<caloHits; i++) {

      if(kSimulation) {
        caloHitSim = (R3BCalifaHitDataSim *) fCaloHitCA->At(i);
        theta = caloHitSim->GetTheta();
        phi   = caloHitSim->GetPhi();
      } else {
        caloHit  = (R3BCalifaHitData *) fCaloHitCA->At(i);
        theta = caloHit->GetTheta();
        phi   = caloHit->GetPhi();
      }
      eta = -TMath::Log(TMath::Tan(theta*0.5f));

      //Matching hit with histogram bin
      for(Int_t b=1;b<=hcalohit->GetNbinsX();b++) {
        if(eta<hcalohit->GetXaxis()->GetBinLowEdge(b)) {
          binx = b-1;
          break;
        }
      }
      for(Int_t b=1;b<=hcalohit->GetNbinsY();b++) {
        if(phi<hcalohit->GetYaxis()->GetBinLowEdge(b)) {
          biny = b-1;
          break;
        }
      }


      // Filling histograms
      if(kSimulation) {
        if(hcalohit->GetBinContent(binx,biny)==0) { hcalohit->SetBinContent(binx,biny,caloHitSim->GetEnergy()*1000);
        } else { hcalohit->SetBinContent(binx,biny,hcalohit->GetBinContent(binx,biny)+caloHitSim->GetEnergy()*1000);
        }
      } else {
        if(hcalohit->GetBinContent(binx,biny)==0) { hcalohit->SetBinContent(binx,biny,caloHit->GetEnergy()*1000);
        } else { hcalohit->SetBinContent(binx,biny,hcalohit->GetBinContent(binx,biny)+caloHit->GetEnergy()*1000);
        }
      }

    }


    // ------- Data Histogram --------------------------------
    fDataHist = new TEveCaloDataHist();
    fDataHist->AddHistogram(hcalohit);

    // ------- Calo3D -------------------------------------
    fCalo3d = new TEveCalo3D(fDataHist);
    fCalo3d->SetBarrelRadius(47.00);
    fCalo3d->SetEndCapPos(50.10);
    sceneRightTop->AddElement(fCalo3d);
    Float_t maxH = 60;
    fCalo3d->SetMaxTowerH(maxH);
    fCalo3d->SetMainColor(5);

    fDataHist->IncDenyDestroy();
    fCalo3d  ->IncDenyDestroy();

    // ------- Projections -------------------------------------
    // --- note.- just two ways of doing the same... -----------
    fProjManager1 = new TEveProjectionManager(TEveProjection::kPT_RPhi);
    sceneLeftBottom->AddElement(fProjManager1);
    TEveProjectionAxes* axes = new TEveProjectionAxes(fProjManager1);
    fProjManager1->ImportElements(fCalo3d);
    sceneLeftBottom->AddElement(axes);

    fProjManager2 = new TEveProjectionManager(TEveProjection::kPT_RhoZ);
    fCalo2d = (TEveCalo2D*) fProjManager2->ImportElements(fCalo3d);
    sceneRightBottom->AddElement(fCalo2d);
    TEveProjectionAxes* axes2 = new TEveProjectionAxes(fProjManager2);
    sceneRightBottom->AddElement(axes2);
    fCalo2d->SetMaxTowerH(maxH);

    // ------ Lego ------------------------------------------
    gStyle->SetPalette(1,0);
    fLego = new TEveCaloLego((TEveCaloData*)fDataHist);
    legoScene->AddElement(fLego);
    fLego->InitMainTrans();
    fLego->RefMainTrans().SetScale(TMath::TwoPi(), TMath::TwoPi(), TMath::Pi());
    // set event handler to move from perspective to orthographic view.
    legoViewer->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
    legoViewer->GetGLViewer()->SetEventHandler
       (new TEveLegoEventHandler((TGWindow*)legoViewer->GetGLViewer()->GetGLWidget(), legoViewer->GetGLViewer(), fLego));
    fLego->SetGridColor(5);
    gEve->FullRedraw3D(kFALSE);

  }

}


// ---- Public method Reset   --------------------------------------------------
void R3BCalifaHitEventDisplay::Reset()
{

  hcalohit->Reset();
  
  if(fCalo3d) delete fCalo3d;
  if(fLego)   delete fLego;

}


// ---- Public method Finish   --------------------------------------------------
void R3BCalifaHitEventDisplay::Finish()
{
}


// -----   Private method CreateHistograms  -------------------------------------
void R3BCalifaHitEventDisplay::CreateHistograms()
{

  // Variable x-bin size: defined by crystals polar angle edges 
  // Virtual backwards end-cap assumed to be equals to forward end-cap
  Double_t xedges[64] = {8.71,11.76,14.74,17.21,19.68,21.83,23.97,26.02,28.06,
         30.00,31.94,33.85,35.75,37.53,39.30,41.23,43.16,
         45.09,47.02,49.08,51.14,53.34,55.53,57.87,60.21,
         62.70,65.18,67.81,70.43,73.18,75.93,78.78,81.63,
         84.59,87.55,90.97,94.39,97.78,101.17,105.67,110.71,
         114.34,118.51,122.24,125.97,129.27,132.56,135.35,
         138.36,141.37,143.84,146.31,148.46,150.60,152.65,
         154.69,156.63,158.57,160.48,162.38,164.16,165.93,
         167.86,169.79};

  Double_t xedgesrad[64], xedgeseta[64];

  for(Int_t i=0; i<64; i++) xedgesrad[i] = xedges[i] * 3.14 / 180. ;

  //conversion of Theta into Eta (default CaloLego x-axis angle)
  for(Int_t i=0; i<64; i++) xedgeseta[i] = TMath::Log(TMath::Tan(xedgesrad[i]*0.5f));

  hcalohit = new TH2F("hcalohit","hcalohit",63,xedgeseta,64,-3.14,3.14);
  hcalohit->GetXaxis()->SetTitle("Eta [rad]");
  hcalohit->GetYaxis()->SetTitle("Phi [rad]");


}


// -----   Private method MakeSlots  -------------------------------------
void R3BCalifaHitEventDisplay::MakeSlots()
{

  // frames
  TEveWindowSlot* slot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());
  TEveWindowPack* packH = slot->MakePack();
  packH->SetElementName("CaloHit Projections");
  packH->SetHorizontal();
  packH->SetShowTitleBar(kFALSE);

  slot = packH->NewSlot();
  TEveWindowPack* pack0 = slot->MakePack();
  pack0->SetShowTitleBar(kFALSE);
  slotLeftTop    = pack0->NewSlot();
  slotLeftBottom = pack0->NewSlot();

  slot = packH->NewSlot();
  TEveWindowPack* pack1 = slot->MakePack();
  pack1->SetShowTitleBar(kFALSE);
  slotRightTop    = pack1->NewSlot();
  slotRightBottom = pack1->NewSlot();

  legoSlot = TEveWindow::CreateWindowInTab(gEve->GetBrowser()->GetTabRight());


}


// -----   Private method MakeViewerScene  ---------------------------------
void R3BCalifaHitEventDisplay::MakeViewerScene()
{

  slotLeftTop->MakeCurrent();
  viewerLeftTop = gEve->SpawnNewViewer("Main","Main");
  viewerLeftTop->AddScene(gEve->GetGlobalScene());
  viewerLeftTop->AddScene(gEve->GetEventScene());
  

  slotRightTop->MakeCurrent();
  viewerRightTop = gEve->SpawnNewViewer("Calo3D","Calo3D");
  sceneRightTop  = gEve->SpawnNewScene("Calo 3D Scene");
  viewerRightTop->AddScene(sceneRightTop);

  slotLeftBottom->MakeCurrent();
  viewerLeftBottom = gEve->SpawnNewViewer("RPhi Projection","RPhi");
  sceneLeftBottom  = gEve->SpawnNewScene("RPhi Scene");
  viewerLeftBottom->AddScene(sceneLeftBottom);
  viewerLeftBottom->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);
  

  slotRightBottom->MakeCurrent();
  viewerRightBottom = gEve->SpawnNewViewer("RhoZ Projection","RhoZ");
  sceneRightBottom  = gEve->SpawnNewScene("RhoZ Scene");
  viewerRightBottom->AddScene(sceneRightBottom);
  viewerRightBottom->GetGLViewer()->SetCurrentCamera(TGLViewer::kCameraOrthoXOY);

  legoSlot->MakeCurrent();
  legoViewer = gEve->SpawnNewViewer("CaloHit LegoView","Lego");
  legoScene  = gEve->SpawnNewScene("Lego Scene");
  legoViewer->AddScene(legoScene);
  TEveCaloLegoOverlay* overlay = new TEveCaloLegoOverlay();
  legoViewer->GetGLViewer()->AddOverlayElement(overlay);
  overlay->SetCaloLego(fLego);

}



ClassImp(R3BCalifaHitEventDisplay)
