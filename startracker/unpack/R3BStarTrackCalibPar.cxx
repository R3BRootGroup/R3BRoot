// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                    from R3BCaloCalibPar                           -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
#include "R3BStarTrackCalibPar.h"
#include "R3BStarTrackUnpackPar.h"
#include "TMath.h"

R3BStarTrackCalibPar::R3BStarTrackCalibPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  //Polynomial ( premier order) parameters for calibration 
  //+ uncertainty. [2×3×double×number_of_crystals].
  eneCalibGain = new Double_t[numberOfStrips];
  eneCalibOffset = new Double_t[numberOfStrips];
  eneCalibGainUncert = new Double_t[numberOfStrips];
  eneCalibOffsetUncert = new Double_t[numberOfStrips];

  // Polynomial (second order) parameters for time calibration 
  //+ uncertainty.
  timeCalib_p0 = new Double_t[numberOfStrips];
  timeCalib_p1 = new Double_t[numberOfStrips];
  timeCalib_p2 = new Double_t[numberOfStrips];
  timeCalibUncert_p0 = new Double_t[numberOfStrips];
  timeCalibUncert_p1 = new Double_t[numberOfStrips];
  timeCalibUncert_p2 = new Double_t[numberOfStrips];
  
  clear();
}

void R3BStarTrackCalibPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BStarTrackCalibPar::putParams() called"<<std::endl;

  if(!list) return;
  list->add("eneCalibGain", (Double_t*)eneCalibGain, numberOfStrips);
  list->add("eneCalibOffset", (Double_t*)eneCalibGain, numberOfStrips);
  list->add("eneCalibGainUncert", (Double_t*)eneCalibGainUncert, numberOfStrips);
  list->add("eneCalibOffsetUncert", (Double_t*)eneCalibOffsetUncert, numberOfStrips);
  list->add("timeCalib_p0", (Double_t*)timeCalib_p0, numberOfStrips);
  list->add("timeCalib_p1", (Double_t*)timeCalib_p1, numberOfStrips);
  list->add("timeCalib_p2", (Double_t*)timeCalib_p2, numberOfStrips);
  list->add("timeCalibUncert_p0", (Double_t*)timeCalibUncert_p0, numberOfStrips);
  list->add("timeCalibUncert_p1", (Double_t*)timeCalibUncert_p1, numberOfStrips);
  list->add("timeCalibUncert_p2", (Double_t*)timeCalibUncert_p2, numberOfStrips);
}


Bool_t R3BStarTrackCalibPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BStarTrackCalibPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BStarTrackCalibPar::getParams() 1 ";
  
  if (!list->fill("eneCalibGain_", eneCalibGain, numberOfStrips)) return kFALSE;
  if (!list->fill("eneCalibOffset", eneCalibOffset, numberOfStrips)) return kFALSE;
  if (!list->fill("eneCalibHighGainUncert_p0", eneCalibGainUncert, numberOfStrips)) return kFALSE;
  if (!list->fill("eneCalibHighGainUncert_p1", eneCalibOffsetUncert, numberOfStrips)) return kFALSE;
  if (!list->fill("timeCalib_p0", timeCalib_p0, numberOfStrips)) return kFALSE;
  if (!list->fill("timeCalib_p1", timeCalib_p1, numberOfStrips)) return kFALSE;
  if (!list->fill("timeCalib_p2", timeCalib_p2, numberOfStrips)) return kFALSE;
  if (!list->fill("timeCalibUncert_p0", timeCalibUncert_p0, numberOfStrips)) return kFALSE;
  if (!list->fill("timeCalibUncert_p1", timeCalibUncert_p1, numberOfStrips)) return kFALSE;
  if (!list->fill("timeCalibUncert_p2", timeCalibUncert_p2, numberOfStrips)) return kFALSE;
 
  return kTRUE;
}


void R3BStarTrackCalibPar::Print(Option_t* option) const
{
    std::cout<<"-I- StarTrack Calib Parameters:"<<std::endl;
    for(Int_t i=0; i< numberOfStrips; i++) {

      std::cout<<" eneCalibGain["<< i<<"] " << eneCalibGain[i]<<std::endl;
      std::cout<<" eneCalibOffset["<< i<<"] " << eneCalibOffset[i]<<std::endl;
      std::cout<<" eneCalibGainUncert["<< i<<"] " << eneCalibGainUncert[i]<<std::endl;
      std::cout<<" eneCalibOffsetUncert["<< i<<"] " << eneCalibOffsetUncert[i]<<std::endl;

      std::cout<<" timeCalib_p0["<< i<<"] " << timeCalib_p0[i]<<std::endl;
      std::cout<<" timeCalib_p1["<< i<<"] " << timeCalib_p1[i]<<std::endl;
      std::cout<<" timeCalib_p2["<< i<<"] " << timeCalib_p2[i]<<std::endl;
      std::cout<<" timeCalibUncert_p0["<< i<<"] " << timeCalibUncert_p0[i]<<std::endl;
      std::cout<<" timeCalibUncert_p1["<< i<<"] " << timeCalibUncert_p1[i]<<std::endl;
      std::cout<<" timeCalibUncert_p2["<< i<<"] " << timeCalibUncert_p2[i]<<std::endl;      
    }
}

ClassImp(R3BStarTrackCalibPar);
