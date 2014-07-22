// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                         R3BCaloCalibPar                           -----
// -----                Created 21/07/2014 by H. Alvarez-Pol               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALOCALIBPAR_H
#define R3BCALOCALIBPAR_H

#include <TVector3.h>
#include <TObjString.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

class R3BCaloCalibPar : public FairParGenericSet
{
 public :
  R3BCaloCalibPar (const char* name="R3BCaloCalibPar",
		   const char* title="Crystal Calibration Parameters",
		   const char* context="TestDefaultContext");
  ~R3BCaloCalibPar(void){};
  void clear(void){};
  void putParams(FairParamList* list);
  Bool_t getParams(FairParamList* list);
  
  void Print(Option_t* option="") const;
  /** Accessor functions **/
  const Double_t GetEneCalibHighGain_p0(Int_t crystal){return eneCalibHighGain_p0[crystal];}
  const Double_t GetEneCalibHighGain_p1(Int_t crystal){return eneCalibHighGain_p1[crystal];}
  const Double_t GetEneCalibHighGain_p2(Int_t crystal){return eneCalibHighGain_p2[crystal];}
  const Double_t GetEneCalibHighGainUncert_p0(Int_t crystal){return eneCalibHighGainUncert_p0[crystal];}
  const Double_t GetEneCalibHighGainUncert_p1(Int_t crystal){return eneCalibHighGainUncert_p1[crystal];}
  const Double_t GetEneCalibHighGainUncert_p2(Int_t crystal){return eneCalibHighGainUncert_p2[crystal];}
  
  const Double_t GetEneCalibLowGain_p0(Int_t crystal){return eneCalibLowGain_p0[crystal];}
  const Double_t GetEneCalibLowGain_p1(Int_t crystal){return eneCalibLowGain_p1[crystal];}
  const Double_t GetEneCalibLowGain_p2(Int_t crystal){return eneCalibLowGain_p2[crystal];}
  const Double_t GetEneCalibLowGainUncert_p0(Int_t crystal){return eneCalibLowGainUncert_p0[crystal];}
  const Double_t GetEneCalibLowGainUncert_p1(Int_t crystal){return eneCalibLowGainUncert_p1[crystal];}
  const Double_t GetEneCalibLowGainUncert_p2(Int_t crystal){return eneCalibLowGainUncert_p2[crystal];}

  const Double_t GetTimeCalib_p0(Int_t crystal){return timeCalib_p0[crystal];}
  const Double_t GetTimeCalib_p1(Int_t crystal){return timeCalib_p1[crystal];}
  const Double_t GetTimeCalib_p2(Int_t crystal){return timeCalib_p2[crystal];}
  const Double_t GetTimeCalibUncert_p0(Int_t crystal){return timeCalibUncert_p0[crystal];}
  const Double_t GetTimeCalibUncert_p1(Int_t crystal){return timeCalibUncert_p1[crystal];}
  const Double_t GetTimeCalibUncert_p2(Int_t crystal){return timeCalibUncert_p2[crystal];}
  
  void SetEneCalibHighGain_p0(Int_t crystal, Double_t value){eneCalibHighGain_p0[crystal]=value;}
  void SetEneCalibHighGain_p1(Int_t crystal, Double_t value){eneCalibHighGain_p1[crystal]=value;}
  void SetEneCalibHighGain_p2(Int_t crystal, Double_t value){eneCalibHighGain_p2[crystal]=value;}
  void SetEneCalibHighGainUncert_p0(Int_t crystal, Double_t value){eneCalibHighGainUncert_p0[crystal]=value;}
  void SetEneCalibHighGainUncert_p1(Int_t crystal, Double_t value){eneCalibHighGainUncert_p1[crystal]=value;}
  void SetEneCalibHighGainUncert_p2(Int_t crystal, Double_t value){eneCalibHighGainUncert_p2[crystal]=value;}

  void SetEneCalibLowGain_p0(Int_t crystal, Double_t value){eneCalibLowGain_p0[crystal]=value;}
  void SetEneCalibLowGain_p1(Int_t crystal, Double_t value){eneCalibLowGain_p1[crystal]=value;}
  void SetEneCalibLowGain_p2(Int_t crystal, Double_t value){eneCalibLowGain_p2[crystal]=value;}
  void SetEneCalibLowGainUncert_p0(Int_t crystal, Double_t value){eneCalibLowGainUncert_p0[crystal]=value;}
  void SetEneCalibLowGainUncert_p1(Int_t crystal, Double_t value){eneCalibLowGainUncert_p1[crystal]=value;}
  void SetEneCalibLowGainUncert_p2(Int_t crystal, Double_t value){eneCalibLowGainUncert_p2[crystal]=value;}

  void SettimeCalib_p0(Int_t crystal, Double_t value){timeCalib_p0[crystal]=value;}
  void SettimeCalib_p1(Int_t crystal, Double_t value){timeCalib_p1[crystal]=value;}
  void SettimeCalib_p2(Int_t crystal, Double_t value){timeCalib_p2[crystal]=value;}
  void SettimeCalibUncert_p0(Int_t crystal, Double_t value){timeCalibUncert_p0[crystal]=value;}
  void SettimeCalibUncert_p1(Int_t crystal, Double_t value){timeCalibUncert_p1[crystal]=value;}
  void SettimeCalibUncert_p2(Int_t crystal, Double_t value){timeCalibUncert_p2[crystal]=value;}
  
 private:
  //Polynomial (second order) parameters for high gain (gammas) calibration 
  //+ uncertainty. [2×3×double×number_of_crystals].
  Double_t* eneCalibHighGain_p0;
  Double_t* eneCalibHighGain_p1;
  Double_t* eneCalibHighGain_p2;
  Double_t* eneCalibHighGainUncert_p0;
  Double_t* eneCalibHighGainUncert_p1;
  Double_t* eneCalibHighGainUncert_p2;

  //Polynomial (second order) parameters for low gain (protons) calibration
  //+ uncertainty. [2×3×double×number_of_crystals].
  Double_t* eneCalibLowGain_p0;
  Double_t* eneCalibLowGain_p1;
  Double_t* eneCalibLowGain_p2;
  Double_t* eneCalibLowGainUncert_p0;
  Double_t* eneCalibLowGainUncert_p1;
  Double_t* eneCalibLowGainUncert_p2;

  // Polynomial (second order) parameters for time calibration 
  //+ uncertainty. [2×3×double×number_of_crystals].
  Double_t* timeCalib_p0;
  Double_t* timeCalib_p1;
  Double_t* timeCalib_p2;
  Double_t* timeCalibUncert_p0;
  Double_t* timeCalibUncert_p1;
  Double_t* timeCalibUncert_p2;

  TString  fGeometryFileName;
  
  ClassDef(R3BCaloCalibPar,1); //
};

#endif /* !R3BCALOCALIBPAR_H*/
