#ifndef R3BFI13READER_H
#define R3BFI13READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBTHIRTEEN_t EXT_STR_h101_FIBTHIRTEEN;
typedef struct EXT_STR_h101_FIBTHIRTEEN_onion_t EXT_STR_h101_FIBTHIRTEEN_onion;

class R3BFi13Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi13Reader(EXT_STR_h101_FIBTHIRTEEN *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBTHIRTEEN_onion *fData;

  public:
    ClassDef(R3BFi13Reader, 1);
};

#endif
