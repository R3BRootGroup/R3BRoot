#include "R3BTCalPar.h"

#include "FairParamList.h" // for FairParamList
#include "FairLogger.h"

ClassImp(R3BTCalPar);

R3BTCalPar::R3BTCalPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fTCalParams(new TObjArray(NMODULEMAX))
{
}

R3BTCalPar::~R3BTCalPar()
{
    if (fTCalParams)
    {
        delete fTCalParams;
        fTCalParams = NULL;
    }
}

void R3BTCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BTCalPar::putParams() called" << FairLogger::endl;
    if (!list)
    {
        return;
    }
    list->addObject(GetName(), fTCalParams);
}

Bool_t R3BTCalPar::getParams(FairParamList* list)
{
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fillObject(GetName(), fTCalParams))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BTCalPar::clear()
{
}

void R3BTCalPar::printParams()
{

    LOG(INFO) << " -----------  " << GetName() << " Time Calib. Parameters -------------  " << FairLogger::endl;

    LOG(INFO) << " Number of TCal Parameters " << fTCalParams->GetEntries() << FairLogger::endl;
    for (Int_t i = 0; i < fTCalParams->GetEntries(); i++)
    {
        R3BTCalModulePar* t_par = (R3BTCalModulePar*)fTCalParams->At(i);
        LOG(INFO) << "----------------------------------------------------------------------" << FairLogger::endl;
        if (t_par)
        {
            t_par->printParams();
        }
    }
}
