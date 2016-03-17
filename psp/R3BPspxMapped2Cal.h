// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxMapped2Cal                              -----
// -----                    Created  21-01-2016 by R. Plag                 -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Mapped data to Cal data
 */

#ifndef R3BPSPXCAL_H
#define R3BPSPXCAL_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;

class R3BPspxMapped2Cal : public FairTask
{
  public:
    R3BPspxMapped2Cal();
    R3BPspxMapped2Cal(const char* name, Int_t iVerbose);
    virtual ~R3BPspxMapped2Cal();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishTask();

  private:

    R3BEventHeader *fHeader;    // do we need that?
    TClonesArray* fMappedItems; // holding input data
    TClonesArray* fCalItems;    // holding output data
	TClonesArray* fCalibration; // holding slopes and offsets

    // void CreateHistos();
    // void WriteHistos();

  public:
    ClassDef(R3BPspxMapped2Cal, 2)
};

#endif
