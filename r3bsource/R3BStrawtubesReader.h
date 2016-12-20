#ifndef R3BSTRAWTUBESREADER_H
#define R3BSTRAWTUBESREADER_H

#include "R3BReader.h"

typedef struct EXT_STR_h101_STRAWTUBES_t EXT_STR_h101_STRAWTUBES;
typedef struct EXT_STR_h101_STRAWTUBES_onion_t EXT_STR_h101_STRAWTUBES_onion;
class FairLogger;

class R3BStrawtubesReader: public R3BReader
{
  public:
    explicit R3BStrawtubesReader(EXT_STR_h101_STRAWTUBES *, UInt_t);
    virtual ~R3BStrawtubesReader();

    Bool_t Init(ext_data_struct_info *);
    Bool_t Read();
    void Reset();

  private:
    EXT_STR_h101_STRAWTUBES_onion *fOnion;
    UInt_t fOffset;
    FairLogger *fLogger;
    TClonesArray *fArray;

  public:
    ClassDef(R3BStrawtubesReader, 0);
};

#endif
