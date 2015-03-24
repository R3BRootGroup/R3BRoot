/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             * 
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *  
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                            R3BLmdSource                           -----
// -----                    Created 27.02.2015 by D.Kresan                 -----
// -----------------------------------------------------------------------------

#ifndef R3BLmdSource_H
#define R3BLmdSource_H

extern "C"
{
#include "f_evt.h"
#include "s_filhe_swap.h"
#include "s_bufhe_swap.h"
}

#include "TString.h"

#include "FairMbsSource.h"


class TList;
class TClonesArray;
class TH1F;
class TH2F;
class R3BEventHeader;


class R3BLmdSource : public FairMbsSource
{
  public:
    R3BLmdSource();
    R3BLmdSource(const R3BLmdSource& source);
    virtual ~R3BLmdSource();

    void AddFile(TString fileName);
    inline const Int_t GetCurrentFile() const { return fCurrentFile; }
    inline const TList* GetFileNames() const { return fFileNames; }

    virtual Bool_t Init();
    virtual Int_t ReadEvent(UInt_t=0);
    virtual void Close();
    
    inline void SetTimeStampUnit(Int_t unit) { fTSUnit = unit; }
    inline void SetMaxDelay(Int_t delayLower, Int_t delayUpper)
    { fDelayCutLower = delayLower; fDelayCutUpper = delayUpper; }
    
  private:
    Int_t ReadData();
    Int_t ReadMbsEvent();
    
    void CopyNeuLandToBuf();
    void CopyNeuLandToOutput();
    void ClearBuf();
    
    R3BEventHeader *fEventHeader;
    TClonesArray *fNeuLandHits;
    TClonesArray *fCalifaHits;
    
    R3BEventHeader *fBufHeader;
    TClonesArray *fBufNeuLand;
    
    Int_t fTSUnit;
    Int_t fDelayCutLower;
    Int_t fDelayCutUpper;
    
    TH1F *fhDelay;

  protected:
    Bool_t OpenNextFile(TString fileName);

    Int_t fCurrentFile;
	Int_t fNEvent;
	Int_t fCurrentEvent;
    TList* fFileNames;
    s_evt_channel* fxInputChannel;
    s_ve10_1* fxEvent;
    s_bufhe* fxBuffer;
    Int_t* fxEventData;
    s_ves10_1* fxSubEvent;
	s_filhe* fxInfoHeader;

    ClassDef(R3BLmdSource, 0)
};


#endif
