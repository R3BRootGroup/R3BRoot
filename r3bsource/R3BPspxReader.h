#ifndef R3BPSPXREADER_H
#define R3BPSPXREADER_H

#include "R3BReader.h"

struct EXT_STR_h101_PSP_t;
typedef struct EXT_STR_h101_PSP_t EXT_STR_h101_PSP;
class FairLogger;
class R3BPspxMappedPar;
class TClonesArray;

/**
 * Class to unpack (with ucesb) to Mapped data for PSPX detector data.
 * This includes: Checking for error messages.
 * @author Ralf Plag (?), Bastian Loeher(?), Ina Syndikus
 */

class R3BPspxReader : public R3BReader
{
  public:
    /** Standard Constructor **/
    R3BPspxReader(EXT_STR_h101_PSP*, UInt_t);
    /** Destructor **/
    ~R3BPspxReader();

    Bool_t Init(ext_data_struct_info*);
    Bool_t ReInit();
    void SetParContainers();
    Bool_t Read();
    void Reset(); /**< Reset the output array **/

  private:
    EXT_STR_h101_PSP* fData;    /**< Reader specific data structure from ucesb */
    UInt_t fOffset;             /**< Data Offset */
    FairLogger* fLogger;        /**< FairLogger */
    TClonesArray* fMappedItems; /**< Array holding output (Mapped) data */

    R3BPspxMappedPar* fMappedPar; /**< Parameter instance holding number of detectors & strips. */

  public:
    ClassDef(R3BPspxReader, 3);
};

#endif
