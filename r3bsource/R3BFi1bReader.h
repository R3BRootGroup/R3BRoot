#ifndef R3BFI1BREADER_H
#define R3BFI1BREADER_H

#include "R3BBunchedFiberReader.h"

typedef struct EXT_STR_h101_FIBONEB_t EXT_STR_h101_FIBONEB;
typedef struct EXT_STR_h101_FIBONEB_onion_t EXT_STR_h101_FIBONEB_onion;

class R3BFi1bReader: public R3BBunchedFiberReader
{
  public:
    R3BFi1bReader(EXT_STR_h101_FIBONEB *, UInt_t);
    Bool_t Init(ext_data_struct_info *);

  private:
    EXT_STR_h101_FIBONEB_onion *fData;

  public:
    ClassDef(R3BFi1bReader, 1);
};

#endif
