// -------------------------------------------------------------------------
// -----         R3BCalifaMapped2CrystalCalPar source file             -----
// -----             Created 22/07/14  by H.Alvarez                    -----
// -----            Modified 20/03/17  by P.Cabanelas                  -----
// -----            Modified 11/12/17  by E.Galiana                    -----
// -------------------------------------------------------------------------

#ifndef R3BCALIFAMAPPED2CRYSTALCALPAR_H
#define R3BCALIFAMAPPED2CRYSTALCALPAR_H

#include "FairTask.h"
#include "TH1F.h"

class TClonesArray;
class R3BCalifaCrystalCalPar;
class R3BEventHeader;

class R3BCalifaMapped2CrystalCalPar : public FairTask {
  
 public:
  
  /** Default constructor **/
  R3BCalifaMapped2CrystalCalPar();
  
  /** Standard constructor **/
  R3BCalifaMapped2CrystalCalPar(const char* name, Int_t iVerbose=1);
  
  /** Destructor **/
  virtual ~R3BCalifaMapped2CrystalCalPar();
  
  /** Virtual method Init **/
  virtual InitStatus Init();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  /** Virtual method FinishEvent **/
  virtual void FinishEvent();
  
  /** Virtual method FinishTask **/
  virtual void FinishTask(); 
  
  /** Virtual method Reset **/
  virtual void Reset();
  
  /** Virtual method ReInit **/
  virtual InitStatus ReInit();
  
  /** Virtual method Search peaks and calibrate **/
  virtual void SearchPeaks();
  
  void SetOutputFile(const char *outFile);
  
  /** Accessor functions **/
  const Int_t GetNumCrystals(){return fNumCrystals;}
  const Int_t GetCalRange_left(){return fMapHistos_left;}
  const Int_t GetCalRange_right(){return fMapHistos_right;}
  const Int_t GetCalRange_bins(){return fMapHistos_bins;}
  const Int_t GetNumPeaks(){return fNumPeaks;}	 
  const Double_t GetSigma(){return fSigma;}
  const Double_t GetThreshold(){return fThreshold;}
  const Int_t GetNumParameterFit(){return fNumParam;} 
  const Int_t GetMinStadistics(){return fMinStadistics;} 
  
  TArrayF* GetEnergyPeaks() {return fEnergyPeaks;}
  
  void SetNumCrystals(Int_t numberCry){fNumCrystals=numberCry;}
  void SetCalRange_left(Int_t Histos_left){fMapHistos_left=Histos_left;}
  void SetCalRange_right(Int_t Histos_right){fMapHistos_right=Histos_right;}
  void SetCalRange_bins(Int_t Histos_bins){fMapHistos_bins=Histos_bins;}
  void SetNumPeaks(Int_t numberpeaks){fNumPeaks=numberpeaks;}
  void SetSigma(Double_t sigma){fSigma=sigma;}
  void SetThreshold(Double_t threshold){fThreshold=threshold;}
  void SetNumParameterFit(Int_t numberParFit){fNumParam=numberParFit;}
  void SetMinStadistics(Int_t minstad){fMinStadistics=minstad;}

  void SetDebugMode(Int_t debug){fDebugMode=debug;}
  
  void SetEnergyPeaks(TArrayF* thePeaks) {
    fEnergyPeaks = thePeaks;
    fNumPeaks = thePeaks->GetSize();
  }
  
  
 protected:
  
  Int_t fNumCrystals;
  Int_t fMapHistos_left;		
  Int_t fMapHistos_right;
  Int_t fMapHistos_bins;
  
  Int_t fNumParam;
  Int_t fMinStadistics;
  
  Int_t fNumPeaks;
  Double_t fSigma;
  Double_t fThreshold;
  
  TArrayF* fEnergyPeaks;
  Double_t* fChannelPeaks;

  Int_t fDebugMode;
  
  R3BCalifaCrystalCalPar* fCal_Par;  /**< Parameter container. >*/ 
  TClonesArray* fCalifaMappedDataCA; /**< Array with CALIFA Mapped- input data. >*/
  
  TH1F** fh_Map_energy_crystal;
  char *fOutputFile;
  
 public:
  ClassDef(R3BCalifaMapped2CrystalCalPar,2);
};

#endif
