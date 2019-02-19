#ifndef R3BFI12READER_H
#define R3BFI12READER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBTWELVE_t EXT_STR_h101_FIBTWELVE;
typedef struct EXT_STR_h101_FIBTWELVE_onion_t EXT_STR_h101_FIBTWELVE_onion;

class R3BFi12Reader: public R3BBunchedFiberReader
{
  public:
    R3BFi12Reader(EXT_STR_h101_FIBTWELVE *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBTWELVE_onion *fData;

  public:
    ClassDef(R3BFi12Reader, 1);
};

#endif
