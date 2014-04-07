// ---------------------------------------------------------------------------------------
// -----                                                                             -----
// -----                           R3BLandMapping                                    -----
// -----                           Version 0.1                                       -----
// -----                      Created  @ 03.2014 by M.I. Cherciu                     -----
// -----                                                                             -----
// ---------------------------------------------------------------------------------------

#ifndef R3BLAND_MAPPING_H
#define R3BLAND_MAPPING_H

#include <vector>

#include "FairTask.h"

class TClonesArray;

/// Class R3BLandMapping allow to read mapping configuration from file and get identificators for each modules and channels.
class R3BLandMapping : public FairTask
{

  public:
    R3BLandMapping();

    virtual ~R3BLandMapping();

    virtual InitStatus Init();

    virtual void Exec(Option_t* option);

    virtual void FinishEvent();

    void SetFileName(const char* A_FileName)
    {
        fname = A_FileName;
    }
    const char* GetFileName() const
    {
        return fname;
    }
    inline void SetNofBarsPerPlane(Int_t nBars)
    {
        fNofBarsPerPlane = nBars;
    }

  private:
    const char* fname;
    Int_t nMappedElements;
    Int_t fnEvents;
    Int_t fNofBarsPerPlane;
    TClonesArray* fRawData;
    TClonesArray* fLandHit;
    Int_t nEntry;
    std::vector<Int_t> v1map;
    std::vector<Int_t> v2map;
    std::vector<Int_t> v3map;
    std::vector<Int_t> v4map;
    std::vector<Int_t> v5map;
    std::vector<Int_t> v6map;
    Bool_t DoMapping();

  public:
    ClassDef(R3BLandMapping, 0);
};

#endif
