#ifndef R3BFI1AREADER_H
#define R3BFI1AREADER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBONEA_t EXT_STR_h101_FIBONEA;
typedef struct EXT_STR_h101_FIBONEA_onion_t EXT_STR_h101_FIBONEA_onion;

class R3BFi1aReader: public R3BBunchedFiberReader
{
  public:
    R3BFi1aReader(EXT_STR_h101_FIBONEA *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBONEA_onion *fData;

  public:
    ClassDef(R3BFi1aReader, 1);
};

#endif
