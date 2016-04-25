#include "R3BTCalPar.h"

#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList

ClassImp(R3BTCalPar);

R3BTCalPar::R3BTCalPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fTCalParams(new TObjArray(NMODULEMAX))
    , fMapInit(kFALSE)
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

R3BTCalModulePar* R3BTCalPar::GetModuleParAt(Int_t plane, Int_t paddle, Int_t side)
{
    if (!fMapInit)
    {
        fIndexMap.clear();
        R3BTCalModulePar* par;
        Int_t tplane;
        Int_t tpaddle;
        Int_t tside;
        Int_t index;
        for (Int_t i = 0; i < fTCalParams->GetEntries(); i++)
        {
            par = (R3BTCalModulePar*)fTCalParams->At(i);
            if (NULL == par)
            {
                continue;
            }
            tplane = par->GetPlane();
            tpaddle = par->GetPaddle();
            tside = par->GetSide();
            if (tplane < 1 || tplane > N_PLANE_MAX || tpaddle < 1 || tpaddle > N_PADDLE_MAX || tside < 1 ||
                tside > N_SIDE_MAX)
            {
                LOG(ERROR) << "R3BTCalPar::GetModuleParAt : error in plane/paddle/side indexing. " << tplane << " / "
                           << tpaddle << " / " << tside << FairLogger::endl;
                continue;
            }
            index = (tplane - 1) * N_PLANE_MAX * N_PADDLE_MAX + (tpaddle - 1) * N_PADDLE_MAX + tside - 1;
            if (fIndexMap.find(index) != fIndexMap.end())
            {
                LOG(ERROR) << "R3BTCalPar::GetModuleParAt : parameter found more than once. " << tplane << " / "
                           << tpaddle << " / " << tside << FairLogger::endl;
                continue;
            }
            fIndexMap[index] = i;
        }
        fMapInit = kTRUE;
    }

    if (plane < 1 || plane > N_PLANE_MAX || paddle < 1 || paddle > N_PADDLE_MAX || side < 1 || side > N_SIDE_MAX)
    {
        LOG(ERROR) << "R3BTCalPar::GetModuleParAt : error in plane/paddle/side indexing. " << plane << " / " << paddle
                   << " / " << side << FairLogger::endl;
        return NULL;
    }
    Int_t index = (plane - 1) * N_PLANE_MAX * N_PADDLE_MAX + (paddle - 1) * N_PADDLE_MAX + side - 1;
    ;
    if (fIndexMap.find(index) == fIndexMap.end())
    {
        LOG(WARNING) << "R3BTCalPar::GetModuleParAt : parameter not found for: " << plane << " / " << paddle << " / "
                     << side << FairLogger::endl;
    }
    Int_t arind = fIndexMap[index];
    R3BTCalModulePar* par = (R3BTCalModulePar*)fTCalParams->At(arind);
    return par;
}

void R3BTCalPar::AddModulePar(R3BTCalModulePar* tch)
{
    fMapInit = kFALSE;
    fTCalParams->Add(tch);
}

void R3BTCalPar::PrintModuleParams(Int_t plane, Int_t paddle, Int_t side)
{
    R3BTCalModulePar* par = GetModuleParAt(plane, paddle, side);
    if (NULL != par)
    {
        par->printParams();
    }
}

void R3BTCalPar::DrawModuleParams(Int_t plane, Int_t paddle, Int_t side)
{
    R3BTCalModulePar* par = GetModuleParAt(plane, paddle, side);
    if (NULL != par)
    {
        par->DrawParams();
    }
}
