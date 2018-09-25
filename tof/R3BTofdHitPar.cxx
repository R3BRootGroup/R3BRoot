#include "R3BTofdHitPar.h"

#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList

ClassImp(R3BTofdHitPar);

R3BTofdHitPar::R3BTofdHitPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fHitParams(new TObjArray(NMODULEMAX))
    , fMapInit(kFALSE)
{
}

R3BTofdHitPar::~R3BTofdHitPar()
{
    if (fHitParams)
    {
        delete fHitParams;
        fHitParams = NULL;
    }
}

void R3BTofdHitPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BTofdHitPar::putParams() called" << FairLogger::endl;
    if (!list)
    {
        return;
    }
    list->addObject(GetName(), fHitParams);
}

Bool_t R3BTofdHitPar::getParams(FairParamList* list)
{
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fillObject(GetName(), fHitParams))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BTofdHitPar::clear()
{
}

void R3BTofdHitPar::printParams()
{

    LOG(INFO) << " -----------  " << GetName() << " Time Calib. Parameters -------------  " << FairLogger::endl;

    LOG(INFO) << " Number of HIT Parameters " << fHitParams->GetEntries() << FairLogger::endl;
    for (Int_t i = 0; i < fHitParams->GetEntries(); i++)
    {
        R3BTofdHitModulePar* t_par = (R3BTofdHitModulePar*)fHitParams->At(i);
        LOG(INFO) << "----------------------------------------------------------------------" << FairLogger::endl;
        if (t_par)
        {
            t_par->printParams();
        }
    }
}

R3BTofdHitModulePar* R3BTofdHitPar::GetModuleParAt(Int_t plane, Int_t paddle)
{
    if (!fMapInit)
    {
        fIndexMap.clear();
        R3BTofdHitModulePar* par;
        Int_t tplane;
        Int_t tpaddle;
        Int_t index;
        for (Int_t i = 0; i < fHitParams->GetEntries(); i++)
        {
            par = (R3BTofdHitModulePar*)fHitParams->At(i);
            if (NULL == par)
            {
                continue;
            }
            tplane = par->GetPlane();
            tpaddle = par->GetPaddle();
            if (tplane < 1 || tplane > N_PLANE_TOFD_MAX || tpaddle < 1 || tpaddle > N_PADDLE_TOFD_MAX)
            {
                LOG(ERROR) << "R3BTofdHitPar::GetModuleParAt : error in plane/paddle indexing. " << tplane << " / "
                           << tpaddle << FairLogger::endl;
                continue;
            }
            index = (tplane - 1) * N_PADDLE_TOFD_MAX + tpaddle - 1;
            if (fIndexMap.find(index) != fIndexMap.end())
            {
                LOG(ERROR) << "R3BTofdHitPar::GetModuleParAt : parameter found more than once. " << tplane << " / "
                           << tpaddle << FairLogger::endl;
                continue;
            }
            fIndexMap[index] = i;
        }
        fMapInit = kTRUE;
    }

    if (plane < 1 || plane > N_PLANE_TOFD_MAX || paddle < 1 || paddle > N_PADDLE_TOFD_MAX)
    {
        LOG(ERROR) << "R3BTofdHitPar::GetModuleParAt : error in plane/paddle indexing. " << plane << " / " << paddle << FairLogger::endl;
        return NULL;
    }
    Int_t index = (plane - 1) * N_PADDLE_TOFD_MAX + paddle - 1;

    if (fIndexMap.find(index) == fIndexMap.end())
    {
        LOG(WARNING) << "R3BTofdHitPar::GetModuleParAt : parameter not found for: " << plane << " / " << paddle << FairLogger::endl;
        return NULL;
    }
    Int_t arind = fIndexMap[index];
    R3BTofdHitModulePar* par = (R3BTofdHitModulePar*)fHitParams->At(arind);
    return par;
}

void R3BTofdHitPar::AddModulePar(R3BTofdHitModulePar* tch)
{
    fMapInit = kFALSE;
    fHitParams->Add(tch);
}

void R3BTofdHitPar::PrintModuleParams(Int_t plane, Int_t paddle)
{
    R3BTofdHitModulePar* par = GetModuleParAt(plane, paddle);
    if (NULL != par)
    {
        par->printParams();
    }
}

void R3BTofdHitPar::DrawModuleParams(Int_t plane, Int_t paddle)
{
    R3BTofdHitModulePar* par = GetModuleParAt(plane, paddle);
    if (NULL != par)
    {
        par->DrawParams();
    }
}
