#include "R3BSTaRTrackerEvent.h"

ClassImp(R3BSTaRTrackerEvent)

R3BSTaRTrackerEvent::R3BSTaRTrackerEvent() :
 fSTHits(new TObjArray()), // clusters in the Silicon Tracker
 fCalifaHits(new TObjArray()), // CALIFA hit
 fGeometryST("geom.root") // geometry Silicon Tracker
{
 for(Int_t i=0;i<3;i++){
   fBeamPosition[i]=0.0;
   fTargetDim[i]=0.0;
   fTargetPosition[i]=0.0;
   fVtxPosition[i]=0.0;
   fVtxError[i]=0.0;
 }

}

//-----------------------------------------------
R3BSTaRTrackerEvent::R3BSTaRTrackerEvent(Int_t nhitsST, Int_t nhitsCalifa) :
 fSTHits(), // clusters in the Silicon Tracker
 fCalifaHits(), // CALIFA hit
 fGeometryST("geom") // geometry Silicon Tracker
{
 for(Int_t i=0;i<3;i++){
   fBeamPosition[i]=0.0;
   fTargetDim[i]=0.0;
   fTargetPosition[i]=0.0;
   fVtxPosition[i]=0.0;
   fVtxError[i]=0.0;
 }
 fSTHits=new TObjArray(nhitsST);
 fCalifaHits=new TObjArray(nhitsCalifa);
}

//-----------------------------------------------

R3BSTaRTrackerEvent::R3BSTaRTrackerEvent( const R3BSTaRTrackerEvent& stevent) :
  //fBeamPosition(stevent.fBeamPosition),
  //fTargetDim(stevent.fTargetDim),
  //fTargetPosition(stevent.fTargetPosition),
  //fVtxPosition(stevent.fVtxPosition),
  //fVtxError(stevent.fVtxError),
 fSTHits(stevent.fSTHits),
 fCalifaHits(stevent.fCalifaHits),
 fGeometryST(stevent.fGeometryST)
{
// copy constructor
//  fSTHits= new TObjArray(stevent.fSTHits);
//  fCalifaHits=new TObjArray(stevent.fCalifaHits);

for(Int_t i=0;i<3;i++){
   fBeamPosition[i]=stevent.fBeamPosition[i];
   fTargetDim[i]=stevent.fTargetDim[i];
   fTargetPosition[i]=stevent.fTargetPosition[i];
   fVtxPosition[i]=stevent.fVtxPosition[i];
   fVtxError[i]=stevent.fVtxError[i];
 }


}

//---------------------------------------
R3BSTaRTrackerEvent::~R3BSTaRTrackerEvent()
{

//destructor
if(fSTHits) delete fSTHits;
if(fCalifaHits) delete fCalifaHits;

}
