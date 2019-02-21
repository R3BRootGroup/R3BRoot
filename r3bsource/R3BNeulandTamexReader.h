#ifndef R3BNEULANDTAMEXREADER_H
#define R3BNEULANDTAMEXREADER_H

#include "R3BReader.h"
class TClonesArray;

struct EXT_STR_h101_raw_nnp_tamex_t;
typedef struct EXT_STR_h101_raw_nnp_tamex_t EXT_STR_h101_raw_nnp_tamex;

class R3BNeulandTamexReader : public R3BReader
{
  public:
    R3BNeulandTamexReader(EXT_STR_h101_raw_nnp_tamex*, UInt_t);
    ~R3BNeulandTamexReader() override;

    Bool_t Init(ext_data_struct_info*) override;
    Bool_t Read() override;
    void Reset() override;

  private:
    EXT_STR_h101_raw_nnp_tamex* fData; // Reader specific data structure from ucesb
    UInt_t fOffset;                    // Data offset
    TClonesArray* fArray;              // Output array
    const UInt_t fNofPlanes;

  public:
    ClassDefOverride(R3BNeulandTamexReader, 0);
};

#endif
