// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxCal                              -----
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

class R3BPspxCal : public FairTask
{
  public:
    R3BPspxCal();
    R3BPspxCal(const char* name, Int_t iVerbose);
    virtual ~R3BPspxCal();

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
    ClassDef(R3BPspxCal, 2)
};

#endif
