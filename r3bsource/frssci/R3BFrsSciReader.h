#ifndef R3BFRSSCIREADER_H
#define R3BFRSSCIREADER_H

#include "R3BReader.h"
#include "TClonesArray.h"

#include <Rtypes.h>

struct EXT_STR_h101_FRSSCI_t;
typedef struct EXT_STR_h101_FRSSCI_t EXT_STR_h101_FRSSCI;

class R3BFrsSciReader : public R3BReader
{
  public:
    // Standard constructor
    R3BFrsSciReader(EXT_STR_h101_FRSSCI*, size_t);
    R3BFrsSciReader(EXT_STR_h101_FRSSCI*, size_t, UShort_t);

    // Destructor
    virtual ~R3BFrsSciReader();

    // Setup structure information
    virtual Bool_t Init(ext_data_struct_info*) override;

    // Read data from full event structure
    virtual Bool_t R3BRead() override;

    // Reset
    virtual void Reset() override;

    // Accessor to select online mode
    void SetOnline(Bool_t option) { fOnline = option; }

  private:
    // Reader specific data structure from ucesb
    EXT_STR_h101_FRSSCI* fData;
    // Data offset
    size_t fOffset;
    // Don't store data for online
    Bool_t fOnline;
    // Output array of type R3BFrsSciMapped
    TClonesArray* fArray;

    UInt_t fNumEntries;
    UShort_t fNumSci;

  public:
    ClassDefOverride(R3BFrsSciReader, 0);
};

#endif // R3BFRSSCIREADER_H
