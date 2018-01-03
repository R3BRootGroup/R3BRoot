// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BFi4Cal2Hit                           -----
// -----		   created 09-03-2016 by J. Tscheuschner	   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Cal data to Hit data, meaning: calculating position, 
 * meaning given a reference-point one can give each det-type to this point a z-shift
 * and angle to the x-axis(if zero no shift in z-direction per fiber) also the thickness
 * of the fiber can be given. If nothing is given reference-point will be (0,0,0), the 
 * z-shift, and angle will be set to zero, too. The thickness of the single fiber is then 
 * set to 200um.
 */

#include <iostream>
using namespace std;
#include <sstream>

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BEventHeader.h"
#include "R3BFi4CalItem.h"
#include "R3BFi4HitItem.h"
#include "R3BFi4Cal2Hit.h"

#include <TMath.h>

R3BFi4Cal2Hit::R3BFi4Cal2Hit()
    : fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BFi4HitItem"))
    , fNDet(1)
    , fGeofile(0)
    , fFiberWidthfile(0)
    , fFiber_width(0)
    , fAngle(0)
    , fRevPoint(0)
{
}

R3BFi4Cal2Hit::R3BFi4Cal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BFi4HitItem"))
    , fNDet(1)
    , fGeofile(0)
    , fFiberWidthfile(0)
    , fFiber_width(0)
    , fAngle(0)
    , fRevPoint(0)
{
}

R3BFi4Cal2Hit::~R3BFi4Cal2Hit()
{
}

InitStatus R3BFi4Cal2Hit::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fCalItems = (TClonesArray*)fMan->GetObject("Fi4Cal"); // = branch name in TTree
    if (!fCalItems)
    {
		printf("Couldnt get handle on FI4 cal items\n");
		return kFATAL;
	}
    //fHitItems = (TClonesArray*)fMan->GetObject("R3BFi4HitItem");
    //fCalibration = NULL; // care about that later
    FairRootManager::Instance()->Register("Fi4Hit", "Land", fHitItems, kTRUE);
    fAngle      =vector<Float_t>(fNDet);
    fRevPoint   =vector<vector<Float_t>>(fNDet);
    for(int ii=0;ii<fNDet;ii++)
      fRevPoint[fNDet]=vector<Float_t>(3);

    ReadGeometry();
    fFiber_width=0.20;//Fiber width in mm
    return kSUCCESS;

}

void R3BFi4Cal2Hit::Exec(Option_t* option)
{
    if (!fCalItems) 
    {
	    printf("Cannot access FI4 cal items\n");
	    return;
    }

    Int_t nMapped = fCalItems->GetEntries();
    for (Int_t i = 0; i < nMapped; i++){
      R3BFi4CalItem* mCalItem = (R3BFi4CalItem*)fCalItems->At(i);
      //get the stuff
      UInt_t det_type = mCalItem->GetDetector();
      UInt_t fiber    = mCalItem->GetFiber();
      
      //calc the position
      Float_t x_pos=XPos(fiber,det_type);
      Float_t y_pos=YPos(fiber,det_type);
      //      Float_t z_pos=ZPos(fiber,det_type);
      new ((*fHitItems)[fHitItems->GetEntriesFast()])
	R3BFi4HitItem(det_type, fiber, x_pos, y_pos, 0,                     // det-type/pos, fiber, xpos, ypos,zpos, 
		      mCalItem->GetEnergy_top(), mCalItem->GetEnergy_bottom(),  // energy top, bottom,
		      mCalItem->GetTime_top(),   mCalItem->GetTime_bottom() );  // time top, bottom
	    
    }
}

void R3BFi4Cal2Hit::FinishEvent()
{
    fHitItems->Clear();
}

void R3BFi4Cal2Hit::FinishTask()
{
}

void R3BFi4Cal2Hit::ReadGeometry(){
  //set with external files parameters like the referencepoint, angle
  /*the first three rows are the reference-point for each detector
    the following row is the eventually tilted angle to the x-axis
    x y z a

    0 0 0 0
    0 0 0 0

    here are just 2 detectors for more the code has to be changed
  */
  /*
  fRevPoint[0]=0;
  fRevPoint[1]=1;
  fRevPoint[2]=2;
  fZPos[0]=0;
  fZPos[1]=0;

  fAngle[0]=0;
  fAngle[1]=0;
    */

  ifstream position;
  position.open(fGeofile);
  if(position.good()){
    LOG(INFO) << "R3BFi4Cal2Hit opening Position file : " << fGeofile << FairLogger::endl;
    std::string line, xpos,ypos,zpos,angle;
    int ndet=0;
    while(position.good()){
      if(ndet==fNDet){
	LOG(WARNING)  << "R3BFi4Cal2Hit in Position file are more detectors given than used: after " << fNDet << " line everything is ignored!!"<< FairLogger::endl;
	break;
      }

      getline(position,line);
      std::istringstream iss(line);
      iss>>xpos>>ypos>>zpos>>angle;
      fRevPoint[ndet][0]=atof(xpos.c_str());
      fRevPoint[ndet][1]=atof(ypos.c_str());
      fRevPoint[ndet][2]=atof(zpos.c_str());
      fAngle[ndet]      =atof(angle.c_str());     

      ndet++;     
    } 
    position.close();

  }else{
    if(fGeofile != "")
      LOG(ERROR) << "R3BFi4Cal2Hit NO Geometry file found : " << fGeofile << " -> Setting all to 0" << FairLogger::endl;
    else{ 
      LOG(INFO)  << "R3BFi4Cal2Hit NO Position file given :  -> Setting all to 0 " << FairLogger::endl;
      //printf("no matching-file given, the factor will be set to 1");
      for(int ndet=0; ndet<fNDet; ndet++){
	fRevPoint[ndet][0] = 0;
	fRevPoint[ndet][1] = 0;
	fRevPoint[ndet][2] = 0;
	fAngle   [ndet]    = 0;
      }//for

    }//else(filename)
  }//else (good())

}//Read

Float_t R3BFi4Cal2Hit::ZPos(UInt_t fiber, UInt_t det_type){
  return fRevPoint[2][det_type]+fiber*fFiber_width*TMath::Sin(fAngle[det_type]);
}

Float_t R3BFi4Cal2Hit::YPos(UInt_t fiber, UInt_t det_type){
  //might be possible from time difference not now!! also rotating the detector against each other possible
  if(det_type==0)  return fiber*fFiber_width;//+fRevPoint[1];
  return -9999;
}

Float_t R3BFi4Cal2Hit::XPos(UInt_t fiber, UInt_t det_type){
  if(det_type==1)  return fiber*fFiber_width;//*TMath::Cos(fAngle[det_type])+fRevPoint[0];
  return -9999;
}
//void R3BFi4Cal2Hit::WriteHistos() {}

ClassImp(R3BFi4Cal2Hit)
