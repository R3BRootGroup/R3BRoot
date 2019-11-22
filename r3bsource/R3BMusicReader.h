#ifndef R3BMUSICREADER_H
#define R3BMUSICREADER_H

#include "R3BReader.h"

class TClonesArray;

#define NUM_MUSIC_ANODES 10
// anodes 0 to 7 : energy and time
// anode 8 and 9 : reference times

struct EXT_STR_h101_MUSIC_t;
typedef struct EXT_STR_h101_MUSIC_t EXT_STR_h101_MUSIC;
typedef struct EXT_STR_h101_MUSIC_onion_t EXT_STR_h101_MUSIC_onion;

class FairLogger;

class R3BMusicReader : public R3BReader
{
  public:
    R3BMusicReader(EXT_STR_h101_MUSIC*, UInt_t);
    ~R3BMusicReader();

  public:
    Bool_t Init(ext_data_struct_info*);
    Bool_t Read();
    void Reset();

    /** Accessor to select online mode **/
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    Bool_t ReadData201911();
    uint32_t multPerAnode[NUM_MUSIC_ANODES];

    // Don't store data for online
    Bool_t fOnline;
    /* Reader specific data structure from ucesb */
    EXT_STR_h101_MUSIC* fData;
    /* Data offset */
    UInt_t fOffset;
    /* FairLogger */
    FairLogger* fLogger;
    /* the structs of type R3BMusicMappedData Item */
    TClonesArray* fArray; /**< Output array. */

  public:
    ClassDef(R3BMusicReader, 0);
};

#endif
