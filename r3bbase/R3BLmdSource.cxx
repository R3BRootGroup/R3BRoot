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
#include <iostream>
using namespace std;

#include "TList.h"
#include "TObjString.h"
#include "TClonesArray.h"
#include "TH1F.h"
#include "TH2F.h"

#include "FairRootManager.h"
#include "FairRunOnline.h"
#include "FairLogger.h"

#include "R3BEventHeader.h"
#include "R3BNeulandUnpackData.h"
#include "R3BCaloRawHit.h"
#include "R3BLmdSource.h"


R3BLmdSource::R3BLmdSource()
  : FairMbsSource(),
    fEventHeader(NULL),
    fNeuLandHits(NULL),
    fCalifaHits(NULL),
    fBufHeader(new R3BEventHeader()),
    fBufNeuLand(new TClonesArray("R3BNeulandUnpackData")),
    fTSUnit(1),
    fDelayCutLower(250),
    fDelayCutUpper(380),
    fCurrentFile(0),
	fNEvent(0),
    fCurrentEvent(0),
    fFileNames(new TList()),
    fxInputChannel(NULL),
    fxEvent(NULL),
    fxBuffer(NULL),
    fxEventData(NULL),
    fxSubEvent(NULL),
    fxInfoHeader(NULL)
{
}


R3BLmdSource::R3BLmdSource(const R3BLmdSource& source)
  : FairMbsSource(source),
    fEventHeader(NULL),
    fNeuLandHits(NULL),
    fCalifaHits(NULL),
    fBufHeader(new R3BEventHeader()),
    fBufNeuLand(new TClonesArray("R3BNeulandUnpackData")),
    fTSUnit(1),
    fDelayCutLower(250),
    fDelayCutUpper(380),
    fCurrentFile(source.GetCurrentFile()),
	fNEvent(0),
    fCurrentEvent(0),
    fFileNames(new TList()),
    fxInputChannel(NULL),
    fxEvent(NULL),
    fxBuffer(NULL),
    fxEventData(NULL),
    fxSubEvent(NULL),
    fxInfoHeader(NULL)
{
}


R3BLmdSource::~R3BLmdSource()
{
  fFileNames->Delete();
  delete fFileNames;
}


void R3BLmdSource::AddFile(TString fileName)
{
  TObjString* str = new TObjString(fileName);
  fFileNames->Add(str);
}


Bool_t R3BLmdSource::Init()
{
  if(! FairMbsSource::Init()) {
    return kFALSE;
  }

  if(fFileNames->GetSize() == 0) {
    return kFALSE;
  }

  TString name = ((TObjString*)fFileNames->At(fCurrentFile))->GetString();
  if(! OpenNextFile(name)) {
    return kFALSE;
  }

  fCurrentFile += 1;

 // Init Counters
  fNEvent=fCurrentEvent=0;
    
    FairRootManager *rootMgr = FairRootManager::Instance();
    
    fEventHeader = (R3BEventHeader*) rootMgr->GetObject("R3BEventHeader");
    if(NULL == fEventHeader)
    {
        return kFALSE;
    }
    
    fNeuLandHits = (TClonesArray*) rootMgr->GetObject("LandRawHit");
    if(NULL == fNeuLandHits)
    {
        return kFALSE;
    }
    
    fCalifaHits = (TClonesArray*) rootMgr->GetObject("CaloRawHit");
    if(NULL == fCalifaHits)
    {
        return kFALSE;
    }
    
    fhDelay = new TH1F("hDelay", "Delay between NeuLAND and CALIFA", 1100, -1000., 10000.);
    FairRunOnline::Instance()->AddObject(fhDelay);

  return kTRUE;
}


Bool_t R3BLmdSource::OpenNextFile(TString fileName)
{
  Int_t inputMode = 1;
  fxInputChannel = new s_evt_channel;  
  void* headptr = &fxInfoHeader;
  INTS4 status;
  status = f_evt_get_open(inputMode,
                          const_cast<char*>(fileName.Data()),
                          fxInputChannel,
                          (Char_t**)headptr,
                          1,
                          1);

  if(status) {
    return kFALSE;
  }


  // Decode File Header
  /*Bool_t result = */Unpack((Int_t*)fxInfoHeader, sizeof(s_filhe), -4, -4, -4, -4, -4);

  cout << "-I- R3BLmdSource::OpenNextFile : file "
       << fileName << " opened." << endl;
 
  

  return kTRUE;
}


Int_t R3BLmdSource::ReadEvent(UInt_t iev)
{
    ClearBuf();
    
    // Read next NeuLAND sub-event
    Int_t status;
    do
    {
        Reset();
        status = ReadData();
        // Check if eof reached
        if(1 == status)
        {
            return status;
        }
    } while (fNeuLandHits->GetEntries() == 0);
    
    // Copy NeuLAND data to the internal buffer
    CopyNeuLandToBuf();

    
    // Read next CALIFA sub-event
    do
    {
        Reset();
        status = ReadData();
        // Check if eof reached
        if(1 == status)
        {
            return status;
        }
        if(fNeuLandHits->GetEntries() > 0)
        {
            // Copy NeuLAND data to the internal buffer
            CopyNeuLandToBuf();
            continue;
        }
    }
    while (fCalifaHits->GetEntries() == 0);
    
    // Get first hit from CALIFA
    R3BCaloRawHit *caloHit = (R3BCaloRawHit*) fCalifaHits->At(0);
    // Calculate time delay between main DAQ and CALIFA
    Long_t tdiff = caloHit->GetTime() - fBufHeader->GetTimeStamp();
    fhDelay->Fill(tdiff);
    // If within the range - save all to output
    if(tdiff >= fDelayCutLower && tdiff <= fDelayCutUpper)
    {
        CopyNeuLandToOutput();
        return 0;
    }
    else
    {
        ClearBuf();
    }

    // Do not save output
    return 2;
}


Int_t R3BLmdSource::ReadData()
{
    Int_t status;
    do
    {
        status = ReadMbsEvent();
        if(1 == status)
        {
            return status;
        }
    }
    while (fNeuLandHits->GetEntries() == 0 && fCalifaHits->GetEntries() == 0);
    
    return 0;
}


void R3BLmdSource::CopyNeuLandToBuf()
{
    ClearBuf();
    
    // Copy event header
    fBufHeader->SetTimeStamp(fEventHeader->GetTimeStamp());
    fBufHeader->SetTrigger(fEventHeader->GetTrigger());
    
    // Copy NeuLAND raw hits
    for(Int_t i = 0; i < fNeuLandHits->GetEntries(); i++)
    {
        new ((*fBufNeuLand)[i]) R3BNeulandUnpackData(*((R3BNeulandUnpackData*)fNeuLandHits->At(i)));
    }
}


void R3BLmdSource::CopyNeuLandToOutput()
{
    // Copy event header
    fEventHeader->SetTimeStamp(fBufHeader->GetTimeStamp());
    fEventHeader->SetTrigger(fBufHeader->GetTrigger());
    
    // Copy NeuLAND raw hits
    fNeuLandHits->Clear();
    for(Int_t i = 0; i < fBufNeuLand->GetEntries(); i++)
    {
        new ((*fNeuLandHits)[i]) R3BNeulandUnpackData(*((R3BNeulandUnpackData*)fBufNeuLand->At(i)));
    }
    
    // Clear the buffer
    ClearBuf();
}


void R3BLmdSource::ClearBuf()
{
    fBufNeuLand->Clear();
}


Int_t R3BLmdSource::ReadMbsEvent()
{
  void* evtptr = &fxEvent;
  void* buffptr = &fxBuffer;

  Int_t status = f_evt_get_event(fxInputChannel, (INTS4**)evtptr,(INTS4**) buffptr);
  //Int_t fuEventCounter = fxEvent->l_count;
  //Int_t fCurrentMbsEventNo = fuEventCounter;

  if(GETEVT__SUCCESS != status) {
    if(fCurrentFile >= fFileNames->GetSize()) {
      return 1;
    }

    if(GETEVT__NOMORE == status) {
      Close();
    }

    TString name = ((TObjString*)fFileNames->At(fCurrentFile))->GetString();
    if(! OpenNextFile(name)) {
      return 1;
    } else {
      fCurrentFile += 1;
      return ReadEvent();
    }
  }

 //Store Start Times
  if (fCurrentEvent==0 ) 
      Unpack((Int_t*)fxBuffer, sizeof(s_bufhe), -4, -4, -4, -4, -4);


  // Decode event header
  Bool_t result = kFALSE;
  /*Bool_t result = */Unpack((Int_t*)fxEvent, sizeof(s_ve10_1), -2, -2, -2, -2, -2);

  Int_t nrSubEvts = f_evt_get_subevent(fxEvent, 0, NULL, NULL, NULL);
  Int_t sebuflength;
  Short_t setype;
  Short_t sesubtype;
  Short_t seprocid;
  Short_t sesubcrate;
  Short_t secontrol;

  //if (fCurrentEvent%10000==0)
  //cout << " -I- LMD_ANA:  evt# " <<  fCurrentEvent << "  n_subevt# " << nrSubEvts << " evt processed# " << fNEvent <<  " : " << fxEvent->l_count << endl;

//  Int_t* SubEventDataPtr = new Int_t;
  for(Int_t i = 1; i <= nrSubEvts; i++) {
    void* SubEvtptr = &fxSubEvent;
    void* EvtDataptr = &fxEventData;
    Int_t nrlongwords;
    status = f_evt_get_subevent(fxEvent, i, (Int_t**)SubEvtptr, (Int_t**)EvtDataptr, &nrlongwords);
    if(status) {
      return 1;
    }
    sebuflength = nrlongwords;
    setype = fxSubEvent->i_type;
    sesubtype = fxSubEvent->i_subtype;
    seprocid = fxSubEvent->i_procid;
    sesubcrate = fxSubEvent->h_subcrate;
    secontrol = fxSubEvent->h_control;

    if(Unpack(fxEventData, sebuflength,
              setype, sesubtype,
              seprocid, sesubcrate, secontrol)) {
      result = kTRUE;
    }
  }

  // Increment evt counters.
  fNEvent++;
  fCurrentEvent++;
 
  if(! result)
  {
    return 2;
  }

 return 0;
}


void R3BLmdSource::Close()
{
  f_evt_get_close(fxInputChannel);
  Unpack((Int_t*)fxBuffer, sizeof(s_bufhe), -4, -4, -4, -4, -4);
  fCurrentEvent=0;
}


ClassImp(R3BLmdSource)


