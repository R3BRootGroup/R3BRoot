// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BFi4Mapped2Cal                        -----
// -----                    Created  21-01-2016 by R. Plag                 -----
// -----		adapted to Fi4 march to ? by j. tscheuschner	   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Mapped data to Cal data
 */

#ifndef R3BFI4MAPPED2CAL_H
#define R3BFI4MAPPED2CAL_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;

class R3BFi4Mapped2Cal : public FairTask
{
  public:
    R3BFi4Mapped2Cal();
    R3BFi4Mapped2Cal(const char* name, Int_t iVerbose);
    virtual ~R3BFi4Mapped2Cal();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    virtual void FinishTask();
    void SetMappingFile     (TString file){fMappingfile =file;};
    void SetGainMatchingFile(TString file){fMatchingfile=file;};
    void SetMinEnergy(UInt_t energy){fMin_E = energy;};
    void SetTimeWindow(Float_t startTime, Float_t endTime){fMin_T=startTime; fMax_T=endTime;};
  private:

    R3BEventHeader *fHeader;    // do we need that?
    TClonesArray* fMappedItems; // holding input data
    TClonesArray* fCalItems;    // holding output data
    TClonesArray* fCalibration; // holding slopes and offsets
    
    // calibration parameters, might be recreated in another way soon
    const static UInt_t fNDet = 8;
    const static UInt_t fNChannels = 16;
    //const static UInt_t fNMPPC = fNDet*fNChannels;  // FIXME: commented out because of error with ROOT 5 dictionary
    const Float_t fTfactor = 1;
    UInt_t fMin_E = 0;
    UInt_t fMin_T = 0;
    UInt_t fMax_T = 10000;
    Float_t fGain[fNDet][fNChannels];
    Bool_t fxDirection[fNDet];
    //UInt_t fFiber_mapping[fNMPPC][fNMPPC];
    TString fMappingfile;
    TString fMatchingfile;    

    Bool_t BasicCut(UInt_t MinE=0, UInt_t Max_deltaT=100);//Energy is unsigned and time counts up to 100a.u. or so
    void ReadGainMatching(TString matchingfile);
    void ReadMapping(TString mappingfile);//
    // void CreateHistos();
    // void WriteHistos();

  public:
    ClassDef(R3BFi4Mapped2Cal, 3)
};

#endif
