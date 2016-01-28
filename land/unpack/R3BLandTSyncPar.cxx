#include "R3BLandTSyncPar.h"

#include "FairParamList.h" // for FairParamList
#include "FairLogger.h"

R3BLandTSyncPar::R3BLandTSyncPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fTSyncParams(new TObjArray(NMODULEMAX))
{
}

R3BLandTSyncPar::~R3BLandTSyncPar()
{
    if (fTSyncParams)
    {
        delete fTSyncParams;
        fTSyncParams = NULL;
    }
}

void R3BLandTSyncPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BLandTSyncPar::putParams() called" << FairLogger::endl;
    if (!list)
    {
        return;
    }
    list->addObject(GetName(), fTSyncParams);
}

Bool_t R3BLandTSyncPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BLandTSyncPar::getParams() called" << FairLogger::endl;
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fillObject(GetName(), fTSyncParams))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BLandTSyncPar::clear()
{
}

void R3BLandTSyncPar::printParams()
{

    LOG(INFO) << " -----------  " << GetName() << " Time Sync. Parameters -------------  " << FairLogger::endl;

    LOG(INFO) << " Number of TSync Parameters " << fTSyncParams->GetEntries() << FairLogger::endl;
    for (Int_t i = 0; i < fTSyncParams->GetEntries(); i++)
    {
        R3BLandTSyncModulePar* t_par = (R3BLandTSyncModulePar*)fTSyncParams->At(i);
        LOG(INFO) << "----------------------------------------------------------------------" << FairLogger::endl;
        if (t_par)
        {
            t_par->printParams();
        }
    }
}

ClassImp(R3BLandTSyncPar);