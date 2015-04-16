// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BCaloUnpack                           -----
// -----                           Version 0.1                             -----
// -----                    Created 11/10/2013 by Y.Gonzalez               -----
// -----                    Modified 23/09/2014 by A.Perea                 -----                
// -----                                                                   -----
// -----------------------------------------------------------------------------

#ifndef R3BCALOUNPACK_H
#define R3BCALOUNPACK_H

#include "FairUnpack.h"

#include "R3BCaloUnpackPar.h"

class TClonesArray;

static const UInt_t gosip_header_size = 16;
static const UInt_t gosip_sub_header_size = 8;

static const UInt_t max_channel = 16;  // channels per card 
static const UInt_t max_card = 256;    // max. number of cards per crate   
static const UInt_t max_sfp_id = 4;    // crates per controler
static const UInt_t max_pc_id = 2;     // max number of controller

static const int event_t_size = 44;        // size of std event 
static const int kEvent115a_t_size = 40;   // size of an event payload in the new version
static const int kTot_size = 12;           // size of the time-over-threshold optional payload
static const int kTrace_head_t_size = 8;   // size of the header for the trace optional payload


//---------------------------------------------
/**
 * 
 *  
 * 
 */
class R3BCaloUnpack : public FairUnpack {
   public:
    //Constructor
    R3BCaloUnpack(char *strCalDir,
                  Short_t type = 10, Short_t subType = 1,
                  Short_t procId = 1,
                  Short_t subCrate = 0, Short_t control = 9);
    
    //Destructor
    virtual ~R3BCaloUnpack();
    
    //Fair specific
    virtual Bool_t Init();
    
    virtual void SetParContainers();
    virtual Bool_t DoUnpack(Int_t* data, Int_t size);
    virtual void Reset();
      
   protected:
    virtual void Register();
   
   private:
    TClonesArray *fRawData;
    Int_t         fNHits;

    R3BCaloUnpackPar* fCaloUnpackPar;

    ULong64_t nEvents;

   public:
    //Class definition
    ClassDef(R3BCaloUnpack, 0)
};

#endif
