// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BFi4Mapped2Cal                        -----
// -----                    Created  21-01-2016 by R. Plag                 -----
// -----		   adapted to new test-det by J. Tscheuschner	   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Mapped data to Cal data, meaning: apply gain 
 * to the energy values. & get the fibernumber (position)
 */

#include <iostream>
using namespace std;

#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include <sstream>

#include "R3BEventHeader.h"
#include "R3BFi4MappedItem.h"
#include "R3BFi4CalItem.h"
#include "R3BFi4Mapped2Cal.h"
		
R3BFi4Mapped2Cal::R3BFi4Mapped2Cal()
    : fMappedItems(NULL)
  , fCalItems(new TClonesArray("R3BFi4CalItem")){//change item to data

}

R3BFi4Mapped2Cal::R3BFi4Mapped2Cal(const char* name, Int_t iVerbose)
    : FairTask(name, iVerbose)
    , fMappedItems(NULL)
    , fCalItems(new TClonesArray("R3BFi4CalItem")){

}

R3BFi4Mapped2Cal::~R3BFi4Mapped2Cal(){
}

InitStatus R3BFi4Mapped2Cal::Init(){
    FairRootManager* fMan = FairRootManager::Instance();
    fHeader = (R3BEventHeader*)fMan->GetObject("R3BEventHeader");
    fMappedItems = (TClonesArray*)fMan->GetObject("Fi4Mapped"); // = branch name in TTree
    if (!fMappedItems)
    {
		printf("Couldnt get handle on FI4 mapped items\n");
		return kFATAL;
	}
    //fCalItems = (TClonesArray*)fMan->GetObject("R3BFi4CalItem");
    fCalibration = NULL; // care about that later
    FairRootManager::Instance()->Register("Fi4Cal", "Land", fCalItems, kTRUE);
    ReadMapping(fMappingfile);
    ReadGainMatching(fMatchingfile);
    return kSUCCESS;
}

void R3BFi4Mapped2Cal::Exec(Option_t* option){
  if (!fMappedItems){//check that the data exist
    printf("Cannot access FI4 mapped items\n");
    return;
  }

  UInt_t fiberdetpos;//as option for later which fiber-det position (z-axis) the detector has
  UInt_t detector_offset = 5;//not sure which detectornumber they will get
  //need two loops for bottom and top MPPC
  Int_t nMapped = fMappedItems->GetEntries();

  for (Int_t i = 0; i < nMapped; i++){

    R3BFi4MappedItem* mItem = (R3BFi4MappedItem*)fMappedItems->At(i);

    // calibrate and add to fCalItems

    //First some cuts
    UInt_t detector_1 = mItem->GetDetector() - detector_offset;
    UInt_t channel_1  = mItem->GetChannel();
    UInt_t energy_1   = fGain[detector_1][channel_1]*mItem->GetEnergy();
    UInt_t time_1     = fGain[detector_1][channel_1]*mItem->GetTime();
    if( !BasicCut(energy_1,time_1) )continue;

    UInt_t MPPC_top    =  999999, Energy_top = 0,    Time_top    =  999999, fiber = 999999;
    UInt_t MPPC_bottom =  999999, Energy_bottom = 0, Time_bottom =  0;

    fiber = channel_1 + 16* (detector_1);
    Energy_top = energy_1;
    Energy_bottom = 0;
    Time_top = time_1*fTfactor;
    Time_bottom = 0;

    if( fxDirection[detector_1] ){//x-direction
      fiberdetpos = 1;
      MPPC_top     = channel_1 + 16* detector_1;
    }else{                        //y-direction
      fiberdetpos = 0;
      MPPC_bottom  = channel_1 + 16* detector_1;
    }

    new ((*fCalItems)[fCalItems->GetEntriesFast()])
      R3BFi4CalItem(fiberdetpos,fiber, MPPC_top, MPPC_bottom,  //Position info in the fiber and the MPPC encoded, 
		                                               //so this is a bit redundant but to keep the class the same 
		                                               //and have max overview fiberdetpos will give again info about y or x positioning
		    Energy_top, Energy_bottom,                 //energy there should be only one with entries (top) different from 0
		    Time_top, Time_bottom);                    //time same as above
  }
  
}


void R3BFi4Mapped2Cal::FinishEvent(){
  fCalItems->Clear();
}

void R3BFi4Mapped2Cal::FinishTask(){
}

Bool_t R3BFi4Mapped2Cal::BasicCut(UInt_t e, UInt_t t){
  //to reduce resource costs (network, harddisk, computing-time
  return kTRUE;
  if(e<fMin_E)           return kFALSE;//Energy cut to reduce dark-count rate
  if(t<fMin_T || t<fMax_T)return kFALSE;//time-cut to reduce dark-count rate and remove unreasonable events

  //  return kTRUE;
}

//read-functions for mapping
void R3BFi4Mapped2Cal::ReadMapping(TString mappingfile){
  //read the mapping to get for the combinations of mppc's the fiber-number
  //different kind of mapping possible, may depend on the detector-number or experiment

  // Loading Fiber mappings
 ifstream mapping;
  mapping.open(mappingfile);
  if(mapping.good()){
    LOG(INFO) << "R3BFi4Mapped2Cal opening Mapping file : " << mappingfile << FairLogger::endl;
    std::string line, factor;
    int detector=0, channel=0;
    while(mapping.good()){

      getline(mapping,line);
      std::istringstream iss(line);
      iss>>factor;
      fxDirection[detector]=atof(factor.c_str());
      detector++;
      
    } 
    mapping.close();

  }else{
    if(mappingfile != "") 
      LOG(ERROR) << "R3BFi4Mapped2Cal NO Mapping file : " << mappingfile << " -> Setting all to y-direction" << FairLogger::endl;
    else 
      LOG(INFO)  << "R3BFi4Mapped2Cal NO Mapping file :  -> Setting all channels to y-direction " << FairLogger::endl;

    for(int detector=0; detector<fNDet; detector++)
      fxDirection[detector] = 0;
  }
  
}

void R3BFi4Mapped2Cal::ReadGainMatching(TString matchingfile){
  //gainmatching of each mppc
  /*
    file-format for each mppc please use one line e.g.

1     //top0 hopefully det 0 channel 0
1.02  //top1 ...
2.01  //top2 ...
...
...
...
  */ 
  //slope-correction might be included in the future, but please do this from cal2hit!!

  ifstream gainmatching;
  gainmatching.open(matchingfile);
  if(gainmatching.good()){
    LOG(INFO) << "R3BFi4Mapped2Cal opening Gainmatching file : " << matchingfile << FairLogger::endl;
    std::string line, factor;
    int detector=0, channel=0;
    while(gainmatching.good()){

      getline(gainmatching,line);
      std::istringstream iss(line);
      iss>>factor;
      fGain[detector][channel]=atof(factor.c_str());
      channel++;
      if(channel==16){
	channel=0;
	detector++;
      }
    } 
    gainmatching.close();

  }else{
    if(matchingfile != "")
      LOG(ERROR) << "R3BFi4Mapped2Cal NO Gainmatching file : " << matchingfile << " -> Setting all gains equal to 1" << FairLogger::endl;
    else 
      LOG(INFO)  << "R3BFi4Mapped2Cal NO Gainmatching file :  -> Setting all gains equal to 1 " << FairLogger::endl;
    //printf("no matching-file given, the factor will be set to 1");
    for(int detector=0; detector<fNDet; detector++)
      for(int channels=0; channels<fNChannels;channels++)
	fGain[detector][channels] = 1;
  }
  
}

//void R3BFi4Mapped2Cal::WriteHistos() {}

ClassImp(R3BFi4Mapped2Cal)
