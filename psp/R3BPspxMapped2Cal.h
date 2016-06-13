// -----------------------------------------------------------------------------
// -----                                                                   -----
// -----                           R3BPspxMapped2Cal                       -----
// -----                    Created  21-01-2016 by R. Plag                 -----
// -----		   renamed 09-03-2016 by I. Syndikus		   -----
// -----                                                                   -----
// -----------------------------------------------------------------------------
/* Convert Mapped data to Cal data
 */

#ifndef R3BPSPXMAPPED2CAL_H
#define R3BPSPXMAPPED2CAL_H

#include "FairTask.h"

class TClonesArray;
class R3BEventHeader;
class R3BPspxCalPar;

class R3BPspxMapped2Cal : public FairTask
{
  public:
    R3BPspxMapped2Cal();
    R3BPspxMapped2Cal(const char* name, Int_t iVerbose);
    virtual ~R3BPspxMapped2Cal();

    virtual InitStatus Init();
    InitStatus ReInit();
    void SetParContainers();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();
    virtual void FinishTask();

  private:

    R3BEventHeader *fHeader;    // do we need that?
    TClonesArray* fMappedItems; // holding input data
    TClonesArray* fCalItems;    // holding output data
    TClonesArray* fCalibration; // holding slopes and offsets
    
    R3BPspxCalPar* fCalPar;
    std::vector<vector<Float_t>> gain;

    // void CreateHistos();
    // void WriteHistos();

  public:
    ClassDef(R3BPspxMapped2Cal, 5)
};

#endif
