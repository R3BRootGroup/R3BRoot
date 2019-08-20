#include "R3BNeulandHitPar.h"

#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList

R3BNeulandHitPar::R3BNeulandHitPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
    , fParams(new TObjArray(NMODULEMAX))
    , fGlobalTimeOffset(0.0)
{
}

R3BNeulandHitPar::~R3BNeulandHitPar()
{
    if (fParams)
    {
        delete fParams;
        fParams = NULL;
    }
}

void R3BNeulandHitPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BNeulandHitPar::putParams() called";
    if (!list)
    {
        return;
    }
    list->addObject(GetName(), fParams);
}

Bool_t R3BNeulandHitPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BNeulandHitPar::getParams() called";
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fillObject(GetName(), fParams))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BNeulandHitPar::clear() {}

void R3BNeulandHitPar::printParams()
{

    LOG(INFO) << " -----------  " << GetName() << " NeuLAND Parameters -------------  ";

    LOG(INFO) << " Number of Parameters " << fParams->GetEntries();
    for (Int_t i = 0; i < fParams->GetEntries(); i++)
    {
        R3BNeulandHitModulePar* t_par = (R3BNeulandHitModulePar*)fParams->At(i);
        LOG(INFO) << "----------------------------------------------------------------------";
        if (t_par)
        {
            t_par->printParams();
        }
    }
}

ClassImp(R3BNeulandHitPar);
