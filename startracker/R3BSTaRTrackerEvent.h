#ifndef R3BSTaRTrackerEvent_H
#define R3BSTaRTrackerEvent_H

#include <TObject.h>
#include <TObjArray.h>
#include "R3BSTaRTrackerHit.h"
#include "../califaData/R3BCalifaHitData.h"


class R3BSTaRTrackerEvent : public TObject {

 public:
 // constructor, destructor, copy

  R3BSTaRTrackerEvent();
  R3BSTaRTrackerEvent(Int_t nhitsST, Int_t nhitsCalifa);

  virtual ~R3BSTaRTrackerEvent();

  R3BSTaRTrackerEvent(const R3BSTaRTrackerEvent& stevent);


  //getters
   Double_t* GetBeamPosition(){return fBeamPosition;}
   Double_t GetBeamX() {return fBeamPosition[0];}
   Double_t* GetTargetDim(){return fTargetDim;}
   Double_t GetTargetLenght() {return fTargetDim[0];}
   Double_t GetTargetWidth() {return fTargetDim[1];}
   Double_t GetTargetThickness() {return fTargetDim[2];}
   Double_t* GetTargetPosition(){return fTargetPosition;}
   Double_t* GetVtxPosition(){return fVtxPosition;}
   Double_t GetVtxPositionX() {return fVtxPosition[0];}
   Double_t* GetVtxError(){return fVtxError;}
   TObjArray* GetSTHits() const {return (TObjArray*)fSTHits;}
   TObjArray* GetCalifaHits() const {return (TObjArray*)fCalifaHits;}
   TString GetGeometryST() {return fGeometryST;}

  //setters
   void SetBeamPosition(Double_t beam[3]){for(Int_t i=0;i<3;i++){fBeamPosition[i]=beam[i];} return;}
   void SetTargetDim(Double_t tgdim[3]){for(Int_t i=0;i<3;i++){fTargetDim[i]=tgdim[i];} return;}
   void SetTargetPosition(Double_t tgpos[3]){for(Int_t i=0;i<3;i++){fTargetPosition[i]=tgpos[i];} return;}
   void SetVtxPosition(Double_t vtxpos[3]){for(Int_t i=0;i<3;i++){fVtxPosition[i]=vtxpos[i];} return;}
   void SetVtxError(Double_t vtxerr[3]){for(Int_t i=0;i<3;i++){fVtxError[i]=vtxerr[i];} return;}
   void SetSTHits(TObjArray *sthits){fSTHits=sthits;return;}
   void SetCalifaHits(TObjArray* califahits){fCalifaHits=califahits; return;}
   void SetSTGeometry(TString geometry="geometry.root"){fGeometryST=geometry.Data();}

 private:
  Double_t fBeamPosition[3]; // beam position 
  Double_t fTargetDim[3]; // target dimension (Lenght,Width,Thickness)
  Double_t fTargetPosition[3]; // target position 
  Double_t fVtxPosition[3]; // primary vertex position
  Double_t fVtxError[3]; // primary vertex error
  TObjArray *fSTHits; // Hits in the Silicon Tracker (R3BSTaRTrackerHit), min 3 (1 track), or 6 (2 protons)
  TObjArray *fCalifaHits; // CALIFA hit (R3BCalifaHitData)
  TString fGeometryST; // geometry Silicon Tracker

  ClassDef(R3BSTaRTrackerEvent,1);
};

#endif
