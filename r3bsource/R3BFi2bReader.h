#ifndef R3BFI2BREADER_H
#define R3BFI2BREADER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBTWOB_t EXT_STR_h101_FIBTWOB;
typedef struct EXT_STR_h101_FIBTWOB_onion_t EXT_STR_h101_FIBTWOB_onion;

class R3BFi2bReader: public R3BBunchedFiberReader
{
  public:
    R3BFi2bReader(EXT_STR_h101_FIBTWOB *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBTWOB_onion *fData;

  public:
    ClassDef(R3BFi2bReader, 1);
};

#endif
