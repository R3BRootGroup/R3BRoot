// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloUnpack                            -----
// -----                           Version 0.1                             -----
// -----                    Created 11.10.2013 by Y.Gonzalez               -----
// -----                                                                   -----
// -----------------------------------------------------------------------------

//ROOT headers
#include "TClonesArray.h"

//Fair headers
#include "FairRootManager.h"
#include "FairLogger.h"

//Califa headers
#include "R3BCaloRawHit.h"
#include "R3BCaloUnpack.h"

#define NUMCHANNELS 1500
#define NUMMODULES 1500



//R3BCaloUnpack: Constructor
R3BCaloUnpack::R3BCaloUnpack(char *strCalDir,
                             Short_t type, Short_t subType,
                             Short_t procId,
                             Short_t subCrate, Short_t control)
: FairUnpack(type, subType, procId, subCrate, control),
fRawData(new TClonesArray("R3BCaloRawHit")),
fNHits(0)
{
}



//Virtual R3BCaloUnpack: Public method
R3BCaloUnpack::~R3BCaloUnpack()
{
  LOG(INFO) << "R3BCaloUnpack: Delete instance" << FairLogger::endl;
  delete fRawData;
}



//Init: Public method
Bool_t R3BCaloUnpack::Init()
{
  Register();
  return kTRUE;
}



//Register: Protected method
void R3BCaloUnpack::Register()
{
  LOG(DEBUG) << "Registering" << FairLogger::endl;
  FairRootManager *fMan = FairRootManager::Instance();
  if(! fMan) {
    return;
  }
  fMan->Register("CaloRawHit", "Raw data from Califa", fRawData, kTRUE);
}



//DoUnpack: Public method
Bool_t R3BCaloUnpack::DoUnpack(Int_t *data, Int_t size)
{
  //	trace_head_t *califa_trace = new trace_head_t;
  //	UShort_t *trace_data;
  
  
  UInt_t l_s = 0;
  UInt_t *pl_data = (UInt_t*) data;
  
  
  LOG(DEBUG) << "Unpacking" << FairLogger::endl;
  
  
  while(l_s < size) {
    // Remove 0xadd... words
    while((data[l_s] & 0xfff00000) == 0xadd00000) {
      l_s++;
    }
    
    
    LOG(DEBUG) << "At GOSIP memory" << FairLogger::endl;
    
    
    UShort_t header_size;
    UShort_t trigger;
    UShort_t module_id;
    UShort_t submemory_id;
    UInt_t data_size;
    header_size = pl_data[l_s] & 0xff;
    trigger = (pl_data[l_s] >> 8) & 0xff;
    module_id = (pl_data[l_s] >> 16) & 0xff;
    submemory_id = (pl_data[l_s++] >> 24) & 0xff;
    data_size = pl_data[l_s++];
    
    
    LOG(DEBUG) << "========== gosip " << FairLogger::endl
    << "     header_size " << header_size << FairLogger::endl
    << "         trigger " << trigger << FairLogger::endl
    << "       module_id " << module_id << FairLogger::endl
    << "    submemory_id " << submemory_id << FairLogger::endl
    << "       data_size " << data_size << FairLogger::endl;
    
    
    if(header_size != 0x34) {
      break;
    }
    
    
    // Data reduction: size == 0 -> no more events
    if(data_size == 0)
      break;
    
    
    // GSI special channel -> skip
    if(submemory_id == 0xff)
    {
      l_s += data_size / 4;
      continue;
    }
    
    
    // Real CALIFA data channel
    UShort_t evsize;
    UShort_t magic_affe;
    UInt_t event_id;
    ULong_t timestamp;
    UShort_t cfd_samples[4];
    UShort_t loverflow;
    UShort_t hoverflow;
    UShort_t self_triggered;
    UShort_t num_pileup;
    UShort_t num_discarded;
    UShort_t energy;
    //    UShort_t reserved;
    UShort_t qpid_size;
    UShort_t magic_babe;
    Short_t n_f;
    UShort_t n_s;
    evsize = pl_data[l_s] & 0xffff;
    magic_affe = (pl_data[l_s++] >> 16) & 0xffff;
    event_id = pl_data[l_s++];
    timestamp = pl_data[l_s++];
    timestamp |= (ULong_t)pl_data[l_s++] << 32;
    cfd_samples[0] = pl_data[l_s] & 0xff;
    cfd_samples[1] = pl_data[l_s++] >> 16;
    cfd_samples[2] = pl_data[l_s] & 0xff;
    cfd_samples[3] = pl_data[l_s++] >> 16;
    loverflow = pl_data[l_s] & 0xffff;
    hoverflow = (pl_data[l_s] >> 16) & 0xff;
    self_triggered = (pl_data[l_s++] >> 24) & 0xff;
    num_pileup = pl_data[l_s] & 0xffff;
    num_discarded = (pl_data[l_s++] >> 16) & 0xffff;
    energy = pl_data[l_s++] & 0xffff;
    qpid_size = pl_data[l_s] & 0xffff;
    magic_babe = (pl_data[l_s++] >> 16) & 0xffff;
    n_f = pl_data[l_s] & 0xffff;
    n_s = (pl_data[l_s++] >> 16) & 0xffff;
    
    
    LOG(DEBUG) << " --------- event " << FairLogger::endl
    << "        event_id " << event_id << FairLogger::endl
    << "          energy " << energy << FairLogger::endl
    << "       timestamp " << timestamp << FairLogger::endl
    << "=================================" << FairLogger::endl;
    
    
    new ((*fRawData)[fNHits]) R3BCaloRawHit(module_id*1500+submemory_id, energy, timestamp);
    fNHits++;
  }
  
  
  LOG(DEBUG) << "End of memory" << FairLogger::endl;
  LOG(INFO) << "R3BCaloUnpack: Number of CALIFA raw hits: " << fNHits << FairLogger::endl;
  
  
  return kTRUE;
}



//Reset: Public method
void R3BCaloUnpack::Reset()
{
  LOG(DEBUG) << "Clearing Data Structure" << FairLogger::endl;
  fRawData->Clear();
  fNHits = 0;
}



ClassImp(R3BCaloUnpack)
