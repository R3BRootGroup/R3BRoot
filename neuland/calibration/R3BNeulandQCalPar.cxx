#include "R3BNeulandQCalPar.h"
#include "FairLogger.h"
#include "FairParamList.h" // for FairParamList

R3BNeulandQCalPar::R3BNeulandQCalPar(const char* name, const char* title, const char* context, Bool_t own)
    : FairParGenericSet(name, title, context, own)
{
}

R3BNeulandQCalPar::~R3BNeulandQCalPar() {}

void R3BNeulandQCalPar::putParams(FairParamList* list)
{
    LOG(INFO) << "R3BNeulandQCalPar::putParams() called" << FairLogger::endl;
    if (!list)
    {
        return;
    }
    list->add("QCalParams", fParams);
}

Bool_t R3BNeulandQCalPar::getParams(FairParamList* list)
{
    LOG(INFO) << "R3BNeulandQCalPar::getParams() called" << FairLogger::endl;
    if (!list)
    {
        return kFALSE;
    }
    if (!list->fill("QCalParams", &fParams))
    {
        return kFALSE;
    }
    return kTRUE;
}

void R3BNeulandQCalPar::SetSize(Int_t size)
{
    fParams.Set(size);
    fParams.Reset();
}

void R3BNeulandQCalPar::printParams()
{

    LOG(INFO) << " -----------  " << GetName() << " NeuLAND Parameters -------------  " << FairLogger::endl;
    LOG(INFO) << " Number of Parameters " << fParams.GetSize() << FairLogger::endl;
    for (Int_t i = 0; i < fParams.GetSize(); i++)
        LOG(INFO) << "PMT Nr. " << i << " : pedestal level = " << fParams.At(i) << FairLogger::endl;
    LOG(INFO) << "----------------------------------------------------------------------" << FairLogger::endl;
}

ClassImp(R3BNeulandQCalPar);