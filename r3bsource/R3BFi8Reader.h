#ifndef R3BFI8READER_H
#define R3BFI8READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBEIGHT_t EXT_STR_h101_FIBEIGHT;
typedef struct EXT_STR_h101_FIBEIGHT_onion_t EXT_STR_h101_FIBEIGHT_onion;

class R3BFi8Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi8Reader(EXT_STR_h101_FIBEIGHT *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBEIGHT_onion *fData;

  public:
    ClassDef(R3BFi8Reader, 1);
};

#endif
