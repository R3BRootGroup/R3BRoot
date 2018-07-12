#ifndef R3BFI7READER_H
#define R3BFI7READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBSEVEN_t EXT_STR_h101_FIBSEVEN;
typedef struct EXT_STR_h101_FIBSEVEN_onion_t EXT_STR_h101_FIBSEVEN_onion;

class R3BFi7Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi7Reader(EXT_STR_h101_FIBSEVEN *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBSEVEN_onion *fData;

  public:
    ClassDef(R3BFi7Reader, 1);
};

#endif
