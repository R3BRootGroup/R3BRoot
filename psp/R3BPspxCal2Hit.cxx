// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxCal2Hit                          -----
// -----		   created 09-03-2016 by I. Syndikus		   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Cal data to Hit data, meaning: calculating uv (internal) and
 * xy (external) coordinates for every event.
 * 
 * TODO replace 17 with nstrips+1
 */

#include <iostream>
using namespace std;

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BEventHeader.h"
#include "R3BPspxCalData.h"
#include "R3BPspxHitData.h"
#include "R3BPspxCal2Hit.h"

R3BPspxCal2Hit::R3BPspxCal2Hit()
    : fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BPspxHitData"))
{
  InitCalibration();
}

R3BPspxCal2Hit::R3BPspxCal2Hit(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fCalItems(NULL)
    , fHitItems(new TClonesArray("R3BPspxHitData"))
{
  InitCalibration();
}

R3BPspxCal2Hit::~R3BPspxCal2Hit()
{
}

InitStatus R3BPspxCal2Hit::Init()
{
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fCalItems = (TClonesArray*)fMan->GetObject("PspxCalData"); // = branch name in TTree
    if (!fCalItems)
    {
		printf("Couldnt get handle on PSPX cal items\n");
		return kFATAL;
	}
    //fHitItems = (TClonesArray*)fMan->GetObject("R3BPspxHitData");
    fCalibration = NULL; // care about that later
    FairRootManager::Instance()->Register("PspxHitData", "Land", fHitItems, kTRUE);

    return kSUCCESS;
}

void R3BPspxCal2Hit::Exec(Option_t* option)
{
    if (!fCalItems) 
    {
	    printf("Cannot access PSPX cal items\n");
	    return;
    }
    
    // Initialize    
    Float_t u;
    Float_t v;
    Float_t x;
    Float_t y;
    Float_t energy;
    UInt_t mult[length_array_detector];
    UInt_t strip_mult[length_array_detector];
                   
    for(UInt_t j = 0; j<length_array_detector;j++){
	mult[j]=0;
	strip_mult[j]=0;
    }

    Int_t nMapped = fCalItems->GetEntries();
    
    
    // calculationg multiplicities
    for (Int_t i = 0; i < nMapped; i++) 
    {
	  R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);
	    
	  // calculating multiplicity
          if(mItem->GetStrip()!=17){
	    strip_mult[mItem->GetDetector()]++;
	  }
	  mult[mItem->GetDetector()]++;
	  if(mItem->GetEnergy2()!=0){
	    mult[mItem->GetDetector()]++;
	  }

    }

    // calculating positions for each detector
    for(UInt_t detector = 1; detector<length_array_detector;detector++){
      if(mult[detector]!=0){ // if nothing hit invalid
	  u=0;
	  v=0;
	  x=0;
	  y=0;
	  energy=0;
	  // 1 strip (2 channels) + cathode hit
	  if(mult[detector]==3 && strip_mult[detector]==1){
	    for(Int_t i = 0; i < nMapped; i++){
	       R3BPspxCalData* mItem = (R3BPspxCalData*)fCalItems->At(i);
	       
	       if(detector==mItem->GetDetector()){
		  if(mItem->GetStrip()==17) energy=mItem->GetEnergy1();
		  else if(mItem->GetStrip()>0 && mItem->GetStrip()<17){
		      if(detector==4){//vertical detector (s438b: detector 4)
			  u=(mItem->GetStrip()-nstrips/2.-0.5)/(nstrips/2.);
			  v=(mItem->GetEnergy1()-mItem->GetEnergy2())/(mItem->GetEnergy1()+mItem->GetEnergy2());
			  
			  x=sign_x[detector]*u*detector_length/2.;
			  y=offset[detector][mItem->GetStrip()]+slope[detector][mItem->GetStrip()]*v;
		      }
		      if(detector==5){//horizontal detector (s438b: detector 5)
			  u=(mItem->GetEnergy1()-mItem->GetEnergy2())/(mItem->GetEnergy1()+mItem->GetEnergy2());
			  v=(mItem->GetStrip()-nstrips/2.-0.5)/(nstrips/2.);
					  
			  x=offset[detector][mItem->GetStrip()]+slope[detector][mItem->GetStrip()]*u;
			  y=sign_y[detector]*v*detector_length/2.;
		      }
		  }
	      }
	    }
	  }

	  // 2 strips (4 channels) + cathode hit
	  if(mult[detector]==5 && strip_mult[detector]==2){
	    for(Int_t i = 0; i < nMapped; i++){
		R3BPspxCalData* mItem1 = (R3BPspxCalData*)fCalItems->At(i);
		
		if(mItem1->GetDetector()==detector){
		  if(mItem1->GetStrip()==17) energy=mItem1->GetEnergy1();
		  else{
		    for(Int_t j = i+1; j < nMapped; j++){ // start with i+1 to avoid double counting of pairs
		      R3BPspxCalData* mItem2 = (R3BPspxCalData*)fCalItems->At(j);
		      
		      if(mItem2->GetDetector()==detector && (mItem1->GetStrip()==mItem2->GetStrip()+1 || mItem1->GetStrip()==mItem2->GetStrip()-1) && mItem2->GetStrip()!=17){
			if(detector==4){//vertical detector (s438b: detector 4)
			    u=((mItem1->GetStrip()+mItem2->GetStrip())/2.-nstrips/2.-0.5)/(nstrips/2.);
			    v=((mItem1->GetEnergy1()+mItem2->GetEnergy1())-(mItem1->GetEnergy2()+mItem2->GetEnergy2()))/(mItem1->GetEnergy1()+mItem2->GetEnergy1()+mItem1->GetEnergy2()+mItem2->GetEnergy2());
			    
			    x=sign_x[detector]*u*detector_length/2.;
			    y=(offset[detector][mItem1->GetStrip()]+offset[detector][mItem2->GetStrip()])/2.+(slope[detector][mItem1->GetStrip()]+slope[detector][mItem2->GetStrip()])/2.*v;
			}
			if(detector==5){//horizontal detector (s438b: detector 5)
			    u=((mItem1->GetEnergy1()+mItem2->GetEnergy1())-(mItem1->GetEnergy2()+mItem2->GetEnergy2()))/(mItem1->GetEnergy1()+mItem2->GetEnergy1()+mItem1->GetEnergy2()+mItem2->GetEnergy2());
			    v=((mItem1->GetStrip()+mItem2->GetStrip())/2.-nstrips/2.-0.5)/(nstrips/2.);
					    
			    x=(offset[detector][mItem1->GetStrip()]+offset[detector][mItem2->GetStrip()])/2.+(slope[detector][mItem1->GetStrip()]+slope[detector][mItem2->GetStrip()])/2.*v;
			    y=sign_y[detector]*v*detector_length/2.;
			}		      
		      }
		    }
		  }
		}
	    }
	  }	

	  if(energy!=0){
	    new ((*fHitItems)[fHitItems->GetEntriesFast()]) R3BPspxHitData(detector,u,v,x,y,energy,mult[detector]); // det,u,v,x,y,energy, multiplicity
	  }
      }
    }


}

void R3BPspxCal2Hit::FinishEvent()
{
    fHitItems->Clear();
}

void R3BPspxCal2Hit::FinishTask()
{
}

void R3BPspxCal2Hit::InitCalibration()
{
  for(UInt_t i=0;i<length_array_detector;i++){	
    sign_x[i]=1;
    sign_y[i]=1;
    
    for(UInt_t j=0;j<length_array_strip;j++){
	offset[i][j]=0;
	slope[i][j]=detector_length/2.;
    }
  }
          
  sign_x[4]=-1;
  sign_y[5]=-1;
  
  // done 28.04.16, test3 (cal), start counting with 1 for detector and channel,
  // valid also for test4 (cal), check was done for most of the parameter
  offset[4][6]=-0.00237587;
  slope[4][6]=2.53953;
  offset[4][7]=-0.00415368;
  slope[4][7]=2.53824;
  offset[4][8]=-0.00278635;
  slope[4][8]=2.53776;
  offset[4][9]=-0.00150658;
  slope[4][9]=2.54448;
  offset[4][10]=-0.00196951; //lower threshold for peak finder: 0.05
  slope[4][10]=2.5441;
  offset[4][11]=-0.0030721; //lower threshold for peak finder: 0.04
  slope[4][11]=2.54395;
  offset[4][12]=-0.00413915;
  slope[4][12]=2.54397;
  offset[4][13]=-0.00486661; //higher threshold for peak finder: 0.25
  slope[4][13]=2.54077;

  offset[5][4]=0.000601069;
  slope[5][4]=2.51852;
  offset[5][5]=0.000991526;
  slope[5][5]=2.51812;
  offset[5][6]=0.000623896;
  slope[5][6]=2.5172;
  offset[5][7]=0.00496261;
  slope[5][7]=2.52705;
  offset[5][8]=0.00704843;
  slope[5][8]=2.52655;
  offset[5][9]=0.00393909;
  slope[5][9]=2.52172;
  offset[5][10]=0.00514579;
  slope[5][10]=2.53048;
  offset[5][11]=0.00135386;
  slope[5][11]=2.53206;
  offset[5][12]=0.000110492;
  slope[5][12]=2.53273;
  offset[5][13]=0.00227417;
  slope[5][13]=2.52753;
  offset[5][14]=0.00386112;
  slope[5][14]=2.53043;
  offset[5][15]=0.00251124;
  slope[5][15]=2.52448;

}


//void R3BPspxCal2Hit::WriteHistos() {}

ClassImp(R3BPspxCal2Hit)
