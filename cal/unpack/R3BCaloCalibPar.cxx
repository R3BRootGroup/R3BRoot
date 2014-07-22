// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                         R3BCaloCalibPar                           -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#include <iostream>
#include "R3BCaloCalibPar.h"
#include "R3BCaloUnpackPar.h"
#include "TMath.h"

R3BCaloCalibPar::R3BCaloCalibPar(const char* name, const char* title, const char* context)
  : FairParGenericSet(name,title,context)
{
  //Polynomial (second order) parameters for high gain (gammas) calibration 
  //+ uncertainty. [2×3×double×number_of_crystals].
  eneCalibHighGain_p0 = new Double_t[numberOfCrystals];
  eneCalibHighGain_p1 = new Double_t[numberOfCrystals];
  eneCalibHighGain_p2 = new Double_t[numberOfCrystals];
  eneCalibHighGainUncert_p0 = new Double_t[numberOfCrystals];
  eneCalibHighGainUncert_p1 = new Double_t[numberOfCrystals];
  eneCalibHighGainUncert_p2 = new Double_t[numberOfCrystals];
  
  //Polynomial (second order) parameters for low gain (protons) calibration
  //+ uncertainty. [2×3×double×number_of_crystals].
  eneCalibLowGain_p0 = new Double_t[numberOfCrystals];
  eneCalibLowGain_p1 = new Double_t[numberOfCrystals];
  eneCalibLowGain_p2 = new Double_t[numberOfCrystals];
  eneCalibLowGainUncert_p0 = new Double_t[numberOfCrystals];
  eneCalibLowGainUncert_p1 = new Double_t[numberOfCrystals];
  eneCalibLowGainUncert_p2 = new Double_t[numberOfCrystals];
  
  // Polynomial (second order) parameters for time calibration 
  //+ uncertainty. [2×3×double×number_of_crystals].
  timeCalib_p0 = new Double_t[numberOfCrystals];
  timeCalib_p1 = new Double_t[numberOfCrystals];
  timeCalib_p2 = new Double_t[numberOfCrystals];
  timeCalibUncert_p0 = new Double_t[numberOfCrystals];
  timeCalibUncert_p1 = new Double_t[numberOfCrystals];
  timeCalibUncert_p2 = new Double_t[numberOfCrystals];

  clear();
}

void R3BCaloCalibPar::putParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloCalibPar::putParams() called"<<std::endl;

  if(!list) return;
  list->add("eneCalibHighGain_p0", (Double_t*)eneCalibHighGain_p0, numberOfCrystals);
  list->add("eneCalibHighGain_p1", (Double_t*)eneCalibHighGain_p1, numberOfCrystals);
  list->add("eneCalibHighGain_p2", (Double_t*)eneCalibHighGain_p2, numberOfCrystals);
  list->add("eneCalibHighGainUncert_p0", (Double_t*)eneCalibHighGainUncert_p0, numberOfCrystals);
  list->add("eneCalibHighGainUncert_p1", (Double_t*)eneCalibHighGainUncert_p1, numberOfCrystals);
  list->add("eneCalibHighGainUncert_p2", (Double_t*)eneCalibHighGainUncert_p2, numberOfCrystals);
  list->add("eneCalibLowGain_p0", (Double_t*)eneCalibLowGain_p0, numberOfCrystals);
  list->add("eneCalibLowGain_p1", (Double_t*)eneCalibLowGain_p1, numberOfCrystals);
  list->add("eneCalibLowGain_p2", (Double_t*)eneCalibLowGain_p2, numberOfCrystals);
  list->add("eneCalibLowGainUncert_p0", (Double_t*)eneCalibLowGainUncert_p0, numberOfCrystals);
  list->add("eneCalibLowGainUncert_p1", (Double_t*)eneCalibLowGainUncert_p1, numberOfCrystals);
  list->add("eneCalibLowGainUncert_p2", (Double_t*)eneCalibLowGainUncert_p2, numberOfCrystals);
  list->add("timeCalib_p0", (Double_t*)timeCalib_p0, numberOfCrystals);
  list->add("timeCalib_p1", (Double_t*)timeCalib_p1, numberOfCrystals);
  list->add("timeCalib_p2", (Double_t*)timeCalib_p2, numberOfCrystals);
  list->add("timeCalibUncert_p0", (Double_t*)timeCalibUncert_p0, numberOfCrystals);
  list->add("timeCalibUncert_p1", (Double_t*)timeCalibUncert_p1, numberOfCrystals);
  list->add("timeCalibUncert_p2", (Double_t*)timeCalibUncert_p2, numberOfCrystals);
}


Bool_t R3BCaloCalibPar::getParams(FairParamList* list)
{
  std::cout<<"-I- R3BCaloCalibPar::getParams() called"<<std::endl;
  if (!list) return kFALSE;
  std::cout<<"-I- R3BCaloCalibPar::getParams() 1 ";
  
  if (!list->fill("eneCalibHighGain_p0", eneCalibHighGain_p0, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibHighGain_p1", eneCalibHighGain_p1, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibHighGain_p2", eneCalibHighGain_p2, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibHighGainUncert_p0", eneCalibHighGainUncert_p0, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibHighGainUncert_p1", eneCalibHighGainUncert_p1, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibHighGainUncert_p2", eneCalibHighGainUncert_p2, numberOfCrystals)) return kFALSE;  
  if (!list->fill("eneCalibLowGain_p0", eneCalibLowGain_p0, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibLowGain_p1", eneCalibLowGain_p1, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibLowGain_p2", eneCalibLowGain_p2, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibLowGainUncert_p0", eneCalibLowGainUncert_p0, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibLowGainUncert_p1", eneCalibLowGainUncert_p1, numberOfCrystals)) return kFALSE;
  if (!list->fill("eneCalibLowGainUncert_p2", eneCalibLowGainUncert_p2, numberOfCrystals)) return kFALSE;
  if (!list->fill("timeCalib_p0", timeCalib_p0, numberOfCrystals)) return kFALSE;
  if (!list->fill("timeCalib_p1", timeCalib_p1, numberOfCrystals)) return kFALSE;
  if (!list->fill("timeCalib_p2", timeCalib_p2, numberOfCrystals)) return kFALSE;
  if (!list->fill("timeCalibUncert_p0", timeCalibUncert_p0, numberOfCrystals)) return kFALSE;
  if (!list->fill("timeCalibUncert_p1", timeCalibUncert_p1, numberOfCrystals)) return kFALSE;
  if (!list->fill("timeCalibUncert_p2", timeCalibUncert_p2, numberOfCrystals)) return kFALSE;
 
  return kTRUE;
}


void R3BCaloCalibPar::Print(Option_t* option) const
{
    std::cout<<"-I- CALIFA Calib Parameters:"<<std::endl;
    for(Int_t i=0; i<numberOfCrystals; i++) {
      std::cout<<" eneCalibHighGain_p0["<< i<<"] " << eneCalibHighGain_p0[i]<<std::endl;
      std::cout<<" eneCalibHighGain_p1["<< i<<"] " << eneCalibHighGain_p1[i]<<std::endl;
      std::cout<<" eneCalibHighGain_p2["<< i<<"] " << eneCalibHighGain_p2[i]<<std::endl;
      std::cout<<" eneCalibHighGainUncert_p0["<< i<<"] " << eneCalibHighGainUncert_p0[i]<<std::endl;
      std::cout<<" eneCalibHighGainUncert_p1["<< i<<"] " << eneCalibHighGainUncert_p1[i]<<std::endl;
      std::cout<<" eneCalibHighGainUncert_p2["<< i<<"] " << eneCalibHighGainUncert_p2[i]<<std::endl;
      std::cout<<" eneCalibLowGain_p0["<< i<<"] " << eneCalibLowGain_p0[i]<<std::endl;
      std::cout<<" eneCalibLowGain_p1["<< i<<"] " << eneCalibLowGain_p1[i]<<std::endl;
      std::cout<<" eneCalibLowGain_p2["<< i<<"] " << eneCalibLowGain_p2[i]<<std::endl;
      std::cout<<" eneCalibLowGainUncert_p0["<< i<<"] " << eneCalibLowGainUncert_p0[i]<<std::endl;
      std::cout<<" eneCalibLowGainUncert_p1["<< i<<"] " << eneCalibLowGainUncert_p1[i]<<std::endl;
      std::cout<<" eneCalibLowGainUncert_p2["<< i<<"] " << eneCalibLowGainUncert_p2[i]<<std::endl;
      std::cout<<" timeCalib_p0["<< i<<"] " << timeCalib_p0[i]<<std::endl;
      std::cout<<" timeCalib_p1["<< i<<"] " << timeCalib_p1[i]<<std::endl;
      std::cout<<" timeCalib_p2["<< i<<"] " << timeCalib_p2[i]<<std::endl;
      std::cout<<" timeCalibUncert_p0["<< i<<"] " << timeCalibUncert_p0[i]<<std::endl;
      std::cout<<" timeCalibUncert_p1["<< i<<"] " << timeCalibUncert_p1[i]<<std::endl;
      std::cout<<" timeCalibUncert_p2["<< i<<"] " << timeCalibUncert_p2[i]<<std::endl;      
    }
}

ClassImp(R3BCaloCalibPar);
